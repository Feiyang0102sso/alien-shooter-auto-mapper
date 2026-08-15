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
#include "auto_mapper/core/vid_properties/vid_armies_as2.h"
#include "auto_mapper/common/logger.h"
#include <algorithm>
#include <cmath>
#include <set>
#include <map>
#include <tuple>
#include <utility>
#include <unordered_map>

namespace auto_mapper::core {

namespace {

constexpr float GEOMETRY_EPSILON = 0.001f;
constexpr float CEILING_POSITION_KEY_SCALE = 1000.0f;

struct LabCeilingSeed {
    io::Sprite sprite;
    WallPartKind kind;
    WallOutsideSide outside_side;
};

struct PhysicalWallBoundary {
    MapPoint start;
    MapPoint end;
};

using CeilingPositionKey = std::pair<long long, long long>;
using CeilingPoint = std::pair<int, int>;

CeilingPositionKey make_ceiling_position_key(float pos_x, float pos_y) {
    long long scaled_x = std::llround(pos_x * CEILING_POSITION_KEY_SCALE);
    long long scaled_y = std::llround(pos_y * CEILING_POSITION_KEY_SCALE);
    return {scaled_x, scaled_y};
}

MapPoint get_lab_ceiling_outward_step(
    WallPartKind kind,
    WallOutsideSide outside_side,
    const AS1CeilingProfile& profile
) {
    float direction_sign = 1.0f;
    if (outside_side == WallOutsideSide::NegativeGridSide) {
        direction_sign = -1.0f;
    }

    if (kind == WallPartKind::DirA) {
        return {
            profile.step_x * direction_sign,
            profile.step_y * direction_sign
        };
    }

    return {
        -profile.step_x * direction_sign,
        profile.step_y * direction_sign
    };
}


float cross_product(MapPoint first, MapPoint second, MapPoint third) {
    float first_x = second.x - first.x;
    float first_y = second.y - first.y;
    float second_x = third.x - first.x;
    float second_y = third.y - first.y;
    return first_x * second_y - first_y * second_x;
}

bool point_is_on_segment(MapPoint point, MapPoint start, MapPoint end) {
    float min_x = std::min(start.x, end.x) - GEOMETRY_EPSILON;
    float max_x = std::max(start.x, end.x) + GEOMETRY_EPSILON;
    float min_y = std::min(start.y, end.y) - GEOMETRY_EPSILON;
    float max_y = std::max(start.y, end.y) + GEOMETRY_EPSILON;
    return point.x >= min_x && point.x <= max_x &&
        point.y >= min_y && point.y <= max_y;
}

bool line_segments_intersect(
    MapPoint first_start,
    MapPoint first_end,
    MapPoint second_start,
    MapPoint second_end
) {
    float first_side_start = cross_product(first_start, first_end, second_start);
    float first_side_end = cross_product(first_start, first_end, second_end);
    float second_side_start = cross_product(second_start, second_end, first_start);
    float second_side_end = cross_product(second_start, second_end, first_end);

    bool first_segment_straddles =
        (first_side_start > GEOMETRY_EPSILON && first_side_end < -GEOMETRY_EPSILON) ||
        (first_side_start < -GEOMETRY_EPSILON && first_side_end > GEOMETRY_EPSILON);
    bool second_segment_straddles =
        (second_side_start > GEOMETRY_EPSILON && second_side_end < -GEOMETRY_EPSILON) ||
        (second_side_start < -GEOMETRY_EPSILON && second_side_end > GEOMETRY_EPSILON);
    if (first_segment_straddles && second_segment_straddles) {
        return true;
    }

    if (std::abs(first_side_start) <= GEOMETRY_EPSILON &&
        point_is_on_segment(second_start, first_start, first_end)) {
        return true;
    }
    if (std::abs(first_side_end) <= GEOMETRY_EPSILON &&
        point_is_on_segment(second_end, first_start, first_end)) {
        return true;
    }
    if (std::abs(second_side_start) <= GEOMETRY_EPSILON &&
        point_is_on_segment(first_start, second_start, second_end)) {
        return true;
    }
    if (std::abs(second_side_end) <= GEOMETRY_EPSILON &&
        point_is_on_segment(first_end, second_start, second_end)) {
        return true;
    }

    return false;
}

MapPoint to_ceiling_grid_delta(
    MapPoint point,
    MapPoint tile_center,
    const AS1CeilingProfile& profile
) {
    float delta_x = point.x - tile_center.x;
    float delta_y = point.y - tile_center.y;
    float normalized_x = delta_x / profile.step_x;
    float normalized_y = delta_y / profile.step_y;
    return {
        (normalized_x + normalized_y) / 2.0f,
        (normalized_y - normalized_x) / 2.0f
    };
}

bool segment_enters_ceiling_footprint(
    const PhysicalWallBoundary& wall,
    MapPoint tile_center,
    const AS1CeilingProfile& profile
) {
    MapPoint start = to_ceiling_grid_delta(wall.start, tile_center, profile);
    MapPoint end = to_ceiling_grid_delta(wall.end, tile_center, profile);

    // A VID 504 tile occupies one isometric cell. Shrinking the open cell by a
    // tiny epsilon permits edge-to-edge contact without accepting overlap.
    float min_bound = -0.5f + GEOMETRY_EPSILON;
    float max_bound = 0.5f - GEOMETRY_EPSILON;
    float t_min = 0.0f;
    float t_max = 1.0f;

    auto clip_axis = [&](float axis_start, float axis_end) {
        float axis_delta = axis_end - axis_start;
        if (std::abs(axis_delta) <= GEOMETRY_EPSILON) {
            return axis_start >= min_bound && axis_start <= max_bound;
        }

        float first_t = (min_bound - axis_start) / axis_delta;
        float second_t = (max_bound - axis_start) / axis_delta;
        if (first_t > second_t) {
            std::swap(first_t, second_t);
        }

        t_min = std::max(t_min, first_t);
        t_max = std::min(t_max, second_t);
        return t_min <= t_max;
    };

    if (!clip_axis(start.x, end.x)) {
        return false;
    }
    return clip_axis(start.y, end.y);
}

std::vector<PhysicalWallBoundary> build_physical_wall_boundaries(
    const std::vector<Segment>& segments,
    float map_size_x
) {
    std::vector<PhysicalWallBoundary> boundaries;
    boundaries.reserve(segments.size());

    for (const Segment& segment : segments) {
        const WallProfile& profile = WallBuilder::get_wall_profile(segment.wall_type);
        MapPoint shift = WallBuilder::get_wall_shift(map_size_x, profile);
        MapPoint start = to_iso(segment.start, profile.step_x, profile.step_y, shift);
        MapPoint end = to_iso(segment.end, profile.step_x, profile.step_y, shift);

        if (segment.start.x == segment.end.x) {
            start.x += profile.offset_a_x;
            start.y += profile.offset_a_y;
            end.x += profile.offset_a_x;
            end.y += profile.offset_a_y;
        } else if (segment.start.y == segment.end.y) {
            start.x += profile.offset_b_x;
            start.y += profile.offset_b_y;
            end.x += profile.offset_b_x;
            end.y += profile.offset_b_y;
        } else {
            continue;
        }

        boundaries.push_back({start, end});
    }

    return boundaries;
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

static bool is_within_lab_ceiling_bounds(
    float x,
    float y,
    float map_size_x,
    float map_size_y,
    const AS1CeilingProfile& profile
) {
    if (x < profile.min_bounds_margin || y < profile.min_bounds_margin) {
        return false;
    }
    if (x > map_size_x + profile.max_bounds_margin || y > map_size_y + profile.max_bounds_margin) {
        return false;
    }
    return true;
}

static GridPoint snap_physical_to_lab_ceiling_grid(
    const MapPoint& pos,
    const MapPoint& ceil_shift,
    const AS1CeilingProfile& profile
) {
    float dx = pos.x - ceil_shift.x;
    float dy = pos.y - ceil_shift.y;
    float gx = (dx / profile.step_x + dy / profile.step_y) / 2.0f;
    float gy = (dy / profile.step_y - dx / profile.step_x) / 2.0f;
    return {
        static_cast<int>(std::round(gx)),
        static_cast<int>(std::round(gy))
    };
}

static void fill_lab_ceiling_grid_holes(
    const std::vector<PhysicalWallBoundary>& boundaries,
    float map_size_x,
    float map_size_y,
    const WallProfile& wall_profile,
    MapPoint wall_shift,
    MapPoint ceil_shift,
    const AS1CeilingProfile& profile,
    const std::set<CeilingPoint>& outside_cells,
    const io::Sprite& template_sprite,
    std::set<std::pair<int, int>>& occupied_grid_cells,
    std::set<CeilingPositionKey>& occupied_physical_positions,
    std::vector<io::Sprite>& ceiling_sprites
) {
    constexpr int MAX_HOLE_FILL_PASSES = 2;
    const std::pair<int, int> neighbor_offsets[4] = {
        { 1,  0},
        {-1,  0},
        { 0,  1},
        { 0, -1}
    };

    for (int pass = 0; pass < MAX_HOLE_FILL_PASSES; ++pass) {
        std::set<std::pair<int, int>> candidate_holes;

        for (const auto& cell : occupied_grid_cells) {
            for (const auto& offset : neighbor_offsets) {
                std::pair<int, int> neighbor = {cell.first + offset.first, cell.second + offset.second};
                if (occupied_grid_cells.count(neighbor) == 0) {
                    candidate_holes.insert(neighbor);
                }
            }
        }

        std::vector<std::pair<int, int>> holes_to_fill;
        for (const auto& hole : candidate_holes) {
            int occupied_neighbor_count = 0;
            bool has_opposite_pair = false;

            bool right_occupied = (occupied_grid_cells.count({hole.first + 1, hole.second}) > 0);
            bool left_occupied  = (occupied_grid_cells.count({hole.first - 1, hole.second}) > 0);
            bool up_occupied    = (occupied_grid_cells.count({hole.first, hole.second + 1}) > 0);
            bool down_occupied  = (occupied_grid_cells.count({hole.first, hole.second - 1}) > 0);

            if (right_occupied) ++occupied_neighbor_count;
            if (left_occupied)  ++occupied_neighbor_count;
            if (up_occupied)    ++occupied_neighbor_count;
            if (down_occupied)  ++occupied_neighbor_count;

            if ((right_occupied && left_occupied) || (up_occupied && down_occupied)) {
                has_opposite_pair = true;
            }

            if (occupied_neighbor_count < 3 && !has_opposite_pair) {
                continue;
            }

            GridPoint grid_pt = {hole.first, hole.second};
            MapPoint candidate_center = to_iso(grid_pt, profile.step_x, profile.step_y, ceil_shift);

            if (!is_within_lab_ceiling_bounds(candidate_center.x, candidate_center.y, map_size_x, map_size_y, profile)) {
                continue;
            }

            float delta_x = candidate_center.x - wall_shift.x;
            float delta_y = candidate_center.y - wall_shift.y;
            float gx = (delta_x / wall_profile.step_x + delta_y / wall_profile.step_y) / 2.0f;
            float gy = (delta_y / wall_profile.step_y - delta_x / wall_profile.step_x) / 2.0f;
            int cell_x = static_cast<int>(std::floor(gx));
            int cell_y = static_cast<int>(std::floor(gy));
            if (outside_cells.count({cell_x, cell_y}) == 0) {
                continue;
            }

            bool hits_wall = false;
            for (const PhysicalWallBoundary& boundary : boundaries) {
                for (const auto& offset : neighbor_offsets) {
                    std::pair<int, int> neighbor_cell = {hole.first + offset.first, hole.second + offset.second};
                    if (occupied_grid_cells.count(neighbor_cell) > 0) {
                        GridPoint n_grid = {neighbor_cell.first, neighbor_cell.second};
                        MapPoint n_center = to_iso(n_grid, profile.step_x, profile.step_y, ceil_shift);
                        if (line_segments_intersect(candidate_center, n_center, boundary.start, boundary.end)) {
                            hits_wall = true;
                            break;
                        }
                    }
                }
                if (hits_wall) {
                    break;
                }
            }

            if (!hits_wall) {
                holes_to_fill.push_back(hole);
            }
        }

        if (holes_to_fill.empty()) {
            break;
        }

        for (const auto& hole : holes_to_fill) {
            occupied_grid_cells.insert(hole);
            GridPoint grid_pt = {hole.first, hole.second};
            MapPoint candidate_center = to_iso(grid_pt, profile.step_x, profile.step_y, ceil_shift);
            occupied_physical_positions.insert(make_ceiling_position_key(candidate_center.x, candidate_center.y));

            io::Sprite layer_sprite = template_sprite;
            layer_sprite.posX = candidate_center.x;
            layer_sprite.posY = candidate_center.y;
            ceiling_sprites.push_back(layer_sprite);
        }
    }
}

struct LabFrontierNode {
    MapPoint center;
    MapPoint outward_step;
};

void append_outward_lab_ceiling_layers(
    const std::vector<Segment>& segments,
    float map_size_x,
    float map_size_y,
    const std::set<CeilingPoint>& outside_cells,
    const std::vector<LabCeilingSeed>& seeds,
    std::vector<io::Sprite>& ceiling_sprites
) {
    const AS1CeilingProfile& profile = CEILING_AS1_LAB;
    if (profile.total_layer_count <= 1 || seeds.empty()) {
        return;
    }

    const WallProfile& wall_profile = WallBuilder::get_wall_profile(WALL_TYPE_LAB);
    MapPoint wall_shift = WallBuilder::get_wall_shift(map_size_x, wall_profile);
    MapPoint ceil_shift = get_floor_ceiling_shift(map_size_x, profile.step_x, profile.step_y, 1);

    std::vector<PhysicalWallBoundary> boundaries =
        build_physical_wall_boundaries(segments, map_size_x);

    std::set<CeilingPositionKey> occupied_physical_positions;
    std::set<std::pair<int, int>> occupied_grid_cells;

    // Layer 1 is already calibrated and must be preserved 100% without deduplication deletions.
    for (const io::Sprite& sprite : ceiling_sprites) {
        if (sprite.vid == profile.vid) {
            occupied_physical_positions.insert(make_ceiling_position_key(sprite.posX, sprite.posY));
            GridPoint snapped_grid = snap_physical_to_lab_ceiling_grid(
                {sprite.posX, sprite.posY},
                ceil_shift,
                profile
            );
            occupied_grid_cells.insert({snapped_grid.x, snapped_grid.y});
        }
    }

    // 4 isometric diagonal directions
    const MapPoint expansion_directions[4] = {
        { profile.step_x,  profile.step_y},
        {-profile.step_x, -profile.step_y},
        {-profile.step_x,  profile.step_y},
        { profile.step_x, -profile.step_y}
    };

    std::vector<LabFrontierNode> current_frontier;
    current_frontier.reserve(seeds.size());
    for (const LabCeilingSeed& seed : seeds) {
        if (!is_within_lab_ceiling_bounds(seed.sprite.posX, seed.sprite.posY, map_size_x, map_size_y, profile)) {
            continue;
        }
        MapPoint outward_step = get_lab_ceiling_outward_step(
            seed.kind,
            seed.outside_side,
            profile
        );
        current_frontier.push_back({{seed.sprite.posX, seed.sprite.posY}, outward_step});
    }

    io::Sprite template_sprite = seeds.front().sprite;

    for (int layer_number = 2;
         layer_number <= profile.total_layer_count;
         ++layer_number) {
        std::vector<LabFrontierNode> next_frontier;
        bool use_grid_snapping = (layer_number >= profile.grid_snapping_start_layer);

        for (const LabFrontierNode& node : current_frontier) {
            for (const MapPoint& dir : expansion_directions) {
                // Strictly forbid expanding in the opposite direction (towards room interior)
                if (std::abs(dir.x + node.outward_step.x) < GEOMETRY_EPSILON &&
                    std::abs(dir.y + node.outward_step.y) < GEOMETRY_EPSILON) {
                    continue;
                }

                MapPoint candidate_center = {
                    node.center.x + dir.x,
                    node.center.y + dir.y
                };
                GridPoint candidate_grid = {0, 0};

                if (use_grid_snapping) {
                    candidate_grid = snap_physical_to_lab_ceiling_grid(
                        candidate_center,
                        ceil_shift,
                        profile
                    );
                    if (occupied_grid_cells.count({candidate_grid.x, candidate_grid.y}) > 0) {
                        continue;
                    }
                    candidate_center = to_iso(candidate_grid, profile.step_x, profile.step_y, ceil_shift);
                } else {
                    CeilingPositionKey pos_key = make_ceiling_position_key(
                        candidate_center.x,
                        candidate_center.y
                    );
                    if (occupied_physical_positions.count(pos_key) > 0) {
                        continue;
                    }
                }

                if (!is_within_lab_ceiling_bounds(candidate_center.x, candidate_center.y, map_size_x, map_size_y, profile)) {
                    continue;
                }

                // Reject any candidate point that falls inside an interior grid cell
                float delta_x = candidate_center.x - wall_shift.x;
                float delta_y = candidate_center.y - wall_shift.y;
                float gx = (delta_x / wall_profile.step_x + delta_y / wall_profile.step_y) / 2.0f;
                float gy = (delta_y / wall_profile.step_y - delta_x / wall_profile.step_x) / 2.0f;
                int cell_x = static_cast<int>(std::floor(gx));
                int cell_y = static_cast<int>(std::floor(gy));
                if (outside_cells.count({cell_x, cell_y}) == 0) {
                    continue;
                }

                bool hits_wall = false;
                for (const PhysicalWallBoundary& boundary : boundaries) {
                    bool crosses_wall = line_segments_intersect(
                        node.center,
                        candidate_center,
                        boundary.start,
                        boundary.end
                    );
                    bool overlaps_wall = segment_enters_ceiling_footprint(
                        boundary,
                        candidate_center,
                        profile
                    );
                    if (crosses_wall || overlaps_wall) {
                        hits_wall = true;
                        break;
                    }
                }

                if (hits_wall) {
                    continue;
                }

                if (use_grid_snapping) {
                    occupied_grid_cells.insert({candidate_grid.x, candidate_grid.y});
                }
                occupied_physical_positions.insert(make_ceiling_position_key(candidate_center.x, candidate_center.y));

                io::Sprite layer_sprite = template_sprite;
                layer_sprite.posX = candidate_center.x;
                layer_sprite.posY = candidate_center.y;
                ceiling_sprites.push_back(layer_sprite);

                next_frontier.push_back({candidate_center, node.outward_step});
            }
        }

        if (next_frontier.empty()) {
            break;
        }

        current_frontier = std::move(next_frontier);
    }

    fill_lab_ceiling_grid_holes(
        boundaries,
        map_size_x,
        map_size_y,
        wall_profile,
        wall_shift,
        ceil_shift,
        profile,
        outside_cells,
        template_sprite,
        occupied_grid_cells,
        occupied_physical_positions,
        ceiling_sprites
    );
}



} // namespace

static bool is_as2_wall_set_type(int wall_type) {
    return wall_type >= WALL_TYPE_AS2_WALL_SET1_FIXED_0
        && wall_type <= WALL_TYPE_AS2_WALL_SET9_RANDOM;
}

WallBuilder::WallBuilder(float map_size_x, float map_size_y, bool randomize_directions)
    : map_size_x_(map_size_x),
      map_size_y_(map_size_y),
      direction_randomizer_(randomize_directions) {}

bool WallBuilder::RareWallPosition::operator<(const RareWallPosition& other) const {
    if (wall_type != other.wall_type) {
        return wall_type < other.wall_type;
    }

    if (kind != other.kind) {
        return kind < other.kind;
    }

    if (gx != other.gx) {
        return gx < other.gx;
    }

    return gy < other.gy;
}

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

const CeilingCurtainProfile* WallBuilder::get_ceiling_curtain_profile(int wall_type) {
    if (wall_type == WALL_TYPE_LAB) {
        // Adapter only: AS1's public profile remains a plain square-tile
        // profile. The shared exterior-edge pipeline expects AS2 part slots.
        static const CeilingCurtainProfile lab_pipeline_adapter = {
            .vid = CEILING_AS1_LAB.vid,
            .pos_z = CEILING_AS1_LAB.pos_z,
            .maximum_wide_opposing_wall_distance = 3,
            .dir_a_long = {
                .direction = 0,
                .offset_x = 0.0f,
                .offset_y = 0.0f,
                .use_side_specific_offsets = true,
                .negative_side_offset =
                    CEILING_AS1_LAB.dir_a_negative_outside_offset,
                .positive_side_offset =
                    CEILING_AS1_LAB.dir_a_positive_outside_offset
            },
            .dir_b_long = {
                .direction = 0,
                .offset_x = 0.0f,
                .offset_y = 0.0f,
                .use_side_specific_offsets = true,
                .negative_side_offset =
                    CEILING_AS1_LAB.dir_b_negative_outside_offset,
                .positive_side_offset =
                    CEILING_AS1_LAB.dir_b_positive_outside_offset
            },
            .dir_a_wide = {
                .direction = 0,
                .offset_x = 0.0f,
                .offset_y = 0.0f,
                .use_side_specific_offsets = true,
                .negative_side_offset =
                    CEILING_AS1_LAB.dir_a_negative_outside_offset,
                .positive_side_offset =
                    CEILING_AS1_LAB.dir_a_positive_outside_offset
            },
            .dir_b_wide = {
                .direction = 0,
                .offset_x = 0.0f,
                .offset_y = 0.0f,
                .use_side_specific_offsets = true,
                .negative_side_offset =
                    CEILING_AS1_LAB.dir_b_negative_outside_offset,
                .positive_side_offset =
                    CEILING_AS1_LAB.dir_b_positive_outside_offset
            }
        };
        return &lab_pipeline_adapter;
    }

    if (wall_type == WALL_TYPE_AS2_WALL_SET1_FIXED_0) {
        return &CEILING_CURTAIN_AS2_SET1;
    }

    if (wall_type == WALL_TYPE_AS2_WALL_SET1_FIXED_1) {
        return &CEILING_CURTAIN_AS2_SET1;
    }

    if (wall_type == WALL_TYPE_AS2_WALL_SET1_RANDOM) {
        return &CEILING_CURTAIN_AS2_SET1;
    }

    if (wall_type == WALL_TYPE_AS2_WALL_SET2_RANDOM) {
        return &CEILING_CURTAIN_AS2_SET2;
    }

    if (wall_type == WALL_TYPE_AS2_WALL_SET3_RANDOM) {
        return &CEILING_CURTAIN_AS2_SET3;
    }

    if (wall_type == WALL_TYPE_AS2_WALL_SET4_RANDOM) {
        return &CEILING_CURTAIN_AS2_SET4;
    }

    if (wall_type == WALL_TYPE_AS2_WALL_SET5_RANDOM) {
        return &CEILING_CURTAIN_AS2_SET5;
    }

    if (wall_type == WALL_TYPE_AS2_WALL_SET6_RANDOM) {
        return &CEILING_CURTAIN_AS2_SET6;
    }

    if (wall_type == WALL_TYPE_AS2_WALL_SET7_RANDOM) {
        return &CEILING_CURTAIN_AS2_SET7;
    }

    if (wall_type == WALL_TYPE_AS2_WALL_SET8_RANDOM) {
        return &CEILING_CURTAIN_AS2_SET8;
    }

    if (wall_type == WALL_TYPE_AS2_WALL_SET9_RANDOM) {
        return &CEILING_CURTAIN_AS2_SET9;
    }

    return nullptr;
}

const AS1CeilingProfile* WallBuilder::get_as1_ceiling_profile(int wall_type) {
    if (wall_type == WALL_TYPE_LAB) {
        return &CEILING_AS1_LAB;
    }

    return nullptr;
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
                    if (is_as2_wall_set_type(ft)) {
                        floor_sprite.army = get_as2_wall_set_asset_army(f_prof.vid);
                    }
                    floor_sprite.gamma = f_prof.gamma;
                    floor_sprite.direction = direction_randomizer_.select_direction(
                        f_prof.direction,
                        f_prof.direction_randomization
                    );
                    floor_sprites.push_back(floor_sprite);
                }
            }
        }
    }
    return floor_sprites;
}

