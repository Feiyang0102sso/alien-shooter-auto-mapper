/**
 * @file dorms_as2.h
 * @brief Authored AS2 dormitory decoration stamp profiles.
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
 * Dormitory room imported from _prepare/AS2/decoration/dorms02.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile DORMS02_DECORATION_PROFILE = {
    .id = "dorms02",
    .frame = {
        .corner_offsets = {{
            {        90.0f,      -384.0f },      // top
            {      -540.0f,        64.0f },      // left
            {       -90.0f,       384.0f },      // bottom
            {       540.0f,       -64.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2300,
            .offset_x = 335.0f,
            .offset_y = -96.0f,
            .direction = 89,
        },
        {
            .vid = 1764,
            .offset_x = 417.0f,
            .offset_y = -94.0f,
            .direction = 83,
        },
        {
            .vid = 1764,
            .offset_x = 452.0f,
            .offset_y = -62.0f,
            .direction = 57,
        },
        {
            .vid = 2300,
            .offset_x = 72.0f,
            .offset_y = 83.0f,
            .direction = 153,
        },
        {
            .vid = 1232,
            .offset_x = -214.0f,
            .offset_y = -50.0f,
            .pos_z = 66.0f,
            .direction = 128,
            .gamma = {
                .r = 20,
                .g = 20,
                .b = 25,
                .a = 0,
            },
        },
        {
            .vid = 2300,
            .offset_x = -89.0f,
            .offset_y = -26.0f,
            .direction = 89,
        },
        {
            .vid = 1763,
            .offset_x = -256.0f,
            .offset_y = -12.0f,
            .direction = 6,
        },
        {
            .vid = 1764,
            .offset_x = -288.0f,
            .offset_y = 21.0f,
            .direction = 64,
        },
        {
            .vid = 1764,
            .offset_x = -356.0f,
            .offset_y = 64.0f,
            .direction = 51,
        },
        {
            .vid = 1763,
            .offset_x = 319.0f,
            .offset_y = -158.0f,
            .direction = 0,
        },
        {
            .vid = 1764,
            .offset_x = 282.0f,
            .offset_y = -177.0f,
            .direction = 70,
        },
        {
            .vid = 1763,
            .offset_x = 88.0f,
            .offset_y = -248.0f,
            .direction = 6,
        },
        {
            .vid = 1764,
            .offset_x = 53.0f,
            .offset_y = -227.0f,
            .direction = 76,
        },
        {
            .vid = 1764,
            .offset_x = 31.0f,
            .offset_y = -212.0f,
            .direction = 76,
        },
        {
            .vid = 1764,
            .offset_x = 121.0f,
            .offset_y = -270.0f,
            .direction = 64,
        },
        {
            .vid = 2300,
            .offset_x = 180.0f,
            .offset_y = -208.0f,
            .direction = 153,
        },
        {
            .vid = 1764,
            .offset_x = -51.0f,
            .offset_y = -156.0f,
            .direction = 89,
        },
        {
            .vid = 1764,
            .offset_x = -5.0f,
            .offset_y = -185.0f,
            .direction = 51,
        },
        {
            .vid = 1765,
            .offset_x = -71.0f,
            .offset_y = -18.0f,
            .pos_z = 28.0f,
            .direction = 64,
        },
        {
            .vid = 1765,
            .offset_x = 276.0f,
            .offset_y = -180.0f,
            .pos_z = 40.0f,
            .direction = 83,
        },
        {
            .vid = 1765,
            .offset_x = 20.0f,
            .offset_y = -186.0f,
            .pos_z = 47.0f,
            .direction = 147,
        },
        {
            .vid = 1765,
            .offset_x = 6.0f,
            .offset_y = -177.0f,
            .pos_z = 48.0f,
            .direction = 51,
        },
        {
            .vid = 1765,
            .offset_x = 13.0f,
            .offset_y = -185.0f,
            .pos_z = 62.0f,
            .direction = 32,
        },
        {
            .vid = 1765,
            .offset_x = 14.0f,
            .offset_y = -183.0f,
            .pos_z = 47.0f,
            .direction = 147,
        },
        {
            .vid = 1289,
            .offset_x = -66.0f,
            .offset_y = -134.0f,
            .direction = 104,
        },
        {
            .vid = 1289,
            .offset_x = -303.0f,
            .offset_y = 92.0f,
            .direction = 88,
        },
        {
            .vid = 1760,
            .offset_x = -305.0f,
            .offset_y = 30.0f,
            .pos_z = 30.0f,
            .direction = 0,
        },
        {
            .vid = 1274,
            .offset_x = 159.0f,
            .offset_y = -254.0f,
            .direction = 128,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Dormitory room imported from _prepare/AS2/decoration/dorms03.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile DORMS03_DECORATION_PROFILE = {
    .id = "dorms03",
    .frame = {
        .corner_offsets = {{
            {        90.0f,      -384.0f },      // top
            {      -540.0f,        64.0f },      // left
            {       -90.0f,       384.0f },      // bottom
            {       540.0f,       -64.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 1764,
            .offset_x = 275.0f,
            .offset_y = 67.0f,
            .direction = 0,
        },
        {
            .vid = 2300,
            .offset_x = 120.0f,
            .offset_y = 146.0f,
            .direction = 25,
        },
        {
            .vid = 2300,
            .offset_x = -34.0f,
            .offset_y = 39.0f,
            .direction = 25,
        },
        {
            .vid = 2300,
            .offset_x = -87.0f,
            .offset_y = 256.0f,
            .direction = 38,
        },
        {
            .vid = 2300,
            .offset_x = -317.0f,
            .offset_y = 94.0f,
            .direction = 38,
        },
        {
            .vid = 1764,
            .offset_x = 335.0f,
            .offset_y = -152.0f,
            .direction = 83,
        },
        {
            .vid = 1764,
            .offset_x = 369.0f,
            .offset_y = -122.0f,
            .direction = 57,
        },
        {
            .vid = 2300,
            .offset_x = 254.0f,
            .offset_y = -155.0f,
            .direction = 128,
        },
        {
            .vid = 2300,
            .offset_x = 398.0f,
            .offset_y = -51.0f,
            .direction = 25,
        },
        {
            .vid = 2300,
            .offset_x = -171.0f,
            .offset_y = -55.0f,
            .direction = 128,
        },
        {
            .vid = 1764,
            .offset_x = -262.0f,
            .offset_y = 7.0f,
            .pos_z = -8.0f,
            .direction = 38,
        },
        {
            .vid = 2300,
            .offset_x = 115.0f,
            .offset_y = -254.0f,
            .direction = 51,
        },
        {
            .vid = 1765,
            .offset_x = 255.0f,
            .offset_y = -147.0f,
            .pos_z = 20.0f,
            .direction = 44,
        },
        {
            .vid = 1765,
            .offset_x = 375.0f,
            .offset_y = -106.0f,
            .pos_z = 26.0f,
            .direction = 25,
        },
        {
            .vid = 1765,
            .offset_x = 355.0f,
            .offset_y = -114.0f,
            .pos_z = 31.0f,
            .direction = 12,
        },
        {
            .vid = 1765,
            .offset_x = 354.0f,
            .offset_y = -119.0f,
            .pos_z = 31.0f,
            .direction = 6,
        },
        {
            .vid = 1765,
            .offset_x = 352.0f,
            .offset_y = -125.0f,
            .pos_z = 44.0f,
            .direction = 0,
        },
        {
            .vid = 1765,
            .offset_x = -251.0f,
            .offset_y = 26.0f,
            .pos_z = 40.0f,
            .direction = 25,
        },
        {
            .vid = 1289,
            .offset_x = 224.0f,
            .offset_y = 46.0f,
            .direction = 80,
        },
        {
            .vid = 1289,
            .offset_x = -234.0f,
            .offset_y = 34.0f,
            .direction = 72,
        },
        {
            .vid = 1274,
            .offset_x = -304.0f,
            .offset_y = 53.0f,
            .direction = 0,
        },
        {
            .vid = 1289,
            .offset_x = 213.0f,
            .offset_y = -209.0f,
            .direction = 72,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Dormitory room imported from _prepare/AS2/decoration/dorms04.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile DORMS04_DECORATION_PROFILE = {
    .id = "dorms04",
    .frame = {
        .corner_offsets = {{
            {       -45.0f,      -288.0f },      // top
            {      -405.0f,       -32.0f },      // left
            {        45.0f,       288.0f },      // bottom
            {       405.0f,        32.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 412,
            .offset_x = 290.0f,
            .offset_y = -16.0f,
            .pos_z = -1.0f,
            .direction = 51,
        },
        {
            .vid = 1767,
            .offset_x = 386.0f,
            .offset_y = 37.0f,
            .pos_z = 50.0f,
            .direction = 59,
        },
        {
            .vid = 1767,
            .offset_x = 351.0f,
            .offset_y = 11.0f,
            .pos_z = 50.0f,
            .direction = 42,
        },
        {
            .vid = 1767,
            .offset_x = 341.0f,
            .offset_y = 23.0f,
            .pos_z = 30.0f,
            .direction = 25,
        },
        {
            .vid = 1763,
            .offset_x = 58.0f,
            .offset_y = 190.0f,
            .direction = 19,
        },
        {
            .vid = 1763,
            .offset_x = -5.0f,
            .offset_y = 149.0f,
            .pos_z = 44.0f,
            .direction = 224,
        },
        {
            .vid = 1763,
            .offset_x = 5.0f,
            .offset_y = 170.0f,
            .pos_z = 40.0f,
            .direction = 160,
        },
        {
            .vid = 1763,
            .offset_x = 1.0f,
            .offset_y = 149.0f,
            .direction = 134,
        },
        {
            .vid = 1763,
            .offset_x = -283.0f,
            .offset_y = -14.0f,
            .direction = 115,
        },
        {
            .vid = 1763,
            .offset_x = 166.0f,
            .offset_y = -102.0f,
            .direction = 147,
        },
        {
            .vid = 1763,
            .offset_x = 237.0f,
            .offset_y = -52.0f,
            .direction = 121,
        },
        {
            .vid = 412,
            .offset_x = 55.0f,
            .offset_y = -190.0f,
            .pos_z = 40.0f,
            .direction = 136,
        },
        {
            .vid = 1763,
            .offset_x = -50.0f,
            .offset_y = -182.0f,
            .direction = 76,
        },
        {
            .vid = 1763,
            .offset_x = -6.0f,
            .offset_y = -214.0f,
            .direction = 64,
        },
        {
            .vid = 2300,
            .offset_x = -84.0f,
            .offset_y = -103.0f,
            .direction = 12,
        },
        {
            .vid = 1763,
            .offset_x = 139.0f,
            .offset_y = -124.0f,
            .pos_z = 70.0f,
            .direction = 57,
        },
        {
            .vid = 412,
            .offset_x = -106.0f,
            .offset_y = -154.0f,
            .pos_z = 40.0f,
            .direction = 136,
        },
        {
            .vid = 1763,
            .offset_x = -147.0f,
            .offset_y = -115.0f,
            .pos_z = 70.0f,
            .direction = 38,
        },
        {
            .vid = 1763,
            .offset_x = -201.0f,
            .offset_y = -83.0f,
            .direction = 89,
        },
        {
            .vid = 1763,
            .offset_x = -247.0f,
            .offset_y = -52.0f,
            .direction = 104,
        },
        {
            .vid = 1748,
            .offset_x = 41.0f,
            .offset_y = -13.0f,
            .direction = 16,
        },
        {
            .vid = 1765,
            .offset_x = 229.0f,
            .offset_y = -55.0f,
            .pos_z = 36.0f,
            .direction = 57,
        },
        {
            .vid = 1765,
            .offset_x = 226.0f,
            .offset_y = -54.0f,
            .pos_z = 36.0f,
            .direction = 25,
        },
        {
            .vid = 1765,
            .offset_x = -13.0f,
            .offset_y = -198.0f,
            .pos_z = 45.0f,
            .direction = 70,
        },
        {
            .vid = 1765,
            .offset_x = -109.0f,
            .offset_y = -79.0f,
            .pos_z = 45.0f,
            .direction = 76,
        },
        {
            .vid = 1765,
            .offset_x = -185.0f,
            .offset_y = -87.0f,
            .pos_z = 45.0f,
            .direction = 57,
        },
        {
            .vid = 1550,
            .offset_x = 357.0f,
            .offset_y = 145.0f,
            .pos_z = 60.0f,
            .direction = 128,
        },
        {
            .vid = 1289,
            .offset_x = 44.0f,
            .offset_y = 122.0f,
            .pos_z = 1.0f,
            .direction = 160,
        },
        {
            .vid = 1289,
            .offset_x = 119.0f,
            .offset_y = -133.0f,
            .direction = 128,
        },
        {
            .vid = 1289,
            .offset_x = 81.0f,
            .offset_y = -161.0f,
            .direction = 128,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Dormitory room imported from _prepare/AS2/decoration/dorms05.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile DORMS05_DECORATION_PROFILE = {
    .id = "dorms05",
    .frame = {
        .corner_offsets = {{
            {       -45.0f,      -288.0f },      // top
            {      -405.0f,       -32.0f },      // left
            {        45.0f,       288.0f },      // bottom
            {       405.0f,        32.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 1763,
            .offset_x = 9.0f,
            .offset_y = 176.0f,
            .pos_z = 40.0f,
            .direction = 160,
        },
        {
            .vid = 1763,
            .offset_x = 2.0f,
            .offset_y = 161.0f,
            .direction = 134,
        },
        {
            .vid = 1763,
            .offset_x = 67.0f,
            .offset_y = 192.0f,
            .direction = 19,
        },
        {
            .vid = 412,
            .offset_x = 304.0f,
            .offset_y = -16.0f,
            .pos_z = 40.0f,
            .direction = 119,
        },
        {
            .vid = 1763,
            .offset_x = 319.0f,
            .offset_y = 17.0f,
            .direction = 96,
        },
        {
            .vid = 2304,
            .offset_x = 323.0f,
            .offset_y = 16.0f,
            .pos_z = 37.0f,
            .direction = 145,
        },
        {
            .vid = 412,
            .offset_x = 64.0f,
            .offset_y = -179.0f,
            .pos_z = -1.0f,
            .direction = 34,
        },
        {
            .vid = 1763,
            .offset_x = 198.0f,
            .offset_y = -80.0f,
            .direction = 121,
        },
        {
            .vid = 412,
            .offset_x = -202.0f,
            .offset_y = -81.0f,
            .pos_z = 40.0f,
            .direction = 136,
        },
        {
            .vid = 1763,
            .offset_x = -68.0f,
            .offset_y = -172.0f,
            .direction = 89,
        },
        {
            .vid = 1763,
            .offset_x = -148.0f,
            .offset_y = -127.0f,
            .pos_z = 60.0f,
            .direction = 25,
        },
        {
            .vid = 1763,
            .offset_x = -220.0f,
            .offset_y = -70.0f,
            .direction = 102,
        },
        {
            .vid = 1763,
            .offset_x = -244.0f,
            .offset_y = -56.0f,
            .direction = 102,
        },
        {
            .vid = 2300,
            .offset_x = -87.0f,
            .offset_y = -100.0f,
            .direction = 115,
        },
        {
            .vid = 1763,
            .offset_x = -282.0f,
            .offset_y = -19.0f,
            .direction = 76,
        },
        {
            .vid = 1763,
            .offset_x = 95.0f,
            .offset_y = -167.0f,
            .pos_z = 60.0f,
            .direction = 44,
        },
        {
            .vid = 1763,
            .offset_x = -19.0f,
            .offset_y = -209.0f,
            .direction = 76,
        },
        {
            .vid = 1765,
            .offset_x = 13.0f,
            .offset_y = 162.0f,
            .pos_z = 50.0f,
            .direction = 44,
        },
        {
            .vid = 1765,
            .offset_x = 187.0f,
            .offset_y = -80.0f,
            .pos_z = 40.0f,
            .direction = 57,
        },
        {
            .vid = 1748,
            .offset_x = 12.0f,
            .offset_y = -32.0f,
            .direction = 16,
        },
        {
            .vid = 1765,
            .offset_x = -65.0f,
            .offset_y = -164.0f,
            .pos_z = 50.0f,
            .direction = 0,
        },
        {
            .vid = 1765,
            .offset_x = -52.0f,
            .offset_y = -175.0f,
            .pos_z = 50.0f,
            .direction = 121,
        },
        {
            .vid = 1765,
            .offset_x = -47.0f,
            .offset_y = -179.0f,
            .pos_z = 50.0f,
            .direction = 147,
        },
        {
            .vid = 1289,
            .offset_x = 42.0f,
            .offset_y = 127.0f,
            .direction = 160,
        },
        {
            .vid = 1289,
            .offset_x = 249.0f,
            .offset_y = -37.0f,
            .direction = 128,
        },
        {
            .vid = 1289,
            .offset_x = 135.0f,
            .offset_y = -122.0f,
            .direction = 128,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Dormitory room imported from _prepare/AS2/decoration/dorms06.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile DORMS06_DECORATION_PROFILE = {
    .id = "dorms06",
    .frame = {
        .corner_offsets = {{
            {       -90.0f,      -256.0f },      // top
            {      -360.0f,       -64.0f },      // left
            {        90.0f,       256.0f },      // bottom
            {       360.0f,        64.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 1763,
            .offset_x = 240.0f,
            .offset_y = 96.0f,
            .pos_z = 40.0f,
            .direction = 160,
        },
        {
            .vid = 1763,
            .offset_x = 261.0f,
            .offset_y = 54.0f,
            .direction = 19,
        },
        {
            .vid = 1764,
            .offset_x = 201.0f,
            .offset_y = 115.0f,
            .direction = 115,
        },
        {
            .vid = 1763,
            .offset_x = -69.0f,
            .offset_y = -140.0f,
            .direction = 76,
        },
        {
            .vid = 1763,
            .offset_x = 37.0f,
            .offset_y = -110.0f,
            .pos_z = 60.0f,
            .direction = 32,
        },
        {
            .vid = 2300,
            .offset_x = -97.0f,
            .offset_y = -59.0f,
            .direction = 115,
        },
        {
            .vid = 1304,
            .offset_x = 247.0f,
            .offset_y = 229.0f,
            .pos_z = 104.0f,
            .direction = 137,
        },
        {
            .vid = 1765,
            .offset_x = 165.0f,
            .offset_y = 137.0f,
            .pos_z = 40.0f,
            .direction = 64,
        },
        {
            .vid = 1765,
            .offset_x = 161.0f,
            .offset_y = 140.0f,
            .pos_z = 40.0f,
            .direction = 51,
        },
        {
            .vid = 1765,
            .offset_x = 160.0f,
            .offset_y = 136.0f,
            .pos_z = 40.0f,
            .direction = 32,
        },
        {
            .vid = 1765,
            .offset_x = 199.0f,
            .offset_y = 110.0f,
            .pos_z = 40.0f,
            .direction = 38,
        },
        {
            .vid = 1289,
            .offset_x = 165.0f,
            .offset_y = 94.0f,
            .direction = 56,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Dormitory room imported from _prepare/AS2/decoration/dorms07.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile DORMS07_DECORATION_PROFILE = {
    .id = "dorms07",
    .frame = {
        .corner_offsets = {{
            {       -90.0f,      -256.0f },      // top
            {      -360.0f,       -64.0f },      // left
            {        90.0f,       256.0f },      // bottom
            {       360.0f,        64.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 1763,
            .offset_x = 284.0f,
            .offset_y = 74.0f,
            .direction = 83,
        },
        {
            .vid = 1764,
            .offset_x = 151.0f,
            .offset_y = 72.0f,
            .direction = 147,
        },
        {
            .vid = 1763,
            .offset_x = 43.0f,
            .offset_y = -96.0f,
            .direction = 108,
        },
        {
            .vid = 1763,
            .offset_x = -69.0f,
            .offset_y = -138.0f,
            .direction = 89,
        },
        {
            .vid = 2300,
            .offset_x = -87.560791015625f,
            .offset_y = -67.298828125f,
            .direction = 12,
        },
        {
            .vid = 1763,
            .offset_x = -159.6640625f,
            .offset_y = -87.55322265625f,
            .pos_z = 60.0f,
            .direction = 38,
        },
        {
            .vid = 1765,
            .offset_x = 164.0f,
            .offset_y = 13.0f,
            .direction = 6,
        },
        {
            .vid = 1765,
            .offset_x = 211.0f,
            .offset_y = 56.0f,
            .direction = 25,
        },
        {
            .vid = 1765,
            .offset_x = 187.0f,
            .offset_y = 13.0f,
            .direction = 19,
        },
        {
            .vid = 1765,
            .offset_x = 217.0f,
            .offset_y = 30.0f,
            .direction = 12,
        },
        {
            .vid = 1289,
            .offset_x = 83.0f,
            .offset_y = 129.0f,
            .direction = 48,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Dormitory room imported from _prepare/AS2/decoration/dorms08.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile DORMS08_DECORATION_PROFILE = {
    .id = "dorms08",
    .frame = {
        .corner_offsets = {{
            {        45.0f,      -288.0f },      // top
            {      -405.0f,        32.0f },      // left
            {       -45.0f,       288.0f },      // bottom
            {       405.0f,       -32.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2300,
            .offset_x = 258.4541015625f,
            .offset_y = 17.818115234375f,
            .direction = 51,
        },
        {
            .vid = 2300,
            .offset_x = -11.5458984375f,
            .offset_y = 135.818115234375f,
            .direction = 166,
        },
        {
            .vid = 1273,
            .offset_x = 48.4541015625f,
            .offset_y = 183.818115234375f,
            .direction = 128,
        },
        {
            .vid = 2300,
            .offset_x = -54.5458984375f,
            .offset_y = -43.181884765625f,
            .direction = 76,
        },
        {
            .vid = 413,
            .offset_x = -154.5458984375f,
            .offset_y = -37.181884765625f,
            .direction = 192,
        },
        {
            .vid = 2300,
            .offset_x = -183.5458984375f,
            .offset_y = 32.818115234375f,
            .direction = 76,
        },
        {
            .vid = 1764,
            .offset_x = 215.4541015625f,
            .offset_y = -58.181884765625f,
            .direction = 134,
        },
        {
            .vid = 2300,
            .offset_x = 58.4541015625f,
            .offset_y = -112.181884765625f,
            .direction = 166,
        },
        {
            .vid = 413,
            .offset_x = -31.5458984375f,
            .offset_y = -108.181884765625f,
            .direction = 192,
        },
        {
            .vid = 1289,
            .offset_x = 174.4541015625f,
            .offset_y = -16.181884765625f,
            .direction = 80,
            .gamma = {
                .r = 30,
                .g = 30,
                .b = 30,
                .a = 0,
            },
        },
        {
            .vid = 411,
            .offset_x = 219.4541015625f,
            .offset_y = -61.181884765625f,
            .pos_z = 37.0f,
            .direction = 32,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

} // namespace auto_mapper::core::decoration_builder
