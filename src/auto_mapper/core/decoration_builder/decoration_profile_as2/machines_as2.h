/**
 * @file machines_as2.h
 * @brief Authored AS2 machine room decoration stamp profiles.
 */

#pragma once

#include "auto_mapper/core/decoration_builder/decoration_profile_types.h"
#include "auto_mapper/core/wall_builder/wall_profiles_as2.h"

namespace auto_mapper::core::decoration_builder {

/**
 * Machine room imported from _prepare/AS2/decoration/04machines/machines_01.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile MACHINES_01_DECORATION_PROFILE = {
    .id = "machines_01",
    .frame = {
        .corner_offsets = {{
            {             0.0f,          -192.0f },      // top
            {          -270.0f,             0.0f },      // left
            {             0.0f,           192.0f },      // bottom
            {           270.0f,             0.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2515,
            .offset_x = 151.0f,
            .offset_y = 75.0f,
            .direction = 128,
        },
        {
            .vid = 1229,
            .offset_x = 43.0f,
            .offset_y = 7.0f,
            .direction = 133,
            .gamma = {
                .r = -40,
                .g = -40,
                .b = 0,
                .a = 0,
            },
        },
        {
            .vid = 2515,
            .offset_x = 179.0f,
            .offset_y = -76.0f,
        },
        {
            .vid = 2515,
            .offset_x = -60.0f,
            .offset_y = -60.0f,
            .direction = 128,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Machine room imported from _prepare/AS2/decoration/04machines/machines_02.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile MACHINES_02_DECORATION_PROFILE = {
    .id = "machines_02",
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
            .vid = 2515,
            .offset_x = 101.512939f,
            .offset_y = 41.2273f,
            .direction = 64,
        },
        {
            .vid = 2515,
            .offset_x = 9.512939f,
            .offset_y = 106.2273f,
            .direction = 64,
        },
        {
            .vid = 2520,
            .offset_x = -16.487061f,
            .offset_y = 60.2273f,
            .direction = 51,
        },
        {
            .vid = 2515,
            .offset_x = -69.487061f,
            .offset_y = 103.2273f,
        },
        {
            .vid = 2515,
            .offset_x = 194.512939f,
            .offset_y = -25.7727f,
            .direction = 64,
        },
        {
            .vid = 2520,
            .offset_x = 148.512939f,
            .offset_y = -43.7727f,
            .direction = 64,
        },
        {
            .vid = 2520,
            .offset_x = 64.512939f,
            .offset_y = 11.2273f,
            .direction = 64,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Machine room imported from _prepare/AS2/decoration/04machines/machines_03.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile MACHINES_03_DECORATION_PROFILE = {
    .id = "machines_03",
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
            .vid = 2515,
            .offset_x = 101.512939f,
            .offset_y = 41.2273f,
            .direction = 64,
        },
        {
            .vid = 2515,
            .offset_x = 9.512939f,
            .offset_y = 106.2273f,
            .direction = 64,
        },
        {
            .vid = 2515,
            .offset_x = -69.487061f,
            .offset_y = 103.2273f,
        },
        {
            .vid = 2520,
            .offset_x = -6.326172f,
            .offset_y = 67.584722f,
            .direction = 35,
        },
        {
            .vid = 2515,
            .offset_x = -60.326172f,
            .offset_y = 107.584722f,
            .direction = 192,
        },
        {
            .vid = 2515,
            .offset_x = 194.512939f,
            .offset_y = -25.7727f,
            .direction = 64,
        },
        {
            .vid = 2515,
            .offset_x = 119.673828f,
            .offset_y = -89.415278f,
            .direction = 128,
        },
        {
            .vid = 2520,
            .offset_x = 150.673828f,
            .offset_y = -39.415278f,
            .direction = 35,
        },
        {
            .vid = 2520,
            .offset_x = 80.673828f,
            .offset_y = 10.584722f,
            .direction = 21,
        },
        {
            .vid = 2520,
            .offset_x = 81.673828f,
            .offset_y = 8.584722f,
            .direction = 35,
        },
        {
            .vid = 2515,
            .offset_x = 35.673828f,
            .offset_y = -32.415278f,
            .direction = 128,
        },
        {
            .vid = 2515,
            .offset_x = -67.326172f,
            .offset_y = 39.584722f,
            .direction = 128,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Machine room imported from _prepare/AS2/decoration/04machines/machines_04.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile MACHINES_04_DECORATION_PROFILE = {
    .id = "machines_04",
    .frame = {
        .corner_offsets = {{
            {           270.0f,          -320.0f },      // top
            {          -450.0f,           192.0f },      // left
            {          -270.0f,           320.0f },      // bottom
            {           450.0f,          -192.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2357,
            .offset_x = 14.800049f,
            .offset_y = 12.75f,
        },
        {
            .vid = 2515,
            .offset_x = -0.199951f,
            .offset_y = 31.75f,
        },
        {
            .vid = 2515,
            .offset_x = -96.199951f,
            .offset_y = 102.75f,
        },
        {
            .vid = 2515,
            .offset_x = -3.199951f,
            .offset_y = 99.75f,
            .direction = 128,
        },
        {
            .vid = 2520,
            .offset_x = -38.199951f,
            .offset_y = 73.75f,
            .direction = 19,
        },
        {
            .vid = 1285,
            .offset_x = -193.199951f,
            .offset_y = 185.75f,
            .direction = 128,
        },
        {
            .vid = 2357,
            .offset_x = -254.199951f,
            .offset_y = 198.75f,
            .direction = 12,
        },
        {
            .vid = 2515,
            .offset_x = -168.199951f,
            .offset_y = 211.75f,
            .direction = 128,
        },
        {
            .vid = 2520,
            .offset_x = 258.800049f,
            .offset_y = -133.25f,
            .direction = 19,
        },
        {
            .vid = 2515,
            .offset_x = 293.800049f,
            .offset_y = -110.583984f,
            .direction = 128,
        },
        {
            .vid = 2515,
            .offset_x = 200.800049f,
            .offset_y = -107.583984f,
        },
        {
            .vid = 2357,
            .offset_x = 296.800049f,
            .offset_y = -209.25f,
            .pos_z = 32.0f,
            .direction = 64,
        },
        {
            .vid = 2357,
            .offset_x = 311.800049f,
            .offset_y = -203.25f,
            .direction = 19,
        },
        {
            .vid = 2515,
            .offset_x = 296.800049f,
            .offset_y = -178.583984f,
        },
        {
            .vid = 1285,
            .offset_x = 117.800049f,
            .offset_y = -29.25f,
            .direction = 128,
        },
        {
            .vid = 2515,
            .offset_x = 139.800049f,
            .offset_y = -2.25f,
            .direction = 128,
        },
        {
            .vid = 2357,
            .offset_x = 104.800049f,
            .offset_y = -64.25f,
            .direction = 6,
        },
        {
            .vid = 2357,
            .offset_x = 43.800049f,
            .offset_y = -20.25f,
            .pos_z = 5.0f,
            .direction = 70,
        },
        {
            .vid = 1286,
            .offset_x = -193.199951f,
            .offset_y = 187.75f,
            .direction = 128,
        },
        {
            .vid = 1286,
            .offset_x = 116.800049f,
            .offset_y = -26.25f,
            .direction = 128,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Machine room imported from _prepare/AS2/decoration/04machines/machines_05.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile MACHINES_05_DECORATION_PROFILE = {
    .id = "machines_05",
    .frame = {
        .corner_offsets = {{
            {           270.0f,          -320.0f },      // top
            {          -450.0f,           192.0f },      // left
            {          -270.0f,           320.0f },      // bottom
            {           450.0f,          -192.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2357,
            .offset_x = 75.388916f,
            .offset_y = 11.888672f,
            .direction = 19,
        },
        {
            .vid = 2515,
            .offset_x = -66.611084f,
            .offset_y = 34.888672f,
            .direction = 128,
        },
        {
            .vid = 2515,
            .offset_x = -65.611084f,
            .offset_y = 101.888672f,
        },
        {
            .vid = 2515,
            .offset_x = 32.388916f,
            .offset_y = 35.888672f,
        },
        {
            .vid = 2520,
            .offset_x = -26.611084f,
            .offset_y = 61.888672f,
            .direction = 253,
        },
        {
            .vid = 2357,
            .offset_x = -140.611084f,
            .offset_y = 156.888672f,
            .direction = 6,
        },
        {
            .vid = 2357,
            .offset_x = -254.199951f,
            .offset_y = 198.75f,
            .direction = 12,
        },
        {
            .vid = 1285,
            .offset_x = -216.611084f,
            .offset_y = 164.888672f,
            .direction = 128,
        },
        {
            .vid = 2357,
            .offset_x = -179.611084f,
            .offset_y = 216.888672f,
            .direction = 12,
        },
        {
            .vid = 2515,
            .offset_x = -231.611084f,
            .offset_y = 144.888672f,
            .direction = 128,
        },
        {
            .vid = 2357,
            .offset_x = 191.388916f,
            .offset_y = -81.111328f,
            .direction = 12,
        },
        {
            .vid = 2515,
            .offset_x = 236.05542f,
            .offset_y = -111.779297f,
        },
        {
            .vid = 2520,
            .offset_x = 271.388916f,
            .offset_y = -147.111328f,
            .direction = 253,
        },
        {
            .vid = 2515,
            .offset_x = 334.05542f,
            .offset_y = -177.779297f,
        },
        {
            .vid = 2515,
            .offset_x = 235.05542f,
            .offset_y = -178.779297f,
            .direction = 128,
        },
        {
            .vid = 1285,
            .offset_x = 112.388916f,
            .offset_y = -53.111328f,
            .direction = 128,
        },
        {
            .vid = 2515,
            .offset_x = 94.388916f,
            .offset_y = -79.111328f,
            .direction = 128,
        },
        {
            .vid = 1286,
            .offset_x = -218.611084f,
            .offset_y = 167.888672f,
            .direction = 128,
        },
        {
            .vid = 1286,
            .offset_x = 112.388916f,
            .offset_y = -53.111328f,
            .direction = 128,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Machine room imported from _prepare/AS2/decoration/04machines/machines_06.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile MACHINES_06_DECORATION_PROFILE = {
    .id = "machines_06",
    .frame = {
        .corner_offsets = {{
            {           -90.0f,          -192.0f },      // top
            {          -270.0f,           -64.0f },      // left
            {            90.0f,           192.0f },      // bottom
            {           270.0f,            64.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2520,
            .offset_x = 90.298828f,
            .offset_y = 40.300049f,
            .direction = 34,
        },
        {
            .vid = 2515,
            .offset_x = 103.298828f,
            .offset_y = 138.300049f,
        },
        {
            .vid = 2515,
            .offset_x = 197.298828f,
            .offset_y = 139.300049f,
            .direction = 128,
        },
        {
            .vid = 2515,
            .offset_x = -107.701172f,
            .offset_y = -6.699951f,
        },
        {
            .vid = 2515,
            .offset_x = -107.701172f,
            .offset_y = -71.699951f,
            .direction = 128,
        },
        {
            .vid = 2520,
            .offset_x = 11.298828f,
            .offset_y = -14.699951f,
            .direction = 34,
        },
        {
            .vid = 2360,
            .offset_x = 121.298828f,
            .offset_y = 119.300049f,
            .direction = 64,
        },
        {
            .vid = 2360,
            .offset_x = -90.701172f,
            .offset_y = -32.699951f,
            .direction = 64,
        },
        {
            .vid = 2362,
            .offset_x = 120.298828f,
            .offset_y = 120.300049f,
            .direction = 64,
        },
        {
            .vid = 2362,
            .offset_x = -92.701172f,
            .offset_y = -31.699951f,
            .direction = 64,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Machine room imported from _prepare/AS2/decoration/04machines/machines_07.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile MACHINES_07_DECORATION_PROFILE = {
    .id = "machines_07",
    .frame = {
        .corner_offsets = {{
            {           -90.0f,          -192.0f },      // top
            {          -270.0f,           -64.0f },      // left
            {            90.0f,           192.0f },      // bottom
            {           270.0f,            64.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2515,
            .offset_x = 197.298828f,
            .offset_y = 139.300049f,
            .direction = 128,
        },
        {
            .vid = 2515,
            .offset_x = 191.216797f,
            .offset_y = 132.018494f,
            .direction = 128,
        },
        {
            .vid = 2515,
            .offset_x = 192.134766f,
            .offset_y = 67.268494f,
        },
        {
            .vid = 2520,
            .offset_x = 49.216797f,
            .offset_y = 57.018494f,
            .direction = 34,
        },
        {
            .vid = 2515,
            .offset_x = -113.783203f,
            .offset_y = -78.981079f,
            .direction = 128,
        },
        {
            .vid = 2515,
            .offset_x = -23.865234f,
            .offset_y = -79.730591f,
        },
        {
            .vid = 2520,
            .offset_x = -29.783203f,
            .offset_y = 2.018433f,
            .direction = 34,
        },
        {
            .vid = 2360,
            .offset_x = 166.216797f,
            .offset_y = 86.018494f,
            .direction = 192,
        },
        {
            .vid = 2360,
            .offset_x = -24.783203f,
            .offset_y = -59.981567f,
            .direction = 192,
        },
        {
            .vid = 2362,
            .offset_x = 165.216797f,
            .offset_y = 87.018494f,
            .direction = 192,
        },
        {
            .vid = 2362,
            .offset_x = -26.783203f,
            .offset_y = -58.981567f,
            .direction = 192,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Machine room imported from _prepare/AS2/decoration/04machines/machines_08.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile MACHINES_08_DECORATION_PROFILE = {
    .id = "machines_08",
    .frame = {
        .corner_offsets = {{
            {          -225.0f,          -416.0f },      // top
            {          -585.0f,          -160.0f },      // left
            {           225.0f,           416.0f },      // bottom
            {           585.0f,           160.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2515,
            .offset_x = 368.749756f,
            .offset_y = 60.214111f,
        },
        {
            .vid = 2515,
            .offset_x = 389.749756f,
            .offset_y = 256.214111f,
            .direction = 128,
        },
        {
            .vid = 2400,
            .offset_x = 243.749756f,
            .offset_y = 171.214111f,
            .direction = 128,
        },
        {
            .vid = 2515,
            .offset_x = 122.749756f,
            .offset_y = 253.214111f,
        },
        {
            .vid = 2515,
            .offset_x = -89.250244f,
            .offset_y = 110.214111f,
        },
        {
            .vid = 2515,
            .offset_x = 148.749756f,
            .offset_y = -89.785889f,
        },
        {
            .vid = 2400,
            .offset_x = 33.749756f,
            .offset_y = 23.214111f,
            .direction = 128,
        },
        {
            .vid = 2515,
            .offset_x = -295.250244f,
            .offset_y = -215.785889f,
            .direction = 128,
        },
        {
            .vid = 2400,
            .offset_x = -165.250244f,
            .offset_y = -116.785889f,
            .direction = 128,
        },
        {
            .vid = 2515,
            .offset_x = -293.250244f,
            .offset_y = -27.785889f,
        },
        {
            .vid = 2515,
            .offset_x = -45.250244f,
            .offset_y = -223.785889f,
        },
        {
            .vid = 1266,
            .offset_x = 382.749756f,
            .offset_y = 112.214111f,
            .direction = 238,
        },
        {
            .vid = 1266,
            .offset_x = 167.749756f,
            .offset_y = -47.785889f,
            .direction = 238,
        },
        {
            .vid = 1266,
            .offset_x = -28.250244f,
            .offset_y = -180.785889f,
            .direction = 238,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Machine room imported from _prepare/AS2/decoration/04machines/machines_09.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile MACHINES_09_DECORATION_PROFILE = {
    .id = "machines_09",
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
            .vid = 443,
            .offset_x = 15.850098f,
            .offset_y = 270.45752f,
            .direction = 128,
        },
        {
            .vid = 443,
            .offset_x = 372.910645f,
            .offset_y = 20.45752f,
            .direction = 128,
        },
        {
            .vid = 443,
            .offset_x = 198.850098f,
            .offset_y = 140.45752f,
            .direction = 128,
        },
        {
            .vid = 443,
            .offset_x = -298.89209f,
            .offset_y = 48.875977f,
            .direction = 128,
        },
        {
            .vid = 443,
            .offset_x = 58.168457f,
            .offset_y = -201.124023f,
            .direction = 128,
        },
        {
            .vid = 443,
            .offset_x = -115.89209f,
            .offset_y = -81.124023f,
            .direction = 128,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

} // namespace auto_mapper::core::decoration_builder
