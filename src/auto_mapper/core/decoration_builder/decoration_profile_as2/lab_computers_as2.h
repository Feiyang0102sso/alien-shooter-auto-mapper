/**
 * @file lab_computers_as2.h
 * @brief Authored AS2 laboratory computer room decoration stamp profiles.
 */

#pragma once

#include "auto_mapper/core/decoration_builder/decoration_profile_types.h"
#include "auto_mapper/core/wall_builder/wall_profiles_as2.h"

namespace auto_mapper::core::decoration_builder {

/**
 * Laboratory computer room imported from _prepare/AS2/decoration/14lab_computers/lab_comp01.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile LAB_COMP01_DECORATION_PROFILE = {
    .id = "lab_comp01",
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
            .vid = 2335,
            .offset_x = 59.199707f,
            .offset_y = -11.333496f,
            .direction = 109,
        },
        {
            .vid = 2370,
            .offset_x = -96.800293f,
            .offset_y = 58.666504f,
            .direction = 25,
        },
        {
            .vid = 2340,
            .offset_x = -43.800293f,
            .offset_y = 110.666504f,
        },
        {
            .vid = 2370,
            .offset_x = -8.800293f,
            .offset_y = 145.666504f,
            .direction = 25,
        },
        {
            .vid = 2370,
            .offset_x = 91.199707f,
            .offset_y = -60.333496f,
            .direction = 64,
        },
        {
            .vid = 2340,
            .offset_x = 154.199707f,
            .offset_y = -4.333496f,
        },
        {
            .vid = 2370,
            .offset_x = 190.199707f,
            .offset_y = 18.666504f,
            .direction = 25,
        },
        {
            .vid = 2337,
            .offset_x = -34.800293f,
            .offset_y = -38.333496f,
        },
        {
            .vid = 2340,
            .offset_x = -119.800293f,
            .offset_y = 40.666504f,
        },
        {
            .vid = 2340,
            .offset_x = 54.199707f,
            .offset_y = -82.333496f,
        },
        {
            .vid = 2344,
            .offset_x = -43.800293f,
            .offset_y = 157.666504f,
            .pos_z = 49.0f,
        },
        {
            .vid = 2344,
            .offset_x = 152.199707f,
            .offset_y = 34.666504f,
            .pos_z = 41.0f,
        },
        {
            .vid = 2339,
            .offset_x = -44.800293f,
            .offset_y = -1.333496f,
            .pos_z = 41.0f,
        },
        {
            .vid = 2344,
            .offset_x = -121.800293f,
            .offset_y = 78.666504f,
            .pos_z = 41.0f,
        },
        {
            .vid = 2344,
            .offset_x = 53.199707f,
            .offset_y = -42.333496f,
            .pos_z = 41.0f,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Laboratory computer room imported from _prepare/AS2/decoration/14lab_computers/lab_comp02.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile LAB_COMP02_DECORATION_PROFILE = {
    .id = "lab_comp02",
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
            .vid = 2341,
            .offset_x = 147.480469f,
            .offset_y = 103.307617f,
            .direction = 74,
        },
        {
            .vid = 2370,
            .offset_x = 89.480469f,
            .offset_y = 94.307617f,
            .direction = 74,
        },
        {
            .vid = 2370,
            .offset_x = 30.044769f,
            .offset_y = 164.587219f,
            .direction = 134,
        },
        {
            .vid = 2341,
            .offset_x = 67.480469f,
            .offset_y = 175.307617f,
            .direction = 74,
        },
        {
            .vid = 2340,
            .offset_x = 146.480469f,
            .offset_y = -29.692383f,
            .direction = 114,
        },
        {
            .vid = 2340,
            .offset_x = 224.480469f,
            .offset_y = 15.307617f,
            .direction = 64,
        },
        {
            .vid = 2370,
            .offset_x = 41.480469f,
            .offset_y = -122.692383f,
            .direction = 134,
        },
        {
            .vid = 2370,
            .offset_x = 111.480469f,
            .offset_y = -61.692383f,
            .direction = 114,
        },
        {
            .vid = 2340,
            .offset_x = 71.480469f,
            .offset_y = -82.692383f,
            .direction = 114,
        },
        {
            .vid = 2344,
            .offset_x = 153.480469f,
            .offset_y = 140.307617f,
            .pos_z = 40.0f,
            .direction = 72,
        },
        {
            .vid = 2344,
            .offset_x = 72.480469f,
            .offset_y = 211.307617f,
            .pos_z = 40.0f,
            .direction = 72,
        },
        {
            .vid = 2342,
            .offset_x = 151.480469f,
            .offset_y = 15.307617f,
            .pos_z = 41.0f,
            .direction = 134,
        },
        {
            .vid = 2342,
            .offset_x = 76.480469f,
            .offset_y = -36.692383f,
            .pos_z = 41.0f,
            .direction = 134,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Laboratory computer room imported from _prepare/AS2/decoration/14lab_computers/lab_comp03.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile LAB_COMP03_DECORATION_PROFILE = {
    .id = "lab_comp03",
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
            .vid = 2370,
            .offset_x = 142.146973f,
            .offset_y = -3.941162f,
            .direction = 176,
        },
        {
            .vid = 2340,
            .offset_x = 83.146973f,
            .offset_y = 51.058838f,
            .direction = 54,
        },
        {
            .vid = 2370,
            .offset_x = 49.146973f,
            .offset_y = 71.058838f,
            .direction = 153,
        },
        {
            .vid = 2370,
            .offset_x = -31.853027f,
            .offset_y = 16.058838f,
            .direction = 134,
        },
        {
            .vid = 2340,
            .offset_x = 7.146973f,
            .offset_y = -7.941162f,
            .direction = 54,
        },
        {
            .vid = 2370,
            .offset_x = -39.853027f,
            .offset_y = 145.058838f,
            .direction = 134,
        },
        {
            .vid = 2340,
            .offset_x = -4.853027f,
            .offset_y = 119.058838f,
            .direction = 54,
        },
        {
            .vid = 2370,
            .offset_x = -120.853027f,
            .offset_y = 93.058838f,
            .direction = 87,
        },
        {
            .vid = 2370,
            .offset_x = 72.146973f,
            .offset_y = -54.941162f,
            .direction = 253,
        },
        {
            .vid = 2340,
            .offset_x = 181.146973f,
            .offset_y = -19.941162f,
            .direction = 54,
        },
        {
            .vid = 2340,
            .offset_x = 111.146973f,
            .offset_y = -72.941162f,
            .direction = 54,
        },
        {
            .vid = 2342,
            .offset_x = 92.146973f,
            .offset_y = 87.058838f,
            .pos_z = 40.0f,
            .direction = 54,
        },
        {
            .vid = 2342,
            .offset_x = -81.853027f,
            .offset_y = 95.058838f,
            .pos_z = 40.0f,
            .direction = 54,
        },
        {
            .vid = 2342,
            .offset_x = 13.146973f,
            .offset_y = 28.058838f,
            .pos_z = 40.0f,
            .direction = 54,
        },
        {
            .vid = 2342,
            .offset_x = 2.146973f,
            .offset_y = 153.058838f,
            .pos_z = 40.0f,
            .direction = 54,
        },
        {
            .vid = 2342,
            .offset_x = 113.146973f,
            .offset_y = -38.941162f,
            .pos_z = 40.0f,
            .direction = 54,
        },
        {
            .vid = 2342,
            .offset_x = 186.146973f,
            .offset_y = 20.058838f,
            .pos_z = 40.0f,
            .direction = 54,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Laboratory computer room imported from _prepare/AS2/decoration/14lab_computers/lab_comp04.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile LAB_COMP04_DECORATION_PROFILE = {
    .id = "lab_comp04",
    .frame = {
        .corner_offsets = {{
            {            90.0f,          -448.0f },      // top
            {          -630.0f,            64.0f },      // left
            {           -90.0f,           448.0f },      // bottom
            {           630.0f,           -64.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2330,
            .offset_x = -26.0f,
            .offset_y = 297.0f,
            .direction = 25,
        },
        {
            .vid = 2335,
            .offset_x = 166.0f,
            .offset_y = 133.0f,
            .direction = 146,
        },
        {
            .vid = 2330,
            .offset_x = 430.0f,
            .offset_y = -51.0f,
            .direction = 12,
        },
        {
            .vid = 2335,
            .offset_x = -32.0f,
            .offset_y = -105.0f,
            .direction = 109,
        },
        {
            .vid = 2370,
            .offset_x = -284.0f,
            .offset_y = 53.0f,
            .direction = 193,
        },
        {
            .vid = 1639,
            .offset_x = 407.0f,
            .offset_y = -97.0f,
            .pos_z = 50.0f,
            .direction = 146,
        },
        {
            .vid = 2370,
            .offset_x = 95.0f,
            .offset_y = -213.0f,
            .direction = 74,
        },
        {
            .vid = 2370,
            .offset_x = 5.0f,
            .offset_y = -150.0f,
            .direction = 4,
        },
        {
            .vid = 2340,
            .offset_x = -39.0f,
            .offset_y = -193.05957f,
            .direction = 240,
        },
        {
            .vid = 2370,
            .offset_x = -202.0f,
            .offset_y = 11.0f,
            .direction = 98,
        },
        {
            .vid = 2340,
            .offset_x = -332.0f,
            .offset_y = 4.94043f,
            .direction = 240,
        },
        {
            .vid = 2340,
            .offset_x = -247.0f,
            .offset_y = -58.05957f,
            .direction = 240,
        },
        {
            .vid = 2337,
            .offset_x = -134.0f,
            .offset_y = -125.582031f,
            .direction = 240,
        },
        {
            .vid = 2340,
            .offset_x = 36.0f,
            .offset_y = -254.05957f,
            .direction = 240,
        },
        {
            .vid = 2343,
            .offset_x = -41.0f,
            .offset_y = -146.05957f,
            .pos_z = 50.0f,
            .direction = 240,
        },
        {
            .vid = 2343,
            .offset_x = -333.0f,
            .offset_y = 50.94043f,
            .pos_z = 50.0f,
            .direction = 240,
        },
        {
            .vid = 2343,
            .offset_x = -249.0f,
            .offset_y = -9.05957f,
            .pos_z = 50.0f,
            .direction = 240,
        },
        {
            .vid = 2338,
            .offset_x = -139.0f,
            .offset_y = -82.582031f,
            .pos_z = 50.0f,
            .direction = 240,
        },
        {
            .vid = 2343,
            .offset_x = 34.0f,
            .offset_y = -207.05957f,
            .pos_z = 50.0f,
            .direction = 240,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET8_RANDOM,
};

} // namespace auto_mapper::core::decoration_builder
