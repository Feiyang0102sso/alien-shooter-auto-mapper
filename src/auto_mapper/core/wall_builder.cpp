/**
 * @file wall_builder.cpp
 * @brief Implements core wall generation pipeline.
 *
 * Supports mixed wall types in a single build:
 * segments are grouped by wall_type, each group is rasterized and
 * projected independently using its own WallProfile, then merged.
 */

#include "auto_mapper/core/wall_builder.h"
#include "auto_mapper/core/geometry.h"
#include "auto_mapper/common/logger.h"
#include <algorithm>
#include <cmath>
#include <set>
#include <map>
#include <utility>
#include <unordered_map>

namespace auto_mapper::core {

WallBuilder::WallBuilder(float map_size_x, float map_size_y)
    : map_size_x_(map_size_x), map_size_y_(map_size_y) {}

const WallProfile& WallBuilder::get_wall_profile(int wall_type) {
    // Static lookup table for wall profiles
    static const WallProfile profiles[] = {
        WALL_STANDARD,  // index 0
        WALL_LAB,       // index 1
    };
    static constexpr int NUM_PROFILES = sizeof(profiles) / sizeof(profiles[0]);

    if (wall_type >= 0 && wall_type < NUM_PROFILES) {
        return profiles[wall_type];
    }
    Logger::warning("Unknown wall_type={}, falling back to STANDARD", wall_type);
    return profiles[WALL_TYPE_STANDARD];
}

const FloorProfile& WallBuilder::get_floor_profile(int floor_type) {
    static const std::unordered_map<int, FloorProfile> profiles = {
        // vid, step_x, step_y, pos_z
        // shift_offset fields removed: shift is computed dynamically from map_size
        {FLOOR_TYPE_STANDARD, {500, 40.0f, 28.0f, 0.0f}},
        {FLOOR_TYPE_LAB,      {503, 80.0f, 56.0f, 0.0f}}
    };
    if (profiles.find(floor_type) != profiles.end()) {
        return profiles.at(floor_type);
    }
    return profiles.at(FLOOR_TYPE_STANDARD);
}

const CeilingProfile& WallBuilder::get_ceiling_profile(int ceiling_type) {
    static const std::unordered_map<int, CeilingProfile> profiles = {
        // vid, step_x, step_y, pos_z, shift_offset_x, shift_offset_y
        {CEILING_TYPE_STANDARD, {504, 80.0f, 56.0f, 90.0f, 40.0f, 14.0f}}
    };
    if (profiles.find(ceiling_type) != profiles.end()) {
        return profiles.at(ceiling_type);
    }
    return profiles.at(CEILING_TYPE_STANDARD);
}

static MapPoint get_wall_shift(float map_size_x, const WallProfile& profile) {
    float divisor = static_cast<float>(profile.grid_divisor);
    float grid_step_x = profile.step_x / divisor;
    float grid_step_y = profile.step_y / divisor;

    float remainder_x = grid_step_x / 2.0f;
    float remainder_y = grid_step_y / 2.0f;

    float raw_shift_x = map_size_x / 2.0f;
    float grid_x_shift = std::round((raw_shift_x - remainder_x) / grid_step_x);
    float shift_x = grid_x_shift * grid_step_x + remainder_x;

    float raw_shift_y = remainder_y;
    float grid_y_shift = std::round((raw_shift_y - remainder_y) / grid_step_y);
    float shift_y = grid_y_shift * grid_step_y + remainder_y + profile.step_y;

    return {shift_x, shift_y};
}

MapPoint WallBuilder::get_phys(int lx, int ly, int w_type) const {
    const WallProfile& profile = get_wall_profile(w_type);
    MapPoint shift = get_wall_shift(map_size_x_, profile);
    return to_iso(GridPoint{lx, ly}, profile.step_x, profile.step_y, shift);
}

std::vector<WallBuilder::RawSprite> WallBuilder::process_wall_sprites(const std::vector<Segment>& segments) const {
    std::vector<RawSprite> raw_sprites;
    using Point = std::pair<int, int>;
    std::map<int, std::vector<const Segment*>> groups;
    for (const auto& seg : segments) {
        groups[seg.wall_type].push_back(&seg);
    }

    for (const auto& [wt, group_segs] : groups) {
        const WallProfile& profile = get_wall_profile(wt);
        std::set<Point> edges_a; 
        std::set<Point> edges_b; 

        for (const auto* seg : group_segs) {
            int x1 = seg->start.x;
            int y1 = seg->start.y;
            int x2 = seg->end.x;
            int y2 = seg->end.y;

            if (x1 == x2) {
                int minY = std::min(y1, y2);
                int maxY = std::max(y1, y2);
                for (int y = minY + 1; y <= maxY; ++y) edges_a.insert({x1, y});
            } else if (y1 == y2) {
                int minX = std::min(x1, x2);
                int maxX = std::max(x1, x2);
                for (int x = minX + 1; x <= maxX; ++x) edges_b.insert({x, y1});
            }
        }

        for (const auto& p : edges_a) {
            raw_sprites.push_back({p.first, p.second, wt, profile.id_dir_a});
        }
        for (const auto& p : edges_b) {
            raw_sprites.push_back({p.first, p.second, wt, profile.id_dir_b});
        }

        std::set<Point> vertices;
        for (const auto& p : edges_a) {
            vertices.insert({p.first, p.second - 1});
            vertices.insert({p.first, p.second});
        }
        for (const auto& p : edges_b) {
            vertices.insert({p.first - 1, p.second});
            vertices.insert({p.first, p.second});
        }

        for (const auto& v : vertices) {
            int x = v.first;
            int y = v.second;

            bool up    = edges_a.count({x, y}) > 0;
            bool down  = edges_a.count({x, y + 1}) > 0;
            bool left  = edges_b.count({x, y}) > 0;
            bool right = edges_b.count({x + 1, y}) > 0;

            bool conn_a = up || down;
            bool conn_b = left || right;
            int total_conns = (up ? 1 : 0) + (down ? 1 : 0) + (left ? 1 : 0) + (right ? 1 : 0);

            if (total_conns == 1 || (conn_a && conn_b)) {
                raw_sprites.push_back({x, y, wt, profile.id_pillar});
            }
        }
    }

    std::sort(raw_sprites.begin(), raw_sprites.end(), [](const RawSprite& a, const RawSprite& b) {
        if (a.gx == b.gx && a.gy == b.gy) return a.vid < b.vid;
        return (a.gx + a.gy) < (b.gx + b.gy);
    });
    raw_sprites.erase(std::unique(raw_sprites.begin(), raw_sprites.end(), [](const RawSprite& a, const RawSprite& b) {
        return a.gx == b.gx && a.gy == b.gy && a.wall_type == b.wall_type && a.vid == b.vid;
    }), raw_sprites.end());

    return raw_sprites;
}

WallBuilder::PhysicalGridContext WallBuilder::build_physical_grid(const std::vector<Segment>& segments) const {
    int cell_size = 5;
    float min_px = 0.0f, max_px = map_size_x_;
    float min_py = 0.0f, max_py = map_size_y_;
    
    for (const auto& seg : segments) {
        MapPoint p1 = get_phys(seg.start.x, seg.start.y, seg.wall_type);
        MapPoint p2 = get_phys(seg.end.x, seg.end.y, seg.wall_type);
        min_px = std::min({min_px, p1.x, p2.x});
        max_px = std::max({max_px, p1.x, p2.x});
        min_py = std::min({min_py, p1.y, p2.y});
        max_py = std::max({max_py, p1.y, p2.y});
    }

    float margin = 200.0f;
    min_px -= margin; max_px += margin;
    min_py -= margin; max_py += margin;

    int grid_w = (max_px - min_px) / cell_size + 1;
    int grid_h = (max_py - min_py) / cell_size + 1;

    std::vector<bool> physical_grid(grid_w * grid_h, false);
    std::vector<int> physical_floor_type(grid_w * grid_h, -1);

    for (const auto& seg : segments) {
        MapPoint p1 = get_phys(seg.start.x, seg.start.y, seg.wall_type);
        MapPoint p2 = get_phys(seg.end.x, seg.end.y, seg.wall_type);
        
        int x1 = (p1.x - min_px) / cell_size;
        int y1 = (p1.y - min_py) / cell_size;
        int x2 = (p2.x - min_px) / cell_size;
        int y2 = (p2.y - min_py) / cell_size;

        int dx = std::abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
        int dy = -std::abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
        int err = dx + dy, e2;

        while (true) {
            for(int i=-1; i<=1; ++i) {
                for(int j=-1; j<=1; ++j) {
                    int nx = x1 + i; int ny = y1 + j;
                    if (nx >= 0 && nx < grid_w && ny >= 0 && ny < grid_h) {
                        int idx = ny * grid_w + nx;
                        physical_grid[idx] = true;
                        physical_floor_type[idx] = seg.wall_type;
                    }
                }
            }
            if (x1 == x2 && y1 == y2) break;
            e2 = 2 * err;
            if (e2 >= dy) { err += dy; x1 += sx; }
            if (e2 <= dx) { err += dx; y1 += sy; }
        }
    }

    std::vector<bool> outside_grid(grid_w * grid_h, false);
    std::vector<std::pair<int, int>> q;
    q.push_back({0, 0});
    outside_grid[0] = true;
    
    size_t head = 0;
    while(head < q.size()) {
        auto [cx, cy] = q[head++];
        int neighbors[4][2] = {{cx+1, cy}, {cx-1, cy}, {cx, cy+1}, {cx, cy-1}};
        for (auto& n : neighbors) {
            int nx = n[0], ny = n[1];
            if (nx >= 0 && nx < grid_w && ny >= 0 && ny < grid_h) {
                int idx = ny * grid_w + nx;
                if (!physical_grid[idx] && !outside_grid[idx]) {
                    outside_grid[idx] = true;
                    q.push_back({nx, ny});
                }
            }
        }
    }

    std::vector<int> floor_type_grid(grid_w * grid_h, -1);
    std::vector<std::pair<int, int>> fq;
    for (int i = 0; i < grid_w * grid_h; ++i) {
        if (physical_grid[i] && physical_floor_type[i] != -1) {
            floor_type_grid[i] = physical_floor_type[i];
            fq.push_back({i % grid_w, i / grid_w});
        }
    }
    
    size_t fhead = 0;
    while(fhead < fq.size()) {
        auto [cx, cy] = fq[fhead++];
        int ft = floor_type_grid[cy * grid_w + cx];
        int neighbors[4][2] = {{cx+1, cy}, {cx-1, cy}, {cx, cy+1}, {cx, cy-1}};
        for (auto& n : neighbors) {
            int nx = n[0], ny = n[1];
            if (nx >= 0 && nx < grid_w && ny >= 0 && ny < grid_h) {
                int idx = ny * grid_w + nx;
                if (!outside_grid[idx] && floor_type_grid[idx] == -1) {
                    floor_type_grid[idx] = ft;
                    fq.push_back({nx, ny});
                }
            }
        }
    }

    return PhysicalGridContext{grid_w, grid_h, min_px, min_py, std::move(physical_grid), std::move(outside_grid), std::move(floor_type_grid)};
}

std::vector<io::Sprite> WallBuilder::place_floors(const std::vector<Segment>& segments, const PhysicalGridContext& grid_ctx) const {
    std::vector<io::Sprite> floor_sprites;
    int cell_size = 5;

    // Floor should be bounded closely to the walls
    float b_min_px = 1e9, b_max_px = -1e9, b_min_py = 1e9, b_max_py = -1e9;
    for (const auto& seg : segments) {
        MapPoint p1 = get_phys(seg.start.x, seg.start.y, seg.wall_type);
        MapPoint p2 = get_phys(seg.end.x, seg.end.y, seg.wall_type);
        b_min_px = std::min({b_min_px, p1.x, p2.x});
        b_max_px = std::max({b_max_px, p1.x, p2.x});
        b_min_py = std::min({b_min_py, p1.y, p2.y});
        b_max_py = std::max({b_max_py, p1.y, p2.y});
    }
    b_min_px -= 200.0f; b_max_px += 200.0f;
    b_min_py -= 200.0f; b_max_py += 200.0f;

    std::vector<int> floor_types = {FLOOR_TYPE_STANDARD, FLOOR_TYPE_LAB};
    for (int ft : floor_types) {
        const FloorProfile& f_prof = get_floor_profile(ft);
        float half_step_x = f_prof.step_x / 2.0f;
        float half_step_y = f_prof.step_y / 2.0f;

        int n = (int)std::round((map_size_x_ / 2.0f - half_step_x) / f_prof.step_x);
        float f_shift_x = n * f_prof.step_x;

        bool n_is_even = (n % 2 == 0);
        float f_shift_y = n_is_even ? (f_prof.step_y + half_step_y) : half_step_y;

        for (int gx = -150; gx <= 150; ++gx) {
            for (int gy = -150; gy <= 150; ++gy) {
                MapPoint pt = to_iso(GridPoint{gx, gy}, f_prof.step_x, f_prof.step_y, {f_shift_x, f_shift_y});
                float px = pt.x;
                float py = pt.y;

                if (px < b_min_px || px > b_max_px || py < b_min_py || py > b_max_py) continue;

                int grid_x = (px - grid_ctx.min_px) / cell_size;
                int grid_y = (py - grid_ctx.min_py) / cell_size;

                if (grid_x >= 0 && grid_x < grid_ctx.grid_w && grid_y >= 0 && grid_y < grid_ctx.grid_h) {
                    bool is_outside = grid_ctx.outside_grid[grid_y * grid_ctx.grid_w + grid_x];
                    if (!is_outside) {
                        int cell_ft = grid_ctx.floor_type_grid[grid_y * grid_ctx.grid_w + grid_x];
                        if (cell_ft == -1) cell_ft = FLOOR_TYPE_STANDARD;
                        
                        if (cell_ft == ft) {
                            io::Sprite spr;
                            spr.vid = f_prof.vid;
                            spr.posX = px;
                            spr.posY = py;
                            spr.posZ = f_prof.pos_z;
                            spr.direction = 0;
                            spr.army = 0;
                            floor_sprites.push_back(spr);
                        }
                    }
                }
            }
        }
    }
    return floor_sprites;
}

std::vector<io::Sprite> WallBuilder::place_ceilings(const std::vector<Segment>& segments, const PhysicalGridContext& grid_ctx) const {
    std::vector<io::Sprite> ceiling_sprites;
    int cell_size = 5;

    const CeilingProfile& c_prof = get_ceiling_profile(CEILING_TYPE_STANDARD);
    float c_shift_x = std::round(map_size_x_ / 2.0f / c_prof.step_x) * c_prof.step_x + c_prof.shift_offset_x;
    float c_shift_y = std::round(map_size_y_ / 2.0f / c_prof.step_y) * c_prof.step_y + c_prof.shift_offset_y;

    int min_gx = 1e9, max_gx = -1e9, min_gy = 1e9, max_gy = -1e9;
    for (const auto& seg : segments) {
        MapPoint p1 = get_phys(seg.start.x, seg.start.y, seg.wall_type);
        MapPoint p2 = get_phys(seg.end.x, seg.end.y, seg.wall_type);
        
        int gx1 = std::round(((p1.x - c_shift_x) / c_prof.step_x + (p1.y - c_shift_y) / c_prof.step_y) / 2.0f);
        int gy1 = std::round(((p1.y - c_shift_y) / c_prof.step_y - (p1.x - c_shift_x) / c_prof.step_x) / 2.0f);
        
        int gx2 = std::round(((p2.x - c_shift_x) / c_prof.step_x + (p2.y - c_shift_y) / c_prof.step_y) / 2.0f);
        int gy2 = std::round(((p2.y - c_shift_y) / c_prof.step_y - (p2.x - c_shift_x) / c_prof.step_x) / 2.0f);
        
        min_gx = std::min({min_gx, gx1, gx2});
        max_gx = std::max({max_gx, gx1, gx2});
        min_gy = std::min({min_gy, gy1, gy2});
        max_gy = std::max({max_gy, gy1, gy2});
    }
    
    int margin_g = 6;
    min_gx -= margin_g; max_gx += margin_g;
    min_gy -= margin_g; max_gy += margin_g;

    for (int gx = min_gx; gx <= max_gx; ++gx) {
        for (int gy = min_gy; gy <= max_gy; ++gy) {
            MapPoint pt = to_iso(GridPoint{gx, gy}, c_prof.step_x, c_prof.step_y, {c_shift_x, c_shift_y});
            float px = pt.x;
            float py = pt.y;

            int grid_x = (px - grid_ctx.min_px) / cell_size;
            int grid_y = (py - grid_ctx.min_py) / cell_size;

            if (grid_x >= 0 && grid_x < grid_ctx.grid_w && grid_y >= 0 && grid_y < grid_ctx.grid_h) {
                bool is_wall = grid_ctx.physical_grid[grid_y * grid_ctx.grid_w + grid_x];
                bool is_outside = grid_ctx.outside_grid[grid_y * grid_ctx.grid_w + grid_x];
                if (is_outside && !is_wall) {
                    io::Sprite spr;
                    spr.vid = c_prof.vid;
                    spr.posX = px;
                    spr.posY = py;
                    spr.posZ = c_prof.pos_z;
                    spr.direction = 0;
                    spr.army = 0;
                    ceiling_sprites.push_back(spr);
                }
            }
        }
    }
    return ceiling_sprites;
}

std::vector<io::Sprite> WallBuilder::convert_to_wall_sprites(const std::vector<RawSprite>& raw_sprites) const {
    std::vector<io::Sprite> wall_sprites;
    wall_sprites.reserve(raw_sprites.size());

    for (const auto& rs : raw_sprites) {
        const WallProfile& profile = get_wall_profile(rs.wall_type);
        float step_x = profile.step_x;
        float step_y = profile.step_y;

        MapPoint shift = get_wall_shift(map_size_x_, profile);
        MapPoint pos = to_iso(GridPoint{rs.gx, rs.gy}, step_x, step_y, shift);

        if (rs.vid == profile.id_dir_a) {
            pos.x += profile.offset_a_x;
            pos.y += profile.offset_a_y;
        } else if (rs.vid == profile.id_dir_b) {
            pos.x += profile.offset_b_x;
            pos.y += profile.offset_b_y;
        } else if (rs.vid == profile.id_pillar) {
            pos.x += profile.offset_p_x;
            pos.y += profile.offset_p_y;
        }

        io::Sprite spr;
        spr.vid = rs.vid;
        spr.posX = pos.x;
        spr.posY = pos.y;
        spr.posZ = 0.0f;
        spr.direction = 32;
        spr.army = 0;
        wall_sprites.push_back(spr);
    }
    return wall_sprites;
}

std::vector<io::Sprite> WallBuilder::build(const std::vector<Segment>& segments, bool gen_floor, bool gen_ceiling) const {
    if (segments.empty()) return {};

    // 1. wall and  pillar
    std::vector<RawSprite> raw_sprites = process_wall_sprites(segments);

    std::vector<io::Sprite> floor_sprites;
    std::vector<io::Sprite> ceiling_sprites;

    // 2. grid and calculate area
    if (gen_floor || gen_ceiling) {
        PhysicalGridContext grid_ctx = build_physical_grid(segments);
        
        // 3. floor
        if (gen_floor) {
            floor_sprites = place_floors(segments, grid_ctx);
        }
        
        // 4. celling
        if (gen_ceiling) {
            ceiling_sprites = place_ceilings(segments, grid_ctx);
        }
    }

    // 5. project physical coordinates and transform wall sprites
    std::vector<io::Sprite> wall_sprites = convert_to_wall_sprites(raw_sprites);

    // 6. combine all spirit lists
    std::vector<io::Sprite> final_sprites;
    final_sprites.reserve(floor_sprites.size() + wall_sprites.size() + ceiling_sprites.size());
    
    final_sprites.insert(final_sprites.end(), floor_sprites.begin(), floor_sprites.end());
    final_sprites.insert(final_sprites.end(), wall_sprites.begin(), wall_sprites.end());
    final_sprites.insert(final_sprites.end(), ceiling_sprites.begin(), ceiling_sprites.end());

    return final_sprites;
}

} // namespace auto_mapper::core
