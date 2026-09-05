/**
 * @file garage_as2.h
 * @brief Authored AS2 garage decoration stamp profiles.
 */

#pragma once

#include "auto_mapper/core/decoration_builder/decoration_profile_types.h"
#include "auto_mapper/core/wall_builder/wall_profiles_as2.h"

namespace auto_mapper::core::decoration_builder {

/**
 * Garage imported from _prepare/AS2/decoration/07garage/garage01.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile GARAGE01_DECORATION_PROFILE = {
    .id = "garage01",
    .frame = {
        .corner_offsets = {{
            {           -90.0f,          -384.0f },      // top
            {          -540.0f,           -64.0f },      // left
            {            90.0f,           384.0f },      // bottom
            {           540.0f,            64.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 1280,
            .offset_x = 183.557129f,
            .offset_y = 61.411377f,
            .pos_z = 23.0f,
            .direction = 250,
        },
        {
            .vid = 442,
            .offset_x = 48.0f,
            .offset_y = 40.0f,
            .direction = 204,
        },
        {
            .vid = 1215,
            .offset_x = 72.0f,
            .offset_y = 16.0f,
        },
        {
            .vid = 442,
            .offset_x = -14.0f,
            .offset_y = -10.0f,
        },
        {
            .vid = 442,
            .offset_x = 101.0f,
            .offset_y = -3.0f,
            .direction = 51,
        },
        {
            .vid = 1294,
            .offset_x = -1.0f,
            .offset_y = -13.0f,
            .pos_z = 26.0f,
            .direction = 16,
        },
        {
            .vid = 1221,
            .offset_x = 131.0f,
            .offset_y = 73.0f,
            .pos_z = 23.0f,
            .direction = 64,
        },
        {
            .vid = 1221,
            .offset_x = 104.0f,
            .offset_y = 107.0f,
            .pos_z = 23.0f,
            .direction = 128,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Garage imported from _prepare/AS2/decoration/07garage/garage02.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile GARAGE02_DECORATION_PROFILE = {
    .id = "garage02",
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
            .vid = 1215,
            .offset_x = 79.0f,
            .offset_y = 54.0f,
        },
        {
            .vid = 1202,
            .offset_x = 54.0f,
            .offset_y = 68.0f,
            .pos_z = -11.0f,
            .direction = 224,
        },
        {
            .vid = 1294,
            .offset_x = -1.0f,
            .offset_y = 10.0f,
            .pos_z = 26.0f,
            .direction = 16,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Garage imported from _prepare/AS2/decoration/07garage/garage03.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile GARAGE03_DECORATION_PROFILE = {
    .id = "garage03",
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
            .vid = 2515,
            .offset_x = 146.224772f,
            .offset_y = 165.572551f,
            .direction = 144,
        },
        {
            .vid = 2515,
            .offset_x = 67.223796f,
            .offset_y = 225.572551f,
            .direction = 144,
        },
        {
            .vid = 2515,
            .offset_x = 416.872355f,
            .offset_y = -93.425251f,
        },
        {
            .vid = 2515,
            .offset_x = 419.224772f,
            .offset_y = -25.427449f,
            .direction = 144,
        },
        {
            .vid = 2515,
            .offset_x = 329.224772f,
            .offset_y = 37.572551f,
            .direction = 144,
        },
        {
            .vid = 2315,
            .offset_x = 54.628215f,
            .offset_y = -44.575154f,
            .pos_z = -2.0f,
            .direction = 64,
        },
        {
            .vid = 2515,
            .offset_x = 237.224772f,
            .offset_y = 101.572551f,
            .direction = 144,
        },
        {
            .vid = 2515,
            .offset_x = -308.529012f,
            .offset_y = -44.425251f,
            .direction = 144,
        },
        {
            .vid = 2515,
            .offset_x = -229.528035f,
            .offset_y = -104.425251f,
            .direction = 144,
        },
        {
            .vid = 1220,
            .offset_x = 123.628215f,
            .offset_y = -255.575154f,
            .pos_z = 30.0f,
            .direction = 23,
        },
        {
            .vid = 1220,
            .offset_x = 131.628215f,
            .offset_y = -233.575154f,
            .pos_z = 30.0f,
            .direction = 139,
        },
        {
            .vid = 1220,
            .offset_x = 121.628215f,
            .offset_y = -221.575154f,
            .pos_z = 20.0f,
            .direction = 116,
        },
        {
            .vid = 1220,
            .offset_x = 133.628215f,
            .offset_y = -248.575154f,
            .pos_z = 20.0f,
            .direction = 116,
        },
        {
            .vid = 1220,
            .offset_x = 125.628215f,
            .offset_y = -232.575154f,
            .pos_z = 11.0f,
            .direction = 186,
        },
        {
            .vid = 1220,
            .offset_x = 126.628215f,
            .offset_y = -269.575154f,
            .pos_z = 11.0f,
            .direction = 209,
        },
        {
            .vid = 1220,
            .offset_x = 102.628215f,
            .offset_y = -288.575154f,
            .direction = 139,
        },
        {
            .vid = 1220,
            .offset_x = 138.628215f,
            .offset_y = -256.575154f,
            .pos_z = 4.0f,
            .direction = 116,
        },
        {
            .vid = 1220,
            .offset_x = 126.628215f,
            .offset_y = -227.575154f,
            .direction = 116,
        },
        {
            .vid = 1220,
            .offset_x = 124.628215f,
            .offset_y = -246.575154f,
            .direction = 139,
        },
        {
            .vid = 1220,
            .offset_x = 125.628215f,
            .offset_y = -266.575154f,
            .direction = 153,
        },
        {
            .vid = 2515,
            .offset_x = 43.471965f,
            .offset_y = -295.425251f,
            .direction = 144,
        },
        {
            .vid = 2515,
            .offset_x = 133.872355f,
            .offset_y = -296.425251f,
        },
        {
            .vid = 1244,
            .offset_x = 232.872355f,
            .offset_y = -191.425251f,
            .pos_z = 6.0f,
            .direction = 128,
        },
        {
            .vid = 2515,
            .offset_x = -46.528035f,
            .offset_y = -232.425251f,
            .direction = 144,
        },
        {
            .vid = 2515,
            .offset_x = -138.528035f,
            .offset_y = -168.425251f,
            .direction = 144,
        },
        {
            .vid = 1221,
            .offset_x = 368.628215f,
            .offset_y = -34.575154f,
            .direction = 64,
        },
        {
            .vid = 1221,
            .offset_x = 342.628215f,
            .offset_y = -83.575154f,
            .direction = 64,
        },
        {
            .vid = 1221,
            .offset_x = 388.628215f,
            .offset_y = -57.575154f,
            .direction = 192,
        },
        {
            .vid = 1221,
            .offset_x = 368.628215f,
            .offset_y = -88.575154f,
            .direction = 160,
        },
        {
            .vid = 1221,
            .offset_x = 332.628215f,
            .offset_y = -105.575154f,
            .direction = 128,
        },
        {
            .vid = 1221,
            .offset_x = 90.628215f,
            .offset_y = -183.575154f,
            .pos_z = 7.0f,
            .direction = 96,
        },
        {
            .vid = 1221,
            .offset_x = 114.628215f,
            .offset_y = -200.575154f,
            .pos_z = 7.0f,
            .direction = 64,
        },
        {
            .vid = 1221,
            .offset_x = 66.628215f,
            .offset_y = -261.575154f,
            .direction = 128,
        },
        {
            .vid = 1221,
            .offset_x = 93.628215f,
            .offset_y = -297.575154f,
            .direction = 96,
        },
        {
            .vid = 1221,
            .offset_x = 91.628215f,
            .offset_y = -256.575154f,
            .direction = 64,
        },
        {
            .vid = 1221,
            .offset_x = 164.628215f,
            .offset_y = -239.575154f,
            .direction = 64,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET5_RANDOM,
};

/**
 * Garage imported from _prepare/AS2/decoration/07garage/garage04.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile GARAGE04_DECORATION_PROFILE = {
    .id = "garage04",
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
            .vid = 2515,
            .offset_x = 153.909485f,
            .offset_y = 152.466614f,
            .direction = 144,
        },
        {
            .vid = 2515,
            .offset_x = 62.909485f,
            .offset_y = 216.466614f,
            .direction = 144,
        },
        {
            .vid = 2515,
            .offset_x = 245.909485f,
            .offset_y = 88.466614f,
            .direction = 144,
        },
        {
            .vid = 1244,
            .offset_x = -171.87793f,
            .offset_y = 159.574951f,
            .direction = 128,
        },
        {
            .vid = 2515,
            .offset_x = 335.909485f,
            .offset_y = 25.466553f,
            .direction = 144,
        },
        {
            .vid = 2315,
            .offset_x = 35.87793f,
            .offset_y = -7.574951f,
            .pos_z = -3.0f,
            .direction = 96,
        },
        {
            .vid = 2515,
            .offset_x = -157.479492f,
            .offset_y = -107.825439f,
            .direction = 144,
        },
        {
            .vid = 2515,
            .offset_x = -65.479492f,
            .offset_y = -171.825439f,
            .direction = 144,
        },
        {
            .vid = 2515,
            .offset_x = -248.479492f,
            .offset_y = -43.825439f,
            .direction = 144,
        },
        {
            .vid = 2515,
            .offset_x = 24.520508f,
            .offset_y = -234.825439f,
            .direction = 144,
        },
        {
            .vid = 1222,
            .offset_x = -206.87793f,
            .offset_y = 167.574951f,
            .direction = 128,
        },
        {
            .vid = 1222,
            .offset_x = -146.87793f,
            .offset_y = 213.574951f,
            .direction = 170,
        },
        {
            .vid = 1222,
            .offset_x = -255.87793f,
            .offset_y = 113.574951f,
            .direction = 85,
        },
        {
            .vid = 1222,
            .offset_x = -266.87793f,
            .offset_y = -113.425049f,
            .direction = 42,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET5_RANDOM,
};

/**
 * Garage imported from _prepare/AS2/decoration/07garage/garage05.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile GARAGE05_DECORATION_PROFILE = {
    .id = "garage05",
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
            .vid = 2515,
            .offset_x = 315.720703f,
            .offset_y = 77.975342f,
            .direction = 144,
        },
        {
            .vid = 1220,
            .offset_x = 124.120117f,
            .offset_y = 240.574951f,
            .direction = 186,
        },
        {
            .vid = 1220,
            .offset_x = 103.120117f,
            .offset_y = 247.574951f,
            .pos_z = 4.0f,
            .direction = 116,
        },
        {
            .vid = 1220,
            .offset_x = 102.120117f,
            .offset_y = 259.574951f,
            .direction = 160,
        },
        {
            .vid = 1220,
            .offset_x = 128.120117f,
            .offset_y = 229.574951f,
            .pos_z = 7.0f,
            .direction = 139,
        },
        {
            .vid = 2515,
            .offset_x = 132.719727f,
            .offset_y = 205.975342f,
            .direction = 144,
        },
        {
            .vid = 1220,
            .offset_x = 146.120117f,
            .offset_y = 227.574951f,
            .direction = 232,
        },
        {
            .vid = 1245,
            .offset_x = 274.120117f,
            .offset_y = 151.574951f,
            .pos_z = -2.0f,
            .direction = 42,
        },
        {
            .vid = 1220,
            .offset_x = 173.120117f,
            .offset_y = 201.574951f,
            .pos_z = -3.0f,
            .direction = 139,
        },
        {
            .vid = 2515,
            .offset_x = 223.719727f,
            .offset_y = 141.975342f,
            .direction = 144,
        },
        {
            .vid = 1220,
            .offset_x = 67.120117f,
            .offset_y = 278.574951f,
            .pos_z = -3.0f,
            .direction = 139,
        },
        {
            .vid = 2515,
            .offset_x = 42.719727f,
            .offset_y = 268.975342f,
            .direction = 144,
        },
        {
            .vid = 2515,
            .offset_x = 405.121094f,
            .offset_y = -50.425049f,
        },
        {
            .vid = 2515,
            .offset_x = 405.720703f,
            .offset_y = 14.975342f,
            .direction = 144,
        },
        {
            .vid = 1244,
            .offset_x = 273.120117f,
            .offset_y = -170.425049f,
        },
        {
            .vid = 2315,
            .offset_x = 76.120117f,
            .offset_y = -4.425049f,
            .pos_z = -1.0f,
        },
        {
            .vid = 2515,
            .offset_x = -68.091309f,
            .offset_y = -186.602509f,
            .direction = 144,
        },
        {
            .vid = 2515,
            .offset_x = -160.092285f,
            .offset_y = -122.602478f,
            .direction = 144,
        },
        {
            .vid = 2515,
            .offset_x = -251.092285f,
            .offset_y = -58.602478f,
            .direction = 144,
        },
        {
            .vid = 2515,
            .offset_x = -341.092285f,
            .offset_y = 4.397522f,
            .direction = 144,
        },
        {
            .vid = 2528,
            .offset_x = 295.120117f,
            .offset_y = -232.425049f,
            .direction = 24,
        },
        {
            .vid = 2515,
            .offset_x = 114.120117f,
            .offset_y = -253.425049f,
        },
        {
            .vid = 2520,
            .offset_x = 143.120117f,
            .offset_y = -305.425049f,
            .direction = 76,
        },
        {
            .vid = 2520,
            .offset_x = 198.120117f,
            .offset_y = -343.425049f,
            .direction = 76,
        },
        {
            .vid = 2515,
            .offset_x = 21.908691f,
            .offset_y = -249.602524f,
            .direction = 144,
        },
        {
            .vid = 1222,
            .offset_x = 118.120117f,
            .offset_y = 361.574951f,
            .direction = 170,
        },
        {
            .vid = 1222,
            .offset_x = 169.120117f,
            .offset_y = 403.574951f,
            .direction = 213,
        },
        {
            .vid = 1222,
            .offset_x = 335.119141f,
            .offset_y = 146.574951f,
        },
        {
            .vid = 1222,
            .offset_x = 223.120117f,
            .offset_y = 186.574951f,
            .direction = 42,
        },
        {
            .vid = 1221,
            .offset_x = 88.120117f,
            .offset_y = 307.574951f,
            .direction = 139,
        },
        {
            .vid = 401,
            .offset_x = 450.121094f,
            .offset_y = -102.425049f,
            .direction = 252,
        },
        {
            .vid = 1222,
            .offset_x = 456.121094f,
            .offset_y = -168.425049f,
            .direction = 213,
        },
        {
            .vid = 401,
            .offset_x = 510.118164f,
            .offset_y = -71.425049f,
            .direction = 196,
        },
        {
            .vid = 1222,
            .offset_x = 533.121094f,
            .offset_y = -138.425049f,
        },
        {
            .vid = 401,
            .offset_x = 483.119141f,
            .offset_y = -67.425049f,
            .direction = 125,
        },
        {
            .vid = 401,
            .offset_x = 452.121094f,
            .offset_y = -47.425049f,
            .direction = 252,
        },
        {
            .vid = 401,
            .offset_x = 492.118164f,
            .offset_y = -106.425049f,
            .direction = 196,
        },
        {
            .vid = 1222,
            .offset_x = 264.120117f,
            .offset_y = -153.425049f,
        },
        {
            .vid = 1222,
            .offset_x = 315.121094f,
            .offset_y = -140.425049f,
            .direction = 213,
        },
        {
            .vid = 1222,
            .offset_x = 220.120117f,
            .offset_y = -202.425049f,
            .direction = 170,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET5_RANDOM,
};

/**
 * Garage imported from _prepare/AS2/decoration/07garage/garage06.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile GARAGE06_DECORATION_PROFILE = {
    .id = "garage06",
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
            .vid = 1220,
            .offset_x = 210.189734f,
            .offset_y = 210.793206f,
            .pos_z = -5.0f,
            .direction = 139,
        },
        {
            .vid = 1220,
            .offset_x = 175.189734f,
            .offset_y = 229.793206f,
            .pos_z = -5.0f,
            .direction = 116,
        },
        {
            .vid = 1220,
            .offset_x = 212.189734f,
            .offset_y = 233.793206f,
            .pos_z = -2.0f,
        },
        {
            .vid = 2515,
            .offset_x = 194.189734f,
            .offset_y = 200.793206f,
            .direction = 144,
        },
        {
            .vid = 1220,
            .offset_x = 44.189734f,
            .offset_y = 313.793206f,
            .pos_z = -1.0f,
            .direction = 69,
        },
        {
            .vid = 2515,
            .offset_x = 12.189734f,
            .offset_y = 264.793206f,
        },
        {
            .vid = 1220,
            .offset_x = 46.189734f,
            .offset_y = 265.793206f,
            .pos_z = -3.0f,
            .direction = 69,
        },
        {
            .vid = 1220,
            .offset_x = 24.189734f,
            .offset_y = 296.793206f,
            .pos_z = -3.0f,
            .direction = 46,
        },
        {
            .vid = 2515,
            .offset_x = 104.189734f,
            .offset_y = 266.793206f,
            .direction = 144,
        },
        {
            .vid = 2508,
            .offset_x = 524.862585f,
            .offset_y = -12.240485f,
        },
        {
            .vid = 2515,
            .offset_x = 285.189734f,
            .offset_y = 136.793206f,
            .direction = 144,
        },
        {
            .vid = 1220,
            .offset_x = 312.189734f,
            .offset_y = 89.793206f,
            .pos_z = -2.0f,
        },
        {
            .vid = 1220,
            .offset_x = 252.189734f,
            .offset_y = 84.793206f,
            .pos_z = -2.0f,
        },
        {
            .vid = 1220,
            .offset_x = 281.189734f,
            .offset_y = 81.793206f,
            .pos_z = -2.0f,
            .direction = 23,
        },
        {
            .vid = 1220,
            .offset_x = 295.189734f,
            .offset_y = 86.793206f,
            .pos_z = 4.0f,
            .direction = 209,
        },
        {
            .vid = 1220,
            .offset_x = 305.189734f,
            .offset_y = -8.206794f,
            .pos_z = -2.0f,
            .direction = 139,
        },
        {
            .vid = 1220,
            .offset_x = 280.189734f,
            .offset_y = 60.793206f,
            .pos_z = -2.0f,
            .direction = 139,
        },
        {
            .vid = 1220,
            .offset_x = 274.189734f,
            .offset_y = 78.793206f,
            .pos_z = 11.0f,
            .direction = 162,
        },
        {
            .vid = 1220,
            .offset_x = 263.189734f,
            .offset_y = 72.793206f,
            .pos_z = 4.0f,
            .direction = 116,
        },
        {
            .vid = 1220,
            .offset_x = 279.189734f,
            .offset_y = 98.793206f,
            .pos_z = -4.0f,
            .direction = 116,
        },
        {
            .vid = 2315,
            .offset_x = 79.189734f,
            .offset_y = 12.793206f,
            .direction = 32,
        },
        {
            .vid = 1244,
            .offset_x = -166.810266f,
            .offset_y = 182.793206f,
        },
        {
            .vid = 2520,
            .offset_x = -280.810266f,
            .offset_y = -18.206794f,
            .direction = 76,
        },
        {
            .vid = 2515,
            .offset_x = -286.810266f,
            .offset_y = 71.793206f,
        },
        {
            .vid = 2314,
            .offset_x = 75.189734f,
            .offset_y = 8.793206f,
            .pos_z = 90.0f,
            .direction = 144,
        },
        {
            .vid = 2520,
            .offset_x = -216.810266f,
            .offset_y = -63.206794f,
            .direction = 76,
        },
        {
            .vid = 131,
            .offset_x = -78.810266f,
            .offset_y = 212.793206f,
            .direction = 144,
        },
        {
            .vid = 131,
            .offset_x = -44.810266f,
            .offset_y = 235.793206f,
            .direction = 144,
        },
        {
            .vid = 131,
            .offset_x = -115.810266f,
            .offset_y = 187.793206f,
            .direction = 144,
        },
        {
            .vid = 131,
            .offset_x = -186.810266f,
            .offset_y = 138.793206f,
            .direction = 144,
        },
        {
            .vid = 131,
            .offset_x = -219.810266f,
            .offset_y = 117.793206f,
            .direction = 144,
        },
        {
            .vid = 131,
            .offset_x = -149.810266f,
            .offset_y = 162.793206f,
            .direction = 144,
        },
        {
            .vid = 401,
            .offset_x = -234.810266f,
            .offset_y = 223.793206f,
            .direction = 185,
        },
        {
            .vid = 401,
            .offset_x = -327.137415f,
            .offset_y = 196.759515f,
        },
        {
            .vid = 1221,
            .offset_x = 234.189734f,
            .offset_y = 149.793206f,
            .direction = 192,
        },
        {
            .vid = 1221,
            .offset_x = 246.189734f,
            .offset_y = 116.793206f,
            .direction = 162,
        },
        {
            .vid = 791,
            .offset_x = 9023.700476f,
            .offset_y = 4077.134026f,
            .pos_z = 10.0f,
        },
        {
            .vid = 791,
            .offset_x = 9023.700476f,
            .offset_y = 4077.134026f,
            .pos_z = 10.0f,
        },
        {
            .vid = 791,
            .offset_x = 9003.700476f,
            .offset_y = 4105.416253f,
            .pos_z = 10.0f,
        },
        {
            .vid = 791,
            .offset_x = 9023.700476f,
            .offset_y = 4077.134026f,
            .pos_z = 10.0f,
        },
        {
            .vid = 791,
            .offset_x = 9023.700476f,
            .offset_y = 4077.134026f,
            .pos_z = 10.0f,
        },
        {
            .vid = 791,
            .offset_x = 9003.700476f,
            .offset_y = 4105.416253f,
            .pos_z = 10.0f,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET5_RANDOM,
};

/**
 * Garage imported from _prepare/AS2/decoration/07garage/garage07.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile GARAGE07_DECORATION_PROFILE = {
    .id = "garage07",
    .frame = {
        .corner_offsets = {{
            {             0.0f,          -512.0f },      // top
            {          -720.0f,             0.0f },      // left
            {             0.0f,           512.0f },      // bottom
            {           720.0f,             0.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2316,
            .offset_x = 269.857178f,
            .offset_y = 26.142578f,
            .direction = 80,
        },
        {
            .vid = 2519,
            .offset_x = -44.143311f,
            .offset_y = 72.142578f,
            .pos_z = -3.0f,
        },
        {
            .vid = 1209,
            .offset_x = 68.857178f,
            .offset_y = 46.142578f,
            .direction = 192,
        },
        {
            .vid = 2313,
            .offset_x = 68.857178f,
            .offset_y = 43.142578f,
            .direction = 64,
            .gamma = {
                .r = -100,
                .g = -100,
                .b = -100,
                .a = 0,
            },
        },
        {
            .vid = 2316,
            .offset_x = 356.857178f,
            .offset_y = -49.857422f,
            .direction = 32,
        },
        {
            .vid = 1245,
            .offset_x = 116.857178f,
            .offset_y = -122.857422f,
            .direction = 64,
        },
        {
            .vid = 2516,
            .offset_x = -18.143188f,
            .offset_y = -161.857422f,
            .gamma = {
                .r = -50,
                .g = -50,
                .b = -50,
                .a = 0,
            },
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Garage imported from _prepare/AS2/decoration/07garage/garage08.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile GARAGE08_DECORATION_PROFILE = {
    .id = "garage08",
    .frame = {
        .corner_offsets = {{
            {          -135.0f,          -672.0f },      // top
            {          -945.0f,           -96.0f },      // left
            {           135.0f,           672.0f },      // bottom
            {           945.0f,            96.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 1219,
            .offset_x = 33.0f,
            .offset_y = 67.0f,
        },
        {
            .vid = 1345,
            .offset_x = 77.0f,
            .offset_y = 113.0f,
            .pos_z = 2.0f,
            .direction = 64,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Garage imported from _prepare/AS2/decoration/07garage/garage09.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile GARAGE09_DECORATION_PROFILE = {
    .id = "garage09",
    .frame = {
        .corner_offsets = {{
            {            90.0f,          -384.0f },      // top
            {          -540.0f,            64.0f },      // left
            {           -90.0f,           384.0f },      // bottom
            {           540.0f,           -64.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2316,
            .offset_x = 182.5f,
            .offset_y = 76.25f,
            .direction = 128,
        },
        {
            .vid = 1243,
            .offset_x = 234.5f,
            .offset_y = 40.25f,
        },
        {
            .vid = 2314,
            .offset_x = 24.5f,
            .offset_y = 46.25f,
            .direction = 16,
        },
        {
            .vid = 2313,
            .offset_x = 48.5f,
            .offset_y = 32.25f,
            .direction = 192,
        },
        {
            .vid = 2357,
            .offset_x = -18.5f,
            .offset_y = 197.25f,
            .direction = 19,
        },
        {
            .vid = 2316,
            .offset_x = -147.5f,
            .offset_y = 92.25f,
            .direction = 160,
        },
        {
            .vid = 1243,
            .offset_x = 131.5f,
            .offset_y = -43.75f,
            .pos_z = 28.0f,
            .direction = 170,
        },
        {
            .vid = 1243,
            .offset_x = 32.5f,
            .offset_y = -104.75f,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

} // namespace auto_mapper::core::decoration_builder
