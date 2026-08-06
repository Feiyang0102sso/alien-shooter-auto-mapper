/**
 * @file door_profiles_as2.h
 * @brief AS2 door profile constants.
 */

#pragma once

#include "door_profile_types.h"

namespace auto_mapper::core {

inline constexpr WallPartAsset NO_COMPENSATION_PILLAR = {
    .vid = 0,
    .direction = 0,
    .offset_x = 0.0f,
    .offset_y = 0.0f
};

inline constexpr As2DoorProfile make_as2_wall_set1_door_profile(uint32_t pillar_direction) {
    return {
        .small = {
            .span_steps = 1,
            .frame_parts = {
                {
                    .vid = 1779,
                    .dir_map = {
                        .dir_a = 51,
                        .dir_b = 0
                    },
                    .offset_a_x = 0.0f,
                    .offset_a_y = 0.0f,
                    .offset_b_x = 0.0f,
                    .offset_b_y = 0.0f
                }
            },
            .frame_part_count = 1,
            .panel = {
                .vid_closed = 1777,
                .vid_open = 1778
            },
            .panel_dir_map = {
                .dir_a = 128,
                .dir_b = 0
            },
            .compensation_pillar_dir_a = NO_COMPENSATION_PILLAR,
            .compensation_pillar_dir_b = NO_COMPENSATION_PILLAR
        },
        .large = {
            .span_steps = 2,
            .frame_parts = {
                {
                    .vid = 1788,
                    .dir_map = {
                        .dir_a = 128,
                        .dir_b = 0
                    },
                    .offset_a_x = 0.0f,
                    .offset_a_y = 0.0f,
                    .offset_b_x = 0.0f,
                    .offset_b_y = 0.0f
                }
            },
            .frame_part_count = 1,
            .panel = {
                .vid_closed = 1785,
                .vid_open = 1786
            },
            .panel_dir_map = {
                .dir_a = 128,
                .dir_b = 0
            },
            .compensation_pillar_dir_a = {
                .vid = 1782,
                .direction = pillar_direction,
                .offset_x = 45.0f,
                .offset_y = -96.0f
            },
            .compensation_pillar_dir_b = {
                .vid = 1782,
                .direction = pillar_direction,
                .offset_x = -135.0f,
                .offset_y = -96.0f
            }
        }
    };
}

inline constexpr As2DoorProfile DOOR_AS2_WALL_SET1_FIXED_0 = make_as2_wall_set1_door_profile(0);
inline constexpr As2DoorProfile DOOR_AS2_WALL_SET1_FIXED_1 = make_as2_wall_set1_door_profile(85);
inline constexpr As2DoorProfile DOOR_AS2_WALL_SET1_RANDOM = make_as2_wall_set1_door_profile(170);

inline constexpr As2DoorProfile make_as2_two_part_large_door_profile(
    int frame_vid,
    uint32_t left_frame_dir_a,
    uint32_t right_frame_dir_a,
    uint32_t left_frame_dir_b,
    uint32_t right_frame_dir_b
) {
    return {
        .small = {
            .span_steps = 1,
            .frame_parts = {},
            .frame_part_count = 0,
            .panel = {
                .vid_closed = 0,
                .vid_open = 0
            },
            .panel_dir_map = {
                .dir_a = 0,
                .dir_b = 0
            },
            .compensation_pillar_dir_a = NO_COMPENSATION_PILLAR,
            .compensation_pillar_dir_b = NO_COMPENSATION_PILLAR
        },
        .large = {
            .span_steps = 2,
            .frame_parts = {
                {
                    .vid = frame_vid,
                    .dir_map = {
                        .dir_a = left_frame_dir_a,
                        .dir_b = left_frame_dir_b
                    },
                    .offset_a_x = -45.0f,
                    .offset_a_y = 32.0f,
                    .offset_b_x = -45.0f,
                    .offset_b_y = -32.0f
                },
                {
                    .vid = frame_vid,
                    .dir_map = {
                        .dir_a = right_frame_dir_a,
                        .dir_b = right_frame_dir_b
                    },
                    .offset_a_x = 45.0f,
                    .offset_a_y = -32.0f,
                    .offset_b_x = 45.0f,
                    .offset_b_y = 32.0f
                }
            },
            .frame_part_count = 2,
            .panel = {
                .vid_closed = 1785,
                .vid_open = 1786
            },
            .panel_dir_map = {
                .dir_a = 128,
                .dir_b = 0
            },
            .compensation_pillar_dir_a = NO_COMPENSATION_PILLAR,
            .compensation_pillar_dir_b = NO_COMPENSATION_PILLAR
        }
    };
}

inline constexpr As2DoorProfile DOOR_AS2_WALL_SET2 = make_as2_two_part_large_door_profile(
    1703,
    128,
    192,
    0,
    64
);

// TODO: need recolor vid
inline constexpr As2DoorProfile DOOR_AS2_WALL_SET3_AND_SET4 = make_as2_two_part_large_door_profile(
    1103,
    64,
    192,
    0,
    128
);

inline constexpr As2DoorProfile make_as2_manual_single_frame_large_door_profile(
    int frame_vid,
    int closed_panel_vid,
    int open_panel_vid,
    uint32_t dir_a,
    uint32_t dir_b,
    float base_offset_a_x = 0.0f,
    float base_offset_a_y = 0.0f,
    float base_offset_b_x = 0.0f,
    float base_offset_b_y = 0.0f,
    float panel_offset_a_x = 0.0f,
    float panel_offset_a_y = 0.0f,
    float panel_offset_b_x = 0.0f,
    float panel_offset_b_y = 0.0f
) {
    return {
        .small = {
            .span_steps = 1,
            .frame_parts = {},
            .frame_part_count = 0,
            .panel = {
                .vid_closed = 0,
                .vid_open = 0
            },
            .panel_dir_map = {
                .dir_a = 0,
                .dir_b = 0
            },
            .compensation_pillar_dir_a = NO_COMPENSATION_PILLAR,
            .compensation_pillar_dir_b = NO_COMPENSATION_PILLAR
        },
        .large = {
            .span_steps = 2,
            .frame_parts = {
                {
                    .vid = frame_vid,
                    .dir_map = {
                        .dir_a = dir_a,
                        .dir_b = dir_b
                    },
                    .offset_a_x = 0.0f,
                    .offset_a_y = 0.0f,
                    .offset_b_x = 0.0f,
                    .offset_b_y = 0.0f
                }
            },
            .frame_part_count = 1,
            .panel = {
                .vid_closed = closed_panel_vid,
                .vid_open = open_panel_vid
            },
            .panel_dir_map = {
                .dir_a = dir_a,
                .dir_b = dir_b
            },
            .use_large_center_offset = false,
            .base_offset_a_x = base_offset_a_x,
            .base_offset_a_y = base_offset_a_y,
            .base_offset_b_x = base_offset_b_x,
            .base_offset_b_y = base_offset_b_y,
            .panel_offset_a_x = panel_offset_a_x,
            .panel_offset_a_y = panel_offset_a_y,
            .panel_offset_b_x = panel_offset_b_x,
            .panel_offset_b_y = panel_offset_b_y,
            .compensation_pillar_dir_a = NO_COMPENSATION_PILLAR,
            .compensation_pillar_dir_b = NO_COMPENSATION_PILLAR
        }
    };
}

inline constexpr As2DoorProfile DOOR_AS2_WALL_SET5 = make_as2_manual_single_frame_large_door_profile(
    2504,
    2506,
    2505,
    128,
    0
);

inline constexpr As2DoorProfile DOOR_AS2_WALL_SET6 = make_as2_manual_single_frame_large_door_profile(
    2604,
    2605,
    2606,
    128,
    0
);

inline constexpr As2DoorProfile DOOR_AS2_WALL_SET7 = make_as2_manual_single_frame_large_door_profile(
    2624,
    2645,
    0,
    128,
    51,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    -79.0f,
    55.0f,
    6.0f,
    -5.0f
);

} // namespace auto_mapper::core
