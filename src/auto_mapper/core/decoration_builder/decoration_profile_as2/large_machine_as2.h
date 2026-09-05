/**
 * @file large_machine_as2.h
 * @brief Authored AS2 large machine room decoration stamp profiles.
 */

#pragma once

#include "auto_mapper/core/decoration_builder/decoration_profile_types.h"
#include "auto_mapper/core/wall_builder/wall_profiles_as2.h"

namespace auto_mapper::core::decoration_builder {

/**
 * Large machine room imported from _prepare/AS2/decoration/11large_machine/large_m01.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile LARGE_M01_DECORATION_PROFILE = {
    .id = "large_m01",
    .frame = {
        .corner_offsets = {{
            {           135.0f,          -480.0f },      // top
            {          -675.0f,            96.0f },      // left
            {          -135.0f,           480.0f },      // bottom
            {           675.0f,           -96.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2310,
            .offset_x = 40.833008f,
            .offset_y = 31.333252f,
            .direction = 192,
        },
        {
            .vid = 2311,
            .offset_x = 29.833008f,
            .offset_y = 69.333252f,
            .direction = 171,
        },
        {
            .vid = 2311,
            .offset_x = 134.833008f,
            .offset_y = -4.666748f,
            .direction = 171,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Large machine room imported from _prepare/AS2/decoration/11large_machine/large_m02.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile LARGE_M02_DECORATION_PROFILE = {
    .id = "large_m02",
    .frame = {
        .corner_offsets = {{
            {          -225.0f,          -736.0f },      // top
            {         -1035.0f,          -160.0f },      // left
            {           225.0f,           736.0f },      // bottom
            {          1035.0f,           160.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2310,
            .offset_x = 188.651001f,
            .offset_y = 108.432129f,
        },
        {
            .vid = 2310,
            .offset_x = 84.65094f,
            .offset_y = 34.432129f,
        },
        {
            .vid = 2310,
            .offset_x = -11.348938f,
            .offset_y = -32.567383f,
        },
        {
            .vid = 2310,
            .offset_x = -112.348877f,
            .offset_y = -101.567383f,
        },
        {
            .vid = 2337,
            .offset_x = 377.663574f,
            .offset_y = 238.817871f,
        },
        {
            .vid = 2311,
            .offset_x = 180.663696f,
            .offset_y = 154.817871f,
            .pos_z = 5.0f,
            .direction = 128,
        },
        {
            .vid = 2311,
            .offset_x = 286.663635f,
            .offset_y = 78.817871f,
            .pos_z = 5.0f,
            .direction = 128,
        },
        {
            .vid = 2339,
            .offset_x = 374.663574f,
            .offset_y = 414.817871f,
            .pos_z = 180.0f,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

} // namespace auto_mapper::core::decoration_builder
