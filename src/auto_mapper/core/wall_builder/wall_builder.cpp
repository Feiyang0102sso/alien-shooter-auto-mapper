/**
 * @file wall_builder.cpp
 * @brief Implements core wall generation pipeline.
 *
 * Supports mixed wall types in a single build:
 * segments are grouped by wall_type, each group is rasterized and
 * projected independently using its own WallProfile, then merged.
 */

#include "auto_mapper/core/wall_builder/wall_builder.h"
#include "auto_mapper/core/geometry.h"
#include "auto_mapper/core/randomizer.h"
#include "auto_mapper/common/logger.h"
#include <algorithm>
#include <cmath>
#include <set>
#include <map>
#include <tuple>
#include <utility>
#include <unordered_map>

namespace auto_mapper::core {

WallBuilder::WallBuilder(float map_size_x, float map_size_y)
    : map_size_x_(map_size_x), map_size_y_(map_size_y) {}

const WallProfile& WallBuilder::get_wall_profile(int wall_type) {
    if (wall_type == WALL_TYPE_STANDARD) {
        return WALL_STANDARD;
    }

    if (wall_type == WALL_TYPE_LAB) {
        return WALL_LAB;
    }

    if (wall_type == WALL_TYPE_STANDARD_DARK) {
        return WALL_STANDARD_DARK;
    }

    if (wall_type == WALL_TYPE_AS2_WALL_SET1_FIXED_0) {
        return WALL_AS2_SET1_FIXED_0;
    }

    if (wall_type == WALL_TYPE_AS2_WALL_SET1_FIXED_1) {
        return WALL_AS2_SET1_FIXED_1;
    }

    if (wall_type == WALL_TYPE_AS2_WALL_SET1_RANDOM) {
        return WALL_AS2_SET1_RANDOM;
    }

    if (wall_type == WALL_TYPE_AS2_WALL_SET2_RANDOM) {
        return WALL_AS2_SET2_RANDOM;
    }

    if (wall_type == WALL_TYPE_AS2_WALL_SET3_RANDOM) {
        return WALL_AS2_SET3_RANDOM;
    }

    if (wall_type == WALL_TYPE_AS2_WALL_SET4_RANDOM) {
        return WALL_AS2_SET4_RANDOM;
    }

    if (wall_type == WALL_TYPE_AS2_WALL_SET5_RANDOM) {
        return WALL_AS2_SET5_RANDOM;
    }

    if (wall_type == WALL_TYPE_AS2_WALL_SET6_RANDOM) {
        return WALL_AS2_SET6_RANDOM;
    }

    if (wall_type == WALL_TYPE_AS2_WALL_SET7_RANDOM) {
        return WALL_AS2_SET7_RANDOM;
    }

    if (wall_type == WALL_TYPE_AS2_WALL_SET8_RANDOM) {
        return WALL_AS2_SET8_RANDOM;
    }

    if (wall_type == WALL_TYPE_AS2_WALL_SET9_RANDOM) {
        return WALL_AS2_SET9_RANDOM;
    }

    Logger::warning("Unknown wall_type={}, falling back to STANDARD", wall_type);
    return WALL_STANDARD;
}

const FloorProfile& WallBuilder::get_floor_profile(int floor_type) {
    static const std::unordered_map<int, FloorProfile> profiles = {
        // AS1 floors
        {FLOOR_TYPE_STANDARD, FLOOR_STANDARD},
        {FLOOR_TYPE_LAB,      FLOOR_LAB},
        {FLOOR_TYPE_STANDARD_DARK, FLOOR_STANDARD_DARK},
        // AS2 floors (floor_type directly reuses wall_type numbers,
        // matching the seg.wall_type stored in physical_floor_type)
        {FLOOR_TYPE_AS2_SET1_FIXED_0, FLOOR_AS2_SET1},
        {FLOOR_TYPE_AS2_SET1_FIXED_1, FLOOR_AS2_SET1},
        {FLOOR_TYPE_AS2_SET1_RANDOM,  FLOOR_AS2_SET1},
        {FLOOR_TYPE_AS2_SET2_RANDOM,  FLOOR_AS2_SET2},
        {FLOOR_TYPE_AS2_SET3_RANDOM,  FLOOR_AS2_SET3},
        {FLOOR_TYPE_AS2_SET4_RANDOM,  FLOOR_AS2_SET4},
        {FLOOR_TYPE_AS2_SET5_RANDOM,  FLOOR_AS2_SET5},
        {FLOOR_TYPE_AS2_SET6_RANDOM,  FLOOR_AS2_SET6},
        {FLOOR_TYPE_AS2_SET7_RANDOM,  FLOOR_AS2_SET7},
        {FLOOR_TYPE_AS2_SET8_RANDOM,  FLOOR_AS2_SET8},
        {FLOOR_TYPE_AS2_SET9_RANDOM,  FLOOR_AS2_SET9}
    };
    if (profiles.find(floor_type) != profiles.end()) {
        return profiles.at(floor_type);
    }
    return profiles.at(FLOOR_TYPE_STANDARD);
}

const CeilingProfile& WallBuilder::get_ceiling_profile(int ceiling_type) {
    static const std::unordered_map<int, CeilingProfile> profiles = {
        {CEILING_TYPE_STANDARD, CEILING_STANDARD}
    };
    if (profiles.find(ceiling_type) != profiles.end()) {
        return profiles.at(ceiling_type);
    }
    return profiles.at(CEILING_TYPE_STANDARD);
}

MapPoint WallBuilder::get_wall_shift(float map_size_x, const WallProfile& profile) {
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

static MapPoint get_floor_ceiling_shift(float map_size_x, float step_x, float step_y, int grid_divisor) {
    float divisor = static_cast<float>(grid_divisor);
    float grid_step_x = step_x / divisor;
    float grid_step_y = step_y / divisor;

    float half_step_x = grid_step_x / 2.0f;
    float half_step_y = grid_step_y / 2.0f;

    int n = static_cast<int>(std::round((map_size_x / 2.0f - half_step_x) / grid_step_x));
    float shift_x = n * grid_step_x;

    bool n_is_even = (n % 2 == 0);
    float shift_y = n_is_even ? (grid_step_y + half_step_y) : half_step_y;

    return {shift_x, shift_y};
}

MapPoint WallBuilder::get_phys(int lx, int ly, int w_type) const {
    const WallProfile& profile = get_wall_profile(w_type);
    MapPoint shift = get_wall_shift(map_size_x_, profile);
    return to_iso(GridPoint{lx, ly}, profile.step_x, profile.step_y, shift);
}

std::vector<WallBuilder::RawSprite> WallBuilder::process_wall_sprites(
    const std::vector<Segment>& segments,
    const std::vector<DoorExcavation>& excavations
) const {
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

        std::set<Point> vertices;
        for (const auto& p : edges_a) {
            vertices.insert({p.first, p.second - 1});
            vertices.insert({p.first, p.second});
        }
        for (const auto& p : edges_b) {
            vertices.insert({p.first - 1, p.second});
            vertices.insert({p.first, p.second});
        }

        auto vertex_has_corner_connection = [&](int x, int y) {
            bool up = edges_a.count({x, y}) > 0;
            bool down = edges_a.count({x, y + 1}) > 0;
            bool left = edges_b.count({x, y}) > 0;
            bool right = edges_b.count({x + 1, y}) > 0;

            bool conn_a = up || down;
            bool conn_b = left || right;
            return conn_a && conn_b;
        };

        for (const auto& p : edges_a) {
            bool uses_corner_wall_variant = vertex_has_corner_connection(p.first, p.second - 1) ||
                vertex_has_corner_connection(p.first, p.second);
            raw_sprites.push_back({p.first, p.second, wt, WallPartKind::DirA, uses_corner_wall_variant});
        }
        for (const auto& p : edges_b) {
            bool uses_corner_wall_variant = vertex_has_corner_connection(p.first - 1, p.second) ||
                vertex_has_corner_connection(p.first, p.second);
            raw_sprites.push_back({p.first, p.second, wt, WallPartKind::DirB, uses_corner_wall_variant});
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
                raw_sprites.push_back({x, y, wt, WallPartKind::Pillar, false, up, down, left, right});
            }
        }
    }

