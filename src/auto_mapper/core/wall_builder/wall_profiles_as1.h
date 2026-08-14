/**
 * @file wall_profiles_as1.h
 * @brief AS1-compatible wall and floor profile constants.
 */

#pragma once

#include "auto_mapper/core/wall_builder/wall_profile_types.h"

namespace auto_mapper::core {

inline constexpr int WALL_TYPE_STANDARD = 0;
inline constexpr int WALL_TYPE_LAB = 1;
inline constexpr int WALL_TYPE_STANDARD_DARK = 2;

inline constexpr WallProfile WALL_STANDARD = {
    .dir_a_vid = 601,
    .dir_b_vid = 602,
    .pillar_vid = 604,
    .step_x = 40.0f,
    .step_y = 28.0f,
    .offset_a_x = 0.0f,
    .offset_a_y = 0.0f,
    .offset_b_x = -40.0f,
    .offset_b_y = 0.0f,
    .offset_p_x = 0.0f,
    .offset_p_y = 0.0f,
    .grid_divisor = 1,
    .variants = {
        make_wall_variant(601, 0, 0.0f, 0.0f, 602, 0, -40.0f, 0.0f, 604, 0, 0.0f, 0.0f)
    },
    .variant_count = 1,
    .variant_pools = {
        make_fixed_wall_variant_pool(0)
    },
    .variant_pool_count = 1,
    .active_variant_pool = 0,
    .part_direction_randomization = FULL_RANGE_DIRECTION_RANDOMIZATION
};

inline constexpr WallProfile WALL_LAB = {
    .dir_a_vid = 651,
    .dir_b_vid = 650,
    .pillar_vid = 652,
    .step_x = 90.0f,
    .step_y = 64.0f,
    .offset_a_x = 90.0f,
    .offset_a_y = 0.0f,
    .offset_b_x = 0.0f,
    .offset_b_y = 0.0f,
    .offset_p_x = 0.0f,
    .offset_p_y = 0.0f,
    .grid_divisor = 2,
    .variants = {
        make_wall_variant(651, 0, 90.0f, 0.0f, 650, 0, 0.0f, 0.0f, 652, 0, 0.0f, 0.0f)
    },
    .variant_count = 1,
    .variant_pools = {
        make_fixed_wall_variant_pool(0)
    },
    .variant_pool_count = 1,
    .active_variant_pool = 0,
    .part_direction_randomization = FULL_RANGE_DIRECTION_RANDOMIZATION
};

inline constexpr WallProfile WALL_STANDARD_DARK = {
    .dir_a_vid = 620,
    .dir_b_vid = 621,
    .pillar_vid = 622,
    .step_x = 40.0f,
    .step_y = 28.0f,
    .offset_a_x = 0.0f,
    .offset_a_y = 0.0f,
    .offset_b_x = -40.0f,
    .offset_b_y = 0.0f,
    .offset_p_x = 0.0f,
    .offset_p_y = 0.0f,
    .grid_divisor = 1,
    .variants = {
        make_wall_variant(620, 0, 0.0f, 0.0f, 621, 0, -40.0f, 0.0f, 622, 0, 0.0f, 0.0f)
    },
    .variant_count = 1,
    .variant_pools = {
        make_fixed_wall_variant_pool(0)
    },
    .variant_pool_count = 1,
    .active_variant_pool = 0,
    .part_direction_randomization = FULL_RANGE_DIRECTION_RANDOMIZATION
};

inline constexpr int FLOOR_TYPE_STANDARD = 0;
inline constexpr int FLOOR_TYPE_LAB = 1;
inline constexpr int FLOOR_TYPE_STANDARD_DARK = 2;

inline constexpr FloorProfile FLOOR_STANDARD = {
    .vid = 500,
    .step_x = 40.0f,
    .step_y = 28.0f,
    .pos_z = 0.0f,
    .grid_divisor = 1,
    .direction_randomization = FULL_RANGE_DIRECTION_RANDOMIZATION
};

inline constexpr FloorProfile FLOOR_LAB = {
    .vid = 503,
    .step_x = 80.0f,
    .step_y = 56.0f,
    .pos_z = 0.0f,
    .grid_divisor = 1,
    .direction_randomization = FULL_RANGE_DIRECTION_RANDOMIZATION
};

inline constexpr FloorProfile FLOOR_STANDARD_DARK = {
    .vid = 502,
    .step_x = 40.0f,
    .step_y = 28.0f,
    .pos_z = 0.0f,
    .grid_divisor = 1,
    .direction_randomization = FULL_RANGE_DIRECTION_RANDOMIZATION
};

} // namespace auto_mapper::core
