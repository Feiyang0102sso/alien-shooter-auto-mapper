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

namespace auto_mapper::core {

WallBuilder::WallBuilder(int grid_size, float map_size_x, float map_size_y)
    : grid_size_(grid_size), map_size_x_(map_size_x), map_size_y_(map_size_y) {}

const WallProfile& WallBuilder::get_profile(int wall_type) {
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

std::vector<io::Sprite> WallBuilder::build(const std::vector<Segment>& segments) const {
    using Point = std::pair<int, int>;

    // ── 1. Group segments by wall_type ──
    std::map<int, std::vector<const Segment*>> groups;
    for (const auto& seg : segments) {
        groups[seg.wall_type].push_back(&seg);
    }

    // Intermediate sprite with grid coords + wall_type (for correct physical projection)
    struct RawSprite {
        int vid;
        int gx;
        int gy;
        int wall_type;
    };
    std::vector<RawSprite> raw_sprites;

    // ── 2. Process each wall_type group independently ──
    for (const auto& [wt, group_segs] : groups) {
        const WallProfile& profile = get_profile(wt);

        std::set<Point> edges_a; // Direction A (/) edges
        std::set<Point> edges_b; // Direction B (\) edges

        // 2a. Rasterize line segments into grid edges
        for (const auto* seg : group_segs) {
            int x1 = seg->start.x;
            int y1 = seg->start.y;
            int x2 = seg->end.x;
            int y2 = seg->end.y;

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

        // 2b. Assign wall sprites using profile IDs
        for (const auto& p : edges_a) {
            raw_sprites.push_back({profile.id_dir_a, p.first, p.second, wt});
        }
        for (const auto& p : edges_b) {
            raw_sprites.push_back({profile.id_dir_b, p.first, p.second, wt});
        }

        // 2c. Extract topological vertices and place pillars
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

            // Place pillar at endpoints or intersections
            if (total_conns == 1 || (conn_a && conn_b)) {
                raw_sprites.push_back({profile.id_pillar, x, y, wt});
            }
        }
    }

    // ── 3. Z-Order depth sorting (draw back-to-front) ──
    // Sort by grid depth (gx + gy), using wall_type step_y as tiebreaker
    std::sort(raw_sprites.begin(), raw_sprites.end(), [](const RawSprite& a, const RawSprite& b) {
        // Compute approximate physical depth for correct cross-type ordering
        const auto& pa = get_profile(a.wall_type);
        const auto& pb = get_profile(b.wall_type);
        float depth_a = (a.gx + a.gy) * pa.step_y;
        float depth_b = (b.gx + b.gy) * pb.step_y;
        return depth_a < depth_b;
    });

    // ── 4. Generate final io::Sprite list ──
    // Each wall_type computes its own shift (origin offset) independently
    std::vector<io::Sprite> final_sprites;
    for (const auto& rs : raw_sprites) {
        const WallProfile& profile = get_profile(rs.wall_type);
        float step_x = profile.step_x;
        float step_y = profile.step_y;

        // Compute shift for this wall_type (center horizontally, pad top)
        float half_step_x = step_x / 2.0f;
        float half_step_y = step_y / 2.0f;

        MapPoint shift;
        float raw_shift_x = map_size_x_ / 2.0f;
        float grid_x = std::round((raw_shift_x - half_step_x) / step_x);
        shift.x = grid_x * step_x + half_step_x;

        float raw_shift_y = half_step_y;
        float grid_y = std::round((raw_shift_y - half_step_y) / step_y);
        shift.y = grid_y * step_y + half_step_y;
        shift.y += step_y; // top padding

        MapPoint pos = to_iso({rs.gx, rs.gy}, step_x, step_y, shift);

        // Apply anchor offset for '\' direction walls
        if (rs.vid == profile.id_dir_b) {
            pos.x += profile.offset_x;
        }

        io::Sprite spr;
        spr.vid = rs.vid;
        spr.posX = pos.x;
        spr.posY = pos.y;
        spr.posZ = 0.0f;
        spr.direction = 32;
        spr.army = 0;

        final_sprites.push_back(spr);
    }

    return final_sprites;
}

} // namespace auto_mapper::core
