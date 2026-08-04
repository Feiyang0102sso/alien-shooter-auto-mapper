/**
 * @file wall_profiles_as2.h
 * @brief AS2 wall profile constants.
 */

#pragma once

#include "auto_mapper/core/wall_builder/wall_profile_types.h"

namespace auto_mapper::core {

inline constexpr int WALL_TYPE_AS2_WALL_SET1_FIXED_0 = 3;
inline constexpr int WALL_TYPE_AS2_WALL_SET1_FIXED_1 = 4;
inline constexpr int WALL_TYPE_AS2_WALL_SET1_RANDOM = 5;

inline constexpr WallProfile WALL_AS2_SET1_FIXED_0 = {
    .dir_a_vid = 1781,
    .dir_b_vid = 1780,
    .pillar_vid = 1782,
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
        make_wall_variant(1781, 51, 90.0f, 0.0f, 1780, 0, 0.0f, 0.0f, 1782, 0, 0.0f, 0.0f),
        make_wall_variant(1781, 0, 90.0f, 0.0f, 1780, 51, 0.0f, 0.0f, 1782, 85, 0.0f, 0.0f),
        make_wall_variant(1781, 102, 90.0f, 0.0f, 1780, 102, 0.0f, 0.0f, 1782, 170, 0.0f, 0.0f),
        make_wall_variant(1781, 153, 90.0f, 0.0f, 1780, 153, 0.0f, 0.0f, 1782, 170, 0.0f, 0.0f),
        make_wall_variant(1781, 204, 90.0f, 0.0f, 1780, 204, 0.0f, 0.0f, 1782, 170, 0.0f, 0.0f)
    },
    .variant_count = 5,
    .variant_pools = {
        make_fixed_wall_variant_pool(0),
        make_fixed_wall_variant_pool(1),
        make_random_wall_variant_pool(2, 3, 4)
    },
    .variant_pool_count = 3,
    .active_variant_pool = 0
};

inline constexpr WallProfile WALL_AS2_SET1_FIXED_1 = {
    .dir_a_vid = 1781,
    .dir_b_vid = 1780,
    .pillar_vid = 1782,
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
        make_wall_variant(1781, 51, 90.0f, 0.0f, 1780, 0, 0.0f, 0.0f, 1782, 0, 0.0f, 0.0f),
        make_wall_variant(1781, 0, 90.0f, 0.0f, 1780, 51, 0.0f, 0.0f, 1782, 85, 0.0f, 0.0f),
        make_wall_variant(1781, 102, 90.0f, 0.0f, 1780, 102, 0.0f, 0.0f, 1782, 170, 0.0f, 0.0f),
        make_wall_variant(1781, 153, 90.0f, 0.0f, 1780, 153, 0.0f, 0.0f, 1782, 170, 0.0f, 0.0f),
        make_wall_variant(1781, 204, 90.0f, 0.0f, 1780, 204, 0.0f, 0.0f, 1782, 170, 0.0f, 0.0f)
    },
    .variant_count = 5,
    .variant_pools = {
        make_fixed_wall_variant_pool(0),
        make_fixed_wall_variant_pool(1),
        make_random_wall_variant_pool(2, 3, 4)
    },
    .variant_pool_count = 3,
    .active_variant_pool = 1
};

inline constexpr WallProfile WALL_AS2_SET1_RANDOM = {
    .dir_a_vid = 1781,
    .dir_b_vid = 1780,
    .pillar_vid = 1782,
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
        make_wall_variant(1781, 51, 90.0f, 0.0f, 1780, 0, 0.0f, 0.0f, 1782, 0, 0.0f, 0.0f),
        make_wall_variant(1781, 0, 90.0f, 0.0f, 1780, 51, 0.0f, 0.0f, 1782, 85, 0.0f, 0.0f),
        make_wall_variant(1781, 102, 90.0f, 0.0f, 1780, 102, 0.0f, 0.0f, 1782, 170, 0.0f, 0.0f),
        make_wall_variant(1781, 153, 90.0f, 0.0f, 1780, 153, 0.0f, 0.0f, 1782, 170, 0.0f, 0.0f),
        make_wall_variant(1781, 204, 90.0f, 0.0f, 1780, 204, 0.0f, 0.0f, 1782, 170, 0.0f, 0.0f)
    },
    .variant_count = 5,
    .variant_pools = {
        make_fixed_wall_variant_pool(0),
        make_fixed_wall_variant_pool(1),
        make_random_wall_variant_pool(2, 3, 4)
    },
    .variant_pool_count = 3,
    .active_variant_pool = 2
};

} // namespace auto_mapper::core
