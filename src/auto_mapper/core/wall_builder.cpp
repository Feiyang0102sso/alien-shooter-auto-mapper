/**
 * @file wall_builder.cpp
 * @brief Implements core wall generation pipeline.
 *
 * All wall IDs, step sizes and anchor offsets are sourced from
 * the WallProfile passed at construction time — no hardcoded magic numbers.
 */

#include "auto_mapper/core/wall_builder.h"
#include "auto_mapper/core/geometry.h"
#include "auto_mapper/common/logger.h"
#include <algorithm>
#include <cmath>
#include <set>
#include <map>
#include <utility>

namespace auto_mapper::core {

WallBuilder::WallBuilder(const WallProfile& profile, int grid_size, float map_size_x, float map_size_y)
    : profile_(profile), grid_size_(grid_size), map_size_x_(map_size_x), map_size_y_(map_size_y) {}

std::vector<io::Sprite> WallBuilder::build(const std::vector<Segment>& segments) const {
    using Point = std::pair<int, int>;
    std::set<Point> edges_a; // Direction A (/) edges. Point(x,y) means edge from (x, y-1) to (x, y).
    std::set<Point> edges_b; // Direction B (\) edges. Point(x,y) means edge from (x-1, y) to (x, y).

    // 1. Rasterize line segments into basic grid edges
    for (const auto& seg : segments) {
        int x1 = seg.start.x;
        int y1 = seg.start.y;
        int x2 = seg.end.x;
        int y2 = seg.end.y;

        if (x1 == x2) {
            // Parallel to Y axis -> Direction A
            int minY = std::min(y1, y2);
            int maxY = std::max(y1, y2);
            for (int y = minY + 1; y <= maxY; ++y) {
                edges_a.insert({x1, y});
            }
        } else if (y1 == y2) {
            // Parallel to X axis -> Direction B
            int minX = std::min(x1, x2);
            int maxX = std::max(x1, x2);
            for (int x = minX + 1; x <= maxX; ++x) {
                edges_b.insert({x, y1});
            }
        }
    }

    // Collect raw sprites with grid coordinates
    struct RawSprite {
        int vid;
        int gx;
        int gy;
    };
    std::vector<RawSprite> raw_sprites;

    // 2. Assign wall sprites using profile IDs
    for (const auto& p : edges_a) {
        raw_sprites.push_back({profile_.id_dir_a, p.first, p.second});
    }

    for (const auto& p : edges_b) {
        raw_sprites.push_back({profile_.id_dir_b, p.first, p.second});
    }

    // 3. Extract topological vertices and place pillars
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

        // Detect connectivity in 4 directions
        bool up    = edges_a.count({x, y}) > 0;
        bool down  = edges_a.count({x, y + 1}) > 0;
        bool left  = edges_b.count({x, y}) > 0;
        bool right = edges_b.count({x + 1, y}) > 0;

        bool conn_a = up || down;
        bool conn_b = left || right;

        int total_conns = (up ? 1 : 0) + (down ? 1 : 0) + (left ? 1 : 0) + (right ? 1 : 0);

        // Place a pillar if it's an isolated endpoint, or an intersection (corner, T-junction, cross).
        if (total_conns == 1 || (conn_a && conn_b)) {
            raw_sprites.push_back({profile_.id_pillar, x, y});
        }
    }

    // 4. Z-Order depth sorting (draw back-to-front to avoid overlapping issues)
    std::sort(raw_sprites.begin(), raw_sprites.end(), [](const RawSprite& a, const RawSprite& b) {
        return (a.gx + a.gy) < (b.gx + b.gy);
    });

    // 5. Calculate global center shift using profile step sizes
    float step_x = profile_.step_x;
    float step_y = profile_.step_y;

    // half_step: anchor point within a single wall unit
    float half_step_x = step_x / 2.0f;
    float half_step_y = step_y / 2.0f;

    // Origin Shift for grid (0,0): center horizontally, align to step grid
    MapPoint shift;
    float raw_shift_x = map_size_x_ / 2.0f;
    float grid_x = std::round((raw_shift_x - half_step_x) / step_x);
    shift.x = grid_x * step_x + half_step_x;

    float raw_shift_y = half_step_y;
    float grid_y = std::round((raw_shift_y - half_step_y) / step_y);
    shift.y = grid_y * step_y + half_step_y;

    // Padding for top (one full step)
    shift.y += step_y;

    // 6. Generate final io::Sprite list
    std::vector<io::Sprite> final_sprites;
    for (const auto& rs : raw_sprites) {
        MapPoint pos = to_iso({rs.gx, rs.gy}, step_x, step_y, shift);

        // Apply anchor offset for '\' direction walls
        if (rs.vid == profile_.id_dir_b) {
            pos.x += profile_.offset_x;
        }

        io::Sprite spr;
        spr.vid = rs.vid;
        spr.posX = pos.x;
        spr.posY = pos.y;
        spr.posZ = 0.0f;
        spr.direction = 32; // Default direction
        spr.army = 0;       // Neutral object

        final_sprites.push_back(spr);
    }

    return final_sprites;
}

} // namespace auto_mapper::core