std::vector<io::Sprite> WallBuilder::place_ceilings(
    const std::vector<Segment>& segments
) const {
    return place_wall_aligned_ceiling_curtains(segments);
}

std::vector<io::Sprite> WallBuilder::place_wall_aligned_ceiling_curtains(
    const std::vector<Segment>& segments
) const {
    std::vector<io::Sprite> ceiling_sprites;
    std::vector<LabCeilingSeed> lab_ceiling_seeds;
    std::map<CeilingPoint, int> edges_a;
    std::map<CeilingPoint, int> edges_b;
    std::set<CeilingPoint> vertices;

    for (const Segment& segment : segments) {
        bool supports_wall_aligned_ceiling =
            segment.wall_type == WALL_TYPE_LAB ||
            is_as2_wall_set_type(segment.wall_type);
        if (!supports_wall_aligned_ceiling) {
            continue;
        }

        int x1 = segment.start.x;
        int y1 = segment.start.y;
        int x2 = segment.end.x;
        int y2 = segment.end.y;

        if (x1 == x2) {
            int min_y = std::min(y1, y2);
            int max_y = std::max(y1, y2);
            for (int y = min_y + 1; y <= max_y; ++y) {
                edges_a.emplace(CeilingPoint{x1, y}, segment.wall_type);
                vertices.insert({x1, y - 1});
                vertices.insert({x1, y});
            }
        }

        if (y1 == y2) {
            int min_x = std::min(x1, x2);
            int max_x = std::max(x1, x2);
            for (int x = min_x + 1; x <= max_x; ++x) {
                edges_b.emplace(CeilingPoint{x, y1}, segment.wall_type);
                vertices.insert({x - 1, y1});
                vertices.insert({x, y1});
            }
        }
    }

    if (vertices.empty()) {
        return ceiling_sprites;
    }

    int min_vertex_x = vertices.begin()->first;
    int max_vertex_x = vertices.begin()->first;
    int min_vertex_y = vertices.begin()->second;
    int max_vertex_y = vertices.begin()->second;

    for (const CeilingPoint& vertex : vertices) {
        min_vertex_x = std::min(min_vertex_x, vertex.first);
        max_vertex_x = std::max(max_vertex_x, vertex.first);
        min_vertex_y = std::min(min_vertex_y, vertex.second);
        max_vertex_y = std::max(max_vertex_y, vertex.second);
    }

    int cell_margin = CEILING_CURTAIN_AS2_SET1.maximum_wide_opposing_wall_distance + 2;
    int min_cell_x = min_vertex_x - cell_margin;
    int max_cell_x = max_vertex_x + cell_margin;
    int min_cell_y = min_vertex_y - cell_margin;
    int max_cell_y = max_vertex_y + cell_margin;

    auto movement_crosses_wall = [&](int cell_x, int cell_y, int next_x, int next_y) {
        if (next_x == cell_x + 1) {
            return edges_a.count({cell_x + 1, cell_y + 1}) > 0;
        }

        if (next_x == cell_x - 1) {
            return edges_a.count({cell_x, cell_y + 1}) > 0;
        }

        if (next_y == cell_y + 1) {
            return edges_b.count({cell_x + 1, cell_y + 1}) > 0;
        }

        return edges_b.count({cell_x + 1, cell_y}) > 0;
    };

    std::set<CeilingPoint> outside_cells;
    std::vector<CeilingPoint> pending_cells;
    pending_cells.push_back({min_cell_x, min_cell_y});
    outside_cells.insert({min_cell_x, min_cell_y});

    std::size_t pending_index = 0;
    while (pending_index < pending_cells.size()) {
        CeilingPoint cell = pending_cells[pending_index];
        pending_index += 1;

        int neighbors[4][2] = {
            {cell.first + 1, cell.second},
            {cell.first - 1, cell.second},
            {cell.first, cell.second + 1},
            {cell.first, cell.second - 1}
        };

        for (const auto& neighbor : neighbors) {
            int next_x = neighbor[0];
            int next_y = neighbor[1];

            if (next_x < min_cell_x || next_x > max_cell_x) {
                continue;
            }

            if (next_y < min_cell_y || next_y > max_cell_y) {
                continue;
            }

            if (outside_cells.count({next_x, next_y}) > 0) {
                continue;
            }

            if (movement_crosses_wall(cell.first, cell.second, next_x, next_y)) {
                continue;
            }

            outside_cells.insert({next_x, next_y});
            pending_cells.push_back({next_x, next_y});
        }
    }

    std::map<CeilingEdgeKey, ExteriorCeilingEdge> exterior_edges;

    for (const auto& entry : edges_a) {
        int gx = entry.first.first;
        int gy = entry.first.second;
        int wall_type = entry.second;
        const CeilingCurtainProfile* profile = get_ceiling_curtain_profile(wall_type);
        if (profile == nullptr) {
            continue;
        }

        bool left_is_outside = outside_cells.count({gx - 1, gy - 1}) > 0;
        bool right_is_outside = outside_cells.count({gx, gy - 1}) > 0;
        if (left_is_outside == right_is_outside) {
            continue;
        }

        CeilingEdgeKey key = {gx, gy, WallPartKind::DirA};
        WallOutsideSide outside_side = WallOutsideSide::PositiveGridSide;
        if (left_is_outside) {
            outside_side = WallOutsideSide::NegativeGridSide;
        }
        exterior_edges.emplace(
            key,
            ExteriorCeilingEdge{gx, gy, wall_type, WallPartKind::DirA, 0, outside_side}
        );
    }

    for (const auto& entry : edges_b) {
        int gx = entry.first.first;
        int gy = entry.first.second;
        int wall_type = entry.second;
        const CeilingCurtainProfile* profile = get_ceiling_curtain_profile(wall_type);
        if (profile == nullptr) {
            continue;
        }

        bool upper_is_outside = outside_cells.count({gx - 1, gy - 1}) > 0;
        bool lower_is_outside = outside_cells.count({gx - 1, gy}) > 0;
        if (upper_is_outside == lower_is_outside) {
            continue;
        }

        CeilingEdgeKey key = {gx, gy, WallPartKind::DirB};
        WallOutsideSide outside_side = WallOutsideSide::PositiveGridSide;
        if (upper_is_outside) {
            outside_side = WallOutsideSide::NegativeGridSide;
        }
        exterior_edges.emplace(
            key,
            ExteriorCeilingEdge{gx, gy, wall_type, WallPartKind::DirB, 0, outside_side}
        );
    }

    // Attach one wall-aligned ceiling or Long curtain to every exterior wall part.
    constexpr bool use_only_long_ceiling_curtains = true;
    if (use_only_long_ceiling_curtains) {
        struct CornerSupplement {
            CeilingEdgeKey edge;
            float grid_x;
            float grid_y;
        };

        struct RecessSideAdjustment {
            float grid_x = 0.0f;
            float grid_y = 0.0f;
        };

        struct LabCornerSupplement {
            CeilingPoint corner;
            WallPartKind kind;
            int count = 0;
            float away_from_corner_adjustment = 0.0f;
        };

        std::map<CeilingPoint, CeilingPoint> deep_corner_outside_cells;
        std::set<CeilingPoint> paired_recess_corners;
        std::set<CeilingEdgeKey> removed_ceiling_edges;
        std::map<CeilingEdgeKey, RecessSideAdjustment> recess_side_adjustments;
        std::vector<CornerSupplement> lower_corner_supplements;
        std::vector<LabCornerSupplement> lab_corner_supplements;
        for (const CeilingPoint& vertex : vertices) {
            int outside_count = 0;
            bool interior_cell_is_above_vertex = false;
            bool lower_corner_is_left = false;
            CeilingPoint only_outside_cell = {0, 0};
            // Neighbor cells are classified relative to the corner vertex:
            // upper-left, upper-right, lower-left, lower-right.
            // A deep recess corner has exactly one outside cell, so its quadrant
            // must be preserved instead of describing it only as an up/down turn.
            CeilingPoint surrounding_cells[4] = {
                {vertex.first - 1, vertex.second - 1},
                {vertex.first, vertex.second - 1},
                {vertex.first - 1, vertex.second},
                {vertex.first, vertex.second}
            };

            for (const CeilingPoint& cell : surrounding_cells) {
                if (outside_cells.count(cell) > 0) {
                    outside_count += 1;
                    only_outside_cell = cell;
                    continue;
                }

                if (cell.second == vertex.second - 1) {
                    interior_cell_is_above_vertex = true;
                    // At a lower exterior corner, the upper interior cell tells
                    // whether the missing corner lies on its screen-left side.
                    lower_corner_is_left = cell.first == vertex.first;
                }
            }

            if (outside_count == 1) {
                deep_corner_outside_cells.emplace(vertex, only_outside_cell);
            }

            // Only the two lower exterior corners need extra coverage. Recess
            // entrances have their interior cell below the vertex, while deep
            // recess corners have only one outside cell.
            if (outside_count != 3 || !interior_cell_is_above_vertex) {
                continue;
            }

            CeilingEdgeKey corner_edges[4] = {
                {vertex.first, vertex.second, WallPartKind::DirA},
                {vertex.first, vertex.second + 1, WallPartKind::DirA},
                {vertex.first, vertex.second, WallPartKind::DirB},
                {vertex.first + 1, vertex.second, WallPartKind::DirB}
            };

            for (const CeilingEdgeKey& corner_edge : corner_edges) {
                auto edge_it = exterior_edges.find(corner_edge);
                if (edge_it == exterior_edges.end()) {
                    continue;
                }

                const ExteriorCeilingEdge& edge = edge_it->second;
                const CeilingCurtainProfile* profile =
                    get_ceiling_curtain_profile(edge.wall_type);
                if (profile == nullptr) {
                    continue;
                }

                int supplement_count = 0;
                if (lower_corner_is_left && edge.kind == WallPartKind::DirA) {
                    supplement_count = profile->left_lower_corner_dir_a_supplement_count;
                }

                if (lower_corner_is_left && edge.kind == WallPartKind::DirB) {
                    supplement_count = profile->left_lower_corner_dir_b_supplement_count;
                }

                if (!lower_corner_is_left && edge.kind == WallPartKind::DirA) {
                    supplement_count = profile->right_lower_corner_dir_a_supplement_count;
                }

                if (!lower_corner_is_left && edge.kind == WallPartKind::DirB) {
                    supplement_count = profile->right_lower_corner_dir_b_supplement_count;
                }

                for (int supplement_number = 1;
                     supplement_number <= supplement_count;
                     ++supplement_number) {
                    CornerSupplement supplement = {corner_edge, 0.0f, 0.0f};
                    float supplement_scale = static_cast<float>(supplement_number);

                    if (edge.kind == WallPartKind::DirA) {
                        bool vertex_is_upper_endpoint =
                            vertex.first == edge.gx && vertex.second == edge.gy - 1;
                        float supplement_distance =
                            profile->dir_a_lower_corner_supplement * supplement_scale;
                        if (vertex_is_upper_endpoint) {
                            supplement.grid_y -= supplement_distance;
                        } else {
                            supplement.grid_y += supplement_distance;
                        }
                    }

                    if (edge.kind == WallPartKind::DirB) {
                        bool vertex_is_left_endpoint =
                            vertex.first == edge.gx - 1 && vertex.second == edge.gy;
                        float supplement_distance =
                            profile->dir_b_lower_corner_supplement * supplement_scale;
                        if (vertex_is_left_endpoint) {
                            supplement.grid_x -= supplement_distance;
                        } else {
                            supplement.grid_x += supplement_distance;
                        }
                    }

                    lower_corner_supplements.push_back(supplement);
                }
            }
        }

        auto horizontal_connector_exists = [&](int left_x, int right_x, int grid_y) {
            for (int grid_x = left_x + 1; grid_x <= right_x; ++grid_x) {
                CeilingEdgeKey edge = {grid_x, grid_y, WallPartKind::DirB};
                if (exterior_edges.count(edge) == 0) {
                    return false;
                }
            }
            return true;
        };

        auto vertical_connector_exists = [&](int grid_x, int upper_y, int lower_y) {
            for (int grid_y = upper_y + 1; grid_y <= lower_y; ++grid_y) {
                CeilingEdgeKey edge = {grid_x, grid_y, WallPartKind::DirA};
                if (exterior_edges.count(edge) == 0) {
                    return false;
                }
            }
            return true;
        };

        auto configure_long_at_corner = [&](const CeilingPoint& corner,
                                            WallPartKind long_kind,
                                            bool keep_long,
                                            float distance) {
            CeilingEdgeKey candidates[2];
            if (long_kind == WallPartKind::DirA) {
                candidates[0] = {corner.first, corner.second, WallPartKind::DirA};
                candidates[1] = {corner.first, corner.second + 1, WallPartKind::DirA};
            } else {
                candidates[0] = {corner.first, corner.second, WallPartKind::DirB};
                candidates[1] = {corner.first + 1, corner.second, WallPartKind::DirB};
            }

            for (const CeilingEdgeKey& candidate : candidates) {
                if (exterior_edges.count(candidate) == 0) {
                    continue;
                }

                if (!keep_long) {
                    removed_ceiling_edges.insert(candidate);
                    return;
                }

                RecessSideAdjustment adjustment;
                if (long_kind == WallPartKind::DirA) {
                    bool corner_is_upper_endpoint =
                        corner.first == std::get<0>(candidate) &&
                        corner.second == std::get<1>(candidate) - 1;
                    if (corner_is_upper_endpoint) {
                        adjustment.grid_y = distance;
                    } else {
                        adjustment.grid_y = -distance;
                    }
                } else {
                    bool corner_is_left_endpoint =
                        corner.first == std::get<0>(candidate) - 1 &&
                        corner.second == std::get<1>(candidate);
                    if (corner_is_left_endpoint) {
                        adjustment.grid_x = distance;
                    } else {
                        adjustment.grid_x = -distance;
                    }
                }

                recess_side_adjustments[candidate] = adjustment;
                return;
            }
        };

        auto get_corner_wall_type = [&](const CeilingPoint& corner) {
            CeilingEdgeKey candidates[4] = {
                {corner.first, corner.second, WallPartKind::DirA},
                {corner.first, corner.second + 1, WallPartKind::DirA},
                {corner.first, corner.second, WallPartKind::DirB},
                {corner.first + 1, corner.second, WallPartKind::DirB}
            };
            for (const CeilingEdgeKey& candidate : candidates) {
                auto edge_it = exterior_edges.find(candidate);
                if (edge_it != exterior_edges.end()) {
                    return edge_it->second.wall_type;
                }
            }
            return -1;
        };

        auto has_deep_corner_between = [&](const CeilingPoint& first, const CeilingPoint& second) {
            for (const auto& entry : deep_corner_outside_cells) {
                const CeilingPoint& candidate = entry.first;
                if (first.second == second.second && candidate.second == first.second) {
                    bool is_between = candidate.first > first.first;
                    is_between = is_between && candidate.first < second.first;
                    if (is_between) {
                        return true;
                    }
                }

                if (first.first == second.first && candidate.first == first.first) {
                    bool is_between = candidate.second > first.second;
                    is_between = is_between && candidate.second < second.second;
                    if (is_between) {
                        return true;
                    }
                }
            }
            return false;
        };

        for (auto first_it = deep_corner_outside_cells.begin();
             first_it != deep_corner_outside_cells.end();
             ++first_it) {
            auto second_it = first_it;
            ++second_it;
            for (; second_it != deep_corner_outside_cells.end(); ++second_it) {
                CeilingPoint first_vertex = first_it->first;
                CeilingPoint second_vertex = second_it->first;
                CeilingPoint first_outside_cell = first_it->second;
                CeilingPoint second_outside_cell = second_it->second;

                if (first_vertex.second == second_vertex.second) {
                    CeilingPoint left_vertex = first_vertex;
                    CeilingPoint right_vertex = second_vertex;
                    CeilingPoint left_outside_cell = first_outside_cell;
                    CeilingPoint right_outside_cell = second_outside_cell;
                    if (left_vertex.first > right_vertex.first) {
                        std::swap(left_vertex, right_vertex);
                        std::swap(left_outside_cell, right_outside_cell);
                    }

                    if (has_deep_corner_between(left_vertex, right_vertex)) {
                        continue;
                    }

                    int grid_y = left_vertex.second;
                    bool is_upper_recess =
                        left_outside_cell == CeilingPoint{left_vertex.first, grid_y - 1};
                    is_upper_recess = is_upper_recess &&
                        right_outside_cell == CeilingPoint{right_vertex.first - 1, grid_y - 1};

                    bool is_lower_recess =
                        left_outside_cell == CeilingPoint{left_vertex.first, grid_y};
                    is_lower_recess = is_lower_recess &&
                        right_outside_cell == CeilingPoint{right_vertex.first - 1, grid_y};

                    if (!is_upper_recess && !is_lower_recess) {
                        continue;
                    }

                    if (!horizontal_connector_exists(
                            left_vertex.first,
                            right_vertex.first,
                            grid_y)) {
                        continue;
                    }

                    paired_recess_corners.insert(left_vertex);
                    paired_recess_corners.insert(right_vertex);

                    CeilingEdgeKey left_edge = {
                        left_vertex.first + 1,
                        grid_y,
                        WallPartKind::DirB
                    };
                    CeilingEdgeKey right_edge = {
                        right_vertex.first,
                        grid_y,
                        WallPartKind::DirB
                    };
                    const ExteriorCeilingEdge& left_exterior_edge = exterior_edges.at(left_edge);
                    const ExteriorCeilingEdge& right_exterior_edge = exterior_edges.at(right_edge);
                    const CeilingCurtainProfile* left_profile =
                        get_ceiling_curtain_profile(left_exterior_edge.wall_type);
                    const CeilingCurtainProfile* right_profile =
                        get_ceiling_curtain_profile(right_exterior_edge.wall_type);

                    const RecessCornerCurtainProfile* left_corner_profile = nullptr;
                    const RecessCornerCurtainProfile* right_corner_profile = nullptr;
                    if (is_upper_recess) {
                        left_corner_profile = &left_profile->upper_recess.left_corner;
                        right_corner_profile = &right_profile->upper_recess.right_corner;
                    }
                    if (is_lower_recess) {
                        left_corner_profile = &left_profile->lower_recess.left_corner;
                        right_corner_profile = &right_profile->lower_recess.right_corner;
                    }

                    const AS1CeilingProfile* left_as1_profile =
                        get_as1_ceiling_profile(left_exterior_edge.wall_type);
                    const AS1CeilingProfile* right_as1_profile =
                        get_as1_ceiling_profile(right_exterior_edge.wall_type);
                    const AS1RecessCornerSupplementProfile* left_as1_corner = nullptr;
                    const AS1RecessCornerSupplementProfile* right_as1_corner = nullptr;
                    if (left_as1_profile != nullptr && is_upper_recess) {
                        left_as1_corner = &left_as1_profile->upper_recess.left_corner;
                    }
                    if (right_as1_profile != nullptr && is_upper_recess) {
                        right_as1_corner = &right_as1_profile->upper_recess.right_corner;
                    }
                    if (left_as1_profile != nullptr && is_lower_recess) {
                        left_as1_corner = &left_as1_profile->lower_recess.left_corner;
                    }
                    if (right_as1_profile != nullptr && is_lower_recess) {
                        right_as1_corner = &right_as1_profile->lower_recess.right_corner;
                    }
                    if (left_as1_corner != nullptr) {
                        lab_corner_supplements.push_back({
                            left_vertex,
                            WallPartKind::DirA,
                            left_as1_corner->supplement_count,
                            left_as1_corner->away_from_corner_adjustment
                        });
                    }
                    if (right_as1_corner != nullptr) {
                        lab_corner_supplements.push_back({
                            right_vertex,
                            WallPartKind::DirA,
                            right_as1_corner->supplement_count,
                            right_as1_corner->away_from_corner_adjustment
                        });
                    }
                    if (left_as1_corner != nullptr &&
                        !left_as1_corner->keep_connector_ceiling) {
                        removed_ceiling_edges.insert(left_edge);
                    }
                    if (right_as1_corner != nullptr &&
                        !right_as1_corner->keep_connector_ceiling) {
                        removed_ceiling_edges.insert(right_edge);
                    }
                    if (left_as1_corner != nullptr &&
                        !left_as1_corner->keep_side_ceiling) {
                        configure_long_at_corner(
                            left_vertex,
                            WallPartKind::DirA,
                            false,
                            0.0f
                        );
                    }
                    if (right_as1_corner != nullptr &&
                        !right_as1_corner->keep_side_ceiling) {
                        configure_long_at_corner(
                            right_vertex,
                            WallPartKind::DirA,
                            false,
                            0.0f
                        );
                    }

                    if (!left_corner_profile->keep_connector_long) {
                        removed_ceiling_edges.insert(left_edge);
                    }
                    if (!right_corner_profile->keep_connector_long) {
                        removed_ceiling_edges.insert(right_edge);
                    }
                    configure_long_at_corner(
                        left_vertex,
                        WallPartKind::DirA,
                        true,
                        left_corner_profile->side_long_away_from_corner_adjustment
                    );
                    configure_long_at_corner(
                        right_vertex,
                        WallPartKind::DirA,
                        true,
                        right_corner_profile->side_long_away_from_corner_adjustment
                    );
                    continue;
                }

                if (first_vertex.first != second_vertex.first) {
                    continue;
                }

                CeilingPoint upper_vertex = first_vertex;
                CeilingPoint lower_vertex = second_vertex;
                CeilingPoint upper_outside_cell = first_outside_cell;
                CeilingPoint lower_outside_cell = second_outside_cell;
                if (upper_vertex.second > lower_vertex.second) {
                    std::swap(upper_vertex, lower_vertex);
                    std::swap(upper_outside_cell, lower_outside_cell);
                }

                if (has_deep_corner_between(upper_vertex, lower_vertex)) {
                    continue;
                }

                int grid_x = upper_vertex.first;
                bool is_left_recess =
                    upper_outside_cell == CeilingPoint{grid_x - 1, upper_vertex.second};
                is_left_recess = is_left_recess &&
                    lower_outside_cell == CeilingPoint{grid_x - 1, lower_vertex.second - 1};

                bool is_right_recess =
                    upper_outside_cell == CeilingPoint{grid_x, upper_vertex.second};
                is_right_recess = is_right_recess &&
                    lower_outside_cell == CeilingPoint{grid_x, lower_vertex.second - 1};

                if (!is_left_recess && !is_right_recess) {
                    continue;
                }

                if (!vertical_connector_exists(
                        grid_x,
                        upper_vertex.second,
                        lower_vertex.second)) {
                    continue;
                }

                paired_recess_corners.insert(upper_vertex);
                paired_recess_corners.insert(lower_vertex);

                CeilingEdgeKey upper_edge = {
                    grid_x,
                    upper_vertex.second + 1,
                    WallPartKind::DirA
                };
                CeilingEdgeKey lower_edge = {
                    grid_x,
                    lower_vertex.second,
                    WallPartKind::DirA
                };
                const ExteriorCeilingEdge& upper_exterior_edge = exterior_edges.at(upper_edge);
                const ExteriorCeilingEdge& lower_exterior_edge = exterior_edges.at(lower_edge);
                const CeilingCurtainProfile* upper_profile =
                    get_ceiling_curtain_profile(upper_exterior_edge.wall_type);
                const CeilingCurtainProfile* lower_profile =
                    get_ceiling_curtain_profile(lower_exterior_edge.wall_type);

                const RecessCornerCurtainProfile* upper_corner_profile = nullptr;
                const RecessCornerCurtainProfile* lower_corner_profile = nullptr;
                if (is_left_recess) {
                    upper_corner_profile = &upper_profile->left_recess.upper_corner;
                    lower_corner_profile = &lower_profile->left_recess.lower_corner;
                }
                if (is_right_recess) {
                    upper_corner_profile = &upper_profile->right_recess.upper_corner;
                    lower_corner_profile = &lower_profile->right_recess.lower_corner;
                }

                const AS1CeilingProfile* upper_as1_profile =
                    get_as1_ceiling_profile(upper_exterior_edge.wall_type);
                const AS1CeilingProfile* lower_as1_profile =
                    get_as1_ceiling_profile(lower_exterior_edge.wall_type);
                const AS1RecessCornerSupplementProfile* upper_as1_corner = nullptr;
                const AS1RecessCornerSupplementProfile* lower_as1_corner = nullptr;
                if (upper_as1_profile != nullptr && is_left_recess) {
                    upper_as1_corner = &upper_as1_profile->left_recess.upper_corner;
                }
                if (lower_as1_profile != nullptr && is_left_recess) {
                    lower_as1_corner = &lower_as1_profile->left_recess.lower_corner;
                }
                if (upper_as1_profile != nullptr && is_right_recess) {
                    upper_as1_corner = &upper_as1_profile->right_recess.upper_corner;
                }
                if (lower_as1_profile != nullptr && is_right_recess) {
                    lower_as1_corner = &lower_as1_profile->right_recess.lower_corner;
                }
                if (upper_as1_corner != nullptr) {
                    lab_corner_supplements.push_back({
                        upper_vertex,
                        WallPartKind::DirB,
                        upper_as1_corner->supplement_count,
                        upper_as1_corner->away_from_corner_adjustment
                    });
                }
                if (lower_as1_corner != nullptr) {
                    lab_corner_supplements.push_back({
                        lower_vertex,
                        WallPartKind::DirB,
                        lower_as1_corner->supplement_count,
                        lower_as1_corner->away_from_corner_adjustment
                    });
                }
                if (upper_as1_corner != nullptr &&
                    !upper_as1_corner->keep_connector_ceiling) {
                    removed_ceiling_edges.insert(upper_edge);
                }
                if (lower_as1_corner != nullptr &&
                    !lower_as1_corner->keep_connector_ceiling) {
                    removed_ceiling_edges.insert(lower_edge);
                }
                if (upper_as1_corner != nullptr &&
                    !upper_as1_corner->keep_side_ceiling) {
                    configure_long_at_corner(
                        upper_vertex,
                        WallPartKind::DirB,
                        false,
                        0.0f
                    );
                }
                if (lower_as1_corner != nullptr &&
                    !lower_as1_corner->keep_side_ceiling) {
                    configure_long_at_corner(
                        lower_vertex,
                        WallPartKind::DirB,
                        false,
                        0.0f
                    );
                }

                if (!upper_corner_profile->keep_connector_long) {
                    removed_ceiling_edges.insert(upper_edge);
                }
                if (!lower_corner_profile->keep_connector_long) {
                    removed_ceiling_edges.insert(lower_edge);
                }
                configure_long_at_corner(
                    upper_vertex,
                    WallPartKind::DirB,
                    true,
                    upper_corner_profile->side_long_away_from_corner_adjustment
                );
                configure_long_at_corner(
                    lower_vertex,
                    WallPartKind::DirB,
                    true,
                    lower_corner_profile->side_long_away_from_corner_adjustment
                );
            }
        }

        for (const auto& entry : deep_corner_outside_cells) {
            const CeilingPoint& corner = entry.first;
            if (paired_recess_corners.count(corner) > 0) {
                continue;
            }

            int wall_type = get_corner_wall_type(corner);
            const CeilingCurtainProfile* profile = get_ceiling_curtain_profile(wall_type);
            if (profile == nullptr) {
                continue;
            }

            const CeilingPoint& outside_cell = entry.second;
            const StandaloneDeepCornerCurtainProfile* corner_profile = nullptr;
            const AS1CeilingProfile* as1_profile =
                get_as1_ceiling_profile(wall_type);
            const AS1StandaloneDeepCornerSupplementProfile* as1_corner_profile =
                nullptr;
            // Configuration names describe where the L-shaped recess opens.
            // The single outside cell lies diagonally opposite that direction.
            if (outside_cell == CeilingPoint{corner.first - 1, corner.second - 1}) {
                corner_profile = &profile->standalone_deep_corners.lower_right;
                if (as1_profile != nullptr) {
                    as1_corner_profile =
                        &as1_profile->standalone_deep_corners.lower_right;
                }
            }
            if (outside_cell == CeilingPoint{corner.first, corner.second - 1}) {
                corner_profile = &profile->standalone_deep_corners.lower_left;
                if (as1_profile != nullptr) {
                    as1_corner_profile =
                        &as1_profile->standalone_deep_corners.lower_left;
                }
            }
            if (outside_cell == CeilingPoint{corner.first - 1, corner.second}) {
                corner_profile = &profile->standalone_deep_corners.upper_right;
                if (as1_profile != nullptr) {
                    as1_corner_profile =
                        &as1_profile->standalone_deep_corners.upper_right;
                }
            }
            if (outside_cell == CeilingPoint{corner.first, corner.second}) {
                corner_profile = &profile->standalone_deep_corners.upper_left;
                if (as1_profile != nullptr) {
                    as1_corner_profile =
                        &as1_profile->standalone_deep_corners.upper_left;
                }
            }
            if (corner_profile == nullptr) {
                continue;
            }

            if (as1_corner_profile != nullptr) {
                lab_corner_supplements.push_back({
                    corner,
                    WallPartKind::DirA,
                    as1_corner_profile->dir_a_supplement_count,
                    as1_corner_profile->dir_a_away_from_corner_adjustment
                });
                lab_corner_supplements.push_back({
                    corner,
                    WallPartKind::DirB,
                    as1_corner_profile->dir_b_supplement_count,
                    as1_corner_profile->dir_b_away_from_corner_adjustment
                });
            }

            configure_long_at_corner(
                corner,
                WallPartKind::DirA,
                corner_profile->keep_dir_a_long,
                corner_profile->dir_a_long_away_from_corner_adjustment
            );
            configure_long_at_corner(
                corner,
                WallPartKind::DirB,
                corner_profile->keep_dir_b_long,
                corner_profile->dir_b_long_away_from_corner_adjustment
            );
        }

        std::map<CeilingEdgeKey, io::Sprite> lab_edge_ceiling_sprites;

        for (const auto& entry : exterior_edges) {
            bool edge_is_removed =
                removed_ceiling_edges.count(entry.first) > 0;
            const ExteriorCeilingEdge& edge = entry.second;
            bool removed_lab_edge_is_supplement_source =
                edge_is_removed && edge.wall_type == WALL_TYPE_LAB;
            if (edge_is_removed && !removed_lab_edge_is_supplement_source) {
                continue;
            }

            io::Sprite curtain_sprite = place_single_ceiling_curtain(
                edge.gx,
                edge.gy,
                edge.wall_type,
                edge.kind,
                false,
                edge.outside_side
            );

            if (edge.wall_type == WALL_TYPE_LAB) {
                const AS1CeilingProfile* ceiling_profile =
                    get_as1_ceiling_profile(edge.wall_type);
                CeilingWallOffset step_adjustment =
                    ceiling_profile->dir_b_step_adjustment;
                if (edge.kind == WallPartKind::DirA) {
                    step_adjustment = ceiling_profile->dir_a_step_adjustment;
                }

                // Keep each straight Lab run attached to its own wall line.
                // VID 504 follows its 80x56 pitch only within that run.
                int run_step_index = 0;
                int previous_gx = edge.gx;
                int previous_gy = edge.gy;
                while (true) {
                    if (edge.kind == WallPartKind::DirA) {
                        previous_gy -= 1;
                    } else {
                        previous_gx -= 1;
                    }

                    CeilingEdgeKey previous_key = {
                        previous_gx,
                        previous_gy,
                        edge.kind
                    };
                    auto previous_edge_it = exterior_edges.find(previous_key);
                    if (previous_edge_it == exterior_edges.end()) {
                        break;
                    }

                    const ExteriorCeilingEdge& previous_edge =
                        previous_edge_it->second;
                    bool belongs_to_same_run =
                        previous_edge.wall_type == edge.wall_type &&
                        previous_edge.outside_side == edge.outside_side;
                    if (!belongs_to_same_run) {
                        break;
                    }

                    run_step_index += 1;
                }

                float run_step = static_cast<float>(run_step_index);
                curtain_sprite.posX +=
                    step_adjustment.x * run_step;
                curtain_sprite.posY +=
                    step_adjustment.y * run_step;
            }

            auto side_adjustment_it = recess_side_adjustments.find(entry.first);
            if (side_adjustment_it != recess_side_adjustments.end()) {
                const RecessSideAdjustment& adjustment = side_adjustment_it->second;
                const WallProfile& wall_profile = get_wall_profile(edge.wall_type);
                curtain_sprite.posX +=
                    (adjustment.grid_x - adjustment.grid_y) * wall_profile.step_x;
                curtain_sprite.posY +=
                    (adjustment.grid_x + adjustment.grid_y) * wall_profile.step_y;
            }

            if (edge.wall_type == WALL_TYPE_LAB) {
                lab_edge_ceiling_sprites.emplace(entry.first, curtain_sprite);
            }
            if (edge_is_removed) {
                continue;
            }

            ceiling_sprites.push_back(curtain_sprite);
            if (edge.wall_type == WALL_TYPE_LAB) {
                lab_ceiling_seeds.push_back({
                    curtain_sprite,
                    edge.kind,
                    edge.outside_side
                });
            }

            for (const CornerSupplement& supplement : lower_corner_supplements) {
                if (supplement.edge != entry.first) {
                    continue;
                }

                const WallProfile& wall_profile = get_wall_profile(edge.wall_type);
                io::Sprite supplemental_sprite = curtain_sprite;
                supplemental_sprite.posX +=
                    (supplement.grid_x - supplement.grid_y) * wall_profile.step_x;
                supplemental_sprite.posY +=
                    (supplement.grid_x + supplement.grid_y) * wall_profile.step_y;
                ceiling_sprites.push_back(supplemental_sprite);
            }
        }

        for (const CeilingPoint& vertex : vertices) {
            int outside_count = 0;
            CeilingPoint inside_cell = {0, 0};
            CeilingPoint surrounding_cells[4] = {
                {vertex.first - 1, vertex.second - 1},
                {vertex.first, vertex.second - 1},
                {vertex.first - 1, vertex.second},
                {vertex.first, vertex.second}
            };

            for (const CeilingPoint& surrounding_cell : surrounding_cells) {
                if (outside_cells.count(surrounding_cell) > 0) {
                    outside_count += 1;
                } else {
                    inside_cell = surrounding_cell;
                }
            }

            // A square room corner has three outside cells and exactly two
            // exterior wall tiles meeting at the vertex.
            if (outside_count != 3) {
                continue;
            }

            CeilingEdgeKey corner_edge_candidates[4] = {
                {vertex.first, vertex.second, WallPartKind::DirA},
                {vertex.first, vertex.second + 1, WallPartKind::DirA},
                {vertex.first, vertex.second, WallPartKind::DirB},
                {vertex.first + 1, vertex.second, WallPartKind::DirB}
            };
            int corner_edge_count = 0;
            for (const CeilingEdgeKey& candidate : corner_edge_candidates) {
                auto sprite_it = lab_edge_ceiling_sprites.find(candidate);
                if (sprite_it != lab_edge_ceiling_sprites.end()) {
                    corner_edge_count += 1;
                }
            }

            if (corner_edge_count != 2) {
                continue;
            }

            const AS1CeilingProfile* profile =
                get_as1_ceiling_profile(WALL_TYPE_LAB);
            int dir_a_supplement_count = 0;
            int dir_b_supplement_count = 0;
            if (inside_cell == CeilingPoint{vertex.first, vertex.second}) {
                dir_a_supplement_count =
                    profile->left_upper_corner_dir_a_supplement_count;
                dir_b_supplement_count =
                    profile->left_upper_corner_dir_b_supplement_count;
            } else if (inside_cell == CeilingPoint{vertex.first, vertex.second - 1}) {
                dir_a_supplement_count =
                    profile->left_lower_corner_dir_a_supplement_count;
                dir_b_supplement_count =
                    profile->left_lower_corner_dir_b_supplement_count;
            } else if (inside_cell == CeilingPoint{vertex.first - 1, vertex.second}) {
                dir_a_supplement_count =
                    profile->right_upper_corner_dir_a_supplement_count;
                dir_b_supplement_count =
                    profile->right_upper_corner_dir_b_supplement_count;
            } else {
                dir_a_supplement_count =
                    profile->right_lower_corner_dir_a_supplement_count;
                dir_b_supplement_count =
                    profile->right_lower_corner_dir_b_supplement_count;
            }

            WallPartKind supplement_kinds[2] = {
                WallPartKind::DirA,
                WallPartKind::DirB
            };
            int supplement_counts[2] = {
                dir_a_supplement_count,
                dir_b_supplement_count
            };

            for (int direction_index = 0; direction_index < 2; ++direction_index) {
                WallPartKind supplement_kind = supplement_kinds[direction_index];
                int supplement_count = supplement_counts[direction_index];
                CeilingEdgeKey supplement_edge_key{};
                const io::Sprite* supplement_edge_sprite = nullptr;

                for (const CeilingEdgeKey& candidate : corner_edge_candidates) {
                    if (std::get<2>(candidate) != supplement_kind) {
                        continue;
                    }

                    auto sprite_it = lab_edge_ceiling_sprites.find(candidate);
                    if (sprite_it == lab_edge_ceiling_sprites.end()) {
                        continue;
                    }

                    supplement_edge_key = candidate;
                    supplement_edge_sprite = &sprite_it->second;
                    break;
                }

                if (supplement_edge_sprite == nullptr) {
                    continue;
                }

                // Continue the selected direction beyond the corner using the
                // original 80x56 ceiling pitch. DirA and DirB are independent.
                float extension_x = profile->step_x;
                float extension_y = profile->step_y;
                if (supplement_kind == WallPartKind::DirA) {
                    int edge_gy = std::get<1>(supplement_edge_key);
                    bool vertex_is_upper_endpoint =
                        edge_gy == vertex.second + 1;
                    if (vertex_is_upper_endpoint) {
                        extension_y = -extension_y;
                    } else {
                        extension_x = -extension_x;
                    }
                } else {
                    int edge_gx = std::get<0>(supplement_edge_key);
                    bool vertex_is_left_endpoint =
                        edge_gx == vertex.first + 1;
                    if (vertex_is_left_endpoint) {
                        extension_x = -extension_x;
                        extension_y = -extension_y;
                    }
                }

                for (int supplement_index = 1;
                     supplement_index <= supplement_count;
                     ++supplement_index) {
                    float step_count = static_cast<float>(supplement_index);
                    io::Sprite supplement_sprite = *supplement_edge_sprite;
                    supplement_sprite.posX += extension_x * step_count;
                    supplement_sprite.posY += extension_y * step_count;
                    ceiling_sprites.push_back(supplement_sprite);

                    const ExteriorCeilingEdge& source_edge =
                        exterior_edges.at(supplement_edge_key);
                    lab_ceiling_seeds.push_back({
                        supplement_sprite,
                        supplement_kind,
                        source_edge.outside_side
                    });
                }
            }
        }

        for (const LabCornerSupplement& supplement : lab_corner_supplements) {
            CeilingEdgeKey corner_edge_candidates[2];
            if (supplement.kind == WallPartKind::DirA) {
                corner_edge_candidates[0] = {
                    supplement.corner.first,
                    supplement.corner.second,
                    WallPartKind::DirA
                };
                corner_edge_candidates[1] = {
                    supplement.corner.first,
                    supplement.corner.second + 1,
                    WallPartKind::DirA
                };
            } else {
                corner_edge_candidates[0] = {
                    supplement.corner.first,
                    supplement.corner.second,
                    WallPartKind::DirB
                };
                corner_edge_candidates[1] = {
                    supplement.corner.first + 1,
                    supplement.corner.second,
                    WallPartKind::DirB
                };
            }

            CeilingEdgeKey supplement_edge_key{};
            const io::Sprite* supplement_edge_sprite = nullptr;
            for (const CeilingEdgeKey& candidate : corner_edge_candidates) {
                auto sprite_it = lab_edge_ceiling_sprites.find(candidate);
                if (sprite_it == lab_edge_ceiling_sprites.end()) {
                    continue;
                }

                supplement_edge_key = candidate;
                supplement_edge_sprite = &sprite_it->second;
                break;
            }

            if (supplement_edge_sprite == nullptr) {
                continue;
            }

            const AS1CeilingProfile* profile =
                get_as1_ceiling_profile(WALL_TYPE_LAB);
            float extension_x = profile->step_x;
            float extension_y = profile->step_y;
            if (supplement.kind == WallPartKind::DirA) {
                int edge_gy = std::get<1>(supplement_edge_key);
                bool corner_is_upper_endpoint =
                    edge_gy == supplement.corner.second + 1;
                if (corner_is_upper_endpoint) {
                    extension_y = -extension_y;
                } else {
                    extension_x = -extension_x;
                }
            } else {
                int edge_gx = std::get<0>(supplement_edge_key);
                bool corner_is_left_endpoint =
                    edge_gx == supplement.corner.first + 1;
                if (corner_is_left_endpoint) {
                    extension_x = -extension_x;
                    extension_y = -extension_y;
                }
            }

            for (int supplement_index = 1;
                 supplement_index <= supplement.count;
                 ++supplement_index) {
                float distance_from_corner =
                    static_cast<float>(supplement_index) +
                    supplement.away_from_corner_adjustment;
                io::Sprite supplement_sprite = *supplement_edge_sprite;
                supplement_sprite.posX +=
                    extension_x * distance_from_corner;
                supplement_sprite.posY +=
                    extension_y * distance_from_corner;
                ceiling_sprites.push_back(supplement_sprite);

                const ExteriorCeilingEdge& source_edge =
                    exterior_edges.at(supplement_edge_key);
                lab_ceiling_seeds.push_back({
                    supplement_sprite,
                    supplement.kind,
                    source_edge.outside_side
                });
            }
        }

        append_outward_lab_ceiling_layers(
            segments,
            map_size_x_,
            map_size_y_,
            outside_cells,
            lab_ceiling_seeds,
            ceiling_sprites
        );
        return ceiling_sprites;
    }

    // Temporarily disabled: the current AS2 strategy intentionally uses only
    // Long Ceiling Curtains. Keep the old Wide strategy behind one method so
    // it can be reviewed or restored without mixing it into the active path.
    return place_as2_ceiling_curtains_with_wide(
        exterior_edges,
        vertices,
        outside_cells
    );
}

