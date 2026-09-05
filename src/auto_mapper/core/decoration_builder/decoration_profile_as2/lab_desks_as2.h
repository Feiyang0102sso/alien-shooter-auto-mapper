/**
 * @file lab_desks_as2.h
 * @brief Authored AS2 laboratory desk room decoration stamp profiles.
 */

#pragma once

#include "auto_mapper/core/decoration_builder/decoration_profile_types.h"
#include "auto_mapper/core/wall_builder/wall_profiles_as2.h"

namespace auto_mapper::core::decoration_builder {

/**
 * Laboratory desk room imported from _prepare/AS2/decoration/12lab_desks/lab_desks01.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile LAB_DESKS01_DECORATION_PROFILE = {
    .id = "lab_desks01",
    .frame = {
        .corner_offsets = {{
            {          -405.0f,          -544.0f },      // top
            {          -765.0f,          -288.0f },      // left
            {           405.0f,           544.0f },      // bottom
            {           765.0f,           288.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2335,
            .offset_x = 547.256077f,
            .offset_y = 363.004321f,
            .direction = 173,
        },
        {
            .vid = 1763,
            .offset_x = 596.256077f,
            .offset_y = 404.004321f,
            .direction = 15,
        },
        {
            .vid = 408,
            .offset_x = 359.256077f,
            .offset_y = 207.004321f,
            .direction = 103,
        },
        {
            .vid = 1763,
            .offset_x = 235.256077f,
            .offset_y = 194.004321f,
            .direction = 15,
        },
        {
            .vid = 2330,
            .offset_x = 446.256077f,
            .offset_y = 395.004321f,
            .direction = 3,
        },
        {
            .vid = 408,
            .offset_x = 422.256077f,
            .offset_y = 164.004321f,
            .direction = 30,
        },
        {
            .vid = 408,
            .offset_x = 89.256077f,
            .offset_y = -21.995679f,
            .direction = 156,
        },
        {
            .vid = 2330,
            .offset_x = 112.256077f,
            .offset_y = 160.004321f,
            .direction = 3,
        },
        {
            .vid = 2335,
            .offset_x = 221.256077f,
            .offset_y = 134.004321f,
            .direction = 173,
        },
        {
            .vid = 2330,
            .offset_x = -198.743923f,
            .offset_y = -58.995679f,
            .direction = 3,
        },
        {
            .vid = 1763,
            .offset_x = -38.743923f,
            .offset_y = -55.995679f,
            .direction = 15,
        },
        {
            .vid = 1763,
            .offset_x = -185.743923f,
            .offset_y = -108.995679f,
            .pos_z = 42.0f,
            .direction = 202,
        },
        {
            .vid = 1763,
            .offset_x = -149.743923f,
            .offset_y = -94.995679f,
            .pos_z = 40.0f,
            .direction = 157,
        },
        {
            .vid = 408,
            .offset_x = -159.743923f,
            .offset_y = -211.995679f,
            .direction = 243,
        },
        {
            .vid = 2335,
            .offset_x = -98.743923f,
            .offset_y = -95.995679f,
            .direction = 173,
        },
        {
            .vid = 2335,
            .offset_x = -383.743923f,
            .offset_y = -291.995679f,
            .direction = 173,
        },
        {
            .vid = 2330,
            .offset_x = -476.743923f,
            .offset_y = -278.995679f,
            .direction = 3,
        },
        {
            .vid = 1763,
            .offset_x = -338.743923f,
            .offset_y = -261.995679f,
            .direction = 15,
        },
        {
            .vid = 408,
            .offset_x = -187.743923f,
            .offset_y = -393.995679f,
            .direction = 229,
        },
        {
            .vid = 1763,
            .offset_x = -420.743923f,
            .offset_y = -330.995679f,
            .pos_z = 40.0f,
            .direction = 47,
        },
        {
            .vid = 1763,
            .offset_x = -458.743923f,
            .offset_y = -329.995679f,
            .pos_z = 42.0f,
            .direction = 195,
        },
        {
            .vid = 2370,
            .offset_x = 558.256077f,
            .offset_y = 441.004321f,
            .direction = 127,
        },
        {
            .vid = 2370,
            .offset_x = 286.256077f,
            .offset_y = 223.004321f,
            .direction = 186,
        },
        {
            .vid = 2370,
            .offset_x = -52.743923f,
            .offset_y = 16.004321f,
            .direction = 42,
        },
        {
            .vid = 2370,
            .offset_x = -322.743923f,
            .offset_y = -205.995679f,
            .direction = 165,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET8_RANDOM,
};

/**
 * Laboratory desk room imported from _prepare/AS2/decoration/12lab_desks/lab_desks02.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile LAB_DESKS02_DECORATION_PROFILE = {
    .id = "lab_desks02",
    .frame = {
        .corner_offsets = {{
            {          -135.0f,          -288.0f },      // top
            {          -405.0f,           -96.0f },      // left
            {           135.0f,           288.0f },      // bottom
            {           405.0f,            96.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 1763,
            .offset_x = 121.003461f,
            .offset_y = 159.002461f,
            .direction = 15,
        },
        {
            .vid = 1763,
            .offset_x = 248.003461f,
            .offset_y = 78.002461f,
            .pos_z = 40.0f,
            .direction = 47,
        },
        {
            .vid = 2335,
            .offset_x = 187.003461f,
            .offset_y = 91.002461f,
            .direction = 137,
        },
        {
            .vid = 2330,
            .offset_x = 192.003461f,
            .offset_y = 131.002461f,
            .direction = 254,
        },
        {
            .vid = 408,
            .offset_x = -161.996539f,
            .offset_y = 121.002461f,
            .direction = 14,
        },
        {
            .vid = 408,
            .offset_x = 29.003461f,
            .offset_y = 30.002461f,
            .direction = 114,
        },
        {
            .vid = 1763,
            .offset_x = 101.003461f,
            .offset_y = -20.997539f,
            .direction = 15,
        },
        {
            .vid = 1763,
            .offset_x = -137.996539f,
            .offset_y = -104.997539f,
            .pos_z = 42.0f,
            .direction = 202,
        },
        {
            .vid = 1763,
            .offset_x = -58.996539f,
            .offset_y = -116.997539f,
            .pos_z = 40.0f,
            .direction = 157,
        },
        {
            .vid = 2335,
            .offset_x = -105.996539f,
            .offset_y = -108.997539f,
            .direction = 137,
        },
        {
            .vid = 2330,
            .offset_x = -105.996539f,
            .offset_y = -77.997539f,
            .direction = 254,
        },
        {
            .vid = 1763,
            .offset_x = -176.996539f,
            .offset_y = -76.997539f,
            .pos_z = 42.0f,
            .direction = 195,
        },
        {
            .vid = 2370,
            .offset_x = 226.003461f,
            .offset_y = 180.002461f,
            .direction = 254,
        },
        {
            .vid = 2370,
            .offset_x = -97.996539f,
            .offset_y = -44.997539f,
            .direction = 134,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET8_RANDOM,
};

/**
 * Laboratory desk room imported from _prepare/AS2/decoration/12lab_desks/lab_desks03.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile LAB_DESKS03_DECORATION_PROFILE = {
    .id = "lab_desks03",
    .frame = {
        .corner_offsets = {{
            {            45.0f,          -160.0f },      // top
            {          -225.0f,            32.0f },      // left
            {           -45.0f,           160.0f },      // bottom
            {           225.0f,           -32.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 1763,
            .offset_x = 31.0f,
            .offset_y = 166.0f,
            .direction = 16,
        },
        {
            .vid = 1763,
            .offset_x = 84.0f,
            .offset_y = 58.0f,
            .pos_z = 43.0f,
            .direction = 200,
        },
        {
            .vid = 1763,
            .offset_x = 31.0f,
            .offset_y = 16.0f,
            .pos_z = 43.0f,
            .direction = 200,
        },
        {
            .vid = 2330,
            .offset_x = 27.0f,
            .offset_y = 60.0f,
            .direction = 7,
        },
        {
            .vid = 2370,
            .offset_x = -6.0f,
            .offset_y = 64.0f,
            .direction = 112,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET8_RANDOM,
};

/**
 * Laboratory desk room imported from _prepare/AS2/decoration/12lab_desks/lab_desks04.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile LAB_DESKS04_DECORATION_PROFILE = {
    .id = "lab_desks04",
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
            .vid = 2330,
            .offset_x = -143.097984f,
            .offset_y = 141.33815f,
            .direction = 19,
        },
        {
            .vid = 1763,
            .offset_x = -121.097984f,
            .offset_y = 113.33815f,
            .pos_z = 43.0f,
            .direction = 210,
        },
        {
            .vid = 1763,
            .offset_x = -197.097984f,
            .offset_y = 62.33815f,
            .direction = 16,
        },
        {
            .vid = 1763,
            .offset_x = 194.902016f,
            .offset_y = -206.66185f,
            .direction = 16,
        },
        {
            .vid = 2330,
            .offset_x = 251.902016f,
            .offset_y = -121.66185f,
            .direction = 19,
        },
        {
            .vid = 1763,
            .offset_x = -23.097984f,
            .offset_y = -53.66185f,
            .direction = 16,
        },
        {
            .vid = 2330,
            .offset_x = 38.902016f,
            .offset_y = 10.33815f,
            .direction = 19,
        },
        {
            .vid = 2370,
            .offset_x = -180.097984f,
            .offset_y = 159.33815f,
            .direction = 107,
        },
        {
            .vid = 2370,
            .offset_x = 234.902016f,
            .offset_y = -139.66185f,
            .direction = 114,
        },
        {
            .vid = 2370,
            .offset_x = 20.902016f,
            .offset_y = 11.33815f,
            .direction = 134,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET8_RANDOM,
};

/**
 * Laboratory desk room imported from _prepare/AS2/decoration/12lab_desks/lab_desks05.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile LAB_DESKS05_DECORATION_PROFILE = {
    .id = "lab_desks05",
    .frame = {
        .corner_offsets = {{
            {          -180.0f,          -512.0f },      // top
            {          -720.0f,          -128.0f },      // left
            {           180.0f,           512.0f },      // bottom
            {           720.0f,           128.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 408,
            .offset_x = 51.980469f,
            .offset_y = 295.922852f,
            .direction = 40,
        },
        {
            .vid = 2330,
            .offset_x = 106.980469f,
            .offset_y = 186.922852f,
            .direction = 25,
        },
        {
            .vid = 408,
            .offset_x = 109.980469f,
            .offset_y = 8.922852f,
            .direction = 7,
        },
        {
            .vid = 2330,
            .offset_x = 239.980469f,
            .offset_y = 74.922852f,
            .direction = 12,
        },
        {
            .vid = 408,
            .offset_x = -124.019531f,
            .offset_y = 15.922852f,
            .direction = 45,
        },
        {
            .vid = 408,
            .offset_x = -49.019531f,
            .offset_y = 191.922852f,
            .direction = 241,
        },
        {
            .vid = 2330,
            .offset_x = -166.019531f,
            .offset_y = 1.922852f,
            .direction = 44,
        },
        {
            .vid = 2330,
            .offset_x = -37.019531f,
            .offset_y = -111.077148f,
        },
        {
            .vid = 408,
            .offset_x = 22.980469f,
            .offset_y = -64.077148f,
            .direction = 149,
        },
        {
            .vid = 2370,
            .offset_x = 78.980469f,
            .offset_y = 133.922852f,
            .direction = 51,
        },
        {
            .vid = 2370,
            .offset_x = 179.980469f,
            .offset_y = 61.922852f,
            .direction = 38,
        },
        {
            .vid = 2370,
            .offset_x = -118.019531f,
            .offset_y = -35.077148f,
            .direction = 91,
        },
        {
            .vid = 2370,
            .offset_x = -14.019531f,
            .offset_y = -73.077148f,
            .direction = 38,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Laboratory desk room imported from _prepare/AS2/decoration/12lab_desks/lab_desks06.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile LAB_DESKS06_DECORATION_PROFILE = {
    .id = "lab_desks06",
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
            .vid = 2330,
            .offset_x = 397.627604f,
            .offset_y = 17.064815f,
            .pos_z = 2.0f,
            .direction = 253,
        },
        {
            .vid = 408,
            .offset_x = 324.627604f,
            .offset_y = -10.935185f,
            .direction = 106,
        },
        {
            .vid = 2330,
            .offset_x = 115.627604f,
            .offset_y = -192.935185f,
            .pos_z = 2.0f,
            .direction = 18,
        },
        {
            .vid = 408,
            .offset_x = 85.627604f,
            .offset_y = -191.935185f,
            .direction = 106,
        },
        {
            .vid = 2331,
            .offset_x = 365.627604f,
            .offset_y = -142.935185f,
            .pos_z = 45.0f,
            .direction = 177,
        },
        {
            .vid = 2331,
            .offset_x = 156.627604f,
            .offset_y = -286.935185f,
            .pos_z = 45.0f,
            .direction = 177,
        },
        {
            .vid = 2331,
            .offset_x = 275.627604f,
            .offset_y = -217.935185f,
            .pos_z = 45.0f,
            .direction = 107,
        },
        {
            .vid = 2370,
            .offset_x = -85.372396f,
            .offset_y = 73.064815f,
        },
        {
            .vid = 2340,
            .offset_x = -38.372396f,
            .offset_y = 45.064815f,
            .direction = 64,
        },
        {
            .vid = 2340,
            .offset_x = -138.372396f,
            .offset_y = 117.064815f,
            .direction = 64,
        },
        {
            .vid = 2370,
            .offset_x = -279.372396f,
            .offset_y = 84.064815f,
            .direction = 23,
        },
        {
            .vid = 2370,
            .offset_x = -181.372396f,
            .offset_y = 143.064815f,
            .direction = 64,
        },
        {
            .vid = 2340,
            .offset_x = -242.372396f,
            .offset_y = 42.064815f,
            .direction = 64,
        },
        {
            .vid = 2337,
            .offset_x = 427.627604f,
            .offset_y = -30.935185f,
            .pos_z = 3.0f,
            .direction = 64,
        },
        {
            .vid = 2340,
            .offset_x = 263.627604f,
            .offset_y = -120.935185f,
            .direction = 64,
        },
        {
            .vid = 2370,
            .offset_x = 225.627604f,
            .offset_y = -84.935185f,
            .direction = 69,
        },
        {
            .vid = 2370,
            .offset_x = -67.372396f,
            .offset_y = -73.935185f,
            .direction = 128,
        },
        {
            .vid = 2340,
            .offset_x = 50.627604f,
            .offset_y = -29.935185f,
            .direction = 64,
        },
        {
            .vid = 2340,
            .offset_x = -39.372396f,
            .offset_y = -105.935185f,
            .direction = 64,
        },
        {
            .vid = 2370,
            .offset_x = 17.627604f,
            .offset_y = 18.064815f,
            .direction = 64,
        },
        {
            .vid = 2340,
            .offset_x = -140.372396f,
            .offset_y = -28.935185f,
            .direction = 64,
        },
        {
            .vid = 2370,
            .offset_x = -182.372396f,
            .offset_y = -0.935185f,
            .direction = 192,
        },
        {
            .vid = 2370,
            .offset_x = 145.627604f,
            .offset_y = -236.935185f,
            .direction = 51,
        },
        {
            .vid = 2337,
            .offset_x = 136.627604f,
            .offset_y = -229.935185f,
            .pos_z = 3.0f,
            .direction = 59,
        },
        {
            .vid = 1639,
            .offset_x = 90.627604f,
            .offset_y = -215.935185f,
            .pos_z = 80.0f,
        },
        {
            .vid = 2342,
            .offset_x = -29.372396f,
            .offset_y = 82.064815f,
            .pos_z = 41.0f,
            .direction = 64,
        },
        {
            .vid = 2342,
            .offset_x = -132.372396f,
            .offset_y = 154.064815f,
            .pos_z = 41.0f,
            .direction = 64,
        },
        {
            .vid = 2342,
            .offset_x = -236.372396f,
            .offset_y = 78.064815f,
            .pos_z = 41.0f,
            .direction = 64,
        },
        {
            .vid = 2338,
            .offset_x = 427.627604f,
            .offset_y = 13.064815f,
            .pos_z = 50.0f,
            .direction = 101,
        },
        {
            .vid = 2348,
            .offset_x = 254.627604f,
            .offset_y = -74.935185f,
            .pos_z = 42.0f,
            .direction = 64,
        },
        {
            .vid = 2342,
            .offset_x = -34.372396f,
            .offset_y = -69.935185f,
            .pos_z = 41.0f,
            .direction = 64,
        },
        {
            .vid = 2342,
            .offset_x = 55.627604f,
            .offset_y = 8.064815f,
            .pos_z = 41.0f,
            .direction = 64,
        },
        {
            .vid = 2342,
            .offset_x = -135.372396f,
            .offset_y = 8.064815f,
            .pos_z = 41.0f,
            .direction = 64,
        },
        {
            .vid = 2338,
            .offset_x = 139.627604f,
            .offset_y = -187.935185f,
            .pos_z = 50.0f,
            .direction = 71,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET8_RANDOM,
};

/**
 * Laboratory desk room imported from _prepare/AS2/decoration/12lab_desks/lab_desks07.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile LAB_DESKS07_DECORATION_PROFILE = {
    .id = "lab_desks07",
    .frame = {
        .corner_offsets = {{
            {           135.0f,          -416.0f },      // top
            {          -585.0f,            96.0f },      // left
            {          -135.0f,           416.0f },      // bottom
            {           585.0f,           -96.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2330,
            .offset_x = 310.254658f,
            .offset_y = -34.996666f,
            .direction = 12,
        },
        {
            .vid = 2330,
            .offset_x = -222.632244f,
            .offset_y = 105.003334f,
            .direction = 38,
        },
        {
            .vid = 427,
            .offset_x = -261.632244f,
            .offset_y = 53.003334f,
            .pos_z = 55.0f,
            .direction = 85,
        },
        {
            .vid = 427,
            .offset_x = 299.254658f,
            .offset_y = -87.996666f,
            .pos_z = 40.0f,
            .direction = 42,
        },
        {
            .vid = 427,
            .offset_x = 60.008076f,
            .offset_y = -167.417076f,
            .pos_z = 40.0f,
        },
        {
            .vid = 2330,
            .offset_x = 127.008076f,
            .offset_y = -153.417076f,
        },
        {
            .vid = 427,
            .offset_x = 210.008076f,
            .offset_y = -167.417076f,
            .pos_z = 56.0f,
            .direction = 42,
        },
        {
            .vid = 427,
            .offset_x = 225.008076f,
            .offset_y = -157.417076f,
            .pos_z = 56.0f,
            .direction = 85,
        },
        {
            .vid = 427,
            .offset_x = 238.008076f,
            .offset_y = -146.417076f,
            .pos_z = 56.0f,
            .direction = 42,
        },
        {
            .vid = 413,
            .offset_x = 265.008076f,
            .offset_y = -121.417076f,
        },
        {
            .vid = 1765,
            .offset_x = 343.254658f,
            .offset_y = -66.996666f,
            .pos_z = 41.0f,
            .direction = 32,
        },
        {
            .vid = 1765,
            .offset_x = 95.008076f,
            .offset_y = -178.417076f,
            .pos_z = 41.0f,
            .direction = 51,
        },
        {
            .vid = 407,
            .offset_x = 272.254658f,
            .offset_y = -33.996666f,
            .direction = 85,
        },
        {
            .vid = 426,
            .offset_x = -225.632244f,
            .offset_y = 29.003334f,
            .pos_z = 60.0f,
        },
        {
            .vid = 407,
            .offset_x = -162.632244f,
            .offset_y = 76.003334f,
            .direction = 170,
        },
        {
            .vid = 426,
            .offset_x = -211.632244f,
            .offset_y = 26.003334f,
            .pos_z = 60.0f,
            .direction = 213,
        },
        {
            .vid = 426,
            .offset_x = -234.632244f,
            .offset_y = 48.003334f,
            .pos_z = 60.0f,
            .direction = 42,
        },
        {
            .vid = 426,
            .offset_x = 332.254658f,
            .offset_y = -86.996666f,
            .pos_z = 40.0f,
            .direction = 128,
        },
        {
            .vid = 426,
            .offset_x = 316.254658f,
            .offset_y = -95.996666f,
            .pos_z = 40.0f,
            .direction = 213,
        },
        {
            .vid = 407,
            .offset_x = 128.008076f,
            .offset_y = -129.417076f,
            .direction = 128,
        },
        {
            .vid = 426,
            .offset_x = 166.008076f,
            .offset_y = -208.417076f,
            .pos_z = 40.0f,
            .direction = 85,
        },
        {
            .vid = 410,
            .offset_x = 127.008076f,
            .offset_y = -193.176842f,
            .pos_z = 40.0f,
            .direction = 128,
        },
        {
            .vid = 426,
            .offset_x = 73.008076f,
            .offset_y = -181.417076f,
            .pos_z = 40.0f,
        },
        {
            .vid = 426,
            .offset_x = 184.008076f,
            .offset_y = -192.417076f,
            .pos_z = 40.0f,
        },
        {
            .vid = 1226,
            .offset_x = 289.254658f,
            .offset_y = -132.996666f,
            .direction = 149,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET8_RANDOM,
};

/**
 * Laboratory desk room imported from _prepare/AS2/decoration/12lab_desks/lab_desks08.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile LAB_DESKS08_DECORATION_PROFILE = {
    .id = "lab_desks08",
    .frame = {
        .corner_offsets = {{
            {            45.0f,          -736.0f },      // top
            {         -1035.0f,            32.0f },      // left
            {           -45.0f,           736.0f },      // bottom
            {          1035.0f,           -32.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2335,
            .offset_x = 205.577881f,
            .offset_y = 173.40918f,
            .direction = 146,
        },
        {
            .vid = 2330,
            .offset_x = -64.41626f,
            .offset_y = 192.240234f,
            .direction = 25,
        },
        {
            .vid = 2330,
            .offset_x = -294.41626f,
            .offset_y = 43.240234f,
            .direction = 38,
        },
        {
            .vid = 409,
            .offset_x = 206.58374f,
            .offset_y = -183.759766f,
        },
        {
            .vid = 2330,
            .offset_x = 271.58374f,
            .offset_y = -99.759766f,
            .direction = 12,
        },
        {
            .vid = 2330,
            .offset_x = 61.58374f,
            .offset_y = -224.759766f,
        },
        {
            .vid = 412,
            .offset_x = -65.41626f,
            .offset_y = -172.759766f,
        },
        {
            .vid = 2335,
            .offset_x = 13.577881f,
            .offset_y = -60.59082f,
            .direction = 109,
        },
        {
            .vid = 1765,
            .offset_x = -285.41626f,
            .offset_y = -30.759766f,
            .pos_z = 57.0f,
            .direction = 64,
        },
        {
            .vid = 1765,
            .offset_x = 114.58374f,
            .offset_y = -266.759766f,
            .pos_z = 41.0f,
            .direction = 51,
        },
        {
            .vid = 426,
            .offset_x = -19.41626f,
            .offset_y = 220.240234f,
            .pos_z = 90.0f,
        },
        {
            .vid = 407,
            .offset_x = -32.41626f,
            .offset_y = 124.240234f,
            .direction = 170,
        },
        {
            .vid = 426,
            .offset_x = -63.41626f,
            .offset_y = 257.240234f,
            .pos_z = 90.0f,
            .direction = 42,
        },
        {
            .vid = 426,
            .offset_x = -40.41626f,
            .offset_y = 239.240234f,
            .pos_z = 90.0f,
        },
        {
            .vid = 426,
            .offset_x = -30.41626f,
            .offset_y = 230.240234f,
            .pos_z = 90.0f,
            .direction = 213,
        },
        {
            .vid = 407,
            .offset_x = -215.41626f,
            .offset_y = 22.240234f,
            .direction = 85,
        },
        {
            .vid = 2333,
            .offset_x = -192.41626f,
            .offset_y = 110.240234f,
            .direction = 51,
        },
        {
            .vid = 2536,
            .offset_x = -155.41626f,
            .offset_y = 254.240234f,
            .direction = 48,
        },
        {
            .vid = 2333,
            .offset_x = -250.41626f,
            .offset_y = 137.240234f,
            .direction = 64,
        },
        {
            .vid = 2333,
            .offset_x = -271.41626f,
            .offset_y = 195.240234f,
            .direction = 76,
        },
        {
            .vid = 2333,
            .offset_x = -219.41626f,
            .offset_y = 226.240234f,
            .direction = 64,
        },
        {
            .vid = 2536,
            .offset_x = -367.41626f,
            .offset_y = 97.240234f,
            .direction = 112,
        },
        {
            .vid = 411,
            .offset_x = 297.58374f,
            .offset_y = -138.759766f,
            .pos_z = 40.0f,
            .direction = 32,
        },
        {
            .vid = 1550,
            .offset_x = 284.58374f,
            .offset_y = -9.759766f,
            .pos_z = 50.0f,
        },
        {
            .vid = 407,
            .offset_x = 235.58374f,
            .offset_y = -100.759766f,
            .direction = 213,
        },
        {
            .vid = 407,
            .offset_x = 69.58374f,
            .offset_y = -206.759766f,
            .direction = 128,
        },
        {
            .vid = 2375,
            .offset_x = -149.41626f,
            .offset_y = -127.759766f,
            .direction = 12,
        },
        {
            .vid = 2337,
            .offset_x = 4.58374f,
            .offset_y = -13.759766f,
        },
        {
            .vid = 426,
            .offset_x = -348.41626f,
            .offset_y = 2.240234f,
            .pos_z = 40.0f,
            .direction = 128,
        },
        {
            .vid = 426,
            .offset_x = 68.58374f,
            .offset_y = -292.759766f,
            .pos_z = 40.0f,
            .direction = 128,
        },
        {
            .vid = 426,
            .offset_x = 91.58374f,
            .offset_y = -274.759766f,
            .pos_z = 40.0f,
            .direction = 213,
        },
        {
            .vid = 426,
            .offset_x = 65.58374f,
            .offset_y = -275.759766f,
            .pos_z = 40.0f,
            .direction = 85,
        },
        {
            .vid = 426,
            .offset_x = 84.58374f,
            .offset_y = -287.759766f,
            .pos_z = 40.0f,
            .direction = 42,
        },
        {
            .vid = 410,
            .offset_x = 11.58374f,
            .offset_y = -242.759766f,
            .pos_z = 40.0f,
        },
        {
            .vid = 426,
            .offset_x = 36.58374f,
            .offset_y = -274.759766f,
            .pos_z = 40.0f,
        },
        {
            .vid = 2339,
            .offset_x = 5.58374f,
            .offset_y = 40.240234f,
            .pos_z = 50.0f,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

} // namespace auto_mapper::core::decoration_builder
