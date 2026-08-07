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
inline constexpr int WALL_TYPE_AS2_WALL_SET2_RANDOM = 6;
inline constexpr int WALL_TYPE_AS2_WALL_SET3_RANDOM = 7;
inline constexpr int WALL_TYPE_AS2_WALL_SET4_RANDOM = 8;
inline constexpr int WALL_TYPE_AS2_WALL_SET5_RANDOM = 9;
inline constexpr int WALL_TYPE_AS2_WALL_SET6_RANDOM = 10;
inline constexpr int WALL_TYPE_AS2_WALL_SET7_RANDOM = 11;
inline constexpr int WALL_TYPE_AS2_WALL_SET8_RANDOM = 12;
inline constexpr int WALL_TYPE_AS2_WALL_SET9_RANDOM = 13;

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

inline constexpr WallProfile WALL_AS2_SET2_RANDOM = {
    .dir_a_vid = 1701,
    .dir_b_vid = 1700,
    .pillar_vid = 1702,
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
        make_wall_variant(1701, 0, 90.0f, 0.0f, 1700, 0, 0.0f, 0.0f, 1702, 0, 0.0f, 0.0f),
        make_wall_variant(1701, 51, 90.0f, 0.0f, 1700, 51, 0.0f, 0.0f, 1702, 0, 0.0f, 0.0f),
        make_wall_variant(1701, 102, 90.0f, 0.0f, 1700, 102, 0.0f, 0.0f, 1702, 0, 0.0f, 0.0f),
        make_wall_variant(1701, 153, 90.0f, 0.0f, 1700, 153, 0.0f, 0.0f, 1702, 0, 0.0f, 0.0f),
        make_wall_variant(1701, 204, 90.0f, 0.0f, 1700, 204, 0.0f, 0.0f, 1702, 0, 0.0f, 0.0f)
    },
    .variant_count = 5,
    .variant_pools = {
        {
            .variant_indices = {0, 1, 2, 3, 4},
            .variant_count = 5,
            .randomize = true
        }
    },
    .variant_pool_count = 1,
    .active_variant_pool = 0,
    .pillar_mode = WallPillarMode::DirectionalSlices,
    .pillar_slice_up = make_wall_part_asset(1702, 64, 90.0f, 0.0f),
    .pillar_slice_down = make_wall_part_asset(1702, 192, 0.0f, 64.0f),
    .pillar_slice_left = make_wall_part_asset(1702, 0, 0.0f, 0.0f),
    .pillar_slice_right = make_wall_part_asset(1702, 128, 90.0f, 64.0f),
    .randomize_wall_parts_independently = true
};

inline constexpr WallProfile WALL_AS2_SET3_RANDOM = {
    .dir_a_vid = 1100,
    .dir_b_vid = 1101,
    .pillar_vid = 1102,
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
        make_wall_variant(1100, 0, 90.0f, 0.0f, 1101, 0, 0.0f, 0.0f, 1102, 0, 0.0f, 0.0f),
        make_wall_variant(1100, 51, 90.0f, 0.0f, 1101, 102, 0.0f, 0.0f, 1102, 0, 0.0f, 0.0f),
        make_wall_variant(1100, 102, 90.0f, 0.0f, 1101, 153, 0.0f, 0.0f, 1102, 0, 0.0f, 0.0f),
        make_wall_variant(1100, 153, 90.0f, 0.0f, 1101, 51, 0.0f, 0.0f, 1102, 0, 0.0f, 0.0f)
    },
    .variant_count = 4,
    .variant_pools = {
        make_random_wall_variant_pool(0, 1, 2)
    },
    .variant_pool_count = 1,
    .active_variant_pool = 0,
    .randomize_wall_parts_independently = true,
    .rare_variant_index = 3,
    .rare_variant_min_interval = 3,
    .rare_variant_max_interval = 7
};

