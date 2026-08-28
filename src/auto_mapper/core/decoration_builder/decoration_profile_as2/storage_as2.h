/**
 * @file storage_as2.h
 * @brief Authored AS2 storage room decoration stamp profiles.
 */

#pragma once

#include "auto_mapper/core/decoration_builder/decoration_profile_types.h"
#include "auto_mapper/core/wall_builder/wall_profiles_as2.h"

namespace auto_mapper::core::decoration_builder {

/**
 * Storage room imported from _prepare/AS2/decoration/03storage/storage01.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile STORAGE01_DECORATION_PROFILE = {
    .id = "storage01",
    .frame = {
        .corner_offsets = {{
            {        180.0f,       -448.0f },      // top
            {       -630.0f,        128.0f },      // left
            {       -180.0f,        448.0f },      // bottom
            {        630.0f,       -128.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 1747,
            .offset_x = -121.75f,
            .offset_y = 139.0f,
            .pos_z = 1.0f,
            .direction = 64,
        },
        {
            .vid = 1280,
            .offset_x = -374.75f,
            .offset_y = 134.0f,
            .direction = 128,
        },
        {
            .vid = 1280,
            .offset_x = -329.75f,
            .offset_y = 189.0f,
            .direction = 213,
        },
        {
            .vid = 1269,
            .offset_x = 359.25f,
            .offset_y = -107.0f,
            .direction = 128,
        },
        {
            .vid = 1269,
            .offset_x = 343.25f,
            .offset_y = -78.0f,
            .pos_z = 20.0f,
            .direction = 64,
        },
        {
            .vid = 1269,
            .offset_x = 328.25f,
            .offset_y = -76.0f,
            .direction = 192,
        },
        {
            .vid = 442,
            .offset_x = 497.25f,
            .offset_y = -76.0f,
            .direction = 102,
        },
        {
            .vid = 1269,
            .offset_x = 420.25f,
            .offset_y = -93.0f,
            .direction = 64,
        },
        {
            .vid = 1269,
            .offset_x = 345.25f,
            .offset_y = -50.0f,
            .direction = 64,
        },
        {
            .vid = 442,
            .offset_x = 350.25f,
            .offset_y = 34.0f,
            .direction = 0,
        },
        {
            .vid = 1269,
            .offset_x = 333.25f,
            .offset_y = -12.0f,
            .pos_z = 20.0f,
            .direction = 64,
        },
        {
            .vid = 1747,
            .offset_x = 171.25f,
            .offset_y = -65.0f,
            .pos_z = 1.0f,
            .direction = 64,
        },
        {
            .vid = 1269,
            .offset_x = 302.25f,
            .offset_y = -7.0f,
            .direction = 64,
        },
        {
            .vid = 1269,
            .offset_x = -155.75f,
            .offset_y = -72.0f,
            .direction = 192,
        },
        {
            .vid = 1747,
            .offset_x = -172.75f,
            .offset_y = -16.0f,
            .pos_z = 1.0f,
            .direction = 48,
        },
        {
            .vid = 1269,
            .offset_x = -121.75f,
            .offset_y = -110.0f,
            .direction = 64,
        },
        {
            .vid = 1747,
            .offset_x = -42.75f,
            .offset_y = -112.0f,
            .pos_z = 1.0f,
            .direction = 48,
        },
        {
            .vid = 1280,
            .offset_x = -421.75f,
            .offset_y = 100.0f,
            .direction = 85,
        },
        {
            .vid = 1754,
            .offset_x = -423.75f,
            .offset_y = 69.0f,
            .pos_z = 40.0f,
            .direction = 128,
        },
        {
            .vid = 1280,
            .offset_x = -366.75f,
            .offset_y = 80.0f,
            .direction = 170,
        },
        {
            .vid = 1269,
            .offset_x = -246.75f,
            .offset_y = -14.0f,
            .direction = 0,
        },
        {
            .vid = 1747,
            .offset_x = -300.75f,
            .offset_y = 70.0f,
            .pos_z = 1.0f,
            .direction = 43,
        },
        {
            .vid = 1280,
            .offset_x = 350.25f,
            .offset_y = -177.0f,
            .direction = 64,
        },
        {
            .vid = 442,
            .offset_x = 427.25f,
            .offset_y = -139.0f,
            .direction = 153,
        },
        {
            .vid = 1747,
            .offset_x = 102.25f,
            .offset_y = -206.0f,
            .pos_z = 1.0f,
            .direction = 48,
        },
        {
            .vid = 1763,
            .offset_x = 217.25f,
            .offset_y = -304.0f,
            .pos_z = 38.0f,
            .direction = 211,
        },
        {
            .vid = 1280,
            .offset_x = 315.25f,
            .offset_y = -232.0f,
            .direction = 85,
        },
        {
            .vid = 1764,
            .offset_x = 226.25f,
            .offset_y = -301.0f,
            .direction = 6,
        },
        {
            .vid = 1272,
            .offset_x = -115.75f,
            .offset_y = -104.0f,
            .pos_z = 20.0f,
            .direction = 128,
        },
        {
            .vid = 1289,
            .offset_x = 106.25f,
            .offset_y = -248.0f,
            .direction = 88,
        },
        {
            .vid = 1272,
            .offset_x = 38.25f,
            .offset_y = -218.0f,
            .direction = 128,
        },
        {
            .vid = 1272,
            .offset_x = 8.25f,
            .offset_y = -198.0f,
            .direction = 128,
        },
        {
            .vid = 1753,
            .offset_x = -16.75f,
            .offset_y = -221.0f,
            .pos_z = 40.0f,
            .direction = 128,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Storage room imported from _prepare/AS2/decoration/03storage/storage02.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile STORAGE02_DECORATION_PROFILE = {
    .id = "storage02",
    .frame = {
        .corner_offsets = {{
            {         90.0f,       -192.0f },      // top
            {       -270.0f,         64.0f },      // left
            {        -90.0f,        192.0f },      // bottom
            {        270.0f,        -64.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 442,
            .offset_x = 189.125f,
            .offset_y = -53.91111111111104f,
            .direction = 51,
        },
        {
            .vid = 428,
            .offset_x = -12.875f,
            .offset_y = -15.91111111111104f,
            .pos_z = 50.0f,
            .direction = 96,
            .gamma = {
                .r = -50,
                .g = -50,
                .b = -50,
                .a = 0,
            },
        },
        {
            .vid = 428,
            .offset_x = -45.875f,
            .offset_y = 2.0888888888889596f,
            .pos_z = 70.0f,
            .direction = 16,
            .gamma = {
                .r = -50,
                .g = -50,
                .b = -50,
                .a = 0,
            },
        },
        {
            .vid = 428,
            .offset_x = -32.875f,
            .offset_y = -8.91111111111104f,
            .pos_z = 70.0f,
            .direction = 48,
            .gamma = {
                .r = -50,
                .g = -50,
                .b = -50,
                .a = 0,
            },
        },
        {
            .vid = 1764,
            .offset_x = 34.125f,
            .offset_y = -53.91111111111104f,
            .direction = 89,
        },
        {
            .vid = 442,
            .offset_x = 117.125f,
            .offset_y = -96.91111111111104f,
            .direction = 0,
        },
        {
            .vid = 1747,
            .offset_x = -20.875f,
            .offset_y = -10.91111111111104f,
            .direction = 64,
        },
        {
            .vid = 1747,
            .offset_x = -88.875f,
            .offset_y = 36.08888888888896f,
            .direction = 0,
        },
        {
            .vid = 1764,
            .offset_x = -133.875f,
            .offset_y = 69.08888888888896f,
            .direction = 76,
        },
        {
            .vid = 1764,
            .offset_x = -156.875f,
            .offset_y = 83.08888888888896f,
            .direction = 76,
        },
        {
            .vid = 1747,
            .offset_x = -142.875f,
            .offset_y = 113.08888888888896f,
            .direction = 48,
        },
        {
            .vid = 442,
            .offset_x = 142.125f,
            .offset_y = -85.91111111111104f,
            .pos_z = 51.0f,
            .direction = 204,
        },
        {
            .vid = 1305,
            .offset_x = 255.125f,
            .offset_y = 53.08888888888896f,
            .pos_z = 81.0f,
            .direction = 34,
        },
        {
            .vid = 1305,
            .offset_x = 246.125f,
            .offset_y = 59.08888888888896f,
            .pos_z = 81.0f,
            .direction = 0,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Storage room imported from _prepare/AS2/decoration/03storage/storage03.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile STORAGE03_DECORATION_PROFILE = {
    .id = "storage03",
    .frame = {
        .corner_offsets = {{
            {       -135.0f,       -160.0f },      // top
            {       -225.0f,        -96.0f },      // left
            {        135.0f,        160.0f },      // bottom
            {        225.0f,         96.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 1764,
            .offset_x = 170.5f,
            .offset_y = 90.0f,
            .direction = 57,
        },
        {
            .vid = 1269,
            .offset_x = -5.5f,
            .offset_y = -28.0f,
            .pos_z = 20.0f,
            .direction = 128,
        },
        {
            .vid = 1269,
            .offset_x = -64.5f,
            .offset_y = -60.0f,
            .pos_z = 20.0f,
            .direction = 128,
        },
        {
            .vid = 1269,
            .offset_x = -89.5f,
            .offset_y = -77.0f,
            .direction = 64,
        },
        {
            .vid = 1269,
            .offset_x = -54.5f,
            .offset_y = -50.0f,
            .direction = 192,
        },
        {
            .vid = 1269,
            .offset_x = -1.5f,
            .offset_y = -25.0f,
            .direction = 128,
        },
        {
            .vid = 1747,
            .offset_x = 71.5f,
            .offset_y = 22.0f,
            .direction = 48,
        },
        {
            .vid = 1740,
            .offset_x = 133.5f,
            .offset_y = 63.0f,
            .direction = 128,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET1_RANDOM,
};

/**
 * Storage room imported from _prepare/AS2/decoration/03storage/storage04.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile STORAGE04_DECORATION_PROFILE = {
    .id = "storage04",
    .frame = {
        .corner_offsets = {{
            {        -90.0f,       -320.0f },      // top
            {       -450.0f,        -64.0f },      // left
            {         90.0f,        320.0f },      // bottom
            {        450.0f,         64.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 442,
            .offset_x = 104.50000934799516f,
            .offset_y = 207.99998542644363f,
            .direction = 51,
        },
        {
            .vid = 442,
            .offset_x = 170.50000934799516f,
            .offset_y = 191.99998542644363f,
            .direction = 51,
        },
        {
            .vid = 1282,
            .offset_x = 290.50000934799516f,
            .offset_y = 164.99998542644363f,
            .pos_z = 5.0f,
            .direction = 142,
        },
        {
            .vid = 2357,
            .offset_x = 179.50000934799516f,
            .offset_y = -57.999984055978246f,
            .pos_z = 5.0f,
            .direction = 96,
        },
        {
            .vid = 2357,
            .offset_x = 225.50000934799516f,
            .offset_y = -5.999984055978246f,
            .pos_z = 5.0f,
            .direction = 108,
        },
        {
            .vid = 2357,
            .offset_x = 207.50000934799516f,
            .offset_y = -40.999984055978246f,
            .pos_z = 5.0f,
            .direction = 96,
        },
        {
            .vid = 1236,
            .offset_x = 72.55951862533891f,
            .offset_y = -107.05027702472825f,
            .pos_z = 1.0f,
            .direction = 102,
        },
        {
            .vid = 1236,
            .offset_x = -52.49999065200484f,
            .offset_y = -184.00001457355637f,
            .pos_z = 1.0f,
            .direction = 0,
        },
        {
            .vid = 1236,
            .offset_x = -131.49999065200484f,
            .offset_y = -125.99998405597825f,
            .pos_z = 1.0f,
            .direction = 0,
        },
        {
            .vid = 1236,
            .offset_x = -281.5303251246611f,
            .offset_y = -21.999984055978246f,
            .pos_z = 0.5876846313476562f,
            .direction = 0,
        },
        {
            .vid = 1236,
            .offset_x = -190.49999065200484f,
            .offset_y = -31.999984055978246f,
            .pos_z = 1.0f,
            .direction = 204,
        },
        {
            .vid = 1766,
            .offset_x = 47.50000934799516f,
            .offset_y = -235.9999993147673f,
            .pos_z = 1.0f,
            .direction = 176,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET4_RANDOM,
};

/**
 * Storage room imported from _prepare/AS2/decoration/03storage/storage05.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile STORAGE05_DECORATION_PROFILE = {
    .id = "storage05",
    .frame = {
        .corner_offsets = {{
            {       -180.0f,       -448.0f },      // top
            {       -630.0f,       -128.0f },      // left
            {        180.0f,        448.0f },      // bottom
            {        630.0f,        128.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2316,
            .offset_x = 421.682208239843f,
            .offset_y = 142.6543185850005f,
            .direction = 176,
        },
        {
            .vid = 2357,
            .offset_x = 393.682208239843f,
            .offset_y = 258.6543185850005f,
            .direction = 57,
        },
        {
            .vid = 2357,
            .offset_x = 232.68220823984302f,
            .offset_y = 227.6543185850005f,
            .direction = 38,
        },
        {
            .vid = 2357,
            .offset_x = 289.680255114843f,
            .offset_y = 287.6543185850005f,
            .direction = 51,
        },
        {
            .vid = 2357,
            .offset_x = 176.68220823984302f,
            .offset_y = 321.6543185850005f,
            .direction = 38,
        },
        {
            .vid = 2357,
            .offset_x = -33.31779176015698f,
            .offset_y = 188.6543185850005f,
            .pos_z = 1.0f,
            .direction = 76,
        },
        {
            .vid = 2357,
            .offset_x = 104.68220823984302f,
            .offset_y = 239.6543185850005f,
            .pos_z = 1.0f,
            .direction = 89,
        },
        {
            .vid = 2357,
            .offset_x = 116.68025511484302f,
            .offset_y = 259.6543185850005f,
            .direction = 32,
        },
        {
            .vid = 2316,
            .offset_x = 413.682208239843f,
            .offset_y = 121.65431858500051f,
            .pos_z = 49.0f,
            .direction = 128,
        },
        {
            .vid = 2316,
            .offset_x = 407.682208239843f,
            .offset_y = 86.65431858500051f,
            .direction = 160,
        },
        {
            .vid = 2316,
            .offset_x = 452.682208239843f,
            .offset_y = 90.65431858500051f,
            .pos_z = 67.0f,
            .direction = 96,
        },
        {
            .vid = 2316,
            .offset_x = 438.682208239843f,
            .offset_y = 133.6543185850005f,
            .pos_z = 49.0f,
            .direction = 160,
        },
        {
            .vid = 2319,
            .offset_x = 443.682208239843f,
            .offset_y = 105.65431858500051f,
            .direction = 32,
        },
        {
            .vid = 2316,
            .offset_x = 439.682208239843f,
            .offset_y = 120.65431858500051f,
            .pos_z = 49.0f,
            .direction = 128,
        },
        {
            .vid = 2316,
            .offset_x = 457.682208239843f,
            .offset_y = 140.6543185850005f,
            .pos_z = 49.0f,
            .direction = 144,
        },
        {
            .vid = 2318,
            .offset_x = 213.68220823984302f,
            .offset_y = -30.34568141499949f,
            .direction = 0,
        },
        {
            .vid = 1747,
            .offset_x = 159.68025511484302f,
            .offset_y = -81.34568141499949f,
            .direction = 48,
        },
        {
            .vid = 2318,
            .offset_x = 231.68220823984302f,
            .offset_y = -13.34568141499949f,
            .pos_z = 40.0f,
            .direction = 48,
        },
        {
            .vid = 2318,
            .offset_x = 254.68416136484302f,
            .offset_y = -6.34568141499949f,
            .direction = 16,
        },
        {
            .vid = 2316,
            .offset_x = 257.682208239843f,
            .offset_y = 73.65431858500051f,
            .direction = 112,
        },
        {
            .vid = 2316,
            .offset_x = 227.68220823984302f,
            .offset_y = 17.65431858500051f,
            .direction = 64,
        },
        {
            .vid = 2316,
            .offset_x = 245.68416136484302f,
            .offset_y = 22.65431858500051f,
            .direction = 80,
        },
        {
            .vid = 2316,
            .offset_x = 354.684161364843f,
            .offset_y = 107.65431858500051f,
            .direction = 48,
        },
        {
            .vid = 2316,
            .offset_x = 371.684161364843f,
            .offset_y = 79.65431858500051f,
            .direction = 32,
        },
        {
            .vid = 2318,
            .offset_x = 337.682208239843f,
            .offset_y = 54.65431858500051f,
            .direction = 80,
        },
        {
            .vid = 2318,
            .offset_x = 297.680255114843f,
            .offset_y = 32.65431858500051f,
            .direction = 64,
        },
        {
            .vid = 2357,
            .offset_x = -4.317791760156979f,
            .offset_y = 134.6543185850005f,
            .direction = 51,
        },
        {
            .vid = 2357,
            .offset_x = -80.31779176015698f,
            .offset_y = 59.65431858500051f,
            .direction = 44,
        },
        {
            .vid = 2357,
            .offset_x = -377.317791760157f,
            .offset_y = -101.34568141499949f,
            .direction = 25,
        },
        {
            .vid = 2357,
            .offset_x = -168.31779176015698f,
            .offset_y = 11.65431858500051f,
            .direction = 32,
        },
        {
            .vid = 2316,
            .offset_x = 134.68025511484302f,
            .offset_y = -87.34568141499949f,
            .pos_z = 69.0f,
            .direction = 128,
        },
        {
            .vid = 2316,
            .offset_x = 141.68025511484302f,
            .offset_y = -89.34568141499949f,
            .pos_z = 61.0f,
            .direction = 144,
        },
        {
            .vid = 2316,
            .offset_x = 179.68220823984302f,
            .offset_y = -73.34568141499949f,
            .pos_z = 50.0f,
            .direction = 160,
        },
        {
            .vid = 2316,
            .offset_x = 173.68220823984302f,
            .offset_y = -94.34568141499949f,
            .pos_z = 28.0f,
            .direction = 96,
        },
        {
            .vid = 1764,
            .offset_x = -71.31779176015698f,
            .offset_y = -263.3456814149995f,
            .direction = 6,
        },
        {
            .vid = 1220,
            .offset_x = -48.31779176015698f,
            .offset_y = -245.3456814149995f,
            .direction = 185,
        },
        {
            .vid = 408,
            .offset_x = -58.31779176015698f,
            .offset_y = -172.3456814149995f,
            .direction = 128,
        },
        {
            .vid = 2318,
            .offset_x = -23.31779176015698f,
            .offset_y = -117.34568141499949f,
            .direction = 80,
        },
        {
            .vid = 2318,
            .offset_x = -16.31779176015698f,
            .offset_y = -147.3456814149995f,
            .direction = 64,
        },
        {
            .vid = 1220,
            .offset_x = -25.31779176015698f,
            .offset_y = -219.3456814149995f,
            .direction = 139,
        },
        {
            .vid = 2316,
            .offset_x = 19.68220823984302f,
            .offset_y = -125.34568141499949f,
            .pos_z = 30.0f,
            .direction = 128,
        },
        {
            .vid = 2318,
            .offset_x = 22.68220823984302f,
            .offset_y = -128.3456814149995f,
            .direction = 80,
        },
        {
            .vid = 2318,
            .offset_x = 8.682208239843021f,
            .offset_y = -184.3456814149995f,
            .direction = 48,
        },
        {
            .vid = 2316,
            .offset_x = 68.68220823984302f,
            .offset_y = -146.3456814149995f,
            .pos_z = 25.0f,
            .direction = 64,
        },
        {
            .vid = 2316,
            .offset_x = 75.68220823984302f,
            .offset_y = -158.3456814149995f,
            .pos_z = 53.0f,
            .direction = 32,
        },
        {
            .vid = 1747,
            .offset_x = 77.68220823984302f,
            .offset_y = -139.3456814149995f,
            .direction = 48,
        },
        {
            .vid = 2357,
            .offset_x = -273.317791760157f,
            .offset_y = -270.3456814149995f,
            .pos_z = -3.0f,
            .direction = 121,
        },
        {
            .vid = 2316,
            .offset_x = -252.31779176015698f,
            .offset_y = -285.3456814149995f,
            .pos_z = 17.0f,
            .direction = 48,
        },
        {
            .vid = 2316,
            .offset_x = -235.31779176015698f,
            .offset_y = -285.3456814149995f,
            .pos_z = 23.0f,
            .direction = 80,
        },
        {
            .vid = 2357,
            .offset_x = -213.31779176015698f,
            .offset_y = -274.3456814149995f,
            .pos_z = -3.0f,
            .direction = 108,
        },
        {
            .vid = 2357,
            .offset_x = -213.31779176015698f,
            .offset_y = -307.3456814149995f,
            .pos_z = -23.0f,
            .direction = 21,
        },
        {
            .vid = 2316,
            .offset_x = -200.31779176015698f,
            .offset_y = -313.3456814149995f,
            .pos_z = 23.0f,
            .direction = 128,
        },
        {
            .vid = 1747,
            .offset_x = -407.317791760157f,
            .offset_y = -166.3456814149995f,
            .direction = 32,
        },
        {
            .vid = 1765,
            .offset_x = -43.31779176015698f,
            .offset_y = -224.3456814149995f,
            .pos_z = 43.0f,
            .direction = 19,
        },
        {
            .vid = 1221,
            .offset_x = 157.67830198984302f,
            .offset_y = -37.34568141499949f,
            .pos_z = 2.0f,
            .direction = 208,
        },
        {
            .vid = 1638,
            .offset_x = 157.68220823984302f,
            .offset_y = -87.34568141499949f,
            .pos_z = 45.0f,
            .direction = 192,
            .gamma = {
                .r = 0,
                .g = -50,
                .b = 0,
                .a = 0,
            },
        },
        {
            .vid = 1221,
            .offset_x = -39.31779176015698f,
            .offset_y = -176.3456814149995f,
            .direction = 139,
        },
        {
            .vid = 1760,
            .offset_x = -68.31779176015698f,
            .offset_y = -254.3456814149995f,
            .pos_z = 40.0f,
            .direction = 28,
        },
        {
            .vid = 1760,
            .offset_x = -31.31779176015698f,
            .offset_y = -242.3456814149995f,
            .pos_z = 40.0f,
            .direction = 182,
        },
        {
            .vid = 1760,
            .offset_x = -85.31779176015698f,
            .offset_y = -245.3456814149995f,
            .pos_z = 41.0f,
            .direction = 54,
        },
        {
            .vid = 1760,
            .offset_x = -90.31779176015698f,
            .offset_y = -266.3456814149995f,
            .pos_z = 40.0f,
            .direction = 156,
        },
        {
            .vid = 1760,
            .offset_x = -68.31779176015698f,
            .offset_y = -251.3456814149995f,
            .pos_z = 51.0f,
            .direction = 3,
        },
        {
            .vid = 1289,
            .offset_x = -95.31779176015698f,
            .offset_y = -214.3456814149995f,
            .direction = 40,
        },
        {
            .vid = 401,
            .offset_x = -159.31779176015698f,
            .offset_y = -316.3456814149995f,
            .direction = 0,
        },
        {
            .vid = 401,
            .offset_x = -355.317791760157f,
            .offset_y = -160.3456814149995f,
            .direction = 42,
        },
        {
            .vid = 401,
            .offset_x = -341.317791760157f,
            .offset_y = -185.3456814149995f,
            .direction = 0,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET5_RANDOM,
};

/**
 * Storage room imported from _prepare/AS2/decoration/03storage/storage06.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile STORAGE06_DECORATION_PROFILE = {
    .id = "storage06",
    .frame = {
        .corner_offsets = {{
            {        -90.0f,       -192.0f },      // top
            {       -270.0f,        -64.0f },      // left
            {         90.0f,        192.0f },      // bottom
            {        270.0f,         64.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 428,
            .offset_x = -37.056396484375f,
            .offset_y = 10.57251044379342f,
            .pos_z = 71.0f,
            .direction = 165,
        },
        {
            .vid = 428,
            .offset_x = -68.056396484375f,
            .offset_y = -55.42748955620658f,
            .pos_z = 30.0f,
            .direction = 203,
        },
        {
            .vid = 428,
            .offset_x = -24.056396484375f,
            .offset_y = 5.572510443793419f,
            .pos_z = 50.0f,
            .direction = 11,
        },
        {
            .vid = 428,
            .offset_x = 66.943603515625f,
            .offset_y = 52.57251044379342f,
            .pos_z = 30.0f,
            .direction = 40,
        },
        {
            .vid = 428,
            .offset_x = 38.943603515625f,
            .offset_y = 65.57251044379342f,
            .pos_z = 30.0f,
            .direction = 26,
        },
        {
            .vid = 428,
            .offset_x = 132.943603515625f,
            .offset_y = 105.38385077582467f,
            .pos_z = 51.0f,
            .direction = 235,
        },
        {
            .vid = 428,
            .offset_x = 110.943603515625f,
            .offset_y = 125.38394232855904f,
            .pos_z = 51.0f,
            .direction = 61,
        },
        {
            .vid = 428,
            .offset_x = 70.943603515625f,
            .offset_y = 40.57251044379342f,
            .pos_z = 71.0f,
            .direction = 237,
        },
        {
            .vid = 428,
            .offset_x = 46.943603515625f,
            .offset_y = 61.57251044379342f,
            .pos_z = 71.0f,
            .direction = 143,
        },
        {
            .vid = 428,
            .offset_x = -17.056396484375f,
            .offset_y = -5.427489556206581f,
            .pos_z = 71.0f,
            .direction = 18,
        },
        {
            .vid = 1747,
            .offset_x = 56.943603515625f,
            .offset_y = 50.57251044379342f,
            .direction = 64,
        },
        {
            .vid = 1747,
            .offset_x = -17.056396484375f,
            .offset_y = -5.427489556206581f,
            .direction = 64,
        },
        {
            .vid = 428,
            .offset_x = 128.943603515625f,
            .offset_y = 111.38394232855904f,
            .pos_z = 30.0f,
            .direction = 148,
        },
        {
            .vid = 1747,
            .offset_x = 125.943603515625f,
            .offset_y = 105.38385077582467f,
            .direction = 64,
        },
        {
            .vid = 428,
            .offset_x = -91.056396484375f,
            .offset_y = -41.42748955620658f,
            .pos_z = 30.0f,
            .direction = 80,
        },
        {
            .vid = 1747,
            .offset_x = -81.056396484375f,
            .offset_y = -58.42748955620658f,
            .direction = 64,
        },
        {
            .vid = 428,
            .offset_x = -81.056396484375f,
            .offset_y = -60.42748955620658f,
            .pos_z = 71.0f,
            .direction = 16,
        },
        {
            .vid = 428,
            .offset_x = -104.056396484375f,
            .offset_y = -47.42748955620658f,
            .pos_z = 71.0f,
            .direction = 2,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET5_RANDOM,
};

/**
 * Storage room imported from _prepare/AS2/decoration/03storage/storage07.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile STORAGE07_DECORATION_PROFILE = {
    .id = "storage07",
    .frame = {
        .corner_offsets = {{
            {        315.0f,       -352.0f },      // top
            {       -495.0f,        224.0f },      // left
            {       -315.0f,        352.0f },      // bottom
            {        495.0f,       -224.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2317,
            .offset_x = -118.75f,
            .offset_y = 121.0f,
            .pos_z = 30.0f,
            .direction = 34,
        },
        {
            .vid = 1747,
            .offset_x = -116.75f,
            .offset_y = 126.0f,
            .direction = 80,
        },
        {
            .vid = 2527,
            .offset_x = -222.75f,
            .offset_y = 244.0f,
            .direction = 196,
        },
        {
            .vid = 2527,
            .offset_x = -243.75f,
            .offset_y = 202.0f,
            .direction = 128,
        },
        {
            .vid = 2318,
            .offset_x = -139.75f,
            .offset_y = 143.0f,
            .direction = 176,
        },
        {
            .vid = 1747,
            .offset_x = -351.75f,
            .offset_y = 216.0f,
            .pos_z = 30.0f,
            .direction = 0,
        },
        {
            .vid = 2319,
            .offset_x = 298.25f,
            .offset_y = -177.0f,
            .direction = 160,
        },
        {
            .vid = 1747,
            .offset_x = 287.25f,
            .offset_y = -163.0f,
            .direction = 80,
        },
        {
            .vid = 2317,
            .offset_x = 180.25f,
            .offset_y = -60.0f,
            .pos_z = 30.0f,
            .direction = 51,
        },
        {
            .vid = 2317,
            .offset_x = 168.25f,
            .offset_y = -69.0f,
            .pos_z = 20.0f,
            .direction = 85,
        },
        {
            .vid = 2527,
            .offset_x = 118.25f,
            .offset_y = -10.0f,
            .direction = 8,
        },
        {
            .vid = 2318,
            .offset_x = 106.25f,
            .offset_y = -48.0f,
            .direction = 96,
        },
        {
            .vid = 2317,
            .offset_x = 0.25f,
            .offset_y = 9.0f,
            .pos_z = 59.0f,
            .direction = 76,
        },
        {
            .vid = 2317,
            .offset_x = -4.75f,
            .offset_y = 11.0f,
            .pos_z = 20.0f,
            .direction = 145,
        },
        {
            .vid = 1747,
            .offset_x = 159.25f,
            .offset_y = -69.0f,
            .direction = 80,
        },
        {
            .vid = 2318,
            .offset_x = 3.25f,
            .offset_y = 14.0f,
            .direction = 128,
        },
        {
            .vid = 1747,
            .offset_x = 1.25f,
            .offset_y = 12.0f,
            .direction = 64,
        },
        {
            .vid = 2527,
            .offset_x = 360.25f,
            .offset_y = -196.0f,
            .direction = 153,
        },
        {
            .vid = 2319,
            .offset_x = 293.25f,
            .offset_y = -172.0f,
            .pos_z = 66.0f,
            .direction = 160,
        },
        {
            .vid = 1607,
            .offset_x = -83.75f,
            .offset_y = 94.0f,
            .pos_z = 68.0f,
            .direction = 192,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET8_RANDOM,
};

/**
 * Storage room imported from _prepare/AS2/decoration/03storage/storage08.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile STORAGE08_DECORATION_PROFILE = {
    .id = "storage08",
    .frame = {
        .corner_offsets = {{
            {        135.0f,       -288.0f },      // top
            {       -405.0f,         96.0f },      // left
            {       -135.0f,        288.0f },      // bottom
            {        405.0f,        -96.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 442,
            .offset_x = -46.4609375f,
            .offset_y = 148.85000000000002f,
            .direction = 12,
        },
        {
            .vid = 2318,
            .offset_x = -25.4609375f,
            .offset_y = 196.85000000000002f,
            .direction = 131,
        },
        {
            .vid = 442,
            .offset_x = -143.4609375f,
            .offset_y = 69.85000000000002f,
            .direction = 64,
        },
        {
            .vid = 2318,
            .offset_x = 327.5390625f,
            .offset_y = -22.149999999999977f,
            .direction = 151,
        },
        {
            .vid = 2318,
            .offset_x = 237.5390625f,
            .offset_y = -26.149999999999977f,
            .direction = 177,
        },
        {
            .vid = 2318,
            .offset_x = 92.5390625f,
            .offset_y = -48.14999999999998f,
            .direction = 136,
        },
        {
            .vid = 2318,
            .offset_x = 260.5390625f,
            .offset_y = -40.14999999999998f,
            .direction = 87,
        },
        {
            .vid = 442,
            .offset_x = 186.5390625f,
            .offset_y = -65.14999999999998f,
            .direction = 12,
        },
        {
            .vid = 442,
            .offset_x = 106.5390625f,
            .offset_y = -127.14999999999998f,
            .direction = 12,
        },
        {
            .vid = 1282,
            .offset_x = -16.4609375f,
            .offset_y = -41.14999999999998f,
            .direction = 64,
        },
        {
            .vid = 2318,
            .offset_x = 49.5390625f,
            .offset_y = -15.149999999999977f,
            .direction = 95,
        },
        {
            .vid = 2318,
            .offset_x = -81.4609375f,
            .offset_y = 34.85000000000002f,
            .direction = 18,
        },
        {
            .vid = 2318,
            .offset_x = 1.5390625f,
            .offset_y = 8.850000000000023f,
            .direction = 223,
        },
        {
            .vid = 401,
            .offset_x = 4.5390625f,
            .offset_y = 74.85000000000002f,
            .direction = 18,
        },
        {
            .vid = 401,
            .offset_x = 63.5390625f,
            .offset_y = 129.85000000000002f,
            .direction = 158,
        },
        {
            .vid = 401,
            .offset_x = 188.5390625f,
            .offset_y = 21.850000000000023f,
            .direction = 124,
        },
        {
            .vid = 401,
            .offset_x = 36.5390625f,
            .offset_y = 36.85000000000002f,
            .direction = 52,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET6_RANDOM,
};

/**
 * Storage room imported from _prepare/AS2/decoration/03storage/storage09.decoration.json.
 * Reference walls are intentionally absent from the member list.
 */
static const DecorationProfile STORAGE09_DECORATION_PROFILE = {
    .id = "storage09",
    .frame = {
        .corner_offsets = {{
            {        -90.0f,       -320.0f },      // top
            {       -450.0f,        -64.0f },      // left
            {         90.0f,        320.0f },      // bottom
            {        450.0f,         64.0f },      // right
        }},
    },
    .members = {
        {
            .vid = 2357,
            .offset_x = 62.63900184631348f,
            .offset_y = 201.13217909071182f,
            .direction = 33,
        },
        {
            .vid = 2357,
            .offset_x = 135.63900184631348f,
            .offset_y = -60.867820909288184f,
            .direction = 27,
        },
        {
            .vid = 2357,
            .offset_x = 60.63900184631348f,
            .offset_y = -37.867820909288184f,
            .direction = 96,
        },
        {
            .vid = 2357,
            .offset_x = 79.63900184631348f,
            .offset_y = -82.86782090928818f,
            .pos_z = 64.0f,
            .direction = 26,
        },
        {
            .vid = 2357,
            .offset_x = 240.63900184631348f,
            .offset_y = -0.8678209092881843f,
            .pos_z = 20.0f,
            .direction = 125,
        },
        {
            .vid = 2357,
            .offset_x = 235.63900184631348f,
            .offset_y = 3.1321790907118157f,
            .direction = 128,
        },
        {
            .vid = 2357,
            .offset_x = -94.36099815368652f,
            .offset_y = -124.86782090928818f,
            .direction = 97,
        },
        {
            .vid = 2357,
            .offset_x = -57.36099815368652f,
            .offset_y = -79.86782090928818f,
            .direction = 97,
        },
        {
            .vid = 2357,
            .offset_x = 1.6390018463134766f,
            .offset_y = -124.86782090928818f,
            .direction = 31,
        },
        {
            .vid = 2357,
            .offset_x = 21.639001846313477f,
            .offset_y = -64.86782090928818f,
            .direction = 97,
        },
        {
            .vid = 2357,
            .offset_x = -36.36099815368652f,
            .offset_y = -37.867820909288184f,
            .direction = 108,
        },
        {
            .vid = 2357,
            .offset_x = -136.36099815368652f,
            .offset_y = -41.867820909288184f,
            .direction = 42,
        },
        {
            .vid = 401,
            .offset_x = -175.36099815368652f,
            .offset_y = 31.132179090711816f,
            .direction = 56,
        },
        {
            .vid = 401,
            .offset_x = 13.639001846313477f,
            .offset_y = -18.867820909288184f,
            .direction = 191,
        },
    },
    .test_boundary_wall_profile = &WALL_AS2_SET5_RANDOM,
};

} // namespace auto_mapper::core::decoration_builder
