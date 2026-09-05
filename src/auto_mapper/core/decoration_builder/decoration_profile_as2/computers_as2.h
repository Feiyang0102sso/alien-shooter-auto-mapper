/**
 * @file computers_as2.h
 * @brief Authored AS2 computer room decoration stamp profiles.
 */

#pragma once

#include "auto_mapper/core/decoration_builder/decoration_profile_types.h"
#include "auto_mapper/core/wall_builder/wall_profiles_as2.h"

namespace auto_mapper::core::decoration_builder {

/**
 * Computer room imported from _prepare/AS2/decoration/05computers/comp01.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile COMP01_DECORATION_PROFILE = {
    .id = "comp01",
    .frame = {
        .corner_offsets = {{
            {          -135.0f,          -544.0f },      // top
            {          -765.0f,           -96.0f },      // left
            {           135.0f,           544.0f },      // bottom
            {           765.0f,            96.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2308,
            .offset_x = 145.919413f,
            .offset_y = 283.90189f,
            .direction = 128,
        },
        {
            .vid = 412,
            .offset_x = 22.919413f,
            .offset_y = 256.90189f,
            .direction = 89,
        },
        {
            .vid = 408,
            .offset_x = 55.919413f,
            .offset_y = 267.90189f,
            .pos_z = 1.0f,
            .direction = 149,
        },
        {
            .vid = 409,
            .offset_x = 52.919413f,
            .offset_y = 237.90189f,
            .direction = 89,
        },
        {
            .vid = 2308,
            .offset_x = 520.919413f,
            .offset_y = 9.90189f,
            .direction = 157,
        },
        {
            .vid = 409,
            .offset_x = 244.919413f,
            .offset_y = 87.90189f,
            .direction = 89,
        },
        {
            .vid = 412,
            .offset_x = 409.919413f,
            .offset_y = -29.09811f,
            .direction = 93,
        },
        {
            .vid = 412,
            .offset_x = 216.919413f,
            .offset_y = 108.90189f,
            .direction = 89,
        },
        {
            .vid = 2308,
            .offset_x = 336.919413f,
            .offset_y = 146.90189f,
            .direction = 128,
        },
        {
            .vid = 2308,
            .offset_x = 100.620585f,
            .offset_y = -22.700649f,
            .direction = 128,
        },
        {
            .vid = 409,
            .offset_x = -260.080587f,
            .offset_y = 37.90189f,
            .direction = 89,
        },
        {
            .vid = 412,
            .offset_x = -282.080587f,
            .offset_y = 58.90189f,
            .direction = 89,
        },
        {
            .vid = 408,
            .offset_x = -332.080587f,
            .offset_y = 77.90189f,
            .pos_z = 1.0f,
            .direction = 149,
        },
        {
            .vid = 409,
            .offset_x = 432.919413f,
            .offset_y = -45.09811f,
            .direction = 89,
        },
        {
            .vid = 2304,
            .offset_x = 248.620585f,
            .offset_y = -235.700649f,
            .pos_z = 48.0f,
            .direction = 128,
        },
        {
            .vid = 2304,
            .offset_x = 370.620585f,
            .offset_y = -150.700649f,
            .pos_z = 48.0f,
            .direction = 128,
        },
        {
            .vid = 409,
            .offset_x = 148.919413f,
            .offset_y = -255.09811f,
            .direction = 89,
        },
        {
            .vid = 412,
            .offset_x = 127.919413f,
            .offset_y = -239.09811f,
            .direction = 89,
        },
        {
            .vid = 408,
            .offset_x = 63.919413f,
            .offset_y = -215.09811f,
            .pos_z = 1.0f,
            .direction = 243,
        },
        {
            .vid = 412,
            .offset_x = -78.080587f,
            .offset_y = -84.09811f,
            .direction = 89,
        },
        {
            .vid = 409,
            .offset_x = -61.080587f,
            .offset_y = -113.09811f,
            .direction = 89,
        },
        {
            .vid = 408,
            .offset_x = -184.080587f,
            .offset_y = -96.09811f,
            .pos_z = 1.0f,
            .direction = 229,
        },
        {
            .vid = 2308,
            .offset_x = -328.080587f,
            .offset_y = -50.09811f,
            .direction = 128,
        },
        {
            .vid = 2308,
            .offset_x = -127.080587f,
            .offset_y = -191.09811f,
            .direction = 128,
        },
        {
            .vid = 2308,
            .offset_x = 63.919413f,
            .offset_y = -308.09811f,
            .direction = 157,
        },
        {
            .vid = 407,
            .offset_x = 315.919413f,
            .offset_y = 245.90189f,
            .direction = 96,
        },
        {
            .vid = 2522,
            .offset_x = 166.919413f,
            .offset_y = 345.90189f,
            .direction = 64,
        },
        {
            .vid = 2522,
            .offset_x = 118.919413f,
            .offset_y = 310.90189f,
            .direction = 99,
        },
        {
            .vid = 2522,
            .offset_x = 70.919413f,
            .offset_y = 276.90189f,
            .direction = 27,
        },
        {
            .vid = 1289,
            .offset_x = 33.919413f,
            .offset_y = 334.90189f,
            .pos_z = 1.0f,
            .direction = 40,
        },
        {
            .vid = 407,
            .offset_x = 102.919413f,
            .offset_y = 386.902867f,
            .direction = 85,
        },
        {
            .vid = 2522,
            .offset_x = 514.919413f,
            .offset_y = 25.90189f,
        },
        {
            .vid = 407,
            .offset_x = 508.919413f,
            .offset_y = 95.90189f,
            .direction = 85,
        },
        {
            .vid = 1289,
            .offset_x = 420.919413f,
            .offset_y = 36.90189f,
            .pos_z = 1.0f,
            .direction = 47,
        },
        {
            .vid = 2522,
            .offset_x = 464.919413f,
            .offset_y = -9.09811f,
            .direction = 64,
        },
        {
            .vid = 2521,
            .offset_x = 564.919413f,
            .offset_y = 60.90189f,
            .direction = 99,
        },
        {
            .vid = 2522,
            .offset_x = 270.919413f,
            .offset_y = 138.90189f,
            .direction = 2,
        },
        {
            .vid = 2522,
            .offset_x = 321.919413f,
            .offset_y = 171.90189f,
            .direction = 64,
        },
        {
            .vid = 1289,
            .offset_x = 226.919413f,
            .offset_y = 192.90189f,
            .pos_z = 1.0f,
            .direction = 179,
        },
        {
            .vid = 2521,
            .offset_x = 370.919413f,
            .offset_y = 204.90189f,
            .direction = 99,
        },
        {
            .vid = 2522,
            .offset_x = -311.080587f,
            .offset_y = 21.90189f,
            .direction = 64,
        },
        {
            .vid = 2522,
            .offset_x = -358.080587f,
            .offset_y = -11.09811f,
            .direction = 254,
        },
        {
            .vid = 1289,
            .offset_x = -455.080587f,
            .offset_y = -1.09811f,
            .pos_z = 1.0f,
            .direction = 55,
        },
        {
            .vid = 1289,
            .offset_x = -383.080587f,
            .offset_y = 58.90189f,
            .pos_z = 1.0f,
            .direction = 181,
        },
        {
            .vid = 1755,
            .offset_x = 268.680156f,
            .offset_y = -152.63937f,
            .direction = 192,
        },
        {
            .vid = 1289,
            .offset_x = -41.080587f,
            .offset_y = -284.09811f,
            .pos_z = 1.0f,
            .direction = 179,
        },
        {
            .vid = 1289,
            .offset_x = 158.620585f,
            .offset_y = -164.700649f,
            .direction = 56,
        },
        {
            .vid = 2521,
            .offset_x = 95.919413f,
            .offset_y = -269.09811f,
            .direction = 98,
        },
        {
            .vid = 1289,
            .offset_x = 44.919413f,
            .offset_y = -220.09811f,
            .pos_z = 1.0f,
            .direction = 40,
        },
        {
            .vid = 2522,
            .offset_x = -159.080587f,
            .offset_y = -161.09811f,
            .direction = 96,
        },
        {
            .vid = 2522,
            .offset_x = -205.080587f,
            .offset_y = -194.09811f,
        },
        {
            .vid = 1289,
            .offset_x = -250.080587f,
            .offset_y = -138.09811f,
            .pos_z = 1.0f,
            .direction = 28,
        },
        {
            .vid = 2522,
            .offset_x = -112.080587f,
            .offset_y = -128.09811f,
        },
        {
            .vid = 1289,
            .offset_x = -164.080587f,
            .offset_y = -65.09811f,
            .pos_z = 1.0f,
            .direction = 179,
        },
        {
            .vid = 2521,
            .offset_x = -406.080587f,
            .offset_y = -49.09811f,
            .direction = 25,
        },
        {
            .vid = 2522,
            .offset_x = 1.919413f,
            .offset_y = -335.09811f,
            .direction = 64,
        },
        {
            .vid = 2522,
            .offset_x = 49.919413f,
            .offset_y = -300.09811f,
            .direction = 64,
        },
        {
            .vid = 2309,
            .offset_x = 139.919413f,
            .offset_y = 281.90189f,
            .direction = 128,
        },
        {
            .vid = 2309,
            .offset_x = 520.919413f,
            .offset_y = 12.90189f,
            .direction = 157,
        },
        {
            .vid = 2309,
            .offset_x = 339.919413f,
            .offset_y = 148.90189f,
            .direction = 128,
        },
        {
            .vid = 2307,
            .offset_x = 100.620585f,
            .offset_y = -22.700649f,
            .direction = 128,
        },
        {
            .vid = 2309,
            .offset_x = -133.080587f,
            .offset_y = -198.09811f,
            .direction = 128,
        },
        {
            .vid = 2309,
            .offset_x = -329.080587f,
            .offset_y = -50.09811f,
            .direction = 128,
        },
        {
            .vid = 2309,
            .offset_x = 58.919413f,
            .offset_y = -309.09811f,
            .direction = 157,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET5_RANDOM,
};

/**
 * Computer room imported from _prepare/AS2/decoration/05computers/comp02.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile COMP02_DECORATION_PROFILE = {
    .id = "comp02",
    .frame = {
        .corner_offsets = {{
            {           180.0f,          -384.0f },      // top
            {          -540.0f,           128.0f },      // left
            {          -180.0f,           384.0f },      // bottom
            {           540.0f,          -128.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 1304,
            .offset_x = 265.0f,
            .offset_y = 183.0f,
            .direction = 124,
        },
        {
            .vid = 2521,
            .offset_x = -349.0f,
            .offset_y = 128.0f,
            .direction = 48,
        },
        {
            .vid = 1289,
            .offset_x = -276.0f,
            .offset_y = 144.0f,
            .direction = 200,
        },
        {
            .vid = 1289,
            .offset_x = -224.0f,
            .offset_y = 178.0f,
            .direction = 216,
        },
        {
            .vid = 401,
            .offset_x = 413.0f,
            .offset_y = -130.0f,
            .direction = 138,
        },
        {
            .vid = 1289,
            .offset_x = 92.0f,
            .offset_y = -87.0f,
            .direction = 168,
        },
        {
            .vid = 2530,
            .offset_x = 0.0f,
            .offset_y = -64.0f,
            .direction = 27,
        },
        {
            .vid = 1289,
            .offset_x = -183.0f,
            .offset_y = 78.0f,
            .direction = 192,
        },
        {
            .vid = 2521,
            .offset_x = -209.0f,
            .offset_y = 30.0f,
            .direction = 48,
        },
        {
            .vid = 2521,
            .offset_x = -302.0f,
            .offset_y = 92.0f,
            .direction = 112,
        },
        {
            .vid = 2521,
            .offset_x = -255.0f,
            .offset_y = 61.0f,
            .direction = 80,
        },
        {
            .vid = 2521,
            .offset_x = 295.0f,
            .offset_y = -247.0f,
            .direction = 32,
        },
        {
            .vid = 1289,
            .offset_x = 340.0f,
            .offset_y = -145.0f,
            .direction = 216,
        },
        {
            .vid = 2521,
            .offset_x = 432.0f,
            .offset_y = -153.0f,
        },
        {
            .vid = 2521,
            .offset_x = 388.0f,
            .offset_y = -181.0f,
            .direction = 96,
        },
        {
            .vid = 2521,
            .offset_x = 342.0f,
            .offset_y = -214.0f,
            .direction = 64,
        },
        {
            .vid = 2521,
            .offset_x = 224.0f,
            .offset_y = -285.0f,
            .direction = 144,
        },
        {
            .vid = 2521,
            .offset_x = 193.0f,
            .offset_y = -251.0f,
            .direction = 80,
        },
        {
            .vid = 1289,
            .offset_x = 201.0f,
            .offset_y = -185.0f,
            .direction = 200,
        },
        {
            .vid = 2521,
            .offset_x = 146.0f,
            .offset_y = -218.0f,
            .direction = 48,
        },
        {
            .vid = 1289,
            .offset_x = 261.0f,
            .offset_y = -208.0f,
            .direction = 208,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET6_RANDOM,
};

/**
 * Computer room imported from _prepare/AS2/decoration/05computers/comp03.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile COMP03_DECORATION_PROFILE = {
    .id = "comp03",
    .frame = {
        .corner_offsets = {{
            {           -90.0f,          -320.0f },      // top
            {          -450.0f,           -64.0f },      // left
            {            90.0f,           320.0f },      // bottom
            {           450.0f,            64.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2308,
            .offset_x = 42.0f,
            .offset_y = 309.0f,
            .direction = 128,
        },
        {
            .vid = 2308,
            .offset_x = -218.0f,
            .offset_y = 139.0f,
            .direction = 128,
        },
        {
            .vid = 2308,
            .offset_x = -343.0f,
            .offset_y = 50.0f,
            .direction = 128,
        },
        {
            .vid = 2522,
            .offset_x = 214.0f,
            .offset_y = 101.0f,
            .direction = 48,
        },
        {
            .vid = 2522,
            .offset_x = 118.0f,
            .offset_y = 174.0f,
            .direction = 48,
        },
        {
            .vid = 2522,
            .offset_x = 77.0f,
            .offset_y = 203.0f,
            .direction = 112,
        },
        {
            .vid = 2522,
            .offset_x = 34.0f,
            .offset_y = 233.0f,
            .direction = 80,
        },
        {
            .vid = 1289,
            .offset_x = -89.0f,
            .offset_y = 140.0f,
            .direction = 96,
        },
        {
            .vid = 2522,
            .offset_x = -155.0f,
            .offset_y = 107.0f,
            .direction = 80,
        },
        {
            .vid = 2522,
            .offset_x = 299.0f,
            .offset_y = 40.0f,
            .direction = 112,
        },
        {
            .vid = 2522,
            .offset_x = 119.0f,
            .offset_y = -82.0f,
            .direction = 16,
        },
        {
            .vid = 2522,
            .offset_x = 29.0f,
            .offset_y = -19.0f,
            .direction = 80,
        },
        {
            .vid = 1289,
            .offset_x = 147.0f,
            .offset_y = -30.0f,
            .direction = 104,
        },
        {
            .vid = 2522,
            .offset_x = 260.0f,
            .offset_y = 69.0f,
            .direction = 80,
        },
        {
            .vid = 2522,
            .offset_x = 74.0f,
            .offset_y = -50.0f,
            .direction = 48,
        },
        {
            .vid = 1289,
            .offset_x = 92.0f,
            .offset_y = 9.0f,
            .direction = 112,
        },
        {
            .vid = 2536,
            .offset_x = -191.0f,
            .offset_y = -108.0f,
            .direction = 252,
        },
        {
            .vid = 2522,
            .offset_x = -139.0f,
            .offset_y = -142.0f,
            .direction = 16,
        },
        {
            .vid = 1289,
            .offset_x = -55.0f,
            .offset_y = -131.0f,
            .direction = 104,
        },
        {
            .vid = 2521,
            .offset_x = -244.0f,
            .offset_y = -74.0f,
            .direction = 80,
        },
        {
            .vid = 1289,
            .offset_x = -229.0f,
            .offset_y = -11.0f,
            .direction = 112,
        },
        {
            .vid = 2521,
            .offset_x = -71.0f,
            .offset_y = 48.0f,
            .direction = 80,
        },
        {
            .vid = 2522,
            .offset_x = -116.0f,
            .offset_y = 80.0f,
            .direction = 112,
        },
        {
            .vid = 1289,
            .offset_x = -282.0f,
            .offset_y = 16.0f,
            .direction = 104,
        },
        {
            .vid = 2522,
            .offset_x = -287.0f,
            .offset_y = -44.0f,
            .direction = 80,
        },
        {
            .vid = 2522,
            .offset_x = -333.0f,
            .offset_y = -11.0f,
            .direction = 48,
        },
        {
            .vid = 2521,
            .offset_x = -95.0f,
            .offset_y = -178.0f,
            .direction = 112,
        },
        {
            .vid = 1289,
            .offset_x = -19.0f,
            .offset_y = -170.0f,
            .direction = 96,
        },
        {
            .vid = 2522,
            .offset_x = -51.0f,
            .offset_y = -208.0f,
            .direction = 80,
        },
        {
            .vid = 2309,
            .offset_x = 35.0f,
            .offset_y = 304.0f,
            .direction = 128,
        },
        {
            .vid = 2309,
            .offset_x = -216.0f,
            .offset_y = 139.0f,
            .direction = 128,
        },
        {
            .vid = 2309,
            .offset_x = -344.0f,
            .offset_y = 48.0f,
            .direction = 128,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET6_RANDOM,
};

/**
 * Computer room imported from _prepare/AS2/decoration/05computers/comp04.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile COMP04_DECORATION_PROFILE = {
    .id = "comp04",
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
            .vid = 1289,
            .offset_x = -29.553349f,
            .offset_y = 107.197144f,
            .direction = 216,
        },
        {
            .vid = 2521,
            .offset_x = -61.658817f,
            .offset_y = 72.975952f,
            .direction = 48,
        },
        {
            .vid = 2521,
            .offset_x = -78.658817f,
            .offset_y = 97.975952f,
            .direction = 160,
        },
        {
            .vid = 1289,
            .offset_x = 16.446651f,
            .offset_y = 81.197144f,
            .direction = 208,
        },
        {
            .vid = 1289,
            .offset_x = 196.446651f,
            .offset_y = -38.802856f,
            .direction = 200,
        },
        {
            .vid = 2521,
            .offset_x = 164.341183f,
            .offset_y = -100.024048f,
            .direction = 144,
        },
        {
            .vid = 2521,
            .offset_x = 133.341183f,
            .offset_y = -66.024048f,
            .direction = 48,
        },
        {
            .vid = 2521,
            .offset_x = 88.341183f,
            .offset_y = -32.024048f,
            .direction = 48,
        },
        {
            .vid = 2536,
            .offset_x = 264.837765f,
            .offset_y = -73.754028f,
            .direction = 80,
        },
        {
            .vid = 2534,
            .offset_x = 22.341183f,
            .offset_y = -6.024048f,
            .direction = 248,
        },
        {
            .vid = 2521,
            .offset_x = -12.658817f,
            .offset_y = 40.975952f,
            .direction = 48,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET6_RANDOM,
};

/**
 * Computer room imported from _prepare/AS2/decoration/05computers/comp05.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile COMP05_DECORATION_PROFILE = {
    .id = "comp05",
    .frame = {
        .corner_offsets = {{
            {            90.0f,          -576.0f },      // top
            {          -810.0f,            64.0f },      // left
            {           -90.0f,           576.0f },      // bottom
            {           810.0f,           -64.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2308,
            .offset_x = -92.278888f,
            .offset_y = 353.264741f,
            .direction = 64,
        },
        {
            .vid = 412,
            .offset_x = -165.278888f,
            .offset_y = 210.264741f,
            .direction = 64,
        },
        {
            .vid = 408,
            .offset_x = -144.278888f,
            .offset_y = 258.264741f,
            .direction = 209,
        },
        {
            .vid = 412,
            .offset_x = -280.278888f,
            .offset_y = 112.264741f,
            .direction = 64,
        },
        {
            .vid = 2308,
            .offset_x = -424.278888f,
            .offset_y = 176.264741f,
            .direction = 48,
        },
        {
            .vid = 408,
            .offset_x = -242.278888f,
            .offset_y = 170.264741f,
            .direction = 53,
        },
        {
            .vid = 2308,
            .offset_x = -253.278888f,
            .offset_y = 237.264741f,
            .direction = 64,
        },
        {
            .vid = 412,
            .offset_x = 621.721112f,
            .offset_y = -127.735259f,
            .direction = 64,
        },
        {
            .vid = 1764,
            .offset_x = 658.721112f,
            .offset_y = -114.735259f,
            .direction = 96,
        },
        {
            .vid = 412,
            .offset_x = 680.721112f,
            .offset_y = -92.735259f,
            .direction = 64,
        },
        {
            .vid = 408,
            .offset_x = 376.721112f,
            .offset_y = -112.735259f,
            .direction = 117,
        },
        {
            .vid = 408,
            .offset_x = 343.721112f,
            .offset_y = -70.735259f,
            .direction = 103,
        },
        {
            .vid = 2308,
            .offset_x = 494.721112f,
            .offset_y = -61.735259f,
            .direction = 64,
        },
        {
            .vid = 2308,
            .offset_x = 341.721112f,
            .offset_y = -171.735259f,
            .direction = 64,
        },
        {
            .vid = 408,
            .offset_x = 167.721112f,
            .offset_y = -159.735259f,
            .direction = 102,
        },
        {
            .vid = 412,
            .offset_x = 162.721112f,
            .offset_y = -187.735259f,
            .direction = 64,
        },
        {
            .vid = 412,
            .offset_x = 287.721112f,
            .offset_y = -107.735259f,
            .direction = 8,
        },
        {
            .vid = 408,
            .offset_x = -118.278888f,
            .offset_y = -154.735259f,
            .direction = 113,
        },
        {
            .vid = 412,
            .offset_x = -117.278888f,
            .offset_y = -173.735259f,
            .direction = 64,
        },
        {
            .vid = 408,
            .offset_x = 34.721112f,
            .offset_y = -195.735259f,
            .direction = 218,
        },
        {
            .vid = 2308,
            .offset_x = -274.278888f,
            .offset_y = -110.735259f,
            .direction = 128,
        },
        {
            .vid = 408,
            .offset_x = -214.278888f,
            .offset_y = -68.735259f,
            .direction = 188,
        },
        {
            .vid = 2308,
            .offset_x = 207.721112f,
            .offset_y = -271.735259f,
            .direction = 48,
        },
        {
            .vid = 2308,
            .offset_x = -112.278888f,
            .offset_y = -224.735259f,
            .direction = 128,
        },
        {
            .vid = 1289,
            .offset_x = -190.278888f,
            .offset_y = 301.264741f,
            .direction = 104,
        },
        {
            .vid = 2536,
            .offset_x = -152.278888f,
            .offset_y = 179.264741f,
            .direction = 89,
        },
        {
            .vid = 2522,
            .offset_x = -353.278888f,
            .offset_y = 148.264741f,
            .direction = 76,
        },
        {
            .vid = 2536,
            .offset_x = -321.278888f,
            .offset_y = 112.264741f,
            .direction = 64,
        },
        {
            .vid = 2522,
            .offset_x = -400.278888f,
            .offset_y = 182.264741f,
            .direction = 22,
        },
        {
            .vid = 1289,
            .offset_x = -322.278888f,
            .offset_y = 194.264741f,
            .direction = 198,
        },
        {
            .vid = 2522,
            .offset_x = -297.278888f,
            .offset_y = 294.264741f,
            .direction = 10,
        },
        {
            .vid = 2522,
            .offset_x = -448.278888f,
            .offset_y = 208.264741f,
            .direction = 8,
        },
        {
            .vid = 2522,
            .offset_x = -252.278888f,
            .offset_y = 262.264741f,
            .direction = 43,
        },
        {
            .vid = 2522,
            .offset_x = -205.278888f,
            .offset_y = 227.264741f,
            .direction = 111,
        },
        {
            .vid = 2522,
            .offset_x = 413.721112f,
            .offset_y = -194.735259f,
            .direction = 14,
        },
        {
            .vid = 2522,
            .offset_x = 368.721112f,
            .offset_y = -163.735259f,
            .direction = 108,
        },
        {
            .vid = 2522,
            .offset_x = 321.721112f,
            .offset_y = -128.735259f,
            .direction = 53,
        },
        {
            .vid = 1289,
            .offset_x = 426.721112f,
            .offset_y = -113.735259f,
            .direction = 194,
        },
        {
            .vid = 2536,
            .offset_x = 253.721112f,
            .offset_y = -92.735259f,
            .direction = 38,
        },
        {
            .vid = 2536,
            .offset_x = 126.721112f,
            .offset_y = -200.735259f,
            .direction = 64,
        },
        {
            .vid = 1289,
            .offset_x = 274.721112f,
            .offset_y = -199.735259f,
            .direction = 206,
        },
        {
            .vid = 1265,
            .offset_x = -158.278888f,
            .offset_y = -205.735259f,
        },
        {
            .vid = 1265,
            .offset_x = -259.278888f,
            .offset_y = -143.735259f,
        },
        {
            .vid = 1755,
            .offset_x = -224.278888f,
            .offset_y = -182.735259f,
        },
        {
            .vid = 1755,
            .offset_x = -377.278888f,
            .offset_y = -83.735259f,
        },
        {
            .vid = 1289,
            .offset_x = -333.278888f,
            .offset_y = -32.735259f,
            .direction = 171,
        },
        {
            .vid = 2522,
            .offset_x = 301.721112f,
            .offset_y = -308.735259f,
            .direction = 19,
        },
        {
            .vid = 2522,
            .offset_x = 252.721112f,
            .offset_y = -273.735259f,
            .direction = 47,
        },
        {
            .vid = 2522,
            .offset_x = 202.721112f,
            .offset_y = -243.735259f,
            .direction = 13,
        },
        {
            .vid = 1289,
            .offset_x = 9.721112f,
            .offset_y = -249.735259f,
            .direction = 54,
        },
        {
            .vid = 1755,
            .offset_x = -68.278888f,
            .offset_y = -292.735259f,
        },
        {
            .vid = 2309,
            .offset_x = -90.278888f,
            .offset_y = 350.264741f,
            .direction = 64,
        },
        {
            .vid = 2309,
            .offset_x = -419.278888f,
            .offset_y = 174.264741f,
            .direction = 48,
        },
        {
            .vid = 2309,
            .offset_x = -249.278888f,
            .offset_y = 237.264741f,
            .direction = 64,
        },
        {
            .vid = 2309,
            .offset_x = 495.721112f,
            .offset_y = -64.735259f,
            .direction = 64,
        },
        {
            .vid = 2309,
            .offset_x = 342.721112f,
            .offset_y = -173.735259f,
            .direction = 64,
        },
        {
            .vid = 2309,
            .offset_x = -277.278888f,
            .offset_y = -110.735259f,
            .direction = 152,
        },
        {
            .vid = 2309,
            .offset_x = 209.721112f,
            .offset_y = -276.735259f,
            .direction = 48,
        },
        {
            .vid = 2309,
            .offset_x = -115.278888f,
            .offset_y = -226.735259f,
            .direction = 152,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET6_RANDOM,
};

/**
 * Computer room imported from _prepare/AS2/decoration/05computers/comp06.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile COMP06_DECORATION_PROFILE = {
    .id = "comp06",
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
            .vid = 2318,
            .offset_x = -252.071777f,
            .offset_y = 189.213867f,
            .pos_z = 44.0f,
        },
        {
            .vid = 412,
            .offset_x = 141.928223f,
            .offset_y = -41.786133f,
            .pos_z = 34.0f,
        },
        {
            .vid = 2521,
            .offset_x = 98.928223f,
            .offset_y = 144.213867f,
            .pos_z = 34.0f,
        },
        {
            .vid = 2521,
            .offset_x = 53.928223f,
            .offset_y = 110.213867f,
            .pos_z = 34.0f,
            .direction = 64,
        },
        {
            .vid = 2521,
            .offset_x = -180.071777f,
            .offset_y = 138.213867f,
            .pos_z = 34.0f,
            .direction = 80,
        },
        {
            .vid = 2521,
            .offset_x = -136.071777f,
            .offset_y = 108.213867f,
            .pos_z = 34.0f,
            .direction = 48,
        },
        {
            .vid = 407,
            .offset_x = -56.071777f,
            .offset_y = 171.213867f,
            .pos_z = 34.0f,
            .direction = 213,
        },
        {
            .vid = 1756,
            .offset_x = 278.928223f,
            .offset_y = -56.786133f,
            .pos_z = 34.0f,
            .direction = 192,
        },
        {
            .vid = 1755,
            .offset_x = 110.928223f,
            .offset_y = 60.213867f,
            .pos_z = 34.0f,
            .direction = 64,
        },
        {
            .vid = 407,
            .offset_x = 238.928223f,
            .offset_y = -26.786133f,
            .pos_z = 34.0f,
            .direction = 85,
        },
        {
            .vid = 2521,
            .offset_x = -91.071777f,
            .offset_y = 76.213867f,
            .pos_z = 34.0f,
            .direction = 16,
        },
        {
            .vid = 2521,
            .offset_x = 8.928223f,
            .offset_y = 76.213867f,
            .pos_z = 34.0f,
            .direction = 96,
        },
        {
            .vid = 2521,
            .offset_x = -58.071777f,
            .offset_y = 41.213867f,
            .pos_z = 34.0f,
            .direction = 144,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Computer room imported from _prepare/AS2/decoration/05computers/comp07.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile COMP07_DECORATION_PROFILE = {
    .id = "comp07",
    .frame = {
        .corner_offsets = {{
            {           450.0f,          -512.0f },      // top
            {          -720.0f,           320.0f },      // left
            {          -450.0f,           512.0f },      // bottom
            {           720.0f,          -320.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2535,
            .offset_x = -214.601562f,
            .offset_y = 167.572222f,
            .direction = 192,
        },
        {
            .vid = 2535,
            .offset_x = 366.398438f,
            .offset_y = -151.427778f,
            .direction = 192,
        },
        {
            .vid = 131,
            .offset_x = -269.601562f,
            .offset_y = 352.572222f,
            .direction = 128,
        },
        {
            .vid = 131,
            .offset_x = -154.601562f,
            .offset_y = 151.572222f,
            .direction = 144,
        },
        {
            .vid = 131,
            .offset_x = -124.601562f,
            .offset_y = 172.572222f,
            .direction = 144,
        },
        {
            .vid = 131,
            .offset_x = -90.601562f,
            .offset_y = 197.572222f,
            .direction = 144,
        },
        {
            .vid = 131,
            .offset_x = -82.601562f,
            .offset_y = 219.572222f,
            .direction = 128,
        },
        {
            .vid = 131,
            .offset_x = -111.601562f,
            .offset_y = 243.572222f,
            .direction = 128,
        },
        {
            .vid = 131,
            .offset_x = -139.601562f,
            .offset_y = 262.572222f,
            .direction = 128,
        },
        {
            .vid = 131,
            .offset_x = -184.601562f,
            .offset_y = 128.572222f,
            .direction = 144,
        },
        {
            .vid = 131,
            .offset_x = -173.601562f,
            .offset_y = 284.572222f,
            .direction = 128,
        },
        {
            .vid = 131,
            .offset_x = -208.601562f,
            .offset_y = 310.572222f,
            .direction = 128,
        },
        {
            .vid = 131,
            .offset_x = -238.601562f,
            .offset_y = 332.572222f,
            .direction = 128,
        },
        {
            .vid = 131,
            .offset_x = 427.398438f,
            .offset_y = -126.427778f,
            .direction = 128,
        },
        {
            .vid = 131,
            .offset_x = 400.398438f,
            .offset_y = -107.427778f,
            .direction = 128,
        },
        {
            .vid = 131,
            .offset_x = 375.398438f,
            .offset_y = -88.427778f,
            .direction = 128,
        },
        {
            .vid = 131,
            .offset_x = 345.398438f,
            .offset_y = -67.427778f,
            .direction = 128,
        },
        {
            .vid = 131,
            .offset_x = 461.398438f,
            .offset_y = -149.427778f,
            .direction = 128,
        },
        {
            .vid = 131,
            .offset_x = 194.398438f,
            .offset_y = -107.427778f,
            .direction = 144,
        },
        {
            .vid = 131,
            .offset_x = 165.398438f,
            .offset_y = -129.427778f,
            .direction = 144,
        },
        {
            .vid = 131,
            .offset_x = 226.398438f,
            .offset_y = -83.427778f,
            .direction = 144,
        },
        {
            .vid = 131,
            .offset_x = 316.398438f,
            .offset_y = -45.427778f,
            .direction = 128,
        },
        {
            .vid = 131,
            .offset_x = 288.398438f,
            .offset_y = -42.427778f,
            .direction = 144,
        },
        {
            .vid = 131,
            .offset_x = 262.398438f,
            .offset_y = -59.427778f,
            .direction = 144,
        },
        {
            .vid = 2534,
            .offset_x = -274.601562f,
            .offset_y = 290.572222f,
            .direction = 192,
        },
        {
            .vid = 407,
            .offset_x = 270.398438f,
            .offset_y = -23.427778f,
            .direction = 170,
        },
        {
            .vid = 2534,
            .offset_x = 231.398438f,
            .offset_y = -144.427778f,
            .direction = 192,
        },
        {
            .vid = 2531,
            .offset_x = 27.398438f,
            .offset_y = 3.572222f,
            .direction = 192,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Computer room imported from _prepare/AS2/decoration/05computers/comp08.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile COMP08_DECORATION_PROFILE = {
    .id = "comp08",
    .frame = {
        .corner_offsets = {{
            {          -270.0f,          -320.0f },      // top
            {          -450.0f,          -192.0f },      // left
            {           270.0f,           320.0f },      // bottom
            {           450.0f,           192.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2515,
            .offset_x = 235.928009f,
            .offset_y = 195.390164f,
        },
        {
            .vid = 2515,
            .offset_x = 30.928009f,
            .offset_y = 48.390164f,
        },
        {
            .vid = 2515,
            .offset_x = -147.071991f,
            .offset_y = -77.609836f,
        },
        {
            .vid = 2536,
            .offset_x = 152.198516f,
            .offset_y = 82.627957f,
            .direction = 46,
        },
        {
            .vid = 2536,
            .offset_x = -184.801484f,
            .offset_y = -160.372043f,
            .direction = 80,
        },
        {
            .vid = 2536,
            .offset_x = -22.801484f,
            .offset_y = -46.372043f,
            .direction = 46,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET6_RANDOM,
};

/**
 * Computer room imported from _prepare/AS2/decoration/05computers/comp09.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile COMP09_DECORATION_PROFILE = {
    .id = "comp09",
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
            .vid = 2523,
            .offset_x = 147.058807f,
            .offset_y = -146.262012f,
            .direction = 32,
        },
        {
            .vid = 2540,
            .offset_x = -55.941193f,
            .offset_y = -92.262012f,
            .direction = 128,
        },
        {
            .vid = 2523,
            .offset_x = 4.058807f,
            .offset_y = -181.262012f,
            .direction = 80,
        },
        {
            .vid = 2523,
            .offset_x = 103.058807f,
            .offset_y = -176.262012f,
        },
        {
            .vid = 2523,
            .offset_x = 34.058807f,
            .offset_y = -214.262012f,
            .direction = 144,
        },
        {
            .vid = 407,
            .offset_x = 241.058807f,
            .offset_y = 23.737988f,
            .direction = 85,
        },
        {
            .vid = 1261,
            .offset_x = 302.058807f,
            .offset_y = -44.262012f,
            .direction = 128,
        },
        {
            .vid = 2521,
            .offset_x = -291.941193f,
            .offset_y = 8.737988f,
            .direction = 80,
        },
        {
            .vid = 2521,
            .offset_x = -245.941193f,
            .offset_y = -21.262012f,
            .direction = 112,
        },
        {
            .vid = 2521,
            .offset_x = -199.941193f,
            .offset_y = -53.262012f,
            .direction = 80,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET6_RANDOM,
};

} // namespace auto_mapper::core::decoration_builder
