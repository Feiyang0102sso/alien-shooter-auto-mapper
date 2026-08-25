/**
 * @file vid_armies_as2.h
 * @brief Default Army values for registered AS2 asset VIDs.
 */

#pragma once

#include "auto_mapper/core/vid_properties/vid_property_types.h"

#include <stdexcept>

namespace auto_mapper::core {

// Wall Set 1
inline constexpr VidArmy AS2_VID_1777_ARMY = 2;
inline constexpr VidArmy AS2_VID_1778_ARMY = 0;
inline constexpr VidArmy AS2_VID_1779_ARMY = 2;
inline constexpr VidArmy AS2_VID_1780_ARMY = 2;
inline constexpr VidArmy AS2_VID_1781_ARMY = 2;
inline constexpr VidArmy AS2_VID_1782_ARMY = 2;
inline constexpr VidArmy AS2_VID_1783_ARMY = 0;
inline constexpr VidArmy AS2_VID_1785_ARMY = 0;
inline constexpr VidArmy AS2_VID_1786_ARMY = 0;
inline constexpr VidArmy AS2_VID_1788_ARMY = 2;

// Wall Set 2
inline constexpr VidArmy AS2_VID_1700_ARMY = 2;
inline constexpr VidArmy AS2_VID_1701_ARMY = 2;
inline constexpr VidArmy AS2_VID_1702_ARMY = 2;
inline constexpr VidArmy AS2_VID_1703_ARMY = 2;
inline constexpr VidArmy AS2_VID_1724_ARMY = 0;

// Wall Set 3
inline constexpr VidArmy AS2_VID_1100_ARMY = 2;
inline constexpr VidArmy AS2_VID_1101_ARMY = 2;
inline constexpr VidArmy AS2_VID_1102_ARMY = 0;
inline constexpr VidArmy AS2_VID_1103_ARMY = 0;
inline constexpr VidArmy AS2_VID_1121_ARMY = 0;

// Wall Set 4
inline constexpr VidArmy AS2_VID_1130_ARMY = 2;
inline constexpr VidArmy AS2_VID_1131_ARMY = 2;
inline constexpr VidArmy AS2_VID_1132_ARMY = 0;

// Wall Set 5
inline constexpr VidArmy AS2_VID_631_ARMY = 2;
inline constexpr VidArmy AS2_VID_2500_ARMY = 2;
inline constexpr VidArmy AS2_VID_2501_ARMY = 2;
inline constexpr VidArmy AS2_VID_2502_ARMY = 2;
inline constexpr VidArmy AS2_VID_2503_ARMY = 0;
inline constexpr VidArmy AS2_VID_2504_ARMY = 2;
inline constexpr VidArmy AS2_VID_2505_ARMY = 0;
inline constexpr VidArmy AS2_VID_2506_ARMY = 0;

// Wall Set 6
inline constexpr VidArmy AS2_VID_2600_ARMY = 2;
inline constexpr VidArmy AS2_VID_2601_ARMY = 2;
inline constexpr VidArmy AS2_VID_2602_ARMY = 2;
inline constexpr VidArmy AS2_VID_2604_ARMY = 2;
inline constexpr VidArmy AS2_VID_2605_ARMY = 0;
inline constexpr VidArmy AS2_VID_2606_ARMY = 0;

// Wall Set 7
inline constexpr VidArmy AS2_VID_2620_ARMY = 2;
inline constexpr VidArmy AS2_VID_2621_ARMY = 2;
inline constexpr VidArmy AS2_VID_2622_ARMY = 2;
inline constexpr VidArmy AS2_VID_2624_ARMY = 2;
inline constexpr VidArmy AS2_VID_2643_ARMY = 0;
inline constexpr VidArmy AS2_VID_2645_ARMY = 2;

// Wall Set 8
inline constexpr VidArmy AS2_VID_2640_ARMY = 2;
inline constexpr VidArmy AS2_VID_2641_ARMY = 2;
inline constexpr VidArmy AS2_VID_2642_ARMY = 2;
inline constexpr VidArmy AS2_VID_2644_ARMY = 2;

// Wall Set 9
inline constexpr VidArmy AS2_VID_1720_ARMY = 2;
inline constexpr VidArmy AS2_VID_1721_ARMY = 2;
inline constexpr VidArmy AS2_VID_1731_ARMY = 0;
inline constexpr VidArmy AS2_VID_1732_ARMY = 0;

// Decoration assets
inline constexpr VidArmy AS2_VID_409_ARMY = 0;
inline constexpr VidArmy AS2_VID_412_ARMY = 0;
inline constexpr VidArmy AS2_VID_413_ARMY = 0;
inline constexpr VidArmy AS2_VID_1270_ARMY = 2;
inline constexpr VidArmy AS2_VID_1288_ARMY = 0;
inline constexpr VidArmy AS2_VID_1289_ARMY = 2;
inline constexpr VidArmy AS2_VID_1764_ARMY = 0;
inline constexpr VidArmy AS2_VID_2300_ARMY = 2;
inline constexpr VidArmy AS2_VID_2304_ARMY = 0;
inline constexpr VidArmy AS2_VID_2357_ARMY = 0;

/**
 * Return the game-defined Army value bound to a registered AS2 asset VID.
 *
 * Every supported asset is registered explicitly, including Army 0.
 * An unknown VID indicates that a profile was added without its Army binding.
 */
// why the walls should be set into army2?
// no one knows... to prevent problems... add them...
inline VidArmy get_as2_vid_army(int vid) {
    switch (vid) {
        // Wall Set 1
        case 1777: return AS2_VID_1777_ARMY;
        case 1778: return AS2_VID_1778_ARMY;
        case 1779: return AS2_VID_1779_ARMY;
        case 1780: return AS2_VID_1780_ARMY;
        case 1781: return AS2_VID_1781_ARMY;
        case 1782: return AS2_VID_1782_ARMY;
        case 1783: return AS2_VID_1783_ARMY;
        case 1785: return AS2_VID_1785_ARMY;
        case 1786: return AS2_VID_1786_ARMY;
        case 1788: return AS2_VID_1788_ARMY;

        // Wall Set 2
        case 1700: return AS2_VID_1700_ARMY;
        case 1701: return AS2_VID_1701_ARMY;
        case 1702: return AS2_VID_1702_ARMY;
        case 1703: return AS2_VID_1703_ARMY;
        case 1724: return AS2_VID_1724_ARMY;

        // Wall Set 3
        case 1100: return AS2_VID_1100_ARMY;
        case 1101: return AS2_VID_1101_ARMY;
        case 1102: return AS2_VID_1102_ARMY;
        case 1103: return AS2_VID_1103_ARMY;
        case 1121: return AS2_VID_1121_ARMY;

        // Wall Set 4
        case 1130: return AS2_VID_1130_ARMY;
        case 1131: return AS2_VID_1131_ARMY;
        case 1132: return AS2_VID_1132_ARMY;

        // Wall Set 5
        case 631: return AS2_VID_631_ARMY;
        case 2500: return AS2_VID_2500_ARMY;
        case 2501: return AS2_VID_2501_ARMY;
        case 2502: return AS2_VID_2502_ARMY;
        case 2503: return AS2_VID_2503_ARMY;
        case 2504: return AS2_VID_2504_ARMY;
        case 2505: return AS2_VID_2505_ARMY;
        case 2506: return AS2_VID_2506_ARMY;

        // Wall Set 6
        case 2600: return AS2_VID_2600_ARMY;
        case 2601: return AS2_VID_2601_ARMY;
        case 2602: return AS2_VID_2602_ARMY;
        case 2604: return AS2_VID_2604_ARMY;
        case 2605: return AS2_VID_2605_ARMY;
        case 2606: return AS2_VID_2606_ARMY;

        // Wall Set 7
        case 2620: return AS2_VID_2620_ARMY;
        case 2621: return AS2_VID_2621_ARMY;
        case 2622: return AS2_VID_2622_ARMY;
        case 2624: return AS2_VID_2624_ARMY;
        case 2643: return AS2_VID_2643_ARMY;
        case 2645: return AS2_VID_2645_ARMY;

        // Wall Set 8
        case 2640: return AS2_VID_2640_ARMY;
        case 2641: return AS2_VID_2641_ARMY;
        case 2642: return AS2_VID_2642_ARMY;
        case 2644: return AS2_VID_2644_ARMY;

        // Wall Set 9
        case 1720: return AS2_VID_1720_ARMY;
        case 1721: return AS2_VID_1721_ARMY;
        case 1731: return AS2_VID_1731_ARMY;
        case 1732: return AS2_VID_1732_ARMY;

        // Decoration assets
        case 409: return AS2_VID_409_ARMY;
        case 412: return AS2_VID_412_ARMY;
        case 413: return AS2_VID_413_ARMY;
        case 1270: return AS2_VID_1270_ARMY;
        case 1288: return AS2_VID_1288_ARMY;
        case 1289: return AS2_VID_1289_ARMY;
        case 1764: return AS2_VID_1764_ARMY;
        case 2300: return AS2_VID_2300_ARMY;
        case 2304: return AS2_VID_2304_ARMY;
        case 2357: return AS2_VID_2357_ARMY;
    }

    throw std::invalid_argument("AS2 asset VID has no Army binding");
}

} // namespace auto_mapper::core
