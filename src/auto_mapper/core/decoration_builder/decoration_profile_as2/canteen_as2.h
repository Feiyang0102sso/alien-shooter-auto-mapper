/**
 * @file canteen_as2.h
 * @brief Authored AS2 canteen decoration stamp profiles.
 */

#pragma once

#include "auto_mapper/core/decoration_builder/decoration_profile_types.h"
#include "auto_mapper/core/wall_builder/wall_profiles_as2.h"

namespace auto_mapper::core::decoration_builder {

/**
 * Canteen imported from _prepare/AS2/decoration/10canteen/canteen01.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile CANTEEN01_DECORATION_PROFILE = {
    .id = "canteen01",
    .frame = {
        .corner_offsets = {{
            {            90.0f,          -192.0f },      // top
            {          -270.0f,            64.0f },      // left
            {           -90.0f,           192.0f },      // bottom
            {           270.0f,           -64.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2303,
            .offset_x = -17.875397f,
            .offset_y = 85.100586f,
            .direction = 119,
        },
        {
            .vid = 2302,
            .offset_x = -89.875397f,
            .offset_y = 71.100586f,
            .direction = 25,
        },
        {
            .vid = 2302,
            .offset_x = -121.875397f,
            .offset_y = 96.100586f,
            .direction = 25,
        },
        {
            .vid = 2303,
            .offset_x = 158.124603f,
            .offset_y = -66.899414f,
            .pos_z = 28.0f,
            .direction = 145,
        },
        {
            .vid = 2303,
            .offset_x = 151.124603f,
            .offset_y = -72.899414f,
            .pos_z = 28.0f,
            .direction = 136,
        },
        {
            .vid = 2303,
            .offset_x = 173.124603f,
            .offset_y = -13.899414f,
            .direction = 93,
        },
        {
            .vid = 2303,
            .offset_x = 177.124603f,
            .offset_y = -52.899414f,
            .pos_z = 27.0f,
            .direction = 68,
        },
        {
            .vid = 2303,
            .offset_x = 164.124603f,
            .offset_y = -74.899414f,
            .pos_z = 30.0f,
            .direction = 59,
        },
        {
            .vid = 2303,
            .offset_x = 214.124603f,
            .offset_y = -38.899414f,
            .pos_z = 30.0f,
            .direction = 42,
        },
        {
            .vid = 2303,
            .offset_x = 149.124603f,
            .offset_y = -81.899414f,
            .pos_z = 32.0f,
            .direction = 17,
        },
        {
            .vid = 2303,
            .offset_x = 139.124603f,
            .offset_y = -94.899414f,
            .pos_z = 32.0f,
            .direction = 8,
        },
        {
            .vid = 2302,
            .offset_x = 144.124603f,
            .offset_y = -81.899414f,
            .direction = 166,
        },
        {
            .vid = 2302,
            .offset_x = 191.124603f,
            .offset_y = -50.899414f,
            .pos_z = 10.0f,
            .direction = 140,
        },
        {
            .vid = 2302,
            .offset_x = 221.124603f,
            .offset_y = -21.899414f,
            .pos_z = 10.0f,
            .direction = 115,
        },
        {
            .vid = 2302,
            .offset_x = 183.124603f,
            .offset_y = -51.899414f,
            .direction = 89,
        },
        {
            .vid = 2302,
            .offset_x = 216.124603f,
            .offset_y = -28.899414f,
            .direction = 64,
        },
        {
            .vid = 2303,
            .offset_x = -58.875397f,
            .offset_y = 45.100586f,
            .pos_z = 38.0f,
            .direction = 17,
        },
        {
            .vid = 2303,
            .offset_x = 18.124603f,
            .offset_y = -10.899414f,
            .pos_z = 38.0f,
            .direction = 179,
        },
        {
            .vid = 2303,
            .offset_x = -60.875397f,
            .offset_y = 53.100586f,
            .pos_z = 28.0f,
            .direction = 170,
        },
        {
            .vid = 2303,
            .offset_x = 4.124603f,
            .offset_y = -4.899414f,
            .pos_z = 28.0f,
            .direction = 162,
        },
        {
            .vid = 2303,
            .offset_x = 25.124603f,
            .offset_y = -34.899414f,
            .pos_z = 28.0f,
            .direction = 136,
        },
        {
            .vid = 2303,
            .offset_x = 35.124603f,
            .offset_y = -19.899414f,
            .pos_z = 32.0f,
            .direction = 76,
        },
        {
            .vid = 2303,
            .offset_x = -16.875397f,
            .offset_y = 3.100586f,
            .pos_z = 32.0f,
            .direction = 68,
        },
        {
            .vid = 2303,
            .offset_x = 38.124603f,
            .offset_y = -29.899414f,
            .pos_z = 32.0f,
            .direction = 25,
        },
        {
            .vid = 2302,
            .offset_x = -22.875397f,
            .offset_y = 14.100586f,
            .direction = 76,
        },
        {
            .vid = 2302,
            .offset_x = 12.124603f,
            .offset_y = -10.899414f,
            .direction = 76,
        },
        {
            .vid = 2302,
            .offset_x = 39.124603f,
            .offset_y = -35.899414f,
            .direction = 128,
        },
        {
            .vid = 2302,
            .offset_x = 48.124603f,
            .offset_y = -36.899414f,
            .direction = 153,
        },
        {
            .vid = 2303,
            .offset_x = 72.124603f,
            .offset_y = -43.899414f,
            .pos_z = 30.0f,
            .direction = 51,
        },
        {
            .vid = 2303,
            .offset_x = 114.124603f,
            .offset_y = -3.899414f,
            .direction = 119,
        },
        {
            .vid = 2303,
            .offset_x = 62.124603f,
            .offset_y = 14.100586f,
            .direction = 110,
        },
        {
            .vid = 2303,
            .offset_x = 131.124603f,
            .offset_y = -39.899414f,
            .direction = 85,
        },
        {
            .vid = 2303,
            .offset_x = 61.124603f,
            .offset_y = -35.899414f,
            .pos_z = 32.0f,
            .direction = 85,
        },
        {
            .vid = 2303,
            .offset_x = 134.124603f,
            .offset_y = -82.899414f,
            .pos_z = 32.0f,
            .direction = 68,
        },
        {
            .vid = 2303,
            .offset_x = 127.124603f,
            .offset_y = -81.899414f,
            .pos_z = 30.0f,
            .direction = 68,
        },
        {
            .vid = 2303,
            .offset_x = 55.124603f,
            .offset_y = -42.899414f,
            .pos_z = 32.0f,
            .direction = 34,
        },
        {
            .vid = 2302,
            .offset_x = -57.875397f,
            .offset_y = 49.100586f,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Canteen imported from _prepare/AS2/decoration/10canteen/canteen02.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile CANTEEN02_DECORATION_PROFILE = {
    .id = "canteen02",
    .frame = {
        .corner_offsets = {{
            {           -45.0f,          -544.0f },      // top
            {          -765.0f,           -32.0f },      // left
            {            45.0f,           544.0f },      // bottom
            {           765.0f,            32.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2303,
            .offset_x = 195.0f,
            .offset_y = 294.0f,
            .pos_z = 40.0f,
            .direction = 117,
        },
        {
            .vid = 2303,
            .offset_x = 180.0f,
            .offset_y = 313.0f,
            .pos_z = 40.0f,
            .direction = 46,
        },
        {
            .vid = 2303,
            .offset_x = 145.0f,
            .offset_y = 310.0f,
            .pos_z = 40.0f,
        },
        {
            .vid = 1764,
            .offset_x = 158.0f,
            .offset_y = 308.0f,
            .direction = 166,
        },
        {
            .vid = 1764,
            .offset_x = -92.0f,
            .offset_y = 102.0f,
            .direction = 179,
        },
        {
            .vid = 2303,
            .offset_x = -47.0f,
            .offset_y = 130.0f,
            .direction = 172,
        },
        {
            .vid = 2303,
            .offset_x = -45.0f,
            .offset_y = 113.0f,
            .direction = 202,
        },
        {
            .vid = 2303,
            .offset_x = -79.0f,
            .offset_y = 134.0f,
            .direction = 120,
        },
        {
            .vid = 2303,
            .offset_x = 134.0f,
            .offset_y = 335.0f,
            .pos_z = 40.0f,
            .direction = 19,
        },
        {
            .vid = 2303,
            .offset_x = 496.0f,
            .offset_y = 13.0f,
            .direction = 25,
        },
        {
            .vid = 2303,
            .offset_x = 515.0f,
            .offset_y = -2.0f,
            .direction = 34,
        },
        {
            .vid = 1764,
            .offset_x = 468.0f,
            .offset_y = -12.0f,
            .direction = 179,
        },
        {
            .vid = 2303,
            .offset_x = 280.0f,
            .offset_y = -182.0f,
            .direction = 145,
        },
        {
            .vid = 1764,
            .offset_x = 252.0f,
            .offset_y = -144.0f,
            .direction = 198,
        },
        {
            .vid = 1764,
            .offset_x = -271.0f,
            .offset_y = -141.0f,
            .direction = 192,
        },
        {
            .vid = 2303,
            .offset_x = 264.0f,
            .offset_y = -206.0f,
            .direction = 131,
        },
        {
            .vid = 2303,
            .offset_x = 313.0f,
            .offset_y = -198.0f,
            .direction = 115,
        },
        {
            .vid = 1764,
            .offset_x = 8.0f,
            .offset_y = -356.0f,
            .direction = 185,
        },
        {
            .vid = 1289,
            .offset_x = 170.0f,
            .offset_y = 361.0f,
            .direction = 112,
        },
        {
            .vid = 1289,
            .offset_x = 98.0f,
            .offset_y = 286.0f,
            .direction = 120,
        },
        {
            .vid = 1289,
            .offset_x = -127.0f,
            .offset_y = 97.0f,
            .direction = 88,
        },
        {
            .vid = 1289,
            .offset_x = 548.0f,
            .offset_y = 6.0f,
            .direction = 112,
        },
        {
            .vid = 1289,
            .offset_x = 260.0f,
            .offset_y = -127.0f,
            .direction = 104,
        },
        {
            .vid = 1289,
            .offset_x = 192.0f,
            .offset_y = -116.0f,
            .direction = 96,
        },
        {
            .vid = 1289,
            .offset_x = -29.0f,
            .offset_y = 28.0f,
            .direction = 96,
        },
        {
            .vid = 1289,
            .offset_x = -364.0f,
            .offset_y = -153.0f,
            .direction = 80,
        },
        {
            .vid = 1289,
            .offset_x = -298.0f,
            .offset_y = -234.0f,
            .direction = 88,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Canteen imported from _prepare/AS2/decoration/10canteen/canteen03.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile CANTEEN03_DECORATION_PROFILE = {
    .id = "canteen03",
    .frame = {
        .corner_offsets = {{
            {           360.0f,          -960.0f },      // top
            {         -1350.0f,           256.0f },      // left
            {          -360.0f,           960.0f },      // bottom
            {          1350.0f,          -256.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 1764,
            .offset_x = 96.083008f,
            .offset_y = 253.520752f,
            .direction = 166,
        },
        {
            .vid = 1764,
            .offset_x = -144.916992f,
            .offset_y = 375.520752f,
            .direction = 166,
        },
        {
            .vid = 2303,
            .offset_x = -96.916992f,
            .offset_y = 192.520752f,
            .direction = 153,
        },
        {
            .vid = 2303,
            .offset_x = -171.916992f,
            .offset_y = 179.520752f,
            .direction = 119,
        },
        {
            .vid = 2303,
            .offset_x = -347.916992f,
            .offset_y = 244.520752f,
            .pos_z = 36.0f,
            .direction = 145,
        },
        {
            .vid = 2303,
            .offset_x = -366.916992f,
            .offset_y = 249.520752f,
            .pos_z = 36.0f,
            .direction = 136,
        },
        {
            .vid = 1764,
            .offset_x = -347.916992f,
            .offset_y = 230.520752f,
            .direction = 166,
        },
        {
            .vid = 2303,
            .offset_x = -184.916992f,
            .offset_y = 133.520752f,
            .direction = 162,
        },
        {
            .vid = 1764,
            .offset_x = 157.083008f,
            .offset_y = -105.479248f,
            .direction = 166,
        },
        {
            .vid = 1764,
            .offset_x = 335.083008f,
            .offset_y = 38.520752f,
            .direction = 166,
        },
        {
            .vid = 1764,
            .offset_x = -106.916992f,
            .offset_y = 127.520752f,
            .direction = 185,
        },
        {
            .vid = 2303,
            .offset_x = -318.916992f,
            .offset_y = -29.479248f,
            .pos_z = 40.0f,
            .direction = 179,
        },
        {
            .vid = 2303,
            .offset_x = -286.916992f,
            .offset_y = -50.479248f,
            .pos_z = 40.0f,
            .direction = 110,
        },
        {
            .vid = 2303,
            .offset_x = -311.916992f,
            .offset_y = -63.479248f,
            .pos_z = 40.0f,
            .direction = 93,
        },
        {
            .vid = 2303,
            .offset_x = -330.916992f,
            .offset_y = -49.479248f,
            .pos_z = 40.0f,
            .direction = 68,
        },
        {
            .vid = 2303,
            .offset_x = -261.916992f,
            .offset_y = -62.479248f,
            .pos_z = 40.0f,
            .direction = 68,
        },
        {
            .vid = 2303,
            .offset_x = -267.916992f,
            .offset_y = -74.479248f,
            .pos_z = 38.0f,
            .direction = 34,
        },
        {
            .vid = 1764,
            .offset_x = -286.916992f,
            .offset_y = -66.479248f,
            .direction = 166,
        },
        {
            .vid = 2303,
            .offset_x = -241.916992f,
            .offset_y = -78.479248f,
            .pos_z = 40.0f,
            .direction = 93,
        },
        {
            .vid = 1220,
            .offset_x = 615.083008f,
            .offset_y = -133.479248f,
            .direction = 23,
        },
        {
            .vid = 1764,
            .offset_x = 345.083008f,
            .offset_y = -254.479248f,
            .direction = 166,
        },
        {
            .vid = 1220,
            .offset_x = 536.083008f,
            .offset_y = -198.479248f,
            .direction = 23,
        },
        {
            .vid = 1764,
            .offset_x = 537.083008f,
            .offset_y = -137.479248f,
            .direction = 179,
        },
        {
            .vid = 2303,
            .offset_x = 181.083008f,
            .offset_y = -111.479248f,
            .pos_z = 40.0f,
            .direction = 179,
        },
        {
            .vid = 2303,
            .offset_x = 204.083008f,
            .offset_y = -125.479248f,
            .pos_z = 40.0f,
            .direction = 162,
        },
        {
            .vid = 1764,
            .offset_x = -48.916992f,
            .offset_y = -273.479248f,
            .direction = 179,
        },
        {
            .vid = 2303,
            .offset_x = -246.916992f,
            .offset_y = -91.479248f,
            .pos_z = 40.0f,
            .direction = 187,
        },
        {
            .vid = 2303,
            .offset_x = -260.916992f,
            .offset_y = -98.479248f,
            .pos_z = 40.0f,
            .direction = 119,
        },
        {
            .vid = 1764,
            .offset_x = 160.083008f,
            .offset_y = -385.479248f,
            .direction = 166,
        },
        {
            .vid = 1289,
            .offset_x = 123.083008f,
            .offset_y = 288.520752f,
            .direction = 80,
        },
        {
            .vid = 1289,
            .offset_x = -48.916992f,
            .offset_y = 180.520752f,
            .direction = 80,
        },
        {
            .vid = 1289,
            .offset_x = -333.916992f,
            .offset_y = 271.520752f,
            .direction = 88,
        },
        {
            .vid = 1289,
            .offset_x = 590.083008f,
            .offset_y = -91.479248f,
            .direction = 88,
        },
        {
            .vid = 1289,
            .offset_x = 545.083008f,
            .offset_y = -80.479248f,
            .direction = 72,
        },
        {
            .vid = 1289,
            .offset_x = 165.083008f,
            .offset_y = -67.479248f,
            .direction = 72,
        },
        {
            .vid = 1289,
            .offset_x = 201.083008f,
            .offset_y = -104.479248f,
            .direction = 80,
        },
        {
            .vid = 1289,
            .offset_x = 334.083008f,
            .offset_y = 81.520752f,
            .direction = 72,
        },
        {
            .vid = 1289,
            .offset_x = -337.916992f,
            .offset_y = -79.479248f,
            .direction = 112,
        },
        {
            .vid = 1289,
            .offset_x = -237.916992f,
            .offset_y = -54.479248f,
            .direction = 96,
        },
        {
            .vid = 1289,
            .offset_x = 390.083008f,
            .offset_y = -238.479248f,
            .direction = 96,
        },
        {
            .vid = 1289,
            .offset_x = 523.083008f,
            .offset_y = -149.479248f,
            .direction = 80,
        },
        {
            .vid = 1289,
            .offset_x = 428.083008f,
            .offset_y = -268.479248f,
            .direction = 72,
        },
        {
            .vid = 1289,
            .offset_x = 180.083008f,
            .offset_y = -355.479248f,
            .direction = 104,
        },
        {
            .vid = 1289,
            .offset_x = -3.916992f,
            .offset_y = -331.479248f,
            .direction = 120,
        },
        {
            .vid = 1289,
            .offset_x = 229.083008f,
            .offset_y = -386.479248f,
            .direction = 120,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Canteen imported from _prepare/AS2/decoration/10canteen/canteen04.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile CANTEEN04_DECORATION_PROFILE = {
    .id = "canteen04",
    .frame = {
        .corner_offsets = {{
            {           180.0f,          -704.0f },      // top
            {          -990.0f,           128.0f },      // left
            {          -180.0f,           704.0f },      // bottom
            {           990.0f,          -128.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2313,
            .offset_x = 135.0f,
            .offset_y = 154.0f,
        },
        {
            .vid = 2304,
            .offset_x = 129.0f,
            .offset_y = 182.0f,
            .direction = 34,
        },
        {
            .vid = 2300,
            .offset_x = -218.0f,
            .offset_y = 93.0f,
            .direction = 166,
        },
        {
            .vid = 2304,
            .offset_x = 593.0f,
            .offset_y = 24.0f,
            .direction = 25,
        },
        {
            .vid = 2302,
            .offset_x = 373.0f,
            .offset_y = -154.0f,
            .direction = 166,
        },
        {
            .vid = 2300,
            .offset_x = -88.0f,
            .offset_y = -6.0f,
            .direction = 166,
        },
        {
            .vid = 2300,
            .offset_x = -365.0f,
            .offset_y = -12.0f,
            .direction = 76,
        },
        {
            .vid = 2300,
            .offset_x = -244.0f,
            .offset_y = -106.0f,
            .direction = 166,
        },
        {
            .vid = 1763,
            .offset_x = -498.0f,
            .offset_y = -68.0f,
            .direction = 166,
        },
        {
            .vid = 2302,
            .offset_x = 164.0f,
            .offset_y = -210.0f,
            .direction = 153,
        },
        {
            .vid = 2302,
            .offset_x = 197.0f,
            .offset_y = -233.0f,
            .direction = 153,
        },
        {
            .vid = 2302,
            .offset_x = 232.0f,
            .offset_y = -251.0f,
            .direction = 166,
        },
        {
            .vid = 2302,
            .offset_x = 264.0f,
            .offset_y = -228.0f,
            .direction = 64,
        },
        {
            .vid = 2302,
            .offset_x = 300.0f,
            .offset_y = -206.0f,
            .direction = 166,
        },
        {
            .vid = 2302,
            .offset_x = 338.0f,
            .offset_y = -177.0f,
            .direction = 89,
        },
        {
            .vid = 1764,
            .offset_x = 12.0f,
            .offset_y = -388.0f,
            .direction = 70,
        },
        {
            .vid = 1764,
            .offset_x = 76.0f,
            .offset_y = -282.0f,
            .direction = 166,
        },
        {
            .vid = 2304,
            .offset_x = 19.0f,
            .offset_y = -397.0f,
            .pos_z = 32.0f,
            .direction = 145,
        },
        {
            .vid = 407,
            .offset_x = -8.0f,
            .offset_y = -304.0f,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET8_RANDOM,
};

} // namespace auto_mapper::core::decoration_builder
