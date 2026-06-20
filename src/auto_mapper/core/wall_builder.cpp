/**
 * @file wall_builder.cpp
 * @brief Implements core wall generation pipeline:
 */

#include "auto_mapper/core/wall_builder.h"
#include <set>
#include <algorithm>
#include <utility>

namespace auto_mapper::core {

WallBuilder::WallBuilder(float map_size_x, float map_size_y)
    : map_size_x_(map_size_x), map_size_y_(map_size_y) {}

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
            // Parallel to Y axis, Direction A
            int minY = std::min(y1, y2);
            int maxY = std::max(y1, y2);
            for (int y = minY + 1; y <= maxY; ++y) {
                edges_a.insert({x1, y});
            }
        } else if (y1 == y2) {
            // Parallel to X axis, Direction B
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

    // 2. Assign basic wall sprites
    for (const auto& p : edges_a) {
        // TODO: Support variant walls, currently fixed to 601
        raw_sprites.push_back({601, p.first, p.second});
    }

    for (const auto& p : edges_b) {
        // TODO: Support variant walls, currently fixed to 602
        raw_sprites.push_back({602, p.first, p.second});
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
            raw_sprites.push_back({604, x, y});
        }
    }

    // 4. Z-Order depth sorting (draw back-to-front to avoid overlapping issues)
    std::sort(raw_sprites.begin(), raw_sprites.end(), [](const RawSprite& a, const RawSprite& b) {
        return (a.gx + a.gy) < (b.gx + b.gy);
    });

    // 5. Calculate global center shift using bounding box
    BoundingBox bbox;
    for (const auto& rs : raw_sprites) {
        MapPoint pos = to_iso({rs.gx, rs.gy}, {0.0f, 0.0f});
        if (rs.vid == 602) {
            // Specific visual correction for wall 602 (Direction B)
            pos.x -= 40.0f;
        }
        bbox.expand(pos);
    }
    MapPoint shift = calculate_shift(bbox, map_size_x_, map_size_y_);

    // 6. Generate final io::Sprite list
    std::vector<io::Sprite> final_sprites;
    for (const auto& rs : raw_sprites) {
        MapPoint pos = to_iso({rs.gx, rs.gy}, shift);
        if (rs.vid == 602) {
            pos.x -= 40.0f;
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
