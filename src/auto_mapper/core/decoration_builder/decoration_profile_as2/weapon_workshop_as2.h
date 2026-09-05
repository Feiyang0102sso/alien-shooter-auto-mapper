/**
 * @file weapon_workshop_as2.h
 * @brief Authored AS2 weapon workshop decoration stamp profiles.
 */

#pragma once

#include "auto_mapper/core/decoration_builder/decoration_profile_types.h"
#include "auto_mapper/core/wall_builder/wall_profiles_as2.h"

namespace auto_mapper::core::decoration_builder {

/**
 * Weapon workshop imported from _prepare/AS2/decoration/08weapon_workshop/weapon_workshop01.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile WEAPON_WORKSHOP01_DECORATION_PROFILE = {
    .id = "weapon_workshop01",
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
            .vid = 2334,
            .offset_x = -83.0f,
            .offset_y = 35.0f,
            .direction = 64,
        },
        {
            .vid = 2335,
            .offset_x = 21.5f,
            .offset_y = 36.5f,
            .direction = 109,
        },
        {
            .vid = 2335,
            .offset_x = 22.5f,
            .offset_y = 135.5f,
            .direction = 146,
        },
        {
            .vid = 2317,
            .offset_x = 4.0f,
            .offset_y = 82.0f,
            .pos_z = 40.0f,
            .direction = 34,
        },
        {
            .vid = 2517,
            .offset_x = -76.0f,
            .offset_y = 146.0f,
            .direction = 128,
            .gamma = {
                .r = -10,
                .g = -10,
                .b = 20,
                .a = 0,
            },
        },
        {
            .vid = 2317,
            .offset_x = 6.0f,
            .offset_y = 84.0f,
            .direction = 180,
        },
        {
            .vid = 2334,
            .offset_x = 144.0f,
            .offset_y = -123.0f,
            .direction = 64,
        },
        {
            .vid = 2335,
            .offset_x = 240.5f,
            .offset_y = -119.5f,
            .direction = 109,
        },
        {
            .vid = 2335,
            .offset_x = 241.5f,
            .offset_y = -20.5f,
            .direction = 146,
        },
        {
            .vid = 2317,
            .offset_x = 231.0f,
            .offset_y = -74.0f,
            .pos_z = 30.0f,
        },
        {
            .vid = 2517,
            .offset_x = 141.0f,
            .offset_y = -32.0f,
            .direction = 74,
            .gamma = {
                .r = -10,
                .g = -10,
                .b = 20,
                .a = 0,
            },
        },
        {
            .vid = 2317,
            .offset_x = 228.0f,
            .offset_y = -72.0f,
            .direction = 180,
        },
        {
            .vid = 2536,
            .offset_x = 90.0f,
            .offset_y = 154.0f,
            .direction = 68,
        },
        {
            .vid = 2536,
            .offset_x = 320.0f,
            .offset_y = -7.0f,
            .direction = 32,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET6_RANDOM,
};

/**
 * Weapon workshop imported from _prepare/AS2/decoration/08weapon_workshop/weapon_workshop02.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile WEAPON_WORKSHOP02_DECORATION_PROFILE = {
    .id = "weapon_workshop02",
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
            .vid = 2335,
            .offset_x = -90.5f,
            .offset_y = 40.5f,
            .direction = 146,
        },
        {
            .vid = 2335,
            .offset_x = 121.5f,
            .offset_y = -107.5f,
            .direction = 146,
        },
        {
            .vid = 2317,
            .offset_x = 112.0f,
            .offset_y = -157.0f,
            .direction = 180,
        },
        {
            .vid = 2335,
            .offset_x = 120.5f,
            .offset_y = -206.5f,
            .direction = 109,
        },
        {
            .vid = 2317,
            .offset_x = 102.0f,
            .offset_y = -165.0f,
            .pos_z = 30.0f,
            .direction = 139,
        },
        {
            .vid = 2334,
            .offset_x = 215.0f,
            .offset_y = -89.0f,
            .direction = 192,
        },
        {
            .vid = 2517,
            .offset_x = 35.0f,
            .offset_y = -90.0f,
            .direction = 100,
            .gamma = {
                .r = -10,
                .g = -10,
                .b = 20,
                .a = 0,
            },
        },
        {
            .vid = 2317,
            .offset_x = -101.0f,
            .offset_y = -6.0f,
            .direction = 179,
        },
        {
            .vid = 2317,
            .offset_x = -102.0f,
            .offset_y = -5.0f,
            .pos_z = 40.0f,
            .direction = 48,
        },
        {
            .vid = 2517,
            .offset_x = -176.0f,
            .offset_y = 8.0f,
            .direction = 48,
            .gamma = {
                .r = -10,
                .g = -10,
                .b = 20,
                .a = 0,
            },
        },
        {
            .vid = 2335,
            .offset_x = -91.5f,
            .offset_y = -58.5f,
            .direction = 109,
        },
        {
            .vid = 2536,
            .offset_x = -189.0f,
            .offset_y = -60.0f,
            .direction = 128,
        },
        {
            .vid = 2536,
            .offset_x = 31.0f,
            .offset_y = -211.0f,
            .direction = 128,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET6_RANDOM,
};

/**
 * Weapon workshop imported from _prepare/AS2/decoration/08weapon_workshop/weapon_workshop03.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile WEAPON_WORKSHOP03_DECORATION_PROFILE = {
    .id = "weapon_workshop03",
    .frame = {
        .corner_offsets = {{
            {           450.0f,          -576.0f },      // top
            {          -810.0f,           320.0f },      // left
            {          -450.0f,           576.0f },      // bottom
            {           810.0f,          -320.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2335,
            .offset_x = -78.5f,
            .offset_y = 178.5f,
            .direction = 146,
        },
        {
            .vid = 2317,
            .offset_x = -313.0f,
            .offset_y = 270.0f,
            .direction = 180,
        },
        {
            .vid = 2335,
            .offset_x = -296.0f,
            .offset_y = 228.0f,
            .direction = 109,
        },
        {
            .vid = 2334,
            .offset_x = -406.0f,
            .offset_y = 215.0f,
            .direction = 64,
        },
        {
            .vid = 2317,
            .offset_x = -311.0f,
            .offset_y = 272.0f,
            .pos_z = 29.0f,
            .direction = 14,
        },
        {
            .vid = 2516,
            .offset_x = -237.0f,
            .offset_y = 207.0f,
            .direction = 192,
        },
        {
            .vid = 2335,
            .offset_x = -295.0f,
            .offset_y = 327.0f,
            .direction = 146,
        },
        {
            .vid = 2517,
            .offset_x = -413.0f,
            .offset_y = 334.0f,
            .direction = 20,
            .gamma = {
                .r = -10,
                .g = -10,
                .b = 20,
                .a = 0,
            },
        },
        {
            .vid = 2335,
            .offset_x = -481.5f,
            .offset_y = 360.5f,
            .direction = 109,
        },
        {
            .vid = 2334,
            .offset_x = -586.0f,
            .offset_y = 340.0f,
            .direction = 64,
        },
        {
            .vid = 2317,
            .offset_x = -502.0f,
            .offset_y = 417.0f,
            .direction = 180,
        },
        {
            .vid = 2335,
            .offset_x = -480.5f,
            .offset_y = 459.5f,
            .direction = 146,
        },
        {
            .vid = 2317,
            .offset_x = -500.0f,
            .offset_y = 418.0f,
            .pos_z = 40.0f,
            .direction = 36,
        },
        {
            .vid = 2335,
            .offset_x = 107.0f,
            .offset_y = 46.0f,
            .direction = 146,
        },
        {
            .vid = 2317,
            .offset_x = 89.0f,
            .offset_y = -11.0f,
            .direction = 180,
        },
        {
            .vid = 2317,
            .offset_x = 91.0f,
            .offset_y = -9.0f,
            .pos_z = 29.0f,
            .direction = 14,
        },
        {
            .vid = 2335,
            .offset_x = 106.0f,
            .offset_y = -53.0f,
            .direction = 109,
        },
        {
            .vid = 2516,
            .offset_x = 165.0f,
            .offset_y = -74.0f,
            .direction = 192,
        },
        {
            .vid = 2317,
            .offset_x = -98.0f,
            .offset_y = 137.0f,
            .pos_z = 40.0f,
            .direction = 36,
        },
        {
            .vid = 2317,
            .offset_x = -100.0f,
            .offset_y = 136.0f,
            .direction = 180,
        },
        {
            .vid = 2335,
            .offset_x = -79.5f,
            .offset_y = 79.5f,
            .direction = 109,
        },
        {
            .vid = 2334,
            .offset_x = -4.0f,
            .offset_y = -66.0f,
            .direction = 64,
        },
        {
            .vid = 2517,
            .offset_x = -11.0f,
            .offset_y = 53.0f,
            .direction = 20,
            .gamma = {
                .r = -10,
                .g = -10,
                .b = 20,
                .a = 0,
            },
        },
        {
            .vid = 2334,
            .offset_x = -184.0f,
            .offset_y = 59.0f,
            .direction = 64,
        },
        {
            .vid = 2335,
            .offset_x = 331.5f,
            .offset_y = -107.5f,
            .direction = 146,
        },
        {
            .vid = 2317,
            .offset_x = 499.0f,
            .offset_y = -297.0f,
            .direction = 180,
        },
        {
            .vid = 2317,
            .offset_x = 501.0f,
            .offset_y = -295.0f,
            .pos_z = 29.0f,
            .direction = 14,
        },
        {
            .vid = 2517,
            .offset_x = 399.0f,
            .offset_y = -233.0f,
            .direction = 20,
            .gamma = {
                .r = -10,
                .g = -10,
                .b = 20,
                .a = 0,
            },
        },
        {
            .vid = 2335,
            .offset_x = 516.0f,
            .offset_y = -339.0f,
            .direction = 109,
        },
        {
            .vid = 2335,
            .offset_x = 517.0f,
            .offset_y = -240.0f,
            .direction = 146,
        },
        {
            .vid = 2334,
            .offset_x = 226.0f,
            .offset_y = -227.0f,
            .direction = 64,
        },
        {
            .vid = 2317,
            .offset_x = 312.0f,
            .offset_y = -149.0f,
            .pos_z = 40.0f,
            .direction = 36,
        },
        {
            .vid = 2317,
            .offset_x = 310.0f,
            .offset_y = -150.0f,
            .direction = 180,
        },
        {
            .vid = 2335,
            .offset_x = 330.5f,
            .offset_y = -206.5f,
            .direction = 109,
        },
        {
            .vid = 2334,
            .offset_x = 406.0f,
            .offset_y = -352.0f,
            .direction = 64,
        },
        {
            .vid = 2516,
            .offset_x = 575.0f,
            .offset_y = -360.0f,
            .direction = 192,
        },
        {
            .vid = 2536,
            .offset_x = -223.0f,
            .offset_y = 345.0f,
            .direction = 32,
        },
        {
            .vid = 2536,
            .offset_x = -416.0f,
            .offset_y = 486.0f,
            .direction = 96,
        },
        {
            .vid = 2536,
            .offset_x = 396.0f,
            .offset_y = -81.0f,
            .direction = 96,
        },
        {
            .vid = 2536,
            .offset_x = 589.0f,
            .offset_y = -222.0f,
            .direction = 64,
        },
        {
            .vid = 2608,
            .offset_x = 807.0f,
            .offset_y = -288.0f,
            .direction = 51,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET6_RANDOM,
};

/**
 * Weapon workshop imported from _prepare/AS2/decoration/08weapon_workshop/weapon_workshop04.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile WEAPON_WORKSHOP04_DECORATION_PROFILE = {
    .id = "weapon_workshop04",
    .frame = {
        .corner_offsets = {{
            {           810.0f,          -960.0f },      // top
            {         -1350.0f,           576.0f },      // left
            {          -810.0f,           960.0f },      // bottom
            {          1350.0f,          -576.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2335,
            .offset_x = 59.75f,
            .offset_y = -11.5f,
            .direction = 146,
        },
        {
            .vid = 2334,
            .offset_x = 141.25f,
            .offset_y = 9.0f,
            .direction = 192,
        },
        {
            .vid = 2517,
            .offset_x = -64.75f,
            .offset_y = 20.0f,
            .direction = 28,
            .gamma = {
                .r = -10,
                .g = -10,
                .b = 20,
                .a = 0,
            },
        },
        {
            .vid = 2334,
            .offset_x = -52.75f,
            .offset_y = 159.0f,
            .direction = 192,
        },
        {
            .vid = 2335,
            .offset_x = -146.25f,
            .offset_y = 36.5f,
            .direction = 109,
        },
        {
            .vid = 2335,
            .offset_x = -145.25f,
            .offset_y = 135.5f,
            .direction = 146,
        },
        {
            .vid = 2317,
            .offset_x = -156.75f,
            .offset_y = 89.0f,
            .direction = 180,
        },
        {
            .vid = 2317,
            .offset_x = -149.75f,
            .offset_y = 84.0f,
            .pos_z = 30.0f,
            .direction = 1,
        },
        {
            .vid = 2517,
            .offset_x = -286.75f,
            .offset_y = 169.0f,
            .gamma = {
                .r = -10,
                .g = -10,
                .b = 20,
                .a = 0,
            },
        },
        {
            .vid = 2335,
            .offset_x = -343.25f,
            .offset_y = 170.5f,
            .direction = 109,
        },
        {
            .vid = 2334,
            .offset_x = -260.75f,
            .offset_y = 290.0f,
            .direction = 192,
        },
        {
            .vid = 2335,
            .offset_x = -342.25f,
            .offset_y = 269.5f,
            .direction = 146,
        },
        {
            .vid = 2317,
            .offset_x = -359.75f,
            .offset_y = 228.0f,
            .direction = 180,
        },
        {
            .vid = 2317,
            .offset_x = -366.75f,
            .offset_y = 222.0f,
            .pos_z = 46.0f,
            .direction = 86,
            .scale = {
                .x = 0.8f,
                .y = 0.8f,
                .z = 1.0f,
            },
        },
        {
            .vid = 2334,
            .offset_x = -454.75f,
            .offset_y = 440.0f,
            .direction = 192,
        },
        {
            .vid = 2517,
            .offset_x = -466.75f,
            .offset_y = 301.0f,
            .direction = 28,
            .gamma = {
                .r = -10,
                .g = -10,
                .b = 20,
                .a = 0,
            },
        },
        {
            .vid = 2335,
            .offset_x = -547.25f,
            .offset_y = 416.5f,
            .direction = 146,
        },
        {
            .vid = 2335,
            .offset_x = -548.25f,
            .offset_y = 317.5f,
            .direction = 109,
        },
        {
            .vid = 2317,
            .offset_x = -551.75f,
            .offset_y = 365.0f,
            .pos_z = 30.0f,
            .direction = 1,
        },
        {
            .vid = 2317,
            .offset_x = -558.75f,
            .offset_y = 370.0f,
            .direction = 180,
        },
        {
            .vid = 2334,
            .offset_x = 551.25f,
            .offset_y = -277.0f,
            .direction = 192,
        },
        {
            .vid = 2517,
            .offset_x = 345.25f,
            .offset_y = -266.0f,
            .direction = 28,
            .gamma = {
                .r = -10,
                .g = -10,
                .b = 20,
                .a = 0,
            },
        },
        {
            .vid = 2335,
            .offset_x = 469.75f,
            .offset_y = -297.5f,
            .direction = 146,
        },
        {
            .vid = 2334,
            .offset_x = 357.25f,
            .offset_y = -127.0f,
            .direction = 192,
        },
        {
            .vid = 2335,
            .offset_x = 263.75f,
            .offset_y = -249.5f,
            .direction = 109,
        },
        {
            .vid = 2335,
            .offset_x = 264.75f,
            .offset_y = -150.5f,
            .direction = 146,
        },
        {
            .vid = 2317,
            .offset_x = 253.25f,
            .offset_y = -197.0f,
            .direction = 180,
        },
        {
            .vid = 2317,
            .offset_x = 260.25f,
            .offset_y = -202.0f,
            .pos_z = 30.0f,
            .direction = 1,
        },
        {
            .vid = 2317,
            .offset_x = 42.25f,
            .offset_y = -53.0f,
            .direction = 180,
        },
        {
            .vid = 2335,
            .offset_x = 58.75f,
            .offset_y = -110.5f,
            .direction = 109,
        },
        {
            .vid = 2517,
            .offset_x = 115.25f,
            .offset_y = -112.0f,
            .gamma = {
                .r = -10,
                .g = -10,
                .b = 20,
                .a = 0,
            },
        },
        {
            .vid = 2317,
            .offset_x = 35.25f,
            .offset_y = -59.0f,
            .pos_z = 46.0f,
            .direction = 86,
            .scale = {
                .x = 0.8f,
                .y = 0.8f,
                .z = 1.0f,
            },
        },
        {
            .vid = 2517,
            .offset_x = 525.25f,
            .offset_y = -398.0f,
            .gamma = {
                .r = -10,
                .g = -10,
                .b = 20,
                .a = 0,
            },
        },
        {
            .vid = 2317,
            .offset_x = 452.25f,
            .offset_y = -339.0f,
            .direction = 180,
        },
        {
            .vid = 2335,
            .offset_x = 468.75f,
            .offset_y = -396.5f,
            .direction = 109,
        },
        {
            .vid = 2317,
            .offset_x = 445.25f,
            .offset_y = -345.0f,
            .pos_z = 46.0f,
            .direction = 86,
            .scale = {
                .x = 0.8f,
                .y = 0.8f,
                .z = 1.0f,
            },
        },
        {
            .vid = 2536,
            .offset_x = -249.75f,
            .offset_y = 30.0f,
            .direction = 96,
        },
        {
            .vid = 2536,
            .offset_x = -447.75f,
            .offset_y = 167.0f,
            .direction = 128,
        },
        {
            .vid = 2536,
            .offset_x = -651.75f,
            .offset_y = 311.0f,
            .direction = 128,
        },
        {
            .vid = 2536,
            .offset_x = 166.25f,
            .offset_y = -261.0f,
            .direction = 128,
        },
        {
            .vid = 2536,
            .offset_x = -37.75f,
            .offset_y = -119.0f,
            .direction = 128,
        },
        {
            .vid = 2536,
            .offset_x = 369.25f,
            .offset_y = -410.0f,
            .direction = 32,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET6_RANDOM,
};

/**
 * Weapon workshop imported from _prepare/AS2/decoration/08weapon_workshop/weapon_workshop05.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile WEAPON_WORKSHOP05_DECORATION_PROFILE = {
    .id = "weapon_workshop05",
    .frame = {
        .corner_offsets = {{
            {           225.0f,          -544.0f },      // top
            {          -765.0f,           160.0f },      // left
            {          -225.0f,           544.0f },      // bottom
            {           765.0f,          -160.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2527,
            .offset_x = -172.0f,
            .offset_y = 167.0f,
            .pos_z = 46.0f,
            .direction = 162,
        },
        {
            .vid = 2317,
            .offset_x = -178.0f,
            .offset_y = 177.0f,
            .direction = 172,
        },
        {
            .vid = 2516,
            .offset_x = -70.0f,
            .offset_y = 260.0f,
            .direction = 192,
        },
        {
            .vid = 408,
            .offset_x = -110.0f,
            .offset_y = 253.0f,
            .direction = 83,
        },
        {
            .vid = 2515,
            .offset_x = 297.0f,
            .offset_y = 64.0f,
            .direction = 128,
        },
        {
            .vid = 2335,
            .offset_x = 147.0f,
            .offset_y = -56.0f,
            .direction = 164,
        },
        {
            .vid = 2527,
            .offset_x = 121.0f,
            .offset_y = -38.0f,
            .pos_z = 46.0f,
            .direction = 179,
        },
        {
            .vid = 2317,
            .offset_x = 134.0f,
            .offset_y = -45.0f,
            .direction = 172,
        },
        {
            .vid = 408,
            .offset_x = 103.0f,
            .offset_y = 67.0f,
            .direction = 83,
        },
        {
            .vid = 2516,
            .offset_x = 247.0f,
            .offset_y = 39.0f,
            .direction = 140,
        },
        {
            .vid = 2335,
            .offset_x = -2.0f,
            .offset_y = 45.0f,
            .direction = 201,
        },
        {
            .vid = 2335,
            .offset_x = -153.0f,
            .offset_y = 100.0f,
            .direction = 128,
        },
        {
            .vid = 2536,
            .offset_x = -62.0f,
            .offset_y = 104.0f,
            .direction = 32,
        },
        {
            .vid = 2536,
            .offset_x = 27.0f,
            .offset_y = 41.0f,
            .direction = 96,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET6_RANDOM,
};

/**
 * Weapon workshop imported from _prepare/AS2/decoration/08weapon_workshop/weapon_workshop06.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile WEAPON_WORKSHOP06_DECORATION_PROFILE = {
    .id = "weapon_workshop06",
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
            .vid = 2335,
            .offset_x = -138.5f,
            .offset_y = 101.666016f,
            .direction = 128,
        },
        {
            .vid = 2335,
            .offset_x = -6.5f,
            .offset_y = 63.666016f,
            .direction = 201,
        },
        {
            .vid = 2527,
            .offset_x = -166.5f,
            .offset_y = 175.0f,
            .pos_z = 46.0f,
            .direction = 42,
        },
        {
            .vid = 408,
            .offset_x = -106.5f,
            .offset_y = 92.0f,
            .direction = 100,
        },
        {
            .vid = 2317,
            .offset_x = -170.5f,
            .offset_y = 180.0f,
            .direction = 172,
        },
        {
            .vid = 2515,
            .offset_x = -352.5f,
            .offset_y = 88.0f,
            .direction = 128,
        },
        {
            .vid = 2516,
            .offset_x = -294.5f,
            .offset_y = 117.0f,
            .direction = 45,
        },
        {
            .vid = 2527,
            .offset_x = 153.5f,
            .offset_y = -61.0f,
            .pos_z = 46.0f,
            .direction = 25,
        },
        {
            .vid = 2335,
            .offset_x = 194.5f,
            .offset_y = -78.333984f,
            .direction = 164,
        },
        {
            .vid = 2317,
            .offset_x = 138.5f,
            .offset_y = -47.0f,
            .direction = 172,
        },
        {
            .vid = 2515,
            .offset_x = -7.5f,
            .offset_y = -160.0f,
            .direction = 128,
        },
        {
            .vid = 2516,
            .offset_x = 38.5f,
            .offset_y = -121.0f,
            .direction = 253,
        },
        {
            .vid = 2536,
            .offset_x = 21.5f,
            .offset_y = 18.0f,
            .direction = 128,
        },
        {
            .vid = 2536,
            .offset_x = -60.5f,
            .offset_y = 71.0f,
            .direction = 128,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET6_RANDOM,
};

/**
 * Weapon workshop imported from _prepare/AS2/decoration/08weapon_workshop/weapon_workshop07.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile WEAPON_WORKSHOP07_DECORATION_PROFILE = {
    .id = "weapon_workshop07",
    .frame = {
        .corner_offsets = {{
            {           810.0f,          -960.0f },      // top
            {         -1350.0f,           576.0f },      // left
            {          -810.0f,           960.0f },      // bottom
            {          1350.0f,          -576.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2516,
            .offset_x = 6.5f,
            .offset_y = 181.0f,
            .direction = 140,
        },
        {
            .vid = 408,
            .offset_x = -134.5f,
            .offset_y = 216.0f,
            .direction = 83,
        },
        {
            .vid = 2515,
            .offset_x = 48.5f,
            .offset_y = 210.0f,
            .direction = 128,
        },
        {
            .vid = 2516,
            .offset_x = -294.5f,
            .offset_y = 395.0f,
            .direction = 192,
        },
        {
            .vid = 2317,
            .offset_x = -415.5f,
            .offset_y = 326.0f,
            .direction = 172,
        },
        {
            .vid = 408,
            .offset_x = -347.5f,
            .offset_y = 402.0f,
            .direction = 83,
        },
        {
            .vid = 2335,
            .offset_x = -390.5f,
            .offset_y = 249.0f,
            .direction = 128,
        },
        {
            .vid = 2527,
            .offset_x = -409.5f,
            .offset_y = 316.0f,
            .pos_z = 46.0f,
            .direction = 162,
        },
        {
            .vid = 2335,
            .offset_x = -240.203125f,
            .offset_y = 200.102539f,
            .direction = 201,
        },
        {
            .vid = 2515,
            .offset_x = -251.5f,
            .offset_y = 425.0f,
            .direction = 128,
        },
        {
            .vid = 2317,
            .offset_x = 167.5f,
            .offset_y = -75.0f,
            .direction = 172,
        },
        {
            .vid = 2515,
            .offset_x = 329.5f,
            .offset_y = 27.0f,
            .direction = 128,
        },
        {
            .vid = 2516,
            .offset_x = 283.5f,
            .offset_y = -6.0f,
            .direction = 128,
        },
        {
            .vid = 408,
            .offset_x = 235.5f,
            .offset_y = 1.0f,
            .direction = 83,
        },
        {
            .vid = 2335,
            .offset_x = -90.5f,
            .offset_y = 93.0f,
            .direction = 164,
        },
        {
            .vid = 2527,
            .offset_x = -116.5f,
            .offset_y = 111.0f,
            .pos_z = 46.0f,
            .direction = 179,
        },
        {
            .vid = 2317,
            .offset_x = -103.5f,
            .offset_y = 104.0f,
            .direction = 172,
        },
        {
            .vid = 2515,
            .offset_x = 630.5f,
            .offset_y = -182.0f,
            .direction = 128,
        },
        {
            .vid = 2317,
            .offset_x = 479.5f,
            .offset_y = -297.0f,
            .direction = 172,
        },
        {
            .vid = 2335,
            .offset_x = 492.5f,
            .offset_y = -308.0f,
            .direction = 164,
        },
        {
            .vid = 2527,
            .offset_x = 465.5f,
            .offset_y = -296.0f,
            .pos_z = 46.0f,
            .direction = 179,
        },
        {
            .vid = 2335,
            .offset_x = 351.5f,
            .offset_y = -210.0f,
            .direction = 164,
        },
        {
            .vid = 408,
            .offset_x = 448.5f,
            .offset_y = -185.0f,
            .direction = 83,
        },
        {
            .vid = 2516,
            .offset_x = 586.5f,
            .offset_y = -211.0f,
            .direction = 140,
        },
        {
            .vid = 2527,
            .offset_x = 173.5f,
            .offset_y = -85.0f,
            .pos_z = 46.0f,
            .direction = 162,
        },
        {
            .vid = 2335,
            .offset_x = 192.5f,
            .offset_y = -152.0f,
            .direction = 128,
        },
        {
            .vid = 2536,
            .offset_x = -210.5f,
            .offset_y = 192.0f,
            .direction = 96,
        },
        {
            .vid = 2536,
            .offset_x = -299.5f,
            .offset_y = 253.0f,
            .direction = 32,
        },
        {
            .vid = 2536,
            .offset_x = 370.5f,
            .offset_y = -209.0f,
            .direction = 96,
        },
        {
            .vid = 2536,
            .offset_x = 283.5f,
            .offset_y = -148.0f,
            .direction = 32,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET6_RANDOM,
};

/**
 * Weapon workshop imported from _prepare/AS2/decoration/08weapon_workshop/weapon_workshop08.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile WEAPON_WORKSHOP08_DECORATION_PROFILE = {
    .id = "weapon_workshop08",
    .frame = {
        .corner_offsets = {{
            {           765.0f,          -992.0f },      // top
            {         -1395.0f,           544.0f },      // left
            {          -765.0f,           992.0f },      // bottom
            {          1395.0f,          -544.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2335,
            .offset_x = 207.0f,
            .offset_y = -150.333984f,
            .direction = 128,
        },
        {
            .vid = 408,
            .offset_x = 239.0f,
            .offset_y = -160.0f,
            .direction = 100,
        },
        {
            .vid = 2335,
            .offset_x = -43.0f,
            .offset_y = 70.666016f,
            .direction = 164,
        },
        {
            .vid = 2516,
            .offset_x = 59.458496f,
            .offset_y = -148.881836f,
            .direction = 45,
        },
        {
            .vid = 2527,
            .offset_x = 179.0f,
            .offset_y = -77.0f,
            .pos_z = 46.0f,
            .direction = 42,
        },
        {
            .vid = 2317,
            .offset_x = 175.0f,
            .offset_y = -72.0f,
            .direction = 172,
        },
        {
            .vid = 2335,
            .offset_x = -244.0f,
            .offset_y = 212.666016f,
            .direction = 201,
        },
        {
            .vid = 2515,
            .offset_x = -238.0f,
            .offset_y = -18.0f,
            .direction = 128,
        },
        {
            .vid = 2516,
            .offset_x = -188.541504f,
            .offset_y = 18.118164f,
            .direction = 253,
        },
        {
            .vid = 2527,
            .offset_x = -84.0f,
            .offset_y = 88.0f,
            .pos_z = 46.0f,
            .direction = 25,
        },
        {
            .vid = 2317,
            .offset_x = -99.0f,
            .offset_y = 102.0f,
            .direction = 172,
        },
        {
            .vid = 2515,
            .offset_x = -568.0f,
            .offset_y = 226.0f,
            .direction = 128,
        },
        {
            .vid = 2527,
            .offset_x = -404.0f,
            .offset_y = 324.0f,
            .pos_z = 46.0f,
            .direction = 42,
        },
        {
            .vid = 2317,
            .offset_x = -408.0f,
            .offset_y = 329.0f,
            .direction = 172,
        },
        {
            .vid = 2516,
            .offset_x = -515.541504f,
            .offset_y = 264.118164f,
            .direction = 45,
        },
        {
            .vid = 2335,
            .offset_x = -376.0f,
            .offset_y = 250.666016f,
            .direction = 128,
        },
        {
            .vid = 408,
            .offset_x = -344.0f,
            .offset_y = 241.0f,
            .direction = 100,
        },
        {
            .vid = 2317,
            .offset_x = 484.0f,
            .offset_y = -299.0f,
            .direction = 172,
        },
        {
            .vid = 2335,
            .offset_x = 540.0f,
            .offset_y = -330.333984f,
            .direction = 164,
        },
        {
            .vid = 2527,
            .offset_x = 499.0f,
            .offset_y = -313.0f,
            .pos_z = 46.0f,
            .direction = 25,
        },
        {
            .vid = 2335,
            .offset_x = 339.0f,
            .offset_y = -188.333984f,
            .direction = 201,
        },
        {
            .vid = 2515,
            .offset_x = 332.0f,
            .offset_y = -403.0f,
            .direction = 128,
        },
        {
            .vid = 2516,
            .offset_x = 377.397949f,
            .offset_y = -368.820312f,
            .direction = 253,
        },
        {
            .vid = 2515,
            .offset_x = 13.0f,
            .offset_y = -182.0f,
            .direction = 128,
        },
        {
            .vid = 2536,
            .offset_x = -5.0f,
            .offset_y = 29.0f,
            .direction = 128,
        },
        {
            .vid = 2536,
            .offset_x = -278.0f,
            .offset_y = 237.0f,
            .direction = 128,
        },
        {
            .vid = 2536,
            .offset_x = 578.0f,
            .offset_y = -372.0f,
            .direction = 128,
        },
        {
            .vid = 2536,
            .offset_x = 305.0f,
            .offset_y = -164.0f,
            .direction = 128,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET6_RANDOM,
};

} // namespace auto_mapper::core::decoration_builder
