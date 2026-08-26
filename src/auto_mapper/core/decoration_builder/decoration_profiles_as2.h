/**
 * @file decoration_profiles_as2.h
 * @brief Authored AS2 decoration stamp profiles.
 */

#pragma once

#include "auto_mapper/core/decoration_builder/decoration_profile_types.h"
#include "auto_mapper/core/wall_builder/wall_profiles_as2.h"

namespace auto_mapper::core::decoration_builder {

/**
 * Dormitory room imported from _prepare/AS2/decoration/dorms01.map.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile DORMS01_DECORATION_PROFILE = {
    .id = "dorms01",
    .frame = {
        .corner_offsets = {{
            {   0.0f, -448.0f },      // top
            {-630.0f,    0.0f },      // left
            {   0.0f,  448.0f },      // bottom
            { 630.0f,    0.0f },      // right
        }},
    },
    // Slides the furniture inside the room along the wall directions, in wall
    // steps. The preview frame is not affected. Keep at zero to place as authored.
    .member_nudge = {
        .along_dir_a = 0.00f,
        .along_dir_b = -0.15f,
    },
    .members = {
        {
            .vid = 2357,
            .offset_x = 64.24999128069203f,
            .offset_y = 108.0f,
            .pos_z = 0.0f,
            .direction = 70,
        },
        {
            .vid = 2300,
            .offset_x = 7.249991280692029f,
            .offset_y = 59.0f,
            .pos_z = 0.0f,
            .direction = 76,
        },
        {
            .vid = 1764,
            .offset_x = -7.750008719307971f,
            .offset_y = 259.0f,
            .pos_z = 0.0f,
            .direction = 25,
        },
        {
            .vid = 1288,
            .offset_x = -40.75000871930797f,
            .offset_y = 263.0f,
            .pos_z = 13.0f,
            .direction = 64,
        },
        {
            .vid = 2357,
            .offset_x = -215.75000871930797f,
            .offset_y = 60.0f,
            .pos_z = 0.0f,
            .direction = 70,
        },
        {
            .vid = 2300,
            .offset_x = -272.75000871930797f,
            .offset_y = 15.0f,
            .pos_z = 0.0f,
            .direction = 38,
        },
        {
            .vid = 413,
            .offset_x = -462.75000871930797f,
            .offset_y = 10.0f,
            .pos_z = 0.0f,
            .direction = 64,
        },
        {
            .vid = 412,
            .offset_x = 337.24999128069203f,
            .offset_y = -137.0f,
            .pos_z = 0.0f,
            .direction = 68,
        },
        {
            .vid = 2304,
            .offset_x = 237.24999128069203f,
            .offset_y = -201.0f,
            .pos_z = 0.0f,
            .direction = 76,
        },
        {
            .vid = 2304,
            .offset_x = 196.24999128069203f,
            .offset_y = -230.0f,
            .pos_z = 0.0f,
            .direction = 59,
        },
        {
            .vid = 2357,
            .offset_x = 197.24999128069203f,
            .offset_y = -1.0f,
            .pos_z = 0.0f,
            .direction = 70,
        },
        {
            .vid = 2304,
            .offset_x = 277.24999128069203f,
            .offset_y = -175.0f,
            .pos_z = 0.0f,
            .direction = 110,
        },
        {
            .vid = 2300,
            .offset_x = 139.24999128069203f,
            .offset_y = -47.0f,
            .pos_z = 0.0f,
            .direction = 76,
        },
        {
            .vid = 2300,
            .offset_x = -34.75000871930797f,
            .offset_y = -161.0f,
            .pos_z = 0.0f,
            .direction = 76,
        },
        {
            .vid = 413,
            .offset_x = -144.75000871930797f,
            .offset_y = -205.0f,
            .pos_z = 0.0f,
            .direction = 192,
        },
        {
            .vid = 413,
            .offset_x = -164.75000871930797f,
            .offset_y = -189.0f,
            .pos_z = 0.0f,
            .direction = 192,
        },
        {
            .vid = 413,
            .offset_x = -185.75000871930797f,
            .offset_y = -175.0f,
            .pos_z = 0.0f,
            .direction = 192,
        },
        {
            .vid = 413,
            .offset_x = -277.75000871930797f,
            .offset_y = -110.0f,
            .pos_z = 0.0f,
            .direction = 192,
        },
        {
            .vid = 413,
            .offset_x = -315.75000871930797f,
            .offset_y = -81.0f,
            .pos_z = 0.0f,
            .direction = 192,
        },
        {
            .vid = 413,
            .offset_x = -297.75000871930797f,
            .offset_y = -96.0f,
            .pos_z = 0.0f,
            .direction = 192,
        },
        {
            .vid = 2300,
            .offset_x = -154.75000871930797f,
            .offset_y = -66.0f,
            .pos_z = 0.0f,
            .direction = 166,
        },
        {
            .vid = 2357,
            .offset_x = -100.75000871930797f,
            .offset_y = -19.0f,
            .pos_z = 0.0f,
            .direction = 70,
        },
        {
            .vid = 413,
            .offset_x = -412.75000871930797f,
            .offset_y = -20.0f,
            .pos_z = 0.0f,
            .direction = 64,
        },
        {
            .vid = 2304,
            .offset_x = 29.24999128069203f,
            .offset_y = -306.0f,
            .pos_z = 0.0f,
            .direction = 0,
        },
        {
            .vid = 409,
            .offset_x = -50.75000871930797f,
            .offset_y = -258.0f,
            .pos_z = 0.0f,
            .direction = 128,
        },
        {
            .vid = 1289,
            .offset_x = 14.24999128069203f,
            .offset_y = 307.0f,
            .pos_z = 0.0f,
            .direction = 48,
        },
        {
            .vid = 1270,
            .offset_x = 21.24999128069203f,
            .offset_y = -113.0f,
            .pos_z = 0.0f,
            .direction = 128,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET6_RANDOM,
};

/**
 * Registry of every authored AS2 decoration profile.
 * The C API enumerates this table, so new profiles only need to be listed here.
 */
static const DecorationProfile* const AS2_DECORATION_PROFILES[] = {
    &DORMS01_DECORATION_PROFILE,
};

inline constexpr int AS2_DECORATION_PROFILE_COUNT =
    static_cast<int>(sizeof(AS2_DECORATION_PROFILES) / sizeof(AS2_DECORATION_PROFILES[0]));

} // namespace auto_mapper::core::decoration_builder
