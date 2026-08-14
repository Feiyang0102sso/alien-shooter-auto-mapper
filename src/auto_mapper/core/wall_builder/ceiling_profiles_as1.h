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
    .right_lower_corner_dir_b_supplement_count = 2
};

} // namespace auto_mapper::core
