/**
 * @file sick_bay_as2.h
 * @brief Authored AS2 sick bay decoration stamp profiles.
 */

#pragma once

#include "auto_mapper/core/decoration_builder/decoration_profile_types.h"
#include "auto_mapper/core/wall_builder/wall_profiles_as2.h"

namespace auto_mapper::core::decoration_builder {

/**
 * Sick bay imported from _prepare/AS2/decoration/09sick_bay/sick_bay01.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile SICK_BAY01_DECORATION_PROFILE = {
    .id = "sick_bay01",
    .frame = {
        .corner_offsets = {{
            {          -225.0f,          -288.0f },      // top
            {          -405.0f,          -160.0f },      // left
            {           225.0f,           288.0f },      // bottom
            {           405.0f,           160.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 408,
            .offset_x = 98.0f,
            .offset_y = 28.0f,
            .direction = 170,
        },
        {
            .vid = 2306,
            .offset_x = -9.0f,
            .offset_y = 19.0f,
            .direction = 85,
        },
        {
            .vid = 1283,
            .offset_x = 146.0f,
            .offset_y = 9.0f,
            .direction = 128,
        },
        {
            .vid = 2306,
            .offset_x = -6.0f,
            .offset_y = -157.0f,
            .direction = 59,
        },
        {
            .vid = 2306,
            .offset_x = -37.0f,
            .offset_y = -132.0f,
            .direction = 34,
        },
        {
            .vid = 1293,
            .offset_x = -97.0f,
            .offset_y = -72.0f,
            .direction = 99,
        },
        {
            .vid = 2306,
            .offset_x = -66.0f,
            .offset_y = -111.0f,
            .direction = 34,
        },
        {
            .vid = 2306,
            .offset_x = 70.0f,
            .offset_y = -38.0f,
            .direction = 85,
        },
        {
            .vid = 1289,
            .offset_x = 44.200439f,
            .offset_y = 25.068359f,
            .direction = 85,
        },
        {
            .vid = 1760,
            .offset_x = 125.0f,
            .offset_y = 23.0f,
            .pos_z = 30.0f,
            .direction = 54,
        },
        {
            .vid = 1760,
            .offset_x = 147.0f,
            .offset_y = 7.0f,
            .pos_z = 30.0f,
            .direction = 16,
        },
        {
            .vid = 1760,
            .offset_x = 147.0f,
            .offset_y = 10.0f,
            .pos_z = 30.0f,
            .direction = 28,
        },
        {
            .vid = 1760,
            .offset_x = 158.0f,
            .offset_y = 34.0f,
            .pos_z = 30.0f,
            .direction = 102,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Sick bay imported from _prepare/AS2/decoration/09sick_bay/sick_bay02.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile SICK_BAY02_DECORATION_PROFILE = {
    .id = "sick_bay02",
    .frame = {
        .corner_offsets = {{
            {          -225.0f,          -288.0f },      // top
            {          -405.0f,          -160.0f },      // left
            {           225.0f,           288.0f },      // bottom
            {           405.0f,           160.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2306,
            .offset_x = -38.0f,
            .offset_y = -124.0f,
            .direction = 42,
        },
        {
            .vid = 2306,
            .offset_x = 31.0f,
            .offset_y = -65.0f,
            .direction = 25,
        },
        {
            .vid = 2306,
            .offset_x = -14.0f,
            .offset_y = -75.0f,
            .direction = 8,
        },
        {
            .vid = 2306,
            .offset_x = -108.0f,
            .offset_y = 41.0f,
            .direction = 93,
        },
        {
            .vid = 2306,
            .offset_x = -30.0f,
            .offset_y = 37.0f,
            .direction = 85,
        },
        {
            .vid = 2306,
            .offset_x = 49.0f,
            .offset_y = -19.0f,
            .direction = 85,
        },
        {
            .vid = 1304,
            .offset_x = 185.984619f,
            .offset_y = 29.265381f,
            .pos_z = 90.0f,
            .direction = 179,
        },
        {
            .vid = 1304,
            .offset_x = 246.0f,
            .offset_y = 69.0f,
            .pos_z = 90.0f,
            .direction = 181,
        },
        {
            .vid = 1750,
            .offset_x = 227.0f,
            .offset_y = 61.0f,
            .pos_z = 50.0f,
        },
        {
            .vid = 1750,
            .offset_x = 167.0f,
            .offset_y = 19.0f,
            .pos_z = 50.0f,
        },
        {
            .vid = 1289,
            .offset_x = 93.0f,
            .offset_y = -9.0f,
            .direction = 99,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Sick bay imported from _prepare/AS2/decoration/09sick_bay/sick_bay03.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile SICK_BAY03_DECORATION_PROFILE = {
    .id = "sick_bay03",
    .frame = {
        .corner_offsets = {{
            {            90.0f,          -320.0f },      // top
            {          -450.0f,            64.0f },      // left
            {           -90.0f,           320.0f },      // bottom
            {           450.0f,           -64.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2306,
            .offset_x = -48.5f,
            .offset_y = 54.0f,
            .direction = 68,
        },
        {
            .vid = 2306,
            .offset_x = -9.5f,
            .offset_y = 51.0f,
            .direction = 93,
        },
        {
            .vid = 2306,
            .offset_x = -8.5f,
            .offset_y = 101.0f,
            .direction = 85,
        },
        {
            .vid = 2306,
            .offset_x = -165.5f,
            .offset_y = 30.0f,
        },
        {
            .vid = 2306,
            .offset_x = -192.5f,
            .offset_y = 107.0f,
            .direction = 42,
        },
        {
            .vid = 1764,
            .offset_x = 207.5f,
            .offset_y = -85.0f,
            .direction = 12,
        },
        {
            .vid = 1763,
            .offset_x = 313.5f,
            .offset_y = -57.0f,
            .direction = 6,
        },
        {
            .vid = 1764,
            .offset_x = 352.5f,
            .offset_y = -24.0f,
            .direction = 57,
        },
        {
            .vid = 2306,
            .offset_x = -90.5f,
            .offset_y = -5.0f,
            .direction = 93,
        },
        {
            .vid = 2306,
            .offset_x = -144.5f,
            .offset_y = -11.0f,
            .direction = 34,
        },
        {
            .vid = 1304,
            .offset_x = 184.003906f,
            .offset_y = -163.566406f,
            .pos_z = 86.0f,
            .direction = 181,
        },
        {
            .vid = 1765,
            .offset_x = 367.5f,
            .offset_y = -2.0f,
            .pos_z = 63.0f,
            .direction = 153,
        },
        {
            .vid = 1765,
            .offset_x = 364.5f,
            .offset_y = -6.0f,
            .pos_z = 62.0f,
            .direction = 128,
        },
        {
            .vid = 1765,
            .offset_x = 362.5f,
            .offset_y = -12.0f,
            .pos_z = 58.0f,
            .direction = 102,
        },
        {
            .vid = 1765,
            .offset_x = 352.5f,
            .offset_y = -14.0f,
            .pos_z = 47.0f,
            .direction = 64,
        },
        {
            .vid = 1765,
            .offset_x = 336.5f,
            .offset_y = -29.0f,
            .pos_z = 40.0f,
            .direction = 57,
        },
        {
            .vid = 1765,
            .offset_x = 333.5f,
            .offset_y = -21.0f,
            .pos_z = 32.0f,
            .direction = 70,
        },
        {
            .vid = 1760,
            .offset_x = 188.5f,
            .offset_y = -86.0f,
            .pos_z = 30.0f,
            .direction = 12,
        },
        {
            .vid = 1759,
            .offset_x = 208.5f,
            .offset_y = -60.0f,
            .pos_z = 40.0f,
            .direction = 51,
        },
        {
            .vid = 1289,
            .offset_x = 282.5f,
            .offset_y = -29.0f,
            .direction = 40,
        },
        {
            .vid = 1759,
            .offset_x = 190.5f,
            .offset_y = -83.0f,
            .pos_z = 30.0f,
            .direction = 25,
        },
        {
            .vid = 1759,
            .offset_x = 168.5f,
            .offset_y = -60.0f,
            .pos_z = 30.0f,
            .direction = 83,
        },
        {
            .vid = 1750,
            .offset_x = 165.003906f,
            .offset_y = -171.566406f,
            .pos_z = 46.0f,
        },
        {
            .vid = 1759,
            .offset_x = 222.5f,
            .offset_y = -83.0f,
            .pos_z = 40.0f,
            .direction = 112,
        },
        {
            .vid = 1750,
            .offset_x = 105.003906f,
            .offset_y = -213.566406f,
            .pos_z = 46.0f,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Sick bay imported from _prepare/AS2/decoration/09sick_bay/sick_bay04.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile SICK_BAY04_DECORATION_PROFILE = {
    .id = "sick_bay04",
    .frame = {
        .corner_offsets = {{
            {            45.0f,          -224.0f },      // top
            {          -315.0f,            32.0f },      // left
            {           -45.0f,           224.0f },      // bottom
            {           315.0f,           -32.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2306,
            .offset_x = 56.25f,
            .offset_y = 56.0f,
            .direction = 8,
        },
        {
            .vid = 2306,
            .offset_x = 113.25f,
            .offset_y = -45.0f,
            .direction = 51,
        },
        {
            .vid = 2306,
            .offset_x = 65.25f,
            .offset_y = -70.0f,
            .direction = 25,
        },
        {
            .vid = 2306,
            .offset_x = -6.75f,
            .offset_y = 44.0f,
            .direction = 85,
        },
        {
            .vid = 1750,
            .offset_x = 194.935547f,
            .offset_y = -9.119385f,
            .pos_z = 51.0f,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET6_RANDOM,
};

/**
 * Sick bay imported from _prepare/AS2/decoration/09sick_bay/sick_bay05.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile SICK_BAY05_DECORATION_PROFILE = {
    .id = "sick_bay05",
    .frame = {
        .corner_offsets = {{
            {           180.0f,          -448.0f },      // top
            {          -630.0f,           128.0f },      // left
            {          -180.0f,           448.0f },      // bottom
            {           630.0f,          -128.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2335,
            .offset_x = 174.53418f,
            .offset_y = 74.524414f,
            .direction = 54,
        },
        {
            .vid = 2300,
            .offset_x = -34.46582f,
            .offset_y = 72.524414f,
            .direction = 38,
        },
        {
            .vid = 1764,
            .offset_x = -204.229004f,
            .offset_y = 90.904785f,
            .direction = 38,
            .gamma = {
                .r = 20,
                .g = 20,
                .b = 20,
                .a = 0,
            },
        },
        {
            .vid = 2306,
            .offset_x = -4.46582f,
            .offset_y = -73.475586f,
            .direction = 34,
        },
        {
            .vid = 2300,
            .offset_x = 153.554199f,
            .offset_y = -72.475586f,
            .direction = 38,
        },
        {
            .vid = 2306,
            .offset_x = 148.554199f,
            .offset_y = -13.475586f,
            .direction = 93,
        },
        {
            .vid = 2306,
            .offset_x = 226.554199f,
            .offset_y = -17.475586f,
            .direction = 85,
        },
        {
            .vid = 2306,
            .offset_x = 195.53418f,
            .offset_y = -125.475586f,
            .direction = 25,
        },
        {
            .vid = 2306,
            .offset_x = -24.46582f,
            .offset_y = -16.475586f,
            .direction = 76,
        },
        {
            .vid = 1765,
            .offset_x = -226.229004f,
            .offset_y = 92.904785f,
            .pos_z = 38.0f,
            .direction = 25,
        },
        {
            .vid = 1765,
            .offset_x = -213.229004f,
            .offset_y = 109.904785f,
            .pos_z = 38.0f,
            .direction = 44,
        },
        {
            .vid = 1765,
            .offset_x = -226.229004f,
            .offset_y = 95.904785f,
            .pos_z = 38.0f,
            .direction = 51,
        },
        {
            .vid = 1765,
            .offset_x = -198.229004f,
            .offset_y = 95.904785f,
            .pos_z = 38.0f,
            .direction = 38,
        },
        {
            .vid = 1765,
            .offset_x = -173.229004f,
            .offset_y = 78.904785f,
            .pos_z = 38.0f,
            .direction = 12,
        },
        {
            .vid = 1765,
            .offset_x = -174.229004f,
            .offset_y = 83.904785f,
            .pos_z = 38.0f,
            .direction = 12,
        },
        {
            .vid = 1765,
            .offset_x = -191.229004f,
            .offset_y = 73.904785f,
            .pos_z = 38.0f,
            .direction = 32,
        },
        {
            .vid = 407,
            .offset_x = -174.229004f,
            .offset_y = 120.904785f,
            .direction = 42,
        },
        {
            .vid = 1641,
            .offset_x = -67.46582f,
            .offset_y = 61.524414f,
            .direction = 128,
        },
        {
            .vid = 2360,
            .offset_x = 97.53418f,
            .offset_y = -122.475586f,
        },
        {
            .vid = 2360,
            .offset_x = -97.46582f,
            .offset_y = 16.524414f,
        },
        {
            .vid = 2362,
            .offset_x = 95.53418f,
            .offset_y = -77.475586f,
            .pos_z = 40.0f,
        },
        {
            .vid = 2364,
            .offset_x = -96.46582f,
            .offset_y = 57.524414f,
            .pos_z = 40.0f,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

} // namespace auto_mapper::core::decoration_builder
