/**
 * @file lab_office_as2.h
 * @brief Authored AS2 laboratory office decoration stamp profiles.
 */

#pragma once

#include "auto_mapper/core/decoration_builder/decoration_profile_types.h"
#include "auto_mapper/core/wall_builder/wall_profiles_as2.h"

namespace auto_mapper::core::decoration_builder {

/**
 * Laboratory office imported from _prepare/AS2/decoration/13lab_office/lab_office01.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile LAB_OFFICE01_DECORATION_PROFILE = {
    .id = "lab_office01",
    .frame = {
        .corner_offsets = {{
            {           -45.0f,          -672.0f },      // top
            {          -945.0f,           -32.0f },      // left
            {            45.0f,           672.0f },      // bottom
            {           945.0f,            32.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 412,
            .offset_x = -298.535706f,
            .offset_y = 87.428589f,
            .direction = 64,
            .gamma = {
                .r = 10,
                .g = 10,
                .b = 10,
                .a = 0,
            },
        },
        {
            .vid = 1770,
            .offset_x = -367.535706f,
            .offset_y = 34.428589f,
            .direction = 32,
            .gamma = {
                .r = 50,
                .g = 50,
                .b = 50,
                .a = 0,
            },
        },
        {
            .vid = 409,
            .offset_x = -181.535706f,
            .offset_y = -150.571411f,
            .direction = 128,
        },
        {
            .vid = 1748,
            .offset_x = 279.464294f,
            .offset_y = 118.428589f,
            .gamma = {
                .r = 40,
                .g = 40,
                .b = 40,
                .a = 0,
            },
        },
        {
            .vid = 1765,
            .offset_x = -340.535706f,
            .offset_y = 51.428589f,
            .pos_z = 38.0f,
            .direction = 44,
        },
        {
            .vid = 1748,
            .offset_x = -27.535706f,
            .offset_y = -71.571411f,
            .gamma = {
                .r = 50,
                .g = 50,
                .b = 50,
                .a = 0,
            },
        },
        {
            .vid = 2370,
            .offset_x = 199.464294f,
            .offset_y = 8.428589f,
            .direction = 102,
        },
        {
            .vid = 2370,
            .offset_x = 36.464294f,
            .offset_y = 45.428589f,
            .direction = 128,
        },
        {
            .vid = 2370,
            .offset_x = 39.464294f,
            .offset_y = 122.428589f,
            .direction = 102,
        },
        {
            .vid = 2370,
            .offset_x = 84.464294f,
            .offset_y = 92.428589f,
            .direction = 102,
        },
        {
            .vid = 1265,
            .offset_x = 122.464294f,
            .offset_y = 40.428589f,
            .gamma = {
                .r = 30,
                .g = 30,
                .b = 30,
                .a = 0,
            },
        },
        {
            .vid = 2370,
            .offset_x = -14.535706f,
            .offset_y = 81.428589f,
            .direction = 128,
        },
        {
            .vid = 1289,
            .offset_x = -316.535706f,
            .offset_y = 1.428589f,
            .direction = 56,
            .gamma = {
                .r = 30,
                .g = 30,
                .b = 30,
                .a = 0,
            },
        },
        {
            .vid = 2370,
            .offset_x = 183.464294f,
            .offset_y = -66.571411f,
            .direction = 128,
        },
        {
            .vid = 2370,
            .offset_x = 139.464294f,
            .offset_y = -35.571411f,
            .direction = 128,
        },
        {
            .vid = 2370,
            .offset_x = 239.464294f,
            .offset_y = -20.571411f,
            .direction = 102,
        },
        {
            .vid = 2370,
            .offset_x = -215.535706f,
            .offset_y = -124.571411f,
            .direction = 102,
        },
        {
            .vid = 2370,
            .offset_x = -144.535706f,
            .offset_y = -174.571411f,
            .direction = 102,
        },
        {
            .vid = 2370,
            .offset_x = -111.535706f,
            .offset_y = -198.571411f,
            .direction = 102,
        },
        {
            .vid = 410,
            .offset_x = -379.535706f,
            .offset_y = 26.428589f,
            .pos_z = 38.0f,
            .direction = 224,
        },
        {
            .vid = 2370,
            .offset_x = -254.535706f,
            .offset_y = -98.571411f,
            .direction = 102,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Laboratory office imported from _prepare/AS2/decoration/13lab_office/lab_office02.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile LAB_OFFICE02_DECORATION_PROFILE = {
    .id = "lab_office02",
    .frame = {
        .corner_offsets = {{
            {          -360.0f,          -640.0f },      // top
            {          -900.0f,          -256.0f },      // left
            {           360.0f,           640.0f },      // bottom
            {           900.0f,           256.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 1730,
            .offset_x = 320.097184f,
            .offset_y = 209.975382f,
            .direction = 128,
            .gamma = {
                .r = 10,
                .g = 10,
                .b = 10,
                .a = 0,
            },
        },
        {
            .vid = 408,
            .offset_x = 269.097184f,
            .offset_y = 252.975382f,
            .pos_z = 18.0f,
            .direction = 128,
        },
        {
            .vid = 412,
            .offset_x = 90.097184f,
            .offset_y = 254.975382f,
            .direction = 64,
            .gamma = {
                .r = 20,
                .g = 20,
                .b = 20,
                .a = 0,
            },
        },
        {
            .vid = 409,
            .offset_x = 413.097123f,
            .offset_y = 9.975443f,
        },
        {
            .vid = 1763,
            .offset_x = 378.097123f,
            .offset_y = -13.024496f,
            .direction = 108,
            .gamma = {
                .r = 50,
                .g = 50,
                .b = 50,
                .a = 0,
            },
        },
        {
            .vid = 1763,
            .offset_x = 343.097184f,
            .offset_y = -41.024557f,
            .direction = 108,
            .gamma = {
                .r = 50,
                .g = 50,
                .b = 50,
                .a = 0,
            },
        },
        {
            .vid = 1763,
            .offset_x = 178.097184f,
            .offset_y = -125.024435f,
            .direction = 147,
        },
        {
            .vid = 1764,
            .offset_x = -214.902816f,
            .offset_y = -142.024435f,
            .direction = 102,
            .gamma = {
                .r = 50,
                .g = 50,
                .b = 50,
                .a = 0,
            },
        },
        {
            .vid = 1763,
            .offset_x = -177.902816f,
            .offset_y = -69.024557f,
            .pos_z = 90.0f,
            .direction = 179,
        },
        {
            .vid = 1763,
            .offset_x = -450.902816f,
            .offset_y = -124.024435f,
            .direction = 19,
        },
        {
            .vid = 1763,
            .offset_x = 139.097184f,
            .offset_y = -244.024496f,
            .pos_z = 53.0f,
            .direction = 32,
        },
        {
            .vid = 1763,
            .offset_x = 227.097184f,
            .offset_y = -183.024466f,
            .pos_z = 53.0f,
            .direction = 57,
        },
        {
            .vid = 1763,
            .offset_x = 320.097184f,
            .offset_y = -115.024496f,
            .pos_z = 53.0f,
            .direction = 32,
        },
        {
            .vid = 408,
            .offset_x = -132.902816f,
            .offset_y = -201.024466f,
            .pos_z = 49.0f,
            .direction = 64,
        },
        {
            .vid = 1763,
            .offset_x = -115.902816f,
            .offset_y = -348.024588f,
            .direction = 64,
            .gamma = {
                .r = 30,
                .g = 30,
                .b = 30,
                .a = 0,
            },
        },
        {
            .vid = 1763,
            .offset_x = -272.902816f,
            .offset_y = -93.024496f,
            .pos_z = 90.0f,
            .direction = 153,
            .gamma = {
                .r = 30,
                .g = 30,
                .b = 30,
                .a = 0,
            },
        },
        {
            .vid = 1285,
            .offset_x = -365.902816f,
            .offset_y = -183.024466f,
            .direction = 128,
            .gamma = {
                .r = 30,
                .g = 30,
                .b = 30,
                .a = 0,
            },
        },
        {
            .vid = 1763,
            .offset_x = -327.902816f,
            .offset_y = -265.024496f,
            .pos_z = 53.0f,
            .direction = 38,
        },
        {
            .vid = 1763,
            .offset_x = -217.902816f,
            .offset_y = -125.024435f,
            .pos_z = 90.0f,
            .direction = 217,
        },
        {
            .vid = 1285,
            .offset_x = -212.902816f,
            .offset_y = -289.024527f,
            .direction = 128,
            .gamma = {
                .r = 30,
                .g = 30,
                .b = 30,
                .a = 0,
            },
        },
        {
            .vid = 1748,
            .offset_x = 32.097184f,
            .offset_y = 8.975443f,
            .direction = 16,
            .gamma = {
                .r = 30,
                .g = 30,
                .b = 30,
                .a = 0,
            },
        },
        {
            .vid = 2337,
            .offset_x = 287.097184f,
            .offset_y = 188.975382f,
            .pos_z = 19.0f,
        },
        {
            .vid = 1289,
            .offset_x = -84.902816f,
            .offset_y = -115.024496f,
            .pos_z = 2.0f,
            .direction = 168,
            .gamma = {
                .r = 30,
                .g = 30,
                .b = 30,
                .a = 0,
            },
        },
        {
            .vid = 1289,
            .offset_x = -177.902816f,
            .offset_y = -36.024557f,
            .pos_z = 2.0f,
            .direction = 176,
            .gamma = {
                .r = 30,
                .g = 30,
                .b = 30,
                .a = 0,
            },
        },
        {
            .vid = 1289,
            .offset_x = -287.902816f,
            .offset_y = -226.024496f,
            .direction = 152,
            .gamma = {
                .r = 30,
                .g = 30,
                .b = 30,
                .a = 0,
            },
        },
        {
            .vid = 1286,
            .offset_x = -365.902816f,
            .offset_y = -155.024496f,
            .pos_z = 25.0f,
            .direction = 64,
        },
        {
            .vid = 1286,
            .offset_x = -213.902816f,
            .offset_y = -262.024496f,
            .pos_z = 25.0f,
            .direction = 64,
        },
        {
            .vid = 2338,
            .offset_x = 287.097184f,
            .offset_y = 237.975382f,
            .pos_z = 65.0f,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET8_RANDOM,
};

/**
 * Laboratory office imported from _prepare/AS2/decoration/13lab_office/lab_office03.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile LAB_OFFICE03_DECORATION_PROFILE = {
    .id = "lab_office03",
    .frame = {
        .corner_offsets = {{
            {           360.0f,          -640.0f },      // top
            {          -900.0f,           256.0f },      // left
            {          -360.0f,           640.0f },      // bottom
            {           900.0f,          -256.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 1730,
            .offset_x = -239.999937f,
            .offset_y = 274.000007f,
            .direction = 192,
            .gamma = {
                .r = 10,
                .g = 10,
                .b = 10,
                .a = 0,
            },
        },
        {
            .vid = 412,
            .offset_x = -293.999937f,
            .offset_y = 117.000007f,
            .direction = 64,
            .gamma = {
                .r = 20,
                .g = 20,
                .b = 20,
                .a = 0,
            },
        },
        {
            .vid = 1285,
            .offset_x = 458.000002f,
            .offset_y = -147.999993f,
            .pos_z = 2.0f,
            .gamma = {
                .r = 30,
                .g = 30,
                .b = 30,
                .a = 0,
            },
        },
        {
            .vid = 1763,
            .offset_x = 337.999941f,
            .offset_y = -106.999993f,
            .pos_z = 50.0f,
            .direction = 224,
        },
        {
            .vid = 408,
            .offset_x = 326.999941f,
            .offset_y = -51.999993f,
            .pos_z = 48.0f,
        },
        {
            .vid = 1764,
            .offset_x = 303.999941f,
            .offset_y = -107.999993f,
            .direction = 108,
            .gamma = {
                .r = 50,
                .g = 50,
                .b = 50,
                .a = 0,
            },
        },
        {
            .vid = 1763,
            .offset_x = 205.999941f,
            .offset_y = -129.999993f,
            .pos_z = 48.0f,
            .direction = 185,
        },
        {
            .vid = 408,
            .offset_x = 156.999941f,
            .offset_y = -36.999993f,
            .pos_z = 3.0f,
            .direction = 213,
        },
        {
            .vid = 1763,
            .offset_x = 423.000002f,
            .offset_y = -239.999993f,
            .pos_z = 53.0f,
            .direction = 44,
        },
        {
            .vid = 1763,
            .offset_x = 274.999941f,
            .offset_y = -156.999993f,
            .pos_z = 50.0f,
            .direction = 160,
            .gamma = {
                .r = 30,
                .g = 30,
                .b = 30,
                .a = 0,
            },
        },
        {
            .vid = 1285,
            .offset_x = 304.999941f,
            .offset_y = -253.999993f,
            .pos_z = 2.0f,
            .gamma = {
                .r = 30,
                .g = 30,
                .b = 30,
                .a = 0,
            },
        },
        {
            .vid = 413,
            .offset_x = 169.999941f,
            .offset_y = -366.999993f,
            .direction = 21,
        },
        {
            .vid = 413,
            .offset_x = 156.060488f,
            .offset_y = -358.061028f,
            .direction = 21,
        },
        {
            .vid = 413,
            .offset_x = 199.999941f,
            .offset_y = -359.999993f,
        },
        {
            .vid = 1763,
            .offset_x = 178.999941f,
            .offset_y = -346.999993f,
            .direction = 19,
        },
        {
            .vid = 413,
            .offset_x = 124.060488f,
            .offset_y = -335.061028f,
            .direction = 21,
        },
        {
            .vid = 1748,
            .offset_x = 54.999941f,
            .offset_y = 68.000007f,
            .gamma = {
                .r = 30,
                .g = 30,
                .b = 30,
                .a = 0,
            },
        },
        {
            .vid = 2337,
            .offset_x = -190.999937f,
            .offset_y = 253.000007f,
            .pos_z = 17.0f,
            .direction = 64,
        },
        {
            .vid = 1227,
            .offset_x = 524.000002f,
            .offset_y = -177.999993f,
            .pos_z = -22.0f,
            .direction = 149,
        },
        {
            .vid = 1289,
            .offset_x = 380.999941f,
            .offset_y = -190.999993f,
            .direction = 128,
            .gamma = {
                .r = 30,
                .g = 30,
                .b = 30,
                .a = 0,
            },
        },
        {
            .vid = 1289,
            .offset_x = 259.999941f,
            .offset_y = -4.999993f,
            .pos_z = 2.0f,
            .direction = 176,
            .gamma = {
                .r = 30,
                .g = 30,
                .b = 30,
                .a = 0,
            },
        },
        {
            .vid = 1289,
            .offset_x = 138.999941f,
            .offset_y = -80.999993f,
            .pos_z = 2.0f,
            .direction = 168,
            .gamma = {
                .r = 30,
                .g = 30,
                .b = 30,
                .a = 0,
            },
        },
        {
            .vid = 2339,
            .offset_x = -191.999937f,
            .offset_y = 308.000007f,
            .pos_z = 70.0f,
            .direction = 128,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET8_RANDOM,
};

/**
 * Laboratory office imported from _prepare/AS2/decoration/13lab_office/lab_office04.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile LAB_OFFICE04_DECORATION_PROFILE = {
    .id = "lab_office04",
    .frame = {
        .corner_offsets = {{
            {          -225.0f,          -864.0f },      // top
            {         -1215.0f,          -160.0f },      // left
            {           225.0f,           864.0f },      // bottom
            {          1215.0f,           160.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 412,
            .offset_x = 384.066406f,
            .offset_y = 321.199707f,
            .direction = 85,
            .gamma = {
                .r = 20,
                .g = 20,
                .b = 20,
                .a = 0,
            },
        },
        {
            .vid = 409,
            .offset_x = 477.066406f,
            .offset_y = 13.199707f,
        },
        {
            .vid = 1770,
            .offset_x = 146.066406f,
            .offset_y = -152.800293f,
            .direction = 96,
            .gamma = {
                .r = 50,
                .g = 50,
                .b = 50,
                .a = 0,
            },
        },
        {
            .vid = 409,
            .offset_x = -405.933594f,
            .offset_y = -169.800293f,
            .direction = 128,
        },
        {
            .vid = 412,
            .offset_x = -511.933594f,
            .offset_y = -80.800293f,
            .direction = 68,
            .gamma = {
                .r = 20,
                .g = 20,
                .b = 20,
                .a = 0,
            },
        },
        {
            .vid = 412,
            .offset_x = -90.933594f,
            .offset_y = -359.800293f,
            .pos_z = 2.0f,
            .direction = 102,
            .gamma = {
                .r = 20,
                .g = 20,
                .b = 20,
                .a = 0,
            },
        },
        {
            .vid = 1763,
            .offset_x = -271.933594f,
            .offset_y = -258.800293f,
            .pos_z = 2.0f,
            .direction = 166,
            .gamma = {
                .r = 30,
                .g = 30,
                .b = 30,
                .a = 0,
            },
        },
        {
            .vid = 1748,
            .offset_x = 338.066406f,
            .offset_y = 98.199707f,
            .direction = 16,
            .gamma = {
                .r = 50,
                .g = 50,
                .b = 50,
                .a = 0,
            },
        },
        {
            .vid = 1748,
            .offset_x = 91.066406f,
            .offset_y = 290.199707f,
            .direction = 16,
            .gamma = {
                .r = 50,
                .g = 50,
                .b = 50,
                .a = 0,
            },
        },
        {
            .vid = 1765,
            .offset_x = 124.066406f,
            .offset_y = -123.800293f,
            .pos_z = 38.0f,
            .direction = 44,
        },
        {
            .vid = 1748,
            .offset_x = -174.933594f,
            .offset_y = -123.800293f,
            .gamma = {
                .r = 50,
                .g = 50,
                .b = 50,
                .a = 0,
            },
        },
        {
            .vid = 2370,
            .offset_x = 565.066406f,
            .offset_y = 77.199707f,
            .direction = 115,
        },
        {
            .vid = 2370,
            .offset_x = 180.066406f,
            .offset_y = 135.199707f,
            .direction = 140,
        },
        {
            .vid = 2370,
            .offset_x = 300.066406f,
            .offset_y = 220.199707f,
            .direction = 140,
        },
        {
            .vid = 2370,
            .offset_x = 351.066406f,
            .offset_y = 260.199707f,
            .direction = 140,
        },
        {
            .vid = 2370,
            .offset_x = 313.066406f,
            .offset_y = 294.199707f,
            .direction = 115,
        },
        {
            .vid = 2370,
            .offset_x = 256.066406f,
            .offset_y = 255.199707f,
            .direction = 115,
        },
        {
            .vid = 1265,
            .offset_x = 210.066406f,
            .offset_y = 206.199707f,
            .direction = 128,
            .gamma = {
                .r = 30,
                .g = 30,
                .b = 30,
                .a = 0,
            },
        },
        {
            .vid = 2370,
            .offset_x = 129.066406f,
            .offset_y = 94.199707f,
            .direction = 140,
        },
        {
            .vid = 2370,
            .offset_x = 132.066406f,
            .offset_y = 169.199707f,
            .direction = 115,
        },
        {
            .vid = 2370,
            .offset_x = 78.066406f,
            .offset_y = 129.199707f,
            .direction = 115,
        },
        {
            .vid = 2370,
            .offset_x = 433.066406f,
            .offset_y = -17.800293f,
            .direction = 115,
        },
        {
            .vid = 2370,
            .offset_x = 517.066406f,
            .offset_y = 45.199707f,
            .direction = 115,
        },
        {
            .vid = 2370,
            .offset_x = 390.066406f,
            .offset_y = -46.800293f,
            .direction = 115,
        },
        {
            .vid = 410,
            .offset_x = 152.066406f,
            .offset_y = -155.800293f,
            .pos_z = 38.0f,
            .direction = 128,
        },
        {
            .vid = 2370,
            .offset_x = -345.933594f,
            .offset_y = -181.800293f,
            .pos_z = 2.0f,
            .direction = 102,
        },
        {
            .vid = 2370,
            .offset_x = -442.933594f,
            .offset_y = -116.800293f,
            .pos_z = 2.0f,
            .direction = 102,
        },
        {
            .vid = 2370,
            .offset_x = -301.933594f,
            .offset_y = -213.800293f,
            .pos_z = 2.0f,
            .direction = 102,
        },
        {
            .vid = 2370,
            .offset_x = -201.933594f,
            .offset_y = -283.800293f,
            .pos_z = 2.0f,
            .direction = 102,
        },
        {
            .vid = 2370,
            .offset_x = -148.933594f,
            .offset_y = -322.800293f,
            .pos_z = 2.0f,
            .direction = 102,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

} // namespace auto_mapper::core::decoration_builder
