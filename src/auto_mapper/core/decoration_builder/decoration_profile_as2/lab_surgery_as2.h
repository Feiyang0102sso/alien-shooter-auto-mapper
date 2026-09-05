/**
 * @file lab_surgery_as2.h
 * @brief Authored AS2 laboratory surgery room decoration stamp profiles.
 */

#pragma once

#include "auto_mapper/core/decoration_builder/decoration_profile_types.h"
#include "auto_mapper/core/wall_builder/wall_profiles_as2.h"

namespace auto_mapper::core::decoration_builder {

/**
 * Laboratory surgery room imported from _prepare/AS2/decoration/16lab_surgery/lab_surgery01.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile LAB_SURGERY01_DECORATION_PROFILE = {
    .id = "lab_surgery01",
    .frame = {
        .corner_offsets = {{
            {            90.0f,          -256.0f },      // top
            {          -360.0f,            64.0f },      // left
            {           -90.0f,           256.0f },      // bottom
            {           360.0f,           -64.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2325,
            .offset_x = 177.5f,
            .offset_y = 48.166504f,
        },
        {
            .vid = 2335,
            .offset_x = 42.5f,
            .offset_y = 33.166504f,
            .direction = 172,
        },
        {
            .vid = 2325,
            .offset_x = 115.5f,
            .offset_y = 94.166504f,
        },
        {
            .vid = 2322,
            .offset_x = 54.5f,
            .offset_y = 28.166504f,
            .direction = 43,
        },
        {
            .vid = 2515,
            .offset_x = -72.0f,
            .offset_y = 112.0f,
        },
        {
            .vid = 2515,
            .offset_x = 161.5f,
            .offset_y = -58.833496f,
            .gamma = {
                .r = 20,
                .g = 20,
                .b = 30,
                .a = 0,
            },
        },
        {
            .vid = 2334,
            .offset_x = 0.5f,
            .offset_y = -12.833496f,
            .direction = 79,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Laboratory surgery room imported from _prepare/AS2/decoration/16lab_surgery/lab_surgery02.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile LAB_SURGERY02_DECORATION_PROFILE = {
    .id = "lab_surgery02",
    .frame = {
        .corner_offsets = {{
            {            45.0f,          -352.0f },      // top
            {          -495.0f,            32.0f },      // left
            {           -45.0f,           352.0f },      // bottom
            {           495.0f,           -32.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2515,
            .offset_x = 76.83252f,
            .offset_y = 140.145996f,
            .direction = 128,
            .gamma = {
                .r = 20,
                .g = 20,
                .b = 30,
                .a = 0,
            },
        },
        {
            .vid = 2325,
            .offset_x = 107.418945f,
            .offset_y = 47.60791f,
            .direction = 160,
        },
        {
            .vid = 2515,
            .offset_x = 207.83252f,
            .offset_y = 53.145996f,
            .direction = 128,
            .gamma = {
                .r = 20,
                .g = 20,
                .b = 30,
                .a = 0,
            },
        },
        {
            .vid = 2335,
            .offset_x = 46.438477f,
            .offset_y = 1.240723f,
            .direction = 164,
        },
        {
            .vid = 2325,
            .offset_x = 53.418945f,
            .offset_y = 94.60791f,
            .direction = 27,
        },
        {
            .vid = 2325,
            .offset_x = -58.581055f,
            .offset_y = -7.39209f,
            .direction = 128,
        },
        {
            .vid = 2322,
            .offset_x = 28.418945f,
            .offset_y = 16.60791f,
            .direction = 192,
        },
        {
            .vid = 2515,
            .offset_x = -98.5f,
            .offset_y = -22.0f,
            .direction = 128,
        },
        {
            .vid = 2334,
            .offset_x = 136.418945f,
            .offset_y = -60.39209f,
        },
        {
            .vid = 2335,
            .offset_x = 20.418945f,
            .offset_y = -32.39209f,
            .direction = 137,
        },
        {
            .vid = 2325,
            .offset_x = -12.581055f,
            .offset_y = -41.39209f,
            .direction = 128,
        },
        {
            .vid = 2515,
            .offset_x = 5.5f,
            .offset_y = -97.0f,
            .direction = 128,
        },
        {
            .vid = 2340,
            .offset_x = -68.520508f,
            .offset_y = 86.60791f,
            .direction = 63,
        },
        {
            .vid = 2343,
            .offset_x = -63.520508f,
            .offset_y = 121.60791f,
            .pos_z = 41.0f,
            .direction = 64,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Laboratory surgery room imported from _prepare/AS2/decoration/16lab_surgery/lab_surgery03.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile LAB_SURGERY03_DECORATION_PROFILE = {
    .id = "lab_surgery03",
    .frame = {
        .corner_offsets = {{
            {            90.0f,          -256.0f },      // top
            {          -360.0f,            64.0f },      // left
            {           -90.0f,           256.0f },      // bottom
            {           360.0f,           -64.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2325,
            .offset_x = 86.375f,
            .offset_y = -45.0f,
            .direction = 64,
        },
        {
            .vid = 2335,
            .offset_x = -13.625f,
            .offset_y = -25.0f,
            .direction = 128,
        },
        {
            .vid = 2335,
            .offset_x = 30.375f,
            .offset_y = -3.0f,
            .direction = 164,
        },
        {
            .vid = 2317,
            .offset_x = -17.625f,
            .offset_y = 33.0f,
            .direction = 170,
        },
        {
            .vid = 2334,
            .offset_x = -102.625f,
            .offset_y = 102.0f,
            .direction = 128,
        },
        {
            .vid = 2340,
            .offset_x = 32.375f,
            .offset_y = 73.0f,
            .pos_z = 2.0f,
        },
        {
            .vid = 1226,
            .offset_x = 120.375f,
            .offset_y = 133.0f,
            .direction = 170,
        },
        {
            .vid = 2342,
            .offset_x = 32.375f,
            .offset_y = 132.0f,
            .pos_z = 60.0f,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Laboratory surgery room imported from _prepare/AS2/decoration/16lab_surgery/lab_surgery04.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile LAB_SURGERY04_DECORATION_PROFILE = {
    .id = "lab_surgery04",
    .frame = {
        .corner_offsets = {{
            {           -45.0f,          -416.0f },      // top
            {          -585.0f,           -32.0f },      // left
            {            45.0f,           416.0f },      // bottom
            {           585.0f,            32.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2325,
            .offset_x = -99.88916f,
            .offset_y = 101.888916f,
            .direction = 64,
        },
        {
            .vid = 2335,
            .offset_x = -103.5f,
            .offset_y = 67.0f,
            .direction = 146,
        },
        {
            .vid = 2325,
            .offset_x = -156.88916f,
            .offset_y = 63.888916f,
            .direction = 192,
        },
        {
            .vid = 2325,
            .offset_x = 169.11084f,
            .offset_y = -86.111084f,
            .direction = 128,
        },
        {
            .vid = 2321,
            .offset_x = -89.88916f,
            .offset_y = 18.888916f,
            .direction = 190,
        },
        {
            .vid = 2334,
            .offset_x = 21.11084f,
            .offset_y = -3.111084f,
        },
        {
            .vid = 2334,
            .offset_x = -25.88916f,
            .offset_y = -40.111084f,
            .direction = 139,
        },
        {
            .vid = 2321,
            .offset_x = 36.11084f,
            .offset_y = -76.111084f,
            .direction = 190,
        },
        {
            .vid = 2325,
            .offset_x = 100.11084f,
            .offset_y = -132.111084f,
            .direction = 73,
        },
        {
            .vid = 2335,
            .offset_x = 37.5f,
            .offset_y = -33.0f,
            .direction = 146,
        },
        {
            .vid = 2370,
            .offset_x = 197.11084f,
            .offset_y = 66.888916f,
            .direction = 134,
        },
        {
            .vid = 2370,
            .offset_x = 156.11084f,
            .offset_y = 148.888916f,
            .direction = 134,
        },
        {
            .vid = 2337,
            .offset_x = 121.11084f,
            .offset_y = 120.888916f,
        },
        {
            .vid = 2370,
            .offset_x = 52.11084f,
            .offset_y = 167.888916f,
            .direction = 134,
        },
        {
            .vid = 2340,
            .offset_x = 4.11084f,
            .offset_y = 141.888916f,
        },
        {
            .vid = 2340,
            .offset_x = 165.11084f,
            .offset_y = 28.888916f,
        },
        {
            .vid = 2349,
            .offset_x = 116.11084f,
            .offset_y = 173.888916f,
            .pos_z = 50.0f,
        },
        {
            .vid = 2343,
            .offset_x = 3.11084f,
            .offset_y = 187.888916f,
            .pos_z = 48.0f,
        },
        {
            .vid = 2343,
            .offset_x = 165.11084f,
            .offset_y = 81.888916f,
            .pos_z = 54.0f,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Laboratory surgery room imported from _prepare/AS2/decoration/16lab_surgery/lab_surgery05.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile LAB_SURGERY05_DECORATION_PROFILE = {
    .id = "lab_surgery05",
    .frame = {
        .corner_offsets = {{
            {           225.0f,          -480.0f },      // top
            {          -675.0f,           160.0f },      // left
            {          -225.0f,           480.0f },      // bottom
            {           675.0f,          -160.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2334,
            .offset_x = -31.081055f,
            .offset_y = 218.972656f,
            .direction = 160,
        },
        {
            .vid = 2334,
            .offset_x = 124.918945f,
            .offset_y = 112.972656f,
            .direction = 160,
        },
        {
            .vid = 2334,
            .offset_x = 268.918945f,
            .offset_y = 1.972656f,
            .direction = 160,
        },
        {
            .vid = 2322,
            .offset_x = -113.081055f,
            .offset_y = 117.972656f,
            .direction = 128,
        },
        {
            .vid = 2322,
            .offset_x = 48.918945f,
            .offset_y = -0.027344f,
            .direction = 128,
        },
        {
            .vid = 2325,
            .offset_x = -27.081055f,
            .offset_y = 48.972656f,
            .direction = 160,
        },
        {
            .vid = 2325,
            .offset_x = -145.081055f,
            .offset_y = 171.972656f,
        },
        {
            .vid = 2325,
            .offset_x = 109.918945f,
            .offset_y = -55.027344f,
            .direction = 128,
        },
        {
            .vid = 2322,
            .offset_x = 187.918945f,
            .offset_y = -100.027344f,
            .direction = 128,
        },
        {
            .vid = 2370,
            .offset_x = -56.081055f,
            .offset_y = -47.027344f,
            .direction = 153,
        },
        {
            .vid = 2340,
            .offset_x = -32.477051f,
            .offset_y = -46.323242f,
            .direction = 128,
        },
        {
            .vid = 2340,
            .offset_x = 111.418945f,
            .offset_y = -150.527344f,
            .direction = 128,
        },
        {
            .vid = 2370,
            .offset_x = 75.918945f,
            .offset_y = -160.027344f,
            .direction = 102,
        },
        {
            .vid = 2343,
            .offset_x = -33.477051f,
            .offset_y = 2.676758f,
            .pos_z = 50.0f,
            .direction = 128,
        },
        {
            .vid = 2343,
            .offset_x = 110.418945f,
            .offset_y = -101.527344f,
            .pos_z = 50.0f,
            .direction = 128,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Laboratory surgery room imported from _prepare/AS2/decoration/16lab_surgery/lab_surgery06.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile LAB_SURGERY06_DECORATION_PROFILE = {
    .id = "lab_surgery06",
    .frame = {
        .corner_offsets = {{
            {            45.0f,          -416.0f },      // top
            {          -585.0f,            32.0f },      // left
            {           -45.0f,           416.0f },      // bottom
            {           585.0f,           -32.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2335,
            .offset_x = 206.214111f,
            .offset_y = 89.285645f,
            .direction = 54,
        },
        {
            .vid = 2321,
            .offset_x = -32.785889f,
            .offset_y = 63.285645f,
            .direction = 128,
        },
        {
            .vid = 2325,
            .offset_x = 98.214111f,
            .offset_y = 7.285645f,
            .direction = 192,
        },
        {
            .vid = 2325,
            .offset_x = -79.785889f,
            .offset_y = 134.285645f,
            .direction = 192,
        },
        {
            .vid = 2321,
            .offset_x = 153.214111f,
            .offset_y = -65.714355f,
            .direction = 128,
        },
        {
            .vid = 414,
            .offset_x = 25.214111f,
            .offset_y = -89.714355f,
            .direction = 36,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

} // namespace auto_mapper::core::decoration_builder
