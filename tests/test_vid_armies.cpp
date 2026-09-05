#include <gtest/gtest.h>

#include "auto_mapper/core/vid_properties/vid_armies_as2.h"

#include <set>

using namespace auto_mapper::core;

namespace {

struct ExpectedArmyBinding {
    int vid;
    VidArmy constant_value;
    VidArmy expected_value;
};

} // namespace

TEST(VidArmiesTest, AS2AssetsHaveCompleteUniqueBindings) {
    const ExpectedArmyBinding expected_bindings[] = {
        // Wall Set 1
        {1777, AS2_VID_1777_ARMY, 2},
        {1778, AS2_VID_1778_ARMY, 0},
        {1779, AS2_VID_1779_ARMY, 2},
        {1780, AS2_VID_1780_ARMY, 2},
        {1781, AS2_VID_1781_ARMY, 2},
        {1782, AS2_VID_1782_ARMY, 2},
        {1783, AS2_VID_1783_ARMY, 0},
        {1785, AS2_VID_1785_ARMY, 0},
        {1786, AS2_VID_1786_ARMY, 0},
        {1788, AS2_VID_1788_ARMY, 2},

        // Wall Set 2
        {1700, AS2_VID_1700_ARMY, 2},
        {1701, AS2_VID_1701_ARMY, 2},
        {1702, AS2_VID_1702_ARMY, 2},
        {1703, AS2_VID_1703_ARMY, 2},
        {1724, AS2_VID_1724_ARMY, 0},

        // Wall Set 3
        {1100, AS2_VID_1100_ARMY, 2},
        {1101, AS2_VID_1101_ARMY, 2},
        {1102, AS2_VID_1102_ARMY, 0},
        {1103, AS2_VID_1103_ARMY, 0},
        {1121, AS2_VID_1121_ARMY, 0},

        // Wall Set 4
        {1130, AS2_VID_1130_ARMY, 2},
        {1131, AS2_VID_1131_ARMY, 2},
        {1132, AS2_VID_1132_ARMY, 0},

        // Wall Set 5
        {631, AS2_VID_631_ARMY, 2},
        {2500, AS2_VID_2500_ARMY, 2},
        {2501, AS2_VID_2501_ARMY, 2},
        {2502, AS2_VID_2502_ARMY, 2},
        {2503, AS2_VID_2503_ARMY, 0},
        {2504, AS2_VID_2504_ARMY, 2},
        {2505, AS2_VID_2505_ARMY, 0},
        {2506, AS2_VID_2506_ARMY, 0},

        // Wall Set 6
        {2600, AS2_VID_2600_ARMY, 2},
        {2601, AS2_VID_2601_ARMY, 2},
        {2602, AS2_VID_2602_ARMY, 2},
        {2604, AS2_VID_2604_ARMY, 2},
        {2605, AS2_VID_2605_ARMY, 0},
        {2606, AS2_VID_2606_ARMY, 0},

        // Wall Set 7
        {2620, AS2_VID_2620_ARMY, 2},
        {2621, AS2_VID_2621_ARMY, 2},
        {2622, AS2_VID_2622_ARMY, 2},
        {2624, AS2_VID_2624_ARMY, 2},
        {2643, AS2_VID_2643_ARMY, 0},
        {2645, AS2_VID_2645_ARMY, 2},

        // Wall Set 8
        {2640, AS2_VID_2640_ARMY, 2},
        {2641, AS2_VID_2641_ARMY, 2},
        {2642, AS2_VID_2642_ARMY, 2},
        {2644, AS2_VID_2644_ARMY, 2},

        // Wall Set 9
        {1720, AS2_VID_1720_ARMY, 2},
        {1721, AS2_VID_1721_ARMY, 2},
        {1731, AS2_VID_1731_ARMY, 0},
        {1732, AS2_VID_1732_ARMY, 0},

        // Decoration assets
        {407, AS2_VID_407_ARMY, 2},
        {408, AS2_VID_408_ARMY, 0},
        {409, AS2_VID_409_ARMY, 0},
        {410, AS2_VID_410_ARMY, 2},
        {412, AS2_VID_412_ARMY, 0},
        {413, AS2_VID_413_ARMY, 0},
        {428, AS2_VID_428_ARMY, 0},
        {1227, AS2_VID_1227_ARMY, 2},
        {1270, AS2_VID_1270_ARMY, 2},
        {1283, AS2_VID_1283_ARMY, 0},
        {1288, AS2_VID_1288_ARMY, 0},
        {1289, AS2_VID_1289_ARMY, 2},
        {1747, AS2_VID_1747_ARMY, 0},
        {1759, AS2_VID_1759_ARMY, 2},
        {1764, AS2_VID_1764_ARMY, 0},
        {2300, AS2_VID_2300_ARMY, 2},
        {2304, AS2_VID_2304_ARMY, 0},
        {2357, AS2_VID_2357_ARMY, 0},
    };

    constexpr size_t EXPECTED_BINDING_COUNT = 69;
    size_t binding_count = sizeof(expected_bindings) / sizeof(expected_bindings[0]);
    ASSERT_EQ(binding_count, EXPECTED_BINDING_COUNT);

    std::set<int> known_vids;
    for (const ExpectedArmyBinding& binding : expected_bindings) {
        EXPECT_TRUE(known_vids.insert(binding.vid).second)
            << "Duplicate VID binding: " << binding.vid;
        EXPECT_EQ(binding.constant_value, binding.expected_value)
            << "Incorrect constant for VID " << binding.vid;
        EXPECT_EQ(get_as2_vid_army(binding.vid), binding.expected_value)
            << "Incorrect lookup value for VID " << binding.vid;
    }
}

TEST(VidArmiesTest, UnknownAS2AssetVidThrows) {
    EXPECT_THROW(get_as2_vid_army(999999), std::invalid_argument);
}

TEST(VidArmiesTest, DecorationArmyConflictsUseGameIniDefaults) {
    // A few authored maps contain stale per-sprite Army values. The game INI
    // remains authoritative because Army is globally bound to the VID.
    EXPECT_EQ(get_as2_vid_army(1221), 2);
    EXPECT_EQ(get_as2_vid_army(1638), 0);
    EXPECT_EQ(get_as2_vid_army(1755), 0);
}