    auto get_sort_vid = [](const RawSprite& sprite) {
        const WallProfile& profile = WallBuilder::get_wall_profile(sprite.wall_type);
        if (sprite.kind == WallPartKind::DirA) {
            return profile.dir_a_vid;
        }

        if (sprite.kind == WallPartKind::DirB) {
            return profile.dir_b_vid;
        }

        return profile.pillar_vid;
    };

    std::sort(raw_sprites.begin(), raw_sprites.end(), [&](const RawSprite& a, const RawSprite& b) {
        if (a.gx == b.gx && a.gy == b.gy) {
            return get_sort_vid(a) < get_sort_vid(b);
        }
        return (a.gx + a.gy) < (b.gx + b.gy);
    });
    raw_sprites.erase(std::unique(raw_sprites.begin(), raw_sprites.end(), [](const RawSprite& a, const RawSprite& b) {
        return a.gx == b.gx && a.gy == b.gy && a.wall_type == b.wall_type && a.kind == b.kind;
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

    // AS1 + AS2 floor types (14 total).
    // AS2 floor_type numbers directly match wall_type numbers stored
    // by build_physical_grid() into floor_type_grid.
    std::vector<int> floor_types = {
        // AS1
        FLOOR_TYPE_STANDARD,
        FLOOR_TYPE_LAB,
        FLOOR_TYPE_STANDARD_DARK,
        // AS2 Set1 (3 variants share same floor VID 1783)
        FLOOR_TYPE_AS2_SET1_FIXED_0,
        FLOOR_TYPE_AS2_SET1_FIXED_1,
        FLOOR_TYPE_AS2_SET1_RANDOM,
        // AS2 Set2 ~ Set9
        FLOOR_TYPE_AS2_SET2_RANDOM,
        FLOOR_TYPE_AS2_SET3_RANDOM,
        FLOOR_TYPE_AS2_SET4_RANDOM,
        FLOOR_TYPE_AS2_SET5_RANDOM,
        FLOOR_TYPE_AS2_SET6_RANDOM,
        FLOOR_TYPE_AS2_SET7_RANDOM,
        FLOOR_TYPE_AS2_SET8_RANDOM,
        FLOOR_TYPE_AS2_SET9_RANDOM
    };
    for (int ft : floor_types) {
        const FloorProfile& f_prof = get_floor_profile(ft);
        MapPoint shift = get_floor_ceiling_shift(map_size_x_, f_prof.step_x, f_prof.step_y, f_prof.grid_divisor);
        float sample_offset_x = f_prof.step_x * 0.65f;
        float sample_offset_y = f_prof.step_y * 0.65f;
        std::vector<std::pair<float, float>> sample_offsets = {
            {0.0f, 0.0f},
            {-sample_offset_x, 0.0f},
            {sample_offset_x, 0.0f},
            {0.0f, -sample_offset_y},
            {0.0f, sample_offset_y},
            {-sample_offset_x, -sample_offset_y},
            {sample_offset_x, -sample_offset_y},
            {-sample_offset_x, sample_offset_y},
            {sample_offset_x, sample_offset_y},
        };

        auto sample_matches_floor = [&](float sample_px, float sample_py) {
            int grid_x = (sample_px - grid_ctx.min_px) / cell_size;
            int grid_y = (sample_py - grid_ctx.min_py) / cell_size;

            if (grid_x < 0 || grid_x >= grid_ctx.grid_w) {
                return false;
            }

            if (grid_y < 0 || grid_y >= grid_ctx.grid_h) {
                return false;
            }

            int grid_index = grid_y * grid_ctx.grid_w + grid_x;
            if (grid_ctx.outside_grid[grid_index]) {
                return false;
            }

            int cell_ft = grid_ctx.floor_type_grid[grid_index];
            if (cell_ft == -1) {
                cell_ft = FLOOR_TYPE_STANDARD;
            }

            return cell_ft == ft;
        };

        for (int gx = -150; gx <= 150; ++gx) {
            for (int gy = -150; gy <= 150; ++gy) {
                MapPoint pt = to_iso(GridPoint{gx, gy}, f_prof.step_x, f_prof.step_y, shift);
                float px = pt.x;
                float py = pt.y;

                if (px < b_min_px || px > b_max_px || py < b_min_py || py > b_max_py) continue;

                bool should_place_floor = false;
                for (const auto& sample_offset : sample_offsets) {
                    float sample_px = px + sample_offset.first;
                    float sample_py = py + sample_offset.second;
                    if (sample_matches_floor(sample_px, sample_py)) {
                        should_place_floor = true;
                        break;
                    }
                }

                if (should_place_floor) {
                    io::Sprite floor_sprite = place_single_floor_celling(gx, gy, f_prof.vid, f_prof.step_x, f_prof.step_y, f_prof.pos_z, f_prof.grid_divisor);
                    floor_sprite.gamma = f_prof.gamma;
                    floor_sprites.push_back(floor_sprite);
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
    MapPoint c_shift = get_floor_ceiling_shift(map_size_x_, c_prof.step_x, c_prof.step_y, c_prof.grid_divisor);

    int min_gx = 1e9, max_gx = -1e9, min_gy = 1e9, max_gy = -1e9;
    for (const auto& seg : segments) {
        MapPoint p1 = get_phys(seg.start.x, seg.start.y, seg.wall_type);
        MapPoint p2 = get_phys(seg.end.x, seg.end.y, seg.wall_type);
        
        int gx1 = std::round(((p1.x - c_shift.x) / c_prof.step_x + (p1.y - c_shift.y) / c_prof.step_y) / 2.0f);
        int gy1 = std::round(((p1.y - c_shift.y) / c_prof.step_y - (p1.x - c_shift.x) / c_prof.step_x) / 2.0f);
        
        int gx2 = std::round(((p2.x - c_shift.x) / c_prof.step_x + (p2.y - c_shift.y) / c_prof.step_y) / 2.0f);
        int gy2 = std::round(((p2.y - c_shift.y) / c_prof.step_y - (p2.x - c_shift.x) / c_prof.step_x) / 2.0f);
        
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
            MapPoint pt = to_iso(GridPoint{gx, gy}, c_prof.step_x, c_prof.step_y, c_shift);
            float px = pt.x;
            float py = pt.y;

            int grid_x = (px - grid_ctx.min_px) / cell_size;
            int grid_y = (py - grid_ctx.min_py) / cell_size;

            if (grid_x >= 0 && grid_x < grid_ctx.grid_w && grid_y >= 0 && grid_y < grid_ctx.grid_h) {
                bool is_wall = grid_ctx.physical_grid[grid_y * grid_ctx.grid_w + grid_x];
                bool is_outside = grid_ctx.outside_grid[grid_y * grid_ctx.grid_w + grid_x];
                if (is_outside && !is_wall) {
                    ceiling_sprites.push_back(place_single_floor_celling(gx, gy, c_prof.vid, c_prof.step_x, c_prof.step_y, c_prof.pos_z, c_prof.grid_divisor));
                }
            }
        }
    }
    return ceiling_sprites;
}

std::vector<io::Sprite> WallBuilder::convert_to_wall_sprites(const std::vector<RawSprite>& raw_sprites) const {
    std::vector<io::Sprite> wall_sprites;
    wall_sprites.reserve(raw_sprites.size());
    std::map<std::tuple<int, int, int>, int> selected_variant_indices;
    std::map<std::pair<int, WallPartKind>, int> rare_variant_remaining_by_wall_part;

    for (const auto& rs : raw_sprites) {
        const WallProfile& profile = get_wall_profile(rs.wall_type);
        if (rs.kind == WallPartKind::Pillar && profile.pillar_mode == WallPillarMode::DirectionalSlices) {
            std::vector<io::Sprite> pillar_sprites = place_pillar_slices(rs);
            wall_sprites.insert(wall_sprites.end(), pillar_sprites.begin(), pillar_sprites.end());
            continue;
        }

        if (rs.kind == WallPartKind::Pillar && profile.pillar_vid <= 0) {
            continue;
        }

        if (rs.kind == WallPartKind::Pillar && profile.use_corner_pillar_assets) {
            const WallPartAsset* pillar_asset = select_corner_pillar_asset(profile, rs);
            if (pillar_asset != nullptr) {
                wall_sprites.push_back(place_wall_part_asset(rs.gx, rs.gy, rs.wall_type, *pillar_asset));
                continue;
            }

            if (profile.skip_unmapped_pillars) {
                continue;
            }
        }

        int variant_index = select_wall_variant_index(profile);
        bool is_wall_part = rs.kind == WallPartKind::DirA || rs.kind == WallPartKind::DirB;
        if (is_wall_part && has_rare_wall_variant(profile)) {
            std::pair<int, WallPartKind> rare_variant_key = {rs.wall_type, rs.kind};
            if (rare_variant_remaining_by_wall_part.find(rare_variant_key) == rare_variant_remaining_by_wall_part.end()) {
                rare_variant_remaining_by_wall_part[rare_variant_key] = reset_rare_wall_variant_interval(profile);
            }

            int rare_variant_remaining = rare_variant_remaining_by_wall_part[rare_variant_key];
            if (rare_variant_remaining <= 0) {
                variant_index = profile.rare_variant_index;
                rare_variant_remaining_by_wall_part[rare_variant_key] = reset_rare_wall_variant_interval(profile);
            } else {
                rare_variant_remaining_by_wall_part[rare_variant_key] = rare_variant_remaining - 1;
            }
        }

        if (!profile.randomize_wall_parts_independently) {
            std::tuple<int, int, int> variant_key = {rs.wall_type, rs.gx, rs.gy};
            if (selected_variant_indices.find(variant_key) == selected_variant_indices.end()) {
                selected_variant_indices[variant_key] = variant_index;
            }
            variant_index = selected_variant_indices[variant_key];
        }

        if (is_wall_part && rs.uses_corner_wall_variant) {
            if (profile.corner_wall_variant_index >= 0 && profile.corner_wall_variant_index < profile.variant_count) {
                variant_index = profile.corner_wall_variant_index;
            }
        }

        wall_sprites.push_back(place_single_wall_with_variant(rs.gx, rs.gy, rs.wall_type, rs.kind, variant_index));
    }
    return wall_sprites;
}

std::vector<io::Sprite> WallBuilder::build(
    const std::vector<Segment>& segments, 
    bool gen_floor, 
    bool gen_ceiling,
    const std::vector<DoorExcavation>& excavations
) const {
    if (segments.empty()) return {};

    // 1. wall and  pillar (generate fully first)
    std::vector<RawSprite> raw_sprites = process_wall_sprites(segments, {});

    // 1b. Perform physical excavations on raw_sprites
    if (!excavations.empty()) {
        std::vector<RawSprite> filtered_raw;
        filtered_raw.reserve(raw_sprites.size());

        for (const auto& rs : raw_sprites) {
            bool to_erase = false;

            for (const auto& ex : excavations) {
                if (ex.wall_type != rs.wall_type) {
                    continue;
                }

                const WallProfile& ex_profile = get_wall_profile(ex.wall_type);
                int flank = ex_profile.door_flank_clear;

                if (ex.direction_type == 0) {  // A direction (vertical, along y axis)
                    // Erase wall segment in range [ex.pos.y, ex.pos.y + ex.size - 1]
                    // Extended by flank on each side for DirA segments only.
                    if (rs.kind == WallPartKind::DirA) {
                        int min_gy = ex.pos.y - flank;
                        int max_gy = ex.pos.y + ex.size - 1 + flank;
                        if (rs.gx == ex.pos.x && rs.gy >= min_gy && rs.gy <= max_gy) {
                            to_erase = true;
                            break;
                        }
                    }
                } else if (ex.direction_type == 1) {  // B direction (horizontal, along x axis)
                    // Erase wall segment in range [ex.pos.x, ex.pos.x + ex.size - 1]
                    // Extended by flank on each side for DirB segments only.
                    if (rs.kind == WallPartKind::DirB) {
                        int min_gx = ex.pos.x - flank;
                        int max_gx = ex.pos.x + ex.size - 1 + flank;
                        if (rs.gy == ex.pos.y && rs.gx >= min_gx && rs.gx <= max_gx) {
                            to_erase = true;
                            break;
                        }
                    }
                }
            }

            if (!to_erase) {
                filtered_raw.push_back(rs);
            }
        }
        raw_sprites = std::move(filtered_raw);
    }

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

io::Sprite WallBuilder::place_single_floor_celling(int gx, int gy, int vid, float step_x, float step_y, float pos_z, int grid_divisor) const {
    MapPoint shift = get_floor_ceiling_shift(map_size_x_, step_x, step_y, grid_divisor);
    MapPoint pt = to_iso(GridPoint{gx, gy}, step_x, step_y, shift);
    return io::Sprite(vid, pt.x, pt.y, pos_z, 0);
}

int WallBuilder::select_wall_variant_index(const WallProfile& profile) {
    if (profile.variant_count <= 0) {
        return 0;
    }

    if (profile.variant_pool_count <= 0) {
        return 0;
    }

    int pool_index = profile.active_variant_pool;
    if (pool_index < 0 || pool_index >= profile.variant_pool_count) {
        pool_index = 0;
    }

    const WallVariantPool& pool = profile.variant_pools[pool_index];
    if (pool.variant_count <= 0) {
        return 0;
    }

    int selected_pool_item = 0;
    if (pool.randomize) {
        selected_pool_item = Random::get(0, pool.variant_count - 1);
    }

    int variant_index = pool.variant_indices[selected_pool_item];
    if (variant_index < 0 || variant_index >= profile.variant_count) {
        return 0;
    }

    return variant_index;
}

const WallVariant& WallBuilder::select_wall_variant(const WallProfile& profile) {
    int variant_index = select_wall_variant_index(profile);
    return profile.variants[variant_index];
}

bool WallBuilder::has_rare_wall_variant(const WallProfile& profile) {
    if (profile.rare_variant_index < 0) {
        return false;
    }

    if (profile.rare_variant_index >= profile.variant_count) {
        return false;
    }

    if (profile.rare_variant_min_interval <= 0) {
        return false;
    }

    if (profile.rare_variant_max_interval < profile.rare_variant_min_interval) {
        return false;
    }

    return true;
}

int WallBuilder::reset_rare_wall_variant_interval(const WallProfile& profile) {
    return Random::get(profile.rare_variant_min_interval, profile.rare_variant_max_interval);
}

const WallPartAsset& WallBuilder::select_wall_part_asset(const WallVariant& variant, WallPartKind kind) {
    if (kind == WallPartKind::DirA) {
        return variant.dir_a;
    }

    if (kind == WallPartKind::DirB) {
        return variant.dir_b;
    }

    return variant.pillar;
}

const WallPartAsset* WallBuilder::select_corner_pillar_asset(const WallProfile& profile, const RawSprite& raw_sprite) {
    int connection_count = 0;
    if (raw_sprite.connects_up) {
        connection_count += 1;
    }
    if (raw_sprite.connects_down) {
        connection_count += 1;
    }
    if (raw_sprite.connects_left) {
        connection_count += 1;
    }
    if (raw_sprite.connects_right) {
        connection_count += 1;
    }

    if (connection_count != 2) {
        return nullptr;
    }

    if (raw_sprite.connects_down && raw_sprite.connects_right) {
        if (profile.pillar_corner_down_right.vid > 0) {
            return &profile.pillar_corner_down_right;
        }
        return nullptr;
    }

    if (raw_sprite.connects_down && raw_sprite.connects_left) {
        if (profile.pillar_corner_down_left.vid > 0) {
            return &profile.pillar_corner_down_left;
        }
        return nullptr;
    }

    if (raw_sprite.connects_up && raw_sprite.connects_left) {
        if (profile.pillar_corner_up_left.vid > 0) {
            return &profile.pillar_corner_up_left;
        }
        return nullptr;
    }

    if (raw_sprite.connects_up && raw_sprite.connects_right) {
        if (profile.pillar_corner_up_right.vid > 0) {
            return &profile.pillar_corner_up_right;
        }
        return nullptr;
    }

    return nullptr;
}

io::Sprite WallBuilder::place_single_wall(int gx, int gy, int wall_type, WallPartKind kind) const {
    const WallProfile& profile = get_wall_profile(wall_type);
    int variant_index = select_wall_variant_index(profile);
    return place_single_wall_with_variant(gx, gy, wall_type, kind, variant_index);
}

io::Sprite WallBuilder::place_single_wall_with_variant(int gx, int gy, int wall_type, WallPartKind kind, int variant_index) const {
    const WallProfile& profile = get_wall_profile(wall_type);
    if (variant_index < 0 || variant_index >= profile.variant_count) {
        variant_index = 0;
    }

    const WallVariant& variant = profile.variants[variant_index];
    const WallPartAsset& asset = select_wall_part_asset(variant, kind);

    return place_wall_part_asset(gx, gy, wall_type, asset);
}

io::Sprite WallBuilder::place_wall_part_asset(int gx, int gy, int wall_type, const WallPartAsset& asset) const {
    const WallProfile& profile = get_wall_profile(wall_type);
    MapPoint shift = get_wall_shift(map_size_x_, profile);
    MapPoint pos = to_iso(GridPoint{gx, gy}, profile.step_x, profile.step_y, shift);

    pos.x += asset.offset_x;
    pos.y += asset.offset_y;

    return io::Sprite(asset.vid, pos.x, pos.y, 0.0f, asset.direction);
}

std::vector<io::Sprite> WallBuilder::place_pillar_slices(const RawSprite& raw_sprite) const {
    const WallProfile& profile = get_wall_profile(raw_sprite.wall_type);
    std::vector<io::Sprite> sprites;

    if (raw_sprite.connects_up) {
        sprites.push_back(place_wall_part_asset(
            raw_sprite.gx,
            raw_sprite.gy,
            raw_sprite.wall_type,
            profile.pillar_slice_up
        ));
    }

    if (raw_sprite.connects_down) {
        sprites.push_back(place_wall_part_asset(
            raw_sprite.gx,
            raw_sprite.gy,
            raw_sprite.wall_type,
            profile.pillar_slice_down
        ));
    }

    if (raw_sprite.connects_left) {
        sprites.push_back(place_wall_part_asset(
            raw_sprite.gx,
            raw_sprite.gy,
            raw_sprite.wall_type,
            profile.pillar_slice_left
        ));
    }

    if (raw_sprite.connects_right) {
        sprites.push_back(place_wall_part_asset(
            raw_sprite.gx,
            raw_sprite.gy,
            raw_sprite.wall_type,
            profile.pillar_slice_right
        ));
    }

    return sprites;
}

} // namespace auto_mapper::core