inline constexpr WallProfile WALL_AS2_SET4_RANDOM = {
    .dir_a_vid = 1131,
    .dir_b_vid = 1130,
    .pillar_vid = 1132,
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
        make_wall_variant(1131, 0, 90.0f, 0.0f, 1130, 0, 0.0f, 0.0f, 1132, 51, 0.0f, 0.0f),
        make_wall_variant(1131, 51, 90.0f, 0.0f, 1130, 102, 0.0f, 0.0f, 1132, 51, 0.0f, 0.0f),
        make_wall_variant(1131, 102, 90.0f, 0.0f, 1130, 153, 0.0f, 0.0f, 1132, 51, 0.0f, 0.0f),
        make_wall_variant(1131, 204, 90.0f, 0.0f, 1130, 204, 0.0f, 0.0f, 1132, 51, 0.0f, 0.0f),
        make_wall_variant(1131, 153, 90.0f, 0.0f, 1130, 51, 0.0f, 0.0f, 1132, 51, 0.0f, 0.0f)
    },
    .variant_count = 5,
    .variant_pools = {
        {
            .variant_indices = {0, 1, 2, 3},
            .variant_count = 4,
            .randomize = true
        }
    },
    .variant_pool_count = 1,
    .active_variant_pool = 0,
    .randomize_wall_parts_independently = true,
    .rare_variant_index = 4,
    .rare_variant_min_interval = 3,
    .rare_variant_max_interval = 7
};

inline constexpr WallProfile WALL_AS2_SET5_RANDOM = {
    .dir_a_vid = 2501,
    .dir_b_vid = 2500,
    .pillar_vid = 2502,
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
        make_wall_variant(2501, 0, 90.0f, 0.0f, 2500, 0, 0.0f, 0.0f, 2502, 128, 0.0f, 0.0f),
        make_wall_variant(2501, 102, 90.0f, 0.0f, 2500, 102, 0.0f, 0.0f, 2502, 128, 0.0f, 0.0f),
        make_wall_variant(2501, 153, 90.0f, 0.0f, 2500, 153, 0.0f, 0.0f, 2502, 128, 0.0f, 0.0f),
        make_wall_variant(2501, 204, 90.0f, 0.0f, 2500, 204, 0.0f, 0.0f, 2502, 128, 0.0f, 0.0f),
        make_wall_variant(2501, 51, 90.0f, 0.0f, 2500, 51, 0.0f, 0.0f, 2502, 128, 0.0f, 0.0f)
    },
    .variant_count = 5,
    .variant_pools = {
        {
            .variant_indices = {0, 1, 2, 3},
            .variant_count = 4,
            .randomize = true
        }
    },
    .variant_pool_count = 1,
    .active_variant_pool = 0,
    .randomize_wall_parts_independently = true,
    .rare_variant_index = 4,
    .rare_variant_min_interval = 3,
    .rare_variant_max_interval = 3,
    .use_corner_pillar_assets = true,
    .pillar_corner_down_right = make_wall_part_asset(2502, 128, 0.0f, 0.0f),
    .pillar_corner_down_left = make_wall_part_asset(2502, 51, 0.0f, 0.0f),
    .pillar_corner_up_left = make_wall_part_asset(2502, 0, 0.0f, 0.0f),
    .pillar_corner_up_right = make_wall_part_asset(2502, 192, 0.0f, 0.0f)
};

inline constexpr WallProfile WALL_AS2_SET6_RANDOM = {
    .dir_a_vid = 2601,
    .dir_b_vid = 2600,
    .pillar_vid = 2602,
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
        make_wall_variant(2601, 0, 90.0f, 0.0f, 2600, 0, 0.0f, 0.0f, 2602, 128, 0.0f, 0.0f),
        make_wall_variant(2601, 51, 90.0f, 0.0f, 2600, 51, 0.0f, 0.0f, 2602, 128, 0.0f, 0.0f),
        make_wall_variant(2601, 102, 90.0f, 0.0f, 2600, 102, 0.0f, 0.0f, 2602, 128, 0.0f, 0.0f),
        make_wall_variant(2601, 153, 90.0f, 0.0f, 2600, 153, 0.0f, 0.0f, 2602, 128, 0.0f, 0.0f),
        make_wall_variant(2601, 204, 90.0f, 0.0f, 2600, 204, 0.0f, 0.0f, 2602, 128, 0.0f, 0.0f)
    },
    .variant_count = 5,
    .variant_pools = {
        {
            .variant_indices = {0, 1, 2, 3, 4},
            .variant_count = 5,
            .randomize = true
        }
    },
    .variant_pool_count = 1,
    .active_variant_pool = 0,
    .randomize_wall_parts_independently = true,
    .use_corner_pillar_assets = true,
    .skip_unmapped_pillars = true,
    .pillar_corner_up_left = make_wall_part_asset(2602, 0, 0.0f, 0.0f),
};

