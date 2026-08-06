/**
 * @file door_profiles_as1.h
 * @brief AS1 door profile constants.
 */

#pragma once

#include "auto_mapper/core/door_profile_types.h"

namespace auto_mapper::core {

inline constexpr StandardDoorProfile DOOR_STANDARD = {
    .small = {
        .span_steps = 1,
        .vid_frame = 606,
        .panel = {
            .vid_active_panel = 605,
            .vid_dead_panel = 617
        },
        .jam_z_range = {.min_z = -51.0f, .max_z = -24.0f},
        .dead_open_z_offset = {.min_z = -63.0f, .max_z = -63.0f}
    },
    .large = {
        .span_steps = 2,
        .vid_frame = 608,
        .panel = {
            .vid_active_panel = 607,
            .vid_dead_panel = 611
        },
        .jam_z_range = {.min_z = -50.0f, .max_z = -28.0f},
        .dead_open_z_offset = {.min_z = -68.0f, .max_z = -68.0f}
    },
    .vid_light_green = 423,
    .vid_light_red = 424,
    .vid_light_broken = 425,
    .frame_dir_map = {
        .dir_a = 0,
        .dir_b = 128
    },
    .panel_dir_map = {
        .dir_a = 0,
        .dir_b = 64
    }
};

inline constexpr StandardDoorProfile DOOR_STANDARD_DARK = {
    .small = {
        .span_steps = 1,
        .vid_frame = 623,
        .panel = {
            .vid_active_panel = 605,
            .vid_dead_panel = 617
        },
        .jam_z_range = {.min_z = -51.0f, .max_z = -24.0f},
        .dead_open_z_offset = {.min_z = -63.0f, .max_z = -63.0f}
    },
    .large = {
        .span_steps = 2,
        .vid_frame = 624,
        .panel = {
            .vid_active_panel = 607,
            .vid_dead_panel = 611
        },
        .jam_z_range = {.min_z = -50.0f, .max_z = -28.0f},
        .dead_open_z_offset = {.min_z = -68.0f, .max_z = -68.0f}
    },
    .vid_light_green = 423,
    .vid_light_red = 424,
    .vid_light_broken = 425,
    .frame_dir_map = {
        .dir_a = 0,
        .dir_b = 128
    },
    .panel_dir_map = {
        .dir_a = 0,
        .dir_b = 64
    }
};

inline constexpr LabLaserDoorProfile DOOR_LAB_LASER = {
    .span_steps = 1,
    .vid_frame = 653,
    .vid_laser_closed = 164,
    .laser_offset_x = 0.0f,
    .laser_offset_y = -18.0f,
    .frame_dir_map = {
        .dir_a = 64,
        .dir_b = 0
    },
    .laser_dir_map = {
        .dir_a = 128,
        .dir_b = 0
    }
};

inline constexpr LabDecorationDoorProfile DOOR_LAB_DECORATION = {
    .span_steps = 1,
    .vid_frame = 654,
    .frame_dir_map = {
        .dir_a = 64,
        .dir_b = 0
    }
};

} // namespace auto_mapper::core
