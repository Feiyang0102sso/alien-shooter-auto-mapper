/**
 * @file lab_bioexp_as2.h
 * @brief Authored AS2 laboratory bio-experiment room decoration stamp profiles.
 */

#pragma once

#include "auto_mapper/core/decoration_builder/decoration_profile_types.h"
#include "auto_mapper/core/wall_builder/wall_profiles_as2.h"

namespace auto_mapper::core::decoration_builder {

/**
 * Laboratory bio-experiment room imported from _prepare/AS2/decoration/15lab_bioexp/lab_bioexp01.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile LAB_BIOEXP01_DECORATION_PROFILE = {
    .id = "lab_bioexp01",
    .frame = {
        .corner_offsets = {{
            {           135.0f,          -352.0f },      // top
            {          -495.0f,            96.0f },      // left
            {          -135.0f,           352.0f },      // bottom
            {           495.0f,           -96.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2515,
            .offset_x = -89.856445f,
            .offset_y = 154.254395f,
            .gamma = {
                .r = 10,
                .g = 10,
                .b = 30,
                .a = 0,
            },
        },
        {
            .vid = 2335,
            .offset_x = -79.856445f,
            .offset_y = 98.254395f,
            .direction = 146,
        },
        {
            .vid = 2515,
            .offset_x = 204.143555f,
            .offset_y = -60.745605f,
            .gamma = {
                .r = 10,
                .g = 10,
                .b = 30,
                .a = 0,
            },
        },
        {
            .vid = 2335,
            .offset_x = 56.143555f,
            .offset_y = 2.254395f,
            .direction = 146,
        },
        {
            .vid = 2340,
            .offset_x = -28.856445f,
            .offset_y = 149.0625f,
            .pos_z = 1.0f,
        },
        {
            .vid = 2375,
            .offset_x = 4.577637f,
            .offset_y = 101.105469f,
            .direction = 9,
        },
        {
            .vid = 2340,
            .offset_x = 107.143555f,
            .offset_y = 55.754395f,
            .pos_z = 1.0f,
        },
        {
            .vid = 2340,
            .offset_x = 60.20459f,
            .offset_y = 87.814941f,
            .pos_z = 1.0f,
        },
        {
            .vid = 2340,
            .offset_x = 195.143555f,
            .offset_y = -7.245605f,
            .pos_z = 1.0f,
        },
        {
            .vid = 2375,
            .offset_x = 126.577637f,
            .offset_y = 16.105469f,
            .direction = 9,
        },
        {
            .vid = 2342,
            .offset_x = -28.856445f,
            .offset_y = 198.0625f,
            .pos_z = 51.0f,
        },
        {
            .vid = 2342,
            .offset_x = 107.143555f,
            .offset_y = 104.754395f,
            .pos_z = 51.0f,
        },
        {
            .vid = 2342,
            .offset_x = 60.20459f,
            .offset_y = 136.814941f,
            .pos_z = 51.0f,
        },
        {
            .vid = 2342,
            .offset_x = 195.143555f,
            .offset_y = 41.754395f,
            .pos_z = 51.0f,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Laboratory bio-experiment room imported from _prepare/AS2/decoration/15lab_bioexp/lab_bioexp02.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile LAB_BIOEXP02_DECORATION_PROFILE = {
    .id = "lab_bioexp02",
    .frame = {
        .corner_offsets = {{
            {             0.0f,          -256.0f },      // top
            {          -360.0f,             0.0f },      // left
            {             0.0f,           256.0f },      // bottom
            {           360.0f,             0.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2515,
            .offset_x = 113.676758f,
            .offset_y = 119.177246f,
            .direction = 128,
            .gamma = {
                .r = 10,
                .g = 10,
                .b = 30,
                .a = 0,
            },
        },
        {
            .vid = 2335,
            .offset_x = 72.959961f,
            .offset_y = -41.806152f,
            .direction = 137,
        },
        {
            .vid = 2515,
            .offset_x = -77.323242f,
            .offset_y = -15.822754f,
            .direction = 128,
            .gamma = {
                .r = 10,
                .g = 10,
                .b = 30,
                .a = 0,
            },
        },
        {
            .vid = 2340,
            .offset_x = -52.323242f,
            .offset_y = 36.657227f,
            .pos_z = 1.0f,
            .direction = 64,
        },
        {
            .vid = 2375,
            .offset_x = 8.416504f,
            .offset_y = 55.882324f,
            .direction = 128,
        },
        {
            .vid = 2340,
            .offset_x = 43.458008f,
            .offset_y = 99.627441f,
            .pos_z = 1.0f,
            .direction = 64,
        },
        {
            .vid = 2342,
            .offset_x = -52.323242f,
            .offset_y = 85.657227f,
            .pos_z = 51.0f,
            .direction = 64,
        },
        {
            .vid = 2342,
            .offset_x = 43.458008f,
            .offset_y = 148.627441f,
            .pos_z = 51.0f,
            .direction = 64,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Laboratory bio-experiment room imported from _prepare/AS2/decoration/15lab_bioexp/lab_bioexp03.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile LAB_BIOEXP03_DECORATION_PROFILE = {
    .id = "lab_bioexp03",
    .frame = {
        .corner_offsets = {{
            {           135.0f,          -224.0f },      // top
            {          -315.0f,            96.0f },      // left
            {          -135.0f,           224.0f },      // bottom
            {           315.0f,           -96.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2515,
            .offset_x = 166.203613f,
            .offset_y = 15.662109f,
            .direction = 128,
            .gamma = {
                .r = 20,
                .g = 20,
                .b = 30,
                .a = 0,
            },
        },
        {
            .vid = 2515,
            .offset_x = -76.796387f,
            .offset_y = 114.662109f,
            .gamma = {
                .r = 20,
                .g = 20,
                .b = 30,
                .a = 0,
            },
        },
        {
            .vid = 2515,
            .offset_x = 16.203613f,
            .offset_y = 115.662109f,
            .direction = 128,
            .gamma = {
                .r = 20,
                .g = 20,
                .b = 30,
                .a = 0,
            },
        },
        {
            .vid = 2515,
            .offset_x = 164.203613f,
            .offset_y = -46.337891f,
            .gamma = {
                .r = 20,
                .g = 20,
                .b = 30,
                .a = 0,
            },
        },
        {
            .vid = 2333,
            .offset_x = 97.203613f,
            .offset_y = 51.662109f,
            .direction = 64,
        },
        {
            .vid = 2333,
            .offset_x = 55.203613f,
            .offset_y = 59.662109f,
            .direction = 76,
        },
        {
            .vid = 2378,
            .offset_x = -19.830566f,
            .offset_y = 54.587402f,
        },
        {
            .vid = 2333,
            .offset_x = 165.203613f,
            .offset_y = -20.337891f,
            .direction = 51,
        },
        {
            .vid = 2378,
            .offset_x = 92.203613f,
            .offset_y = -4.883789f,
            .pos_z = 8.0f,
        },
        {
            .vid = 2333,
            .offset_x = 15.203613f,
            .offset_y = -0.337891f,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Laboratory bio-experiment room imported from _prepare/AS2/decoration/15lab_bioexp/lab_bioexp04.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile LAB_BIOEXP04_DECORATION_PROFILE = {
    .id = "lab_bioexp04",
    .frame = {
        .corner_offsets = {{
            {          -405.0f,          -736.0f },      // top
            {         -1035.0f,          -288.0f },      // left
            {           405.0f,           736.0f },      // bottom
            {          1035.0f,           288.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2323,
            .offset_x = 312.632812f,
            .offset_y = 175.73291f,
            .direction = 250,
        },
        {
            .vid = 2335,
            .offset_x = 163.632812f,
            .offset_y = 287.73291f,
            .direction = 80,
        },
        {
            .vid = 2323,
            .offset_x = -167.367188f,
            .offset_y = -172.26709f,
        },
        {
            .vid = 2335,
            .offset_x = -321.367188f,
            .offset_y = -58.26709f,
            .direction = 80,
        },
        {
            .vid = 2324,
            .offset_x = 237.632812f,
            .offset_y = 34.73291f,
        },
        {
            .vid = 2340,
            .offset_x = 80.632812f,
            .offset_y = 172.73291f,
            .direction = 64,
        },
        {
            .vid = 2333,
            .offset_x = 121.632812f,
            .offset_y = -94.26709f,
            .direction = 179,
        },
        {
            .vid = 2360,
            .offset_x = 71.632812f,
            .offset_y = -3.26709f,
            .direction = 56,
        },
        {
            .vid = 2333,
            .offset_x = -56.367188f,
            .offset_y = -57.26709f,
            .direction = 192,
        },
        {
            .vid = 2333,
            .offset_x = -59.367188f,
            .offset_y = -91.26709f,
            .direction = 164,
        },
        {
            .vid = 2324,
            .offset_x = 36.632812f,
            .offset_y = -115.26709f,
        },
        {
            .vid = 2340,
            .offset_x = -160.367188f,
            .offset_y = -3.26709f,
            .direction = 64,
        },
        {
            .vid = 2343,
            .offset_x = 86.632812f,
            .offset_y = 211.73291f,
            .pos_z = 41.0f,
            .direction = 64,
        },
        {
            .vid = 2361,
            .offset_x = 71.632812f,
            .offset_y = 40.73291f,
            .pos_z = 40.0f,
            .direction = 56,
        },
        {
            .vid = 2343,
            .offset_x = -155.367188f,
            .offset_y = 31.73291f,
            .pos_z = 41.0f,
            .direction = 64,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Laboratory bio-experiment room imported from _prepare/AS2/decoration/15lab_bioexp/lab_bioexp05.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile LAB_BIOEXP05_DECORATION_PROFILE = {
    .id = "lab_bioexp05",
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
            .vid = 2323,
            .offset_x = 40.642578f,
            .offset_y = -4.428711f,
            .direction = 67,
        },
        {
            .vid = 2334,
            .offset_x = -199.357422f,
            .offset_y = 36.571289f,
            .direction = 64,
        },
        {
            .vid = 2334,
            .offset_x = 103.642578f,
            .offset_y = -171.428711f,
            .direction = 64,
        },
        {
            .vid = 2333,
            .offset_x = 95.642578f,
            .offset_y = 148.571289f,
            .direction = 153,
        },
        {
            .vid = 2333,
            .offset_x = 68.642578f,
            .offset_y = 199.571289f,
            .direction = 166,
        },
        {
            .vid = 2360,
            .offset_x = -140.357422f,
            .offset_y = 210.571289f,
            .direction = 64,
        },
        {
            .vid = 2326,
            .offset_x = -73.5f,
            .offset_y = 121.0f,
            .direction = 192,
        },
        {
            .vid = 2326,
            .offset_x = 216.5f,
            .offset_y = -82.0f,
            .direction = 192,
        },
        {
            .vid = 2364,
            .offset_x = -140.357422f,
            .offset_y = 210.571289f,
            .direction = 64,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Laboratory bio-experiment room imported from _prepare/AS2/decoration/15lab_bioexp/lab_bioexp06.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile LAB_BIOEXP06_DECORATION_PROFILE = {
    .id = "lab_bioexp06",
    .frame = {
        .corner_offsets = {{
            {          -315.0f,          -480.0f },      // top
            {          -675.0f,          -224.0f },      // left
            {           315.0f,           480.0f },      // bottom
            {           675.0f,           224.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2335,
            .offset_x = 178.074951f,
            .offset_y = 170.969238f,
            .direction = 146,
        },
        {
            .vid = 2334,
            .offset_x = 262.650879f,
            .offset_y = 162.247559f,
            .direction = 192,
        },
        {
            .vid = 2335,
            .offset_x = -0.349121f,
            .offset_y = -3.752441f,
            .direction = 192,
        },
        {
            .vid = 2515,
            .offset_x = -3.349121f,
            .offset_y = 71.247559f,
            .gamma = {
                .r = 20,
                .g = 20,
                .b = 30,
                .a = 0,
            },
        },
        {
            .vid = 2335,
            .offset_x = -80.18457f,
            .offset_y = -108.434082f,
            .direction = 109,
        },
        {
            .vid = 2334,
            .offset_x = -161.349121f,
            .offset_y = -127.752441f,
            .direction = 64,
        },
        {
            .vid = 2340,
            .offset_x = 204.650879f,
            .offset_y = 133.247559f,
            .direction = 64,
        },
        {
            .vid = 2326,
            .offset_x = 41.0f,
            .offset_y = 34.0f,
            .direction = 192,
        },
        {
            .vid = 2333,
            .offset_x = 112.650879f,
            .offset_y = 5.247559f,
            .direction = 102,
        },
        {
            .vid = 2333,
            .offset_x = -126.349121f,
            .offset_y = -7.752441f,
            .direction = 128,
        },
        {
            .vid = 2333,
            .offset_x = -62.349121f,
            .offset_y = 20.247559f,
            .direction = 140,
        },
        {
            .vid = 2333,
            .offset_x = 55.650879f,
            .offset_y = -36.752441f,
            .direction = 102,
        },
        {
            .vid = 2333,
            .offset_x = 83.650879f,
            .offset_y = -16.752441f,
            .direction = 102,
        },
        {
            .vid = 2333,
            .offset_x = -192.349121f,
            .offset_y = -84.752441f,
            .direction = 76,
        },
        {
            .vid = 2340,
            .offset_x = -101.349121f,
            .offset_y = -73.752441f,
            .direction = 64,
        },
        {
            .vid = 2342,
            .offset_x = 212.650879f,
            .offset_y = 179.247559f,
            .pos_z = 51.0f,
            .direction = 64,
        },
        {
            .vid = 2342,
            .offset_x = -94.349121f,
            .offset_y = -26.752441f,
            .pos_z = 51.0f,
            .direction = 64,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Laboratory bio-experiment room imported from _prepare/AS2/decoration/15lab_bioexp/lab_bioexp07.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile LAB_BIOEXP07_DECORATION_PROFILE = {
    .id = "lab_bioexp07",
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
            .vid = 2334,
            .offset_x = 162.25f,
            .offset_y = 58.25f,
            .direction = 160,
        },
        {
            .vid = 2334,
            .offset_x = 96.25f,
            .offset_y = 109.25f,
            .direction = 160,
        },
        {
            .vid = 2326,
            .offset_x = 62.5f,
            .offset_y = 38.0f,
            .direction = 137,
        },
        {
            .vid = 2340,
            .offset_x = 10.25f,
            .offset_y = -44.75f,
            .direction = 128,
        },
        {
            .vid = 2343,
            .offset_x = 17.25f,
            .offset_y = 1.25f,
            .pos_z = 41.0f,
            .direction = 128,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Laboratory bio-experiment room imported from _prepare/AS2/decoration/15lab_bioexp/lab_bioexp08.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile LAB_BIOEXP08_DECORATION_PROFILE = {
    .id = "lab_bioexp08",
    .frame = {
        .corner_offsets = {{
            {           -90.0f,          -448.0f },      // top
            {          -630.0f,           -64.0f },      // left
            {            90.0f,           448.0f },      // bottom
            {           630.0f,            64.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 1770,
            .offset_x = 215.045313f,
            .offset_y = 269.032222f,
            .direction = 224,
            .gamma = {
                .r = 50,
                .g = 50,
                .b = 50,
                .a = 0,
            },
        },
        {
            .vid = 2335,
            .offset_x = 97.045313f,
            .offset_y = 6.032222f,
            .direction = 36,
        },
        {
            .vid = 2335,
            .offset_x = -171.954687f,
            .offset_y = -13.967778f,
            .direction = 91,
        },
        {
            .vid = 2333,
            .offset_x = 232.045313f,
            .offset_y = 171.032222f,
            .direction = 115,
        },
        {
            .vid = 2340,
            .offset_x = 330.6625f,
            .offset_y = 128.032222f,
            .direction = 64,
        },
        {
            .vid = 2333,
            .offset_x = 288.045313f,
            .offset_y = 199.032222f,
            .direction = 102,
        },
        {
            .vid = 1270,
            .offset_x = 189.045313f,
            .offset_y = 286.032222f,
            .pos_z = 35.0f,
        },
        {
            .vid = 407,
            .offset_x = 158.045313f,
            .offset_y = 240.032222f,
            .direction = 213,
        },
        {
            .vid = 2340,
            .offset_x = 172.171777f,
            .offset_y = 15.988765f,
            .direction = 64,
        },
        {
            .vid = 2324,
            .offset_x = 351.045313f,
            .offset_y = 49.032222f,
            .pos_z = -1.0f,
        },
        {
            .vid = 2324,
            .offset_x = 183.045313f,
            .offset_y = -69.967778f,
            .pos_z = -1.0f,
        },
        {
            .vid = 2340,
            .offset_x = 12.136621f,
            .offset_y = -96.874028f,
            .direction = 64,
        },
        {
            .vid = 2333,
            .offset_x = -168.954687f,
            .offset_y = 64.032222f,
            .direction = 153,
        },
        {
            .vid = 2340,
            .offset_x = -189.771094f,
            .offset_y = -67.640629f,
            .direction = 228,
        },
        {
            .vid = 2324,
            .offset_x = -278.954687f,
            .offset_y = -73.967778f,
            .pos_z = -1.0f,
            .direction = 128,
        },
        {
            .vid = 2333,
            .offset_x = -27.954687f,
            .offset_y = -252.967778f,
            .direction = 89,
        },
        {
            .vid = 2333,
            .offset_x = -54.954687f,
            .offset_y = -298.967778f,
            .direction = 64,
        },
        {
            .vid = 2324,
            .offset_x = 23.045313f,
            .offset_y = -188.967778f,
            .pos_z = -1.0f,
        },
        {
            .vid = 2342,
            .offset_x = 330.6625f,
            .offset_y = 178.032222f,
            .pos_z = 50.0f,
            .direction = 64,
        },
        {
            .vid = 2342,
            .offset_x = 172.171777f,
            .offset_y = 67.988765f,
            .pos_z = 50.0f,
            .direction = 64,
        },
        {
            .vid = 2342,
            .offset_x = 12.136621f,
            .offset_y = -46.874028f,
            .pos_z = 50.0f,
            .direction = 64,
        },
        {
            .vid = 2342,
            .offset_x = -189.771094f,
            .offset_y = -17.640629f,
            .pos_z = 50.0f,
            .direction = 228,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET8_RANDOM,
};

} // namespace auto_mapper::core::decoration_builder