std::vector<io::Sprite> WallBuilder::place_as2_ceiling_curtains_with_wide(
    std::map<CeilingEdgeKey, ExteriorCeilingEdge> exterior_edges,
    const std::set<CeilingPoint>& vertices,
    const std::set<CeilingPoint>& outside_cells
) const {
    std::vector<io::Sprite> ceiling_sprites;

    for (auto& entry : exterior_edges) {
        ExteriorCeilingEdge& edge = entry.second;
        const CeilingCurtainProfile* profile = get_ceiling_curtain_profile(edge.wall_type);
        if (profile == nullptr) {
            continue;
        }

        int search_step_x = 0;
        int search_step_y = 0;
        if (edge.kind == WallPartKind::DirA) {
            search_step_x = 1;
        } else {
            search_step_y = 1;
        }

        if (edge.outside_side == WallOutsideSide::NegativeGridSide) {
            search_step_x = -search_step_x;
            search_step_y = -search_step_y;
        }

        for (
            int distance = 1;
            distance <= profile->maximum_wide_opposing_wall_distance;
            ++distance
        ) {
            CeilingEdgeKey opposing_key = {
                edge.gx + search_step_x * distance,
                edge.gy + search_step_y * distance,
                edge.kind
            };
            auto opposing_it = exterior_edges.find(opposing_key);
            if (opposing_it == exterior_edges.end()) {
                continue;
            }

            const ExteriorCeilingEdge& opposing_edge = opposing_it->second;
            if (opposing_edge.outside_side == edge.outside_side) {
                continue;
            }

            edge.opposing_wall_distance = distance;
            break;
        }
    }

    std::set<CeilingEdgeKey> wide_edges;
    std::set<CeilingEdgeKey> long_edges_replaced_by_wide;
    std::set<CeilingEdgeKey> dir_a_long_edges_covered_by_corner;
    std::set<CeilingEdgeKey> processed_narrow_edges;

    for (const CeilingPoint& vertex : vertices) {
        int outside_count = 0;
        CeilingPoint surrounding_cells[4] = {
            {vertex.first - 1, vertex.second - 1},
            {vertex.first, vertex.second - 1},
            {vertex.first - 1, vertex.second},
            {vertex.first, vertex.second}
        };

        for (const CeilingPoint& cell : surrounding_cells) {
            if (outside_cells.count(cell) > 0) {
                outside_count += 1;
            }
        }

        CeilingEdgeKey candidates[4] = {
            {vertex.first, vertex.second, WallPartKind::DirA},
            {vertex.first, vertex.second + 1, WallPartKind::DirA},
            {vertex.first, vertex.second, WallPartKind::DirB},
            {vertex.first + 1, vertex.second, WallPartKind::DirB}
        };

        if (outside_count == 1) {
            bool has_dir_b_edge_at_corner = false;
            for (const CeilingEdgeKey& candidate : candidates) {
                auto edge_it = exterior_edges.find(candidate);
                if (edge_it == exterior_edges.end()) {
                    continue;
                }

                if (edge_it->second.kind == WallPartKind::DirB) {
                    has_dir_b_edge_at_corner = true;
                    break;
                }
            }

            if (has_dir_b_edge_at_corner) {
                for (const CeilingEdgeKey& candidate : candidates) {
                    auto edge_it = exterior_edges.find(candidate);
                    if (edge_it == exterior_edges.end()) {
                        continue;
                    }

                    const ExteriorCeilingEdge& edge = edge_it->second;
                    if (edge.kind != WallPartKind::DirA) {
                        continue;
                    }

                    // The adjacent DirB Long already covers the deep recess corner.
                    // Keeping this terminal DirA Long lets its visual tail cross the turn.
                    if (edge.opposing_wall_distance == 0) {
                        dir_a_long_edges_covered_by_corner.insert(candidate);
                    }
                }
            }
        }

        if (outside_count != 3) {
            continue;
        }

        for (const CeilingEdgeKey& candidate : candidates) {
            auto edge_it = exterior_edges.find(candidate);
            if (edge_it == exterior_edges.end()) {
                continue;
            }

            const ExteriorCeilingEdge& edge = edge_it->second;
            const CeilingCurtainProfile* profile = get_ceiling_curtain_profile(edge.wall_type);
            if (profile == nullptr) {
                continue;
            }

            if (edge.opposing_wall_distance == 0) {
                continue;
            }

            if (processed_narrow_edges.count(candidate) > 0) {
                continue;
            }

            CeilingEdgeKey covered_edge = candidate;
            int covered_edge_step_x = 0;
            int covered_edge_step_y = 0;
            if (edge.kind == WallPartKind::DirA) {
                bool vertex_is_upper_endpoint = vertex.first == edge.gx && vertex.second == edge.gy - 1;
                if (vertex_is_upper_endpoint) {
                    covered_edge_step_y = 1;
                } else {
                    covered_edge_step_y = -1;
                }
            } else {
                bool vertex_is_left_endpoint = vertex.first == edge.gx - 1 && vertex.second == edge.gy;
                if (vertex_is_left_endpoint) {
                    covered_edge_step_x = 1;
                } else {
                    covered_edge_step_x = -1;
                }
            }

            // Every wall part in the facing narrow run is covered by Wide.
            // One Wide replaces up to three consecutive Long parts.
            std::vector<CeilingEdgeKey> narrow_run;
            while (true) {
                auto covered_it = exterior_edges.find(covered_edge);
                if (covered_it == exterior_edges.end()) {
                    break;
                }

                const ExteriorCeilingEdge& covered_exterior_edge = covered_it->second;
                if (covered_exterior_edge.opposing_wall_distance == 0) {
                    break;
                }

                if (covered_exterior_edge.outside_side != edge.outside_side) {
                    break;
                }

                processed_narrow_edges.insert(covered_edge);
                long_edges_replaced_by_wide.insert(covered_edge);
                narrow_run.push_back(covered_edge);
                covered_edge = {
                    std::get<0>(covered_edge) + covered_edge_step_x,
                    std::get<1>(covered_edge) + covered_edge_step_y,
                    edge.kind
                };
            }

            int run_size = static_cast<int>(narrow_run.size());
            for (int group_start = 0; group_start < run_size; group_start += 3) {
                int anchor_index = group_start;
                int remaining_count = run_size - group_start;
                if (remaining_count < 3) {
                    anchor_index = std::max(0, run_size - 3);
                }
                wide_edges.insert(narrow_run[anchor_index]);
            }
        }
    }

    for (const CeilingEdgeKey& key : wide_edges) {
        const ExteriorCeilingEdge& edge = exterior_edges.at(key);
        ceiling_sprites.push_back(place_single_ceiling_curtain(
            edge.gx,
            edge.gy,
            edge.wall_type,
            edge.kind,
            true,
            edge.outside_side
        ));
    }

    for (const auto& entry : exterior_edges) {
        if (long_edges_replaced_by_wide.count(entry.first) > 0) {
            continue;
        }

        if (dir_a_long_edges_covered_by_corner.count(entry.first) > 0) {
            continue;
        }

        const ExteriorCeilingEdge& edge = entry.second;
        ceiling_sprites.push_back(place_single_ceiling_curtain(
            edge.gx,
            edge.gy,
            edge.wall_type,
            edge.kind,
            false,
            edge.outside_side
        ));
    }

    return ceiling_sprites;
}

