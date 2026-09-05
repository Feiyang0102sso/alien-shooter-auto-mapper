/**
 * @file other_rooms_as2.h
 * @brief Authored AS2 miscellaneous room decoration stamp profiles.
 */

#pragma once

#include "auto_mapper/core/decoration_builder/decoration_profile_types.h"
#include "auto_mapper/core/wall_builder/wall_profiles_as2.h"

namespace auto_mapper::core::decoration_builder {

/**
 * Miscellaneous room imported from _prepare/AS2/decoration/99_other_rooms/others01.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile OTHERS01_DECORATION_PROFILE = {
    .id = "others01",
    .frame = {
        .corner_offsets = {{
            {            45.0f,          -288.0f },      // top
            {          -405.0f,            32.0f },      // left
            {           -45.0f,           288.0f },      // bottom
            {           405.0f,           -32.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 1236,
            .offset_x = 151.0f,
            .offset_y = -25.0f,
        },
        {
            .vid = 1764,
            .offset_x = 305.0f,
            .offset_y = -32.0f,
            .direction = 57,
        },
        {
            .vid = 1240,
            .offset_x = 129.0f,
            .offset_y = -64.0f,
            .pos_z = 12.0f,
            .direction = 237,
        },
        {
            .vid = 1240,
            .offset_x = 236.0f,
            .offset_y = -56.0f,
            .pos_z = 12.0f,
            .direction = 164,
        },
        {
            .vid = 1764,
            .offset_x = 50.0f,
            .offset_y = -57.0f,
            .direction = 83,
        },
        {
            .vid = 1764,
            .offset_x = -124.0f,
            .offset_y = -4.0f,
            .direction = 38,
        },
        {
            .vid = 1240,
            .offset_x = -21.0f,
            .offset_y = -60.0f,
            .pos_z = 12.0f,
            .direction = 164,
        },
        {
            .vid = 1240,
            .offset_x = -156.0f,
            .offset_y = -31.0f,
            .pos_z = 12.0f,
            .direction = 9,
        },
        {
            .vid = 1764,
            .offset_x = 78.0f,
            .offset_y = -35.0f,
            .direction = 83,
        },
        {
            .vid = 1240,
            .offset_x = -248.0f,
            .offset_y = 34.0f,
            .pos_z = 12.0f,
            .direction = 27,
        },
        {
            .vid = 1240,
            .offset_x = 103.0f,
            .offset_y = -149.0f,
            .pos_z = 12.0f,
        },
        {
            .vid = 1240,
            .offset_x = 191.0f,
            .offset_y = -87.0f,
            .pos_z = 12.0f,
            .direction = 82,
        },
        {
            .vid = 1232,
            .offset_x = -158.0f,
            .offset_y = -46.0f,
            .pos_z = 70.0f,
        },
        {
            .vid = 1240,
            .offset_x = -67.0f,
            .offset_y = -94.0f,
            .pos_z = 12.0f,
            .direction = 73,
        },
        {
            .vid = 1240,
            .offset_x = -3.0f,
            .offset_y = -116.0f,
            .pos_z = 12.0f,
            .direction = 219,
        },
        {
            .vid = 1240,
            .offset_x = 63.0f,
            .offset_y = -91.0f,
            .pos_z = 12.0f,
            .direction = 18,
        },
        {
            .vid = 410,
            .offset_x = -129.0f,
            .offset_y = 1.0f,
            .pos_z = 40.0f,
        },
        {
            .vid = 1289,
            .offset_x = -65.0f,
            .offset_y = 41.0f,
            .direction = 136,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET6_RANDOM,
};

/**
 * Miscellaneous room imported from _prepare/AS2/decoration/99_other_rooms/others02.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile OTHERS02_DECORATION_PROFILE = {
    .id = "others02",
    .frame = {
        .corner_offsets = {{
            {           180.0f,          -576.0f },      // top
            {          -810.0f,           128.0f },      // left
            {          -180.0f,           576.0f },      // bottom
            {           810.0f,          -128.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2355,
            .offset_x = 75.5f,
            .offset_y = 192.0f,
            .direction = 32,
        },
        {
            .vid = 2355,
            .offset_x = 1.5f,
            .offset_y = 241.0f,
            .direction = 32,
        },
        {
            .vid = 2537,
            .offset_x = -219.5f,
            .offset_y = 74.0f,
        },
        {
            .vid = 443,
            .offset_x = 122.065674f,
            .offset_y = -137.209473f,
            .direction = 64,
        },
        {
            .vid = 2350,
            .offset_x = 249.037598f,
            .offset_y = -41.830261f,
        },
        {
            .vid = 2360,
            .offset_x = 57.5f,
            .offset_y = -60.0f,
        },
        {
            .vid = 2536,
            .offset_x = -49.5f,
            .offset_y = -31.0f,
            .direction = 32,
        },
        {
            .vid = 2364,
            .offset_x = 58.5f,
            .offset_y = -1.0f,
            .pos_z = 56.0f,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Miscellaneous room imported from _prepare/AS2/decoration/99_other_rooms/others03.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile OTHERS03_DECORATION_PROFILE = {
    .id = "others03",
    .frame = {
        .corner_offsets = {{
            {           315.0f,          -416.0f },      // top
            {          -585.0f,           224.0f },      // left
            {          -315.0f,           416.0f },      // bottom
            {           585.0f,          -224.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 1240,
            .offset_x = 98.803711f,
            .offset_y = 131.043457f,
            .pos_z = 50.0f,
            .direction = 201,
        },
        {
            .vid = 1240,
            .offset_x = -90.196289f,
            .offset_y = 114.043457f,
            .direction = 27,
        },
        {
            .vid = 1240,
            .offset_x = -93.196289f,
            .offset_y = 101.043457f,
            .pos_z = 50.0f,
            .direction = 219,
        },
        {
            .vid = 1240,
            .offset_x = 2.803711f,
            .offset_y = 48.043457f,
            .direction = 9,
        },
        {
            .vid = 1240,
            .offset_x = 1.803711f,
            .offset_y = 149.043457f,
            .pos_z = 50.0f,
        },
        {
            .vid = 1240,
            .offset_x = -161.196289f,
            .offset_y = 109.043457f,
            .direction = 9,
        },
        {
            .vid = 1240,
            .offset_x = -154.196289f,
            .offset_y = 159.043457f,
            .direction = 27,
        },
        {
            .vid = 1240,
            .offset_x = -190.196289f,
            .offset_y = 148.043457f,
            .pos_z = 50.0f,
            .direction = 9,
        },
        {
            .vid = 1240,
            .offset_x = 260.803711f,
            .offset_y = -136.956543f,
            .direction = 137,
        },
        {
            .vid = 1240,
            .offset_x = 149.803711f,
            .offset_y = 8.043457f,
            .direction = 237,
        },
        {
            .vid = 1240,
            .offset_x = 216.803711f,
            .offset_y = -17.956543f,
            .direction = 27,
        },
        {
            .vid = 1240,
            .offset_x = 269.803711f,
            .offset_y = -55.956543f,
            .direction = 210,
        },
        {
            .vid = 1240,
            .offset_x = 177.803711f,
            .offset_y = -77.956543f,
            .direction = 9,
        },
        {
            .vid = 1240,
            .offset_x = 91.803711f,
            .offset_y = -14.956543f,
            .direction = 73,
        },
        {
            .vid = 1240,
            .offset_x = -38.196289f,
            .offset_y = 22.043457f,
            .direction = 9,
        },
        {
            .vid = 1240,
            .offset_x = 43.803711f,
            .offset_y = -36.956543f,
            .direction = 137,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Miscellaneous room imported from _prepare/AS2/decoration/99_other_rooms/others04.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile OTHERS04_DECORATION_PROFILE = {
    .id = "others04",
    .frame = {
        .corner_offsets = {{
            {             0.0f,          -384.0f },      // top
            {          -540.0f,             0.0f },      // left
            {             0.0f,           384.0f },      // bottom
            {           540.0f,             0.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2318,
            .offset_x = 335.13278f,
            .offset_y = 10.627755f,
            .direction = 48,
        },
        {
            .vid = 2317,
            .offset_x = 214.13278f,
            .offset_y = 124.627755f,
            .pos_z = 26.0f,
            .direction = 42,
        },
        {
            .vid = 1764,
            .offset_x = 211.13278f,
            .offset_y = 132.627755f,
            .direction = 166,
        },
        {
            .vid = 2318,
            .offset_x = -178.86722f,
            .offset_y = 10.627755f,
            .direction = 192,
        },
        {
            .vid = 2357,
            .offset_x = -7.86722f,
            .offset_y = 221.627755f,
            .direction = 57,
        },
        {
            .vid = 2317,
            .offset_x = -16.86722f,
            .offset_y = 40.627755f,
            .pos_z = 14.0f,
            .direction = 136,
        },
        {
            .vid = 1764,
            .offset_x = -14.86722f,
            .offset_y = 43.627755f,
            .direction = 172,
        },
        {
            .vid = 2527,
            .offset_x = 76.13278f,
            .offset_y = -13.372245f,
            .pos_z = 33.0f,
            .direction = 119,
        },
        {
            .vid = 2319,
            .offset_x = 53.13278f,
            .offset_y = -196.372245f,
        },
        {
            .vid = 1764,
            .offset_x = 83.13278f,
            .offset_y = -25.372245f,
            .direction = 172,
        },
        {
            .vid = 2318,
            .offset_x = -191.86722f,
            .offset_y = -55.372245f,
            .direction = 64,
        },
        {
            .vid = 1763,
            .offset_x = -162.86722f,
            .offset_y = -107.372245f,
            .direction = 12,
        },
        {
            .vid = 2319,
            .offset_x = 33.13278f,
            .offset_y = -152.372245f,
            .direction = 208,
        },
        {
            .vid = 1770,
            .offset_x = -89.86722f,
            .offset_y = -138.372245f,
            .direction = 96,
        },
        {
            .vid = 1232,
            .offset_x = -235.86722f,
            .offset_y = -86.372245f,
            .pos_z = 57.0f,
        },
        {
            .vid = 2318,
            .offset_x = -251.86722f,
            .offset_y = -17.372245f,
            .direction = 128,
        },
        {
            .vid = 1270,
            .offset_x = 152.13278f,
            .offset_y = -140.372245f,
        },
        {
            .vid = 407,
            .offset_x = -48.86722f,
            .offset_y = -109.372245f,
            .direction = 170,
        },
        {
            .vid = 1759,
            .offset_x = -98.86722f,
            .offset_y = -130.372245f,
            .pos_z = 27.0f,
            .direction = 128,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET6_RANDOM,
};

/**
 * Miscellaneous room imported from _prepare/AS2/decoration/99_other_rooms/others05.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile OTHERS05_DECORATION_PROFILE = {
    .id = "others05",
    .frame = {
        .corner_offsets = {{
            {           -45.0f,          -288.0f },      // top
            {          -405.0f,           -32.0f },      // left
            {            45.0f,           288.0f },      // bottom
            {           405.0f,            32.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 1139,
            .offset_x = -184.25f,
            .offset_y = 107.0f,
            .pos_z = 30.0f,
            .direction = 128,
        },
        {
            .vid = 1289,
            .offset_x = 168.75f,
            .offset_y = 77.0f,
            .direction = 176,
        },
        {
            .vid = 1755,
            .offset_x = 208.75f,
            .offset_y = 37.0f,
            .direction = 160,
        },
        {
            .vid = 1265,
            .offset_x = 89.75f,
            .offset_y = -72.0f,
            .direction = 160,
        },
        {
            .vid = 1261,
            .offset_x = -144.25f,
            .offset_y = -67.0f,
            .direction = 192,
        },
        {
            .vid = 2351,
            .offset_x = -2.25f,
            .offset_y = -104.0f,
            .direction = 160,
        },
        {
            .vid = 1289,
            .offset_x = -124.25f,
            .offset_y = -29.0f,
            .direction = 168,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET6_RANDOM,
};

/**
 * Miscellaneous room imported from _prepare/AS2/decoration/99_other_rooms/others06.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile OTHERS06_DECORATION_PROFILE = {
    .id = "others06",
    .frame = {
        .corner_offsets = {{
            {             0.0f,          -320.0f },      // top
            {          -450.0f,             0.0f },      // left
            {             0.0f,           320.0f },      // bottom
            {           450.0f,             0.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2319,
            .offset_x = 102.816553f,
            .offset_y = 113.026298f,
            .direction = 112,
        },
        {
            .vid = 1139,
            .offset_x = 154.816553f,
            .offset_y = 305.026298f,
            .pos_z = 29.0f,
        },
        {
            .vid = 2517,
            .offset_x = -33.183447f,
            .offset_y = 113.026298f,
            .direction = 128,
        },
        {
            .vid = 2520,
            .offset_x = 221.816553f,
            .offset_y = -34.973702f,
            .direction = 68,
        },
        {
            .vid = 2319,
            .offset_x = 280.816553f,
            .offset_y = 3.026298f,
            .direction = 128,
        },
        {
            .vid = 428,
            .offset_x = -8.183447f,
            .offset_y = -170.973702f,
            .pos_z = 43.0f,
            .direction = 64,
        },
        {
            .vid = 428,
            .offset_x = -51.183447f,
            .offset_y = -145.973702f,
            .pos_z = 32.0f,
            .direction = 48,
        },
        {
            .vid = 1232,
            .offset_x = -86.183447f,
            .offset_y = -162.973702f,
            .pos_z = 57.0f,
        },
        {
            .vid = 2357,
            .offset_x = 75.816553f,
            .offset_y = -148.973702f,
            .direction = 96,
        },
        {
            .vid = 1764,
            .offset_x = -61.183447f,
            .offset_y = -136.973702f,
            .direction = 64,
        },
        {
            .vid = 1764,
            .offset_x = -16.183447f,
            .offset_y = -175.973702f,
            .direction = 89,
        },
        {
            .vid = 1764,
            .offset_x = 13.816553f,
            .offset_y = -197.973702f,
            .direction = 76,
        },
        {
            .vid = 2518,
            .offset_x = -101.183447f,
            .offset_y = -67.973702f,
        },
        {
            .vid = 2319,
            .offset_x = -237.183447f,
            .offset_y = -10.973702f,
            .direction = 16,
        },
        {
            .vid = 428,
            .offset_x = -9.183447f,
            .offset_y = -166.973702f,
            .pos_z = 69.0f,
            .direction = 64,
        },
        {
            .vid = 1765,
            .offset_x = -75.183447f,
            .offset_y = -123.973702f,
            .pos_z = 36.0f,
            .direction = 38,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET6_RANDOM,
};

/**
 * Miscellaneous room imported from _prepare/AS2/decoration/99_other_rooms/others07.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile OTHERS07_DECORATION_PROFILE = {
    .id = "others07",
    .frame = {
        .corner_offsets = {{
            {           135.0f,          -288.0f },      // top
            {          -405.0f,            96.0f },      // left
            {          -135.0f,           288.0f },      // bottom
            {           405.0f,           -96.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2319,
            .offset_x = 69.0f,
            .offset_y = 108.0f,
            .pos_z = 37.0f,
            .direction = 208,
        },
        {
            .vid = 1764,
            .offset_x = 69.0f,
            .offset_y = 110.0f,
            .direction = 128,
        },
        {
            .vid = 413,
            .offset_x = 359.0f,
            .offset_y = 1.0f,
            .direction = 42,
        },
        {
            .vid = 413,
            .offset_x = 302.0f,
            .offset_y = -36.0f,
            .direction = 42,
        },
        {
            .vid = 1764,
            .offset_x = 207.0f,
            .offset_y = 7.0f,
            .direction = 128,
        },
        {
            .vid = 1766,
            .offset_x = -124.0f,
            .offset_y = -123.0f,
            .pos_z = -3.0f,
        },
        {
            .vid = 409,
            .offset_x = -21.0f,
            .offset_y = -170.0f,
            .direction = 128,
        },
        {
            .vid = 1764,
            .offset_x = 25.0f,
            .offset_y = -197.0f,
            .direction = 64,
        },
        {
            .vid = 1764,
            .offset_x = 112.0f,
            .offset_y = -70.0f,
            .direction = 128,
        },
        {
            .vid = 1764,
            .offset_x = -37.0f,
            .offset_y = 35.0f,
            .direction = 128,
        },
        {
            .vid = 428,
            .offset_x = -249.0f,
            .offset_y = -14.0f,
            .pos_z = 57.0f,
            .direction = 208,
        },
        {
            .vid = 1763,
            .offset_x = -203.0f,
            .offset_y = -36.0f,
            .direction = 12,
        },
        {
            .vid = 1764,
            .offset_x = -252.0f,
            .offset_y = -17.0f,
            .direction = 51,
        },
        {
            .vid = 427,
            .offset_x = 11.0f,
            .offset_y = -189.0f,
            .pos_z = 32.0f,
            .direction = 42,
        },
        {
            .vid = 1765,
            .offset_x = 222.0f,
            .offset_y = 2.0f,
            .pos_z = 40.0f,
            .direction = 64,
        },
        {
            .vid = 1765,
            .offset_x = 197.0f,
            .offset_y = 21.0f,
            .pos_z = 40.0f,
            .direction = 38,
        },
        {
            .vid = 1765,
            .offset_x = -44.0f,
            .offset_y = 43.0f,
            .pos_z = 40.0f,
            .direction = 38,
        },
        {
            .vid = 407,
            .offset_x = 5.0f,
            .offset_y = 75.0f,
            .direction = 42,
        },
        {
            .vid = 407,
            .offset_x = 98.0f,
            .offset_y = 145.0f,
            .direction = 42,
        },
        {
            .vid = 407,
            .offset_x = 145.0f,
            .offset_y = -52.0f,
            .direction = 42,
        },
        {
            .vid = 407,
            .offset_x = 230.0f,
            .offset_y = 33.0f,
            .direction = 42,
        },
        {
            .vid = 1640,
            .offset_x = 113.0f,
            .offset_y = -67.0f,
            .pos_z = 37.0f,
            .direction = 32,
        },
        {
            .vid = 426,
            .offset_x = -23.0f,
            .offset_y = 23.0f,
            .pos_z = 37.0f,
            .direction = 42,
        },
        {
            .vid = 426,
            .offset_x = -237.0f,
            .offset_y = -27.0f,
            .pos_z = 19.0f,
        },
        {
            .vid = 426,
            .offset_x = 33.0f,
            .offset_y = -206.0f,
            .pos_z = 32.0f,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET6_RANDOM,
};

/**
 * Miscellaneous room imported from _prepare/AS2/decoration/99_other_rooms/others08.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile OTHERS08_DECORATION_PROFILE = {
    .id = "others08",
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
            .vid = 2516,
            .offset_x = -116.75f,
            .offset_y = 62.0f,
            .direction = 48,
        },
        {
            .vid = 1282,
            .offset_x = 92.25f,
            .offset_y = 229.0f,
            .direction = 128,
        },
        {
            .vid = 1139,
            .offset_x = -342.75f,
            .offset_y = 106.0f,
            .pos_z = 29.0f,
            .direction = 128,
        },
        {
            .vid = 427,
            .offset_x = 35.25f,
            .offset_y = -196.0f,
            .pos_z = 37.0f,
            .direction = 42,
        },
        {
            .vid = 1770,
            .offset_x = 20.25f,
            .offset_y = -203.0f,
            .direction = 128,
        },
        {
            .vid = 1763,
            .offset_x = -56.75f,
            .offset_y = -186.0f,
            .direction = 12,
        },
        {
            .vid = 1764,
            .offset_x = -242.75f,
            .offset_y = -54.0f,
            .direction = 64,
        },
        {
            .vid = 427,
            .offset_x = 65.25f,
            .offset_y = -210.0f,
            .pos_z = 37.0f,
            .direction = 85,
        },
        {
            .vid = 427,
            .offset_x = -18.75f,
            .offset_y = -203.0f,
            .pos_z = 39.0f,
            .direction = 170,
        },
        {
            .vid = 1304,
            .offset_x = -319.75f,
            .offset_y = -42.0f,
            .pos_z = 73.0f,
            .direction = 187,
        },
        {
            .vid = 435,
            .offset_x = 229.25f,
            .offset_y = 69.0f,
            .direction = 128,
        },
        {
            .vid = 435,
            .offset_x = 319.25f,
            .offset_y = 91.0f,
            .direction = 128,
        },
        {
            .vid = 435,
            .offset_x = 254.25f,
            .offset_y = 107.0f,
            .direction = 128,
        },
        {
            .vid = 435,
            .offset_x = 202.25f,
            .offset_y = 107.0f,
            .direction = 128,
        },
        {
            .vid = 435,
            .offset_x = -7.75f,
            .offset_y = 68.0f,
            .direction = 128,
        },
        {
            .vid = 2536,
            .offset_x = 130.25f,
            .offset_y = -190.0f,
            .pos_z = 10.0f,
            .direction = 48,
        },
        {
            .vid = 2536,
            .offset_x = 361.25f,
            .offset_y = -4.0f,
            .direction = 16,
        },
        {
            .vid = 436,
            .offset_x = 145.25f,
            .offset_y = -133.0f,
        },
        {
            .vid = 1638,
            .offset_x = 307.25f,
            .offset_y = 9.0f,
            .direction = 128,
        },
        {
            .vid = 436,
            .offset_x = 262.25f,
            .offset_y = -11.0f,
        },
        {
            .vid = 436,
            .offset_x = 300.25f,
            .offset_y = -21.0f,
        },
        {
            .vid = 407,
            .offset_x = 27.25f,
            .offset_y = -165.0f,
            .direction = 85,
        },
        {
            .vid = 426,
            .offset_x = -248.75f,
            .offset_y = -47.0f,
            .pos_z = 32.0f,
        },
        {
            .vid = 426,
            .offset_x = -262.75f,
            .offset_y = -48.0f,
            .pos_z = 34.0f,
            .direction = 42,
        },
        {
            .vid = 426,
            .offset_x = -242.75f,
            .offset_y = -58.0f,
            .pos_z = 34.0f,
            .direction = 128,
        },
        {
            .vid = 426,
            .offset_x = -230.75f,
            .offset_y = -71.0f,
            .pos_z = 34.0f,
            .direction = 85,
        },
        {
            .vid = 1550,
            .offset_x = -305.75f,
            .offset_y = -55.0f,
            .pos_z = 44.0f,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET6_RANDOM,
};

/**
 * Miscellaneous room imported from _prepare/AS2/decoration/99_other_rooms/others09.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile OTHERS09_DECORATION_PROFILE = {
    .id = "others09",
    .frame = {
        .corner_offsets = {{
            {           -45.0f,          -352.0f },      // top
            {          -495.0f,           -32.0f },      // left
            {            45.0f,           352.0f },      // bottom
            {           495.0f,            32.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2520,
            .offset_x = 188.439559f,
            .offset_y = 94.093883f,
            .direction = 34,
        },
        {
            .vid = 2520,
            .offset_x = 291.439559f,
            .offset_y = 9.093883f,
            .direction = 59,
        },
        {
            .vid = 443,
            .offset_x = -162.560441f,
            .offset_y = -20.906117f,
        },
        {
            .vid = 2520,
            .offset_x = 64.439559f,
            .offset_y = 152.093883f,
            .direction = 25,
        },
        {
            .vid = 1232,
            .offset_x = -223.560441f,
            .offset_y = -64.906117f,
            .pos_z = 61.0f,
        },
        {
            .vid = 2521,
            .offset_x = -125.560441f,
            .offset_y = -104.906117f,
            .direction = 112,
        },
        {
            .vid = 2521,
            .offset_x = 70.439559f,
            .offset_y = -165.906117f,
            .direction = 64,
        },
        {
            .vid = 2521,
            .offset_x = -80.560441f,
            .offset_y = -134.906117f,
            .direction = 16,
        },
        {
            .vid = 2521,
            .offset_x = -36.560441f,
            .offset_y = -166.906117f,
            .direction = 48,
        },
        {
            .vid = 2521,
            .offset_x = -1.560441f,
            .offset_y = -203.906117f,
            .direction = 144,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET6_RANDOM,
};

} // namespace auto_mapper::core::decoration_builder
