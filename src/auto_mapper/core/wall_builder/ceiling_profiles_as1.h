/**
 * @file ceiling_profiles_as1.h
 * @brief AS1 ceiling tile profiles and alignment constants.
 */

#pragma once

#include "auto_mapper/core/wall_builder/wall_profile_types.h"

namespace auto_mapper::core {

// Lab walls use a 90x64 pitch while VID 504 uses an 80x56 visual footprint.
// Every straight run stays wall-aligned, then simple square supplements close
// the four convex corners without moving the existing run tiles.
inline constexpr AS1CeilingProfile CEILING_AS1_LAB = {
    .vid = 504,
    .pos_z = 90.0f,
    .step_x = 80.0f,
    .step_y = 56.0f,
    // Outward expansion uses 2D BFS wavefront layers.
    .total_layer_count = 6,
    .grid_snapping_start_layer = 3,
    .min_bounds_margin = -20.0f,
    .max_bounds_margin = 30.0f,
    .dir_a_negative_outside_offset = {-72.0f, -6.0f},
    .dir_a_positive_outside_offset = {38.0f, 46.0f},
    .dir_b_negative_outside_offset = {23.0f, -22.0f},
    .dir_b_positive_outside_offset = {-37.0f, 42.0f},
    .dir_a_step_adjustment = {10.0f, -8.0f},
    .dir_b_step_adjustment = {-10.0f, -8.0f},
    .left_upper_corner_dir_a_supplement_count = 1,
    .left_upper_corner_dir_b_supplement_count = 1,
    .left_lower_corner_dir_a_supplement_count = 1,
    .left_lower_corner_dir_b_supplement_count = 1,
    .right_upper_corner_dir_a_supplement_count = 1,
    .right_upper_corner_dir_b_supplement_count = 2,
    .right_lower_corner_dir_a_supplement_count = 2,
    .right_lower_corner_dir_b_supplement_count = 2,
    .upper_recess = {
        .left_corner = {
            .keep_connector_ceiling = true,
            .keep_side_ceiling = true,
            .supplement_count = 0,
            .away_from_corner_adjustment = 0.0f
        },
        .right_corner = {
            .keep_connector_ceiling = true,
            .keep_side_ceiling = false,
            .supplement_count = 1,
            .away_from_corner_adjustment = -1.05f
        }
    },
    .lower_recess = {
        .left_corner = {
            .keep_connector_ceiling = true,
            .keep_side_ceiling = true,
            .supplement_count = 0,
            .away_from_corner_adjustment = 0.0f
        },
        .right_corner = {
            .keep_connector_ceiling = true,
            .keep_side_ceiling = true,
            .supplement_count = 1,
            .away_from_corner_adjustment = -0.5f
        }
    },
    .left_recess = {
        .upper_corner = {
            .keep_connector_ceiling = true,
            .keep_side_ceiling = true,
            .supplement_count = 0,
            .away_from_corner_adjustment = 0.0f
        },
        .lower_corner = {
            .keep_connector_ceiling = false,
            .keep_side_ceiling = true,
            .supplement_count = 1,
            .away_from_corner_adjustment = -0.7f
        }
    },
    .right_recess = {
        .upper_corner = {
            .keep_connector_ceiling = true,
            .keep_side_ceiling = true,
            .supplement_count = 0,
            .away_from_corner_adjustment = 0.0f
        },
        .lower_corner = {
            .keep_connector_ceiling = true,
            .keep_side_ceiling = true,
            .supplement_count = 0,
            .away_from_corner_adjustment = 0.0f
        }
    },
    .standalone_deep_corners = {
        .upper_left = {
            .dir_a_supplement_count = 0,
            .dir_a_away_from_corner_adjustment = 0.0f,
            .dir_b_supplement_count = 0,
            .dir_b_away_from_corner_adjustment = 0.0f
        },
        .upper_right = {
            .dir_a_supplement_count = 0,
            .dir_a_away_from_corner_adjustment = 0.0f,
            .dir_b_supplement_count = 1,
            .dir_b_away_from_corner_adjustment = -0.5f
        },
        .lower_left = {
            .dir_a_supplement_count = 0,
            .dir_a_away_from_corner_adjustment = 0.0f,
            .dir_b_supplement_count = 0,
            .dir_b_away_from_corner_adjustment = 0.0f
        },
        .lower_right = {
            .dir_a_supplement_count = 1,
            .dir_a_away_from_corner_adjustment = -0.65f,
            .dir_b_supplement_count = 0,
            .dir_b_away_from_corner_adjustment = 0.0f
        }
    }
};

} // namespace auto_mapper::core