std::set<WallBuilder::RareWallPosition> WallBuilder::select_rare_wall_positions(
    const std::vector<RawSprite>& raw_sprites
) const {
    std::set<RareWallPosition> selected_positions;
    if (!direction_randomizer_.is_enabled()) {
        return selected_positions;
    }

    using DirectionBudgetKey = std::pair<int, WallPartKind>;
    using LineKey = std::tuple<int, WallPartKind, int>;
    using VertexKey = std::tuple<int, int, int>;

    std::map<DirectionBudgetKey, int> wall_part_counts;
    std::map<LineKey, std::vector<RawSprite>> wall_parts_by_line;
    std::set<VertexKey> corner_vertices;

    // Collect post-excavation wall counts, axis-aligned lines, and topology
    // vertices that split one logical line into separate straight runs.
    for (const RawSprite& raw_sprite : raw_sprites) {
        const WallProfile& profile = get_wall_profile(raw_sprite.wall_type);
        if (!has_rare_wall_variant(profile)) {
            continue;
        }

        bool is_wall_part = raw_sprite.kind == WallPartKind::DirA ||
            raw_sprite.kind == WallPartKind::DirB;
        if (is_wall_part) {
            DirectionBudgetKey budget_key = {raw_sprite.wall_type, raw_sprite.kind};
            wall_part_counts[budget_key] += 1;

            int fixed_coordinate = raw_sprite.gx;
            if (raw_sprite.kind == WallPartKind::DirB) {
                fixed_coordinate = raw_sprite.gy;
            }

            LineKey line_key = {raw_sprite.wall_type, raw_sprite.kind, fixed_coordinate};
            wall_parts_by_line[line_key].push_back(raw_sprite);
            continue;
        }

        bool has_dir_a_connection = raw_sprite.connects_up || raw_sprite.connects_down;
        bool has_dir_b_connection = raw_sprite.connects_left || raw_sprite.connects_right;
        if (has_dir_a_connection && has_dir_b_connection) {
            corner_vertices.insert({raw_sprite.wall_type, raw_sprite.gx, raw_sprite.gy});
        }
    }

    // Density is an upper budget. Spatial rules may intentionally leave part
    // of the budget unused when no visually safe candidate remains.
    std::map<DirectionBudgetKey, int> remaining_budgets;
    for (const auto& [budget_key, wall_part_count] : wall_part_counts) {
        const WallProfile& profile = get_wall_profile(budget_key.first);
        float raw_budget = static_cast<float>(wall_part_count) * profile.rare_target_density;
        int budget = static_cast<int>(std::floor(raw_budget));
        if (budget > 0) {
            remaining_budgets[budget_key] = budget;
        }
    }

    // Split each axis-aligned line at gaps and corners, then keep only the
    // middle candidates that satisfy the configured straight-wall buffer.
    std::vector<StraightWallRun> straight_runs;
    for (auto& [line_key, line_wall_parts] : wall_parts_by_line) {
        int wall_type = std::get<0>(line_key);
        WallPartKind kind = std::get<1>(line_key);
        const WallProfile& profile = get_wall_profile(wall_type);

        std::sort(line_wall_parts.begin(), line_wall_parts.end(), [kind](const RawSprite& left, const RawSprite& right) {
            if (kind == WallPartKind::DirA) {
                return left.gy < right.gy;
            }

            return left.gx < right.gx;
        });

        std::size_t run_start_index = 0;
        for (std::size_t wall_index = 0; wall_index < line_wall_parts.size(); ++wall_index) {
            bool split_after_current = wall_index + 1 == line_wall_parts.size();
            if (!split_after_current) {
                const RawSprite& current_wall = line_wall_parts[wall_index];
                const RawSprite& next_wall = line_wall_parts[wall_index + 1];

                int current_coordinate = current_wall.gy;
                int next_coordinate = next_wall.gy;
                if (kind == WallPartKind::DirB) {
                    current_coordinate = current_wall.gx;
                    next_coordinate = next_wall.gx;
                }

                bool coordinates_are_contiguous = next_coordinate == current_coordinate + 1;
                VertexKey shared_vertex = {wall_type, current_wall.gx, current_wall.gy};
                bool shared_vertex_is_corner = corner_vertices.contains(shared_vertex);
                split_after_current = !coordinates_are_contiguous || shared_vertex_is_corner;
            }

            if (!split_after_current) {
                continue;
            }

            std::size_t run_end_index = wall_index + 1;
            std::size_t run_length = run_end_index - run_start_index;
            int required_run_length = profile.rare_straight_buffer * 2 + 1;
            if (run_length >= static_cast<std::size_t>(required_run_length)) {
                StraightWallRun run = {
                    .wall_type = wall_type,
                    .kind = kind
                };

                std::size_t first_candidate_index = run_start_index + profile.rare_straight_buffer;
                std::size_t last_candidate_index = run_end_index - profile.rare_straight_buffer;
                for (std::size_t candidate_index = first_candidate_index;
                     candidate_index < last_candidate_index;
                     ++candidate_index) {
                    run.candidates.push_back(line_wall_parts[candidate_index]);
                }

                for (std::size_t remaining = run.candidates.size(); remaining > 1; --remaining) {
                    int random_index = Random::get(0, static_cast<int>(remaining) - 1);
                    std::swap(run.candidates[remaining - 1], run.candidates[static_cast<std::size_t>(random_index)]);
                }

                straight_runs.push_back(std::move(run));
            }

            run_start_index = run_end_index;
        }
    }

    // Give every active straight run one randomized opportunity per round.
    // Selected positions are shared by both directions of the same wall set.
    std::map<int, std::vector<MapPoint>> selected_map_positions_by_wall_type;
    bool placed_in_previous_round = true;
    while (placed_in_previous_round) {
        placed_in_previous_round = false;
        std::vector<std::size_t> active_run_indices;

        for (std::size_t run_index = 0; run_index < straight_runs.size(); ++run_index) {
            StraightWallRun& run = straight_runs[run_index];
            DirectionBudgetKey budget_key = {run.wall_type, run.kind};
            bool has_budget = remaining_budgets[budget_key] > 0;
            bool has_candidates = run.next_candidate_index < run.candidates.size();
            if (has_budget && has_candidates) {
                active_run_indices.push_back(run_index);
            }
        }

        for (std::size_t remaining = active_run_indices.size(); remaining > 1; --remaining) {
            int random_index = Random::get(0, static_cast<int>(remaining) - 1);
            std::swap(
                active_run_indices[remaining - 1],
                active_run_indices[static_cast<std::size_t>(random_index)]
            );
        }

        for (std::size_t run_index : active_run_indices) {
            StraightWallRun& run = straight_runs[run_index];
            DirectionBudgetKey budget_key = {run.wall_type, run.kind};
            if (remaining_budgets[budget_key] <= 0) {
                continue;
            }

            const WallProfile& profile = get_wall_profile(run.wall_type);
            const WallVariant& rare_variant = profile.variants[profile.rare_variant_index];
            const WallPartAsset& rare_asset = select_wall_part_asset(rare_variant, run.kind);
            float wall_step_length = std::hypot(profile.step_x, profile.step_y);
            float min_distance = wall_step_length * profile.rare_min_distance_steps;
            float min_distance_squared = min_distance * min_distance;

            while (run.next_candidate_index < run.candidates.size()) {
                const RawSprite& candidate = run.candidates[run.next_candidate_index];
                run.next_candidate_index += 1;

                MapPoint candidate_position = get_phys(candidate.gx, candidate.gy, candidate.wall_type);
                candidate_position.x += rare_asset.offset_x;
                candidate_position.y += rare_asset.offset_y;

                bool is_far_enough = true;
                const std::vector<MapPoint>& selected_map_positions =
                    selected_map_positions_by_wall_type[run.wall_type];
                for (const MapPoint& selected_map_position : selected_map_positions) {
                    float distance_x = candidate_position.x - selected_map_position.x;
                    float distance_y = candidate_position.y - selected_map_position.y;
                    float distance_squared = distance_x * distance_x + distance_y * distance_y;
                    if (distance_squared < min_distance_squared) {
                        is_far_enough = false;
                        break;
                    }
                }

                if (!is_far_enough) {
                    continue;
                }

                RareWallPosition selected_position = {
                    .wall_type = candidate.wall_type,
                    .kind = candidate.kind,
                    .gx = candidate.gx,
                    .gy = candidate.gy
                };
                selected_positions.insert(selected_position);
                selected_map_positions_by_wall_type[run.wall_type].push_back(candidate_position);
                remaining_budgets[budget_key] -= 1;
                placed_in_previous_round = true;
                break;
            }
        }
    }

    return selected_positions;
}