inline constexpr WallProfile WALL_AS2_SET7_RANDOM = {
    .dir_a_vid = 2621,
    .dir_b_vid = 2620,
    .pillar_vid = 2622,
    .step_x = 90.0f,
    .step_y = 64.0f,
    .offset_a_x = 90.0f,
    .offset_a_y = 0.0f,
    .offset_b_x = 0.0f,
    .offset_b_y = 0.0f,
    .offset_p_x = -0.5f,
    .offset_p_y = 64.0f,
    .grid_divisor = 2,
    .variants = {
        make_wall_variant(2621, 51, 90.0f, 0.0f, 2620, 51, 0.0f, 0.0f, 2622, 204, -0.5f, 64.0f),
        make_wall_variant(2621, 102, 90.0f, 0.0f, 2620, 102, 0.0f, 0.0f, 2622, 204, -0.5f, 64.0f),
        make_wall_variant(2621, 0, 90.0f, 0.0f, 2620, 0, 0.0f, 0.0f, 2622, 204, -0.5f, 64.0f)
    },
    .variant_count = 3,
    .variant_pools = {
        {
            .variant_indices = {0, 1},
            .variant_count = 2,
            .randomize = true
        }
    },
    .variant_pool_count = 1,
    .active_variant_pool = 0,
    .randomize_wall_parts_independently = true,
    .rare_variant_index = 2,
    .rare_variant_min_interval = 3,
    .rare_variant_max_interval = 3,
    .door_flank_clear = 1,
    .door_flank_variant_index = 1
};

inline constexpr WallProfile WALL_AS2_SET8_RANDOM = {
    .dir_a_vid = 2641,
    .dir_b_vid = 2640,
    .pillar_vid = 2642,
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
        make_wall_variant(2641, 0, 90.0f, 0.0f, 2640, 0, 0.0f, 0.0f, 2642, 128, 0.0f, 0.0f),
        make_wall_variant(2641, 51, 90.0f, 0.0f, 2640, 51, 0.0f, 0.0f, 2642, 128, 0.0f, 0.0f),
        make_wall_variant(2641, 102, 90.0f, 0.0f, 2640, 102, 0.0f, 0.0f, 2642, 128, 0.0f, 0.0f),
        make_wall_variant(2641, 153, 90.0f, 0.0f, 2640, 153, 0.0f, 0.0f, 2642, 128, 0.0f, 0.0f),
        make_wall_variant(2641, 204, 90.0f, 0.0f, 2640, 204, 0.0f, 0.0f, 2642, 128, 0.0f, 0.0f)
    },
    .variant_count = 5,
    .variant_pools = {
        {
            .variant_indices = {0, 1, 2, 3, 4},
            .variant_count = 5,
            .randomize = true
        }
    },
    .variant_pool_count = 1,
    .active_variant_pool = 0,
    .randomize_wall_parts_independently = true,
    .use_corner_pillar_assets = true,
    .pillar_corner_down_right = make_wall_part_asset(2642, 128, 82.0f, 75.0f),
    .pillar_corner_down_left = make_wall_part_asset(2642, 64, -5.0f, 68.0f),
    .pillar_corner_up_left = make_wall_part_asset(2642, 0, 6.0f, 4.0f),
    .pillar_corner_up_right = make_wall_part_asset(2642, 192, 95.0f, 12.0f)
};

inline constexpr WallProfile WALL_AS2_SET9_RANDOM = {
    .dir_a_vid = 1721,
    .dir_b_vid = 1720,
    .pillar_vid = 0,
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
        make_wall_variant(1721, 0, 90.0f, 0.0f, 1720, 0, 0.0f, 0.0f, 0, 0, 0.0f, 0.0f),
        make_wall_variant(1721, 51, 90.0f, 0.0f, 1720, 51, 0.0f, 0.0f, 0, 0, 0.0f, 0.0f),
        make_wall_variant(1721, 102, 90.0f, 0.0f, 1720, 102, 0.0f, 0.0f, 0, 0, 0.0f, 0.0f),
        make_wall_variant(1721, 153, 90.0f, 0.0f, 1720, 153, 0.0f, 0.0f, 0, 0, 0.0f, 0.0f),
        make_wall_variant(1721, 204, 90.0f, 0.0f, 1720, 204, 0.0f, 0.0f, 0, 0, 0.0f, 0.0f)
    },
    .variant_count = 5,
    .variant_pools = {
        {
            .variant_indices = {0, 1, 2, 3},
            .variant_count = 4,
            .randomize = true
        }
    },
    .variant_pool_count = 1,
    .active_variant_pool = 0,
    .randomize_wall_parts_independently = true,
    .corner_wall_variant_index = 0,
    .door_flank_clear = 1,
    .door_flank_variant_index = 2
};

} // namespace auto_mapper::core
