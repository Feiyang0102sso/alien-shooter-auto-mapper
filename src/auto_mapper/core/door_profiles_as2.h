/**
 * @file door_profiles_as2.h
 * @brief AS2 door profile constants.
 */

#pragma once

#include "auto_mapper/core/door_profile_types.h"

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
            .vid_frame = 1779,
            .panel = {
                .vid_closed = 1777,
                .vid_open = 1778
            },
            .frame_dir_map = {
                .dir_a = 51,
                .dir_b = 0
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
            .vid_frame = 1788,
            .panel = {
                .vid_closed = 1785,
                .vid_open = 1786
            },
            .frame_dir_map = {
                .dir_a = 128,
                .dir_b = 0
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

} // namespace auto_mapper::core