std::vector<io::Sprite> WallBuilder::convert_to_wall_sprites(const std::vector<RawSprite>& raw_sprites) const {
    std::vector<io::Sprite> wall_sprites;
    wall_sprites.reserve(raw_sprites.size());
    std::map<std::tuple<int, int, int>, int> selected_variant_indices;
    std::set<RareWallPosition> rare_wall_positions = select_rare_wall_positions(raw_sprites);

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
        RareWallPosition rare_wall_position = {
            .wall_type = rs.wall_type,
            .kind = rs.kind,
            .gx = rs.gx,
            .gy = rs.gy
        };
        if (is_wall_part && rare_wall_positions.contains(rare_wall_position)) {
            variant_index = profile.rare_variant_index;
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

    // 2. Build the physical grid only when floor generation needs it.
    if (gen_floor) {
        PhysicalGridContext grid_ctx = build_physical_grid(segments);
        floor_sprites = place_floors(segments, grid_ctx);
    }

    // 3. Wall-aligned ceilings do not depend on the floor raster.
    if (gen_ceiling) {
        ceiling_sprites = place_ceilings(segments);
    }

    // 4. project physical coordinates and transform wall sprites
    std::vector<io::Sprite> wall_sprites = convert_to_wall_sprites(raw_sprites);

    // 5. combine all spirit lists
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

io::Sprite WallBuilder::place_single_ceiling_curtain(
    int gx,
    int gy,
    int wall_type,
    WallPartKind kind,
    bool use_wide,
    WallOutsideSide outside_side
) const {
    const CeilingCurtainProfile* curtain_profile = get_ceiling_curtain_profile(wall_type);
    if (curtain_profile == nullptr) {
        Logger::warning("Missing Ceiling Curtain profile for wall_type={}", wall_type);
        return {};
    }

    const WallProfile& wall_profile = get_wall_profile(wall_type);
    const CeilingCurtainPartProfile* part_profile = nullptr;
    float wall_offset_x = 0.0f;
    float wall_offset_y = 0.0f;

    if (kind == WallPartKind::DirA) {
        wall_offset_x = wall_profile.offset_a_x;
        wall_offset_y = wall_profile.offset_a_y;
        if (use_wide) {
            part_profile = &curtain_profile->dir_a_wide;
        } else {
            part_profile = &curtain_profile->dir_a_long;
        }
    }

    if (kind == WallPartKind::DirB) {
        wall_offset_x = wall_profile.offset_b_x;
        wall_offset_y = wall_profile.offset_b_y;
        if (use_wide) {
            part_profile = &curtain_profile->dir_b_wide;
        } else {
            part_profile = &curtain_profile->dir_b_long;
        }
    }

    if (part_profile == nullptr) {
        Logger::warning("Ceiling Curtain cannot attach to pillar wall part");
        return {};
    }

    if (part_profile->use_side_specific_offsets) {
        const CeilingWallOffset* side_offset =
            &part_profile->positive_side_offset;
        if (outside_side == WallOutsideSide::NegativeGridSide) {
            side_offset = &part_profile->negative_side_offset;
        }

        io::Sprite wall_sprite = place_single_wall(gx, gy, wall_type, kind);
        io::Sprite ceiling_sprite(
            curtain_profile->vid,
            wall_sprite.posX + side_offset->x,
            wall_sprite.posY + side_offset->y,
            curtain_profile->pos_z,
            part_profile->direction
        );
        ceiling_sprite.army = 0;
        return ceiling_sprite;
    }

    // Profile offsets come from one calibrated sample side. Convert the offset
    // back to logical-grid axes so the outward component can follow the actual
    // exterior side while the along-wall component stays unchanged.
    float offset_grid_x = (
        part_profile->offset_x / wall_profile.step_x
        + part_profile->offset_y / wall_profile.step_y
    ) / 2.0f;
    float offset_grid_y = (
        part_profile->offset_y / wall_profile.step_y
        - part_profile->offset_x / wall_profile.step_x
    ) / 2.0f;

    if (kind == WallPartKind::DirA) {
        bool offset_points_positive = offset_grid_x > 0.0f;
        bool outside_is_positive = outside_side == WallOutsideSide::PositiveGridSide;
        if (offset_points_positive != outside_is_positive) {
            offset_grid_x = -offset_grid_x;
        }

        if (outside_is_positive) {
            offset_grid_x += part_profile->positive_side_outward_adjustment;
        } else {
            offset_grid_x -= part_profile->negative_side_outward_adjustment;
        }
    }

    if (kind == WallPartKind::DirB) {
        bool offset_points_positive = offset_grid_y > 0.0f;
        bool outside_is_positive = outside_side == WallOutsideSide::PositiveGridSide;
        if (offset_points_positive != outside_is_positive) {
            offset_grid_y = -offset_grid_y;
        }

        if (outside_is_positive) {
            offset_grid_y += part_profile->positive_side_outward_adjustment;
        } else {
            offset_grid_y -= part_profile->negative_side_outward_adjustment;
        }
    }

    float oriented_offset_x = (offset_grid_x - offset_grid_y) * wall_profile.step_x;
    float oriented_offset_y = (offset_grid_x + offset_grid_y) * wall_profile.step_y;

    MapPoint logical_anchor = get_phys(gx, gy, wall_type);
    float pos_x = logical_anchor.x + wall_offset_x + oriented_offset_x;
    float pos_y = logical_anchor.y + wall_offset_y + oriented_offset_y;

    io::Sprite curtain_sprite(
        curtain_profile->vid,
        pos_x,
        pos_y,
        curtain_profile->pos_z,
        part_profile->direction
    );
    curtain_sprite.army = 0;
    return curtain_sprite;
}

int WallBuilder::select_wall_variant_index(const WallProfile& profile) const {
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

    int variant_index = pool.variant_indices[0];
    if (pool.randomize) {
        std::span<const int> variant_indices(
            pool.variant_indices,
            static_cast<std::size_t>(pool.variant_count)
        );
        variant_index = direction_randomizer_.select(
            variant_index,
            variant_indices
        );
    }

    if (variant_index < 0 || variant_index >= profile.variant_count) {
        return 0;
    }

    return variant_index;
}

const WallVariant& WallBuilder::select_wall_variant(const WallProfile& profile) const {
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

    if (profile.rare_target_density <= 0.0f) {
        return false;
    }

    if (profile.rare_straight_buffer < 0) {
        return false;
    }

    if (profile.rare_min_distance_steps <= 0.0f) {
        return false;
    }

    return true;
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

    uint32_t direction = direction_randomizer_.select_direction(
        asset.direction,
        profile.part_direction_randomization
    );

    io::Sprite sprite(asset.vid, pos.x, pos.y, 0.0f, direction);
    if (is_as2_wall_set_type(wall_type)) {
        sprite.army = get_as2_wall_set_asset_army(asset.vid);
    }

    return sprite;
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
