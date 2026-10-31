/**
 * @file placeholder_vids.cpp
 * @brief Hard-coded AS2R / Conscription nvid categories for the editor stamp preview.
 *
 * Without a game folder the editor cannot read objects.res, so the categories
 * below were extracted from Conscription objects.res once and written down here.
 */

#include "auto_mapper/core/decoration_builder/placeholder_vids.h"

#include <algorithm>
#include <array>

namespace auto_mapper::core::decoration_builder {
namespace {

// objects.res "BLOOD:" entries.
constexpr std::array<int32_t, 9> BLOOD_VIDS = {110, 120, 152, 154, 166, 167, 198, 918, 1319};

// objects.res "SHADOWS:" entries.
constexpr std::array<int32_t, 14> SHADOW_VIDS = {
    108, 133, 135, 144, 1205, 1300, 1301, 1302, 1303, 1507, 1879, 3009, 4045, 4066,
};

// objects.res "TERRAIN:" entries (floors, ground, grass, trees, fences).
constexpr std::array<int32_t, 33> TERRAIN_VIDS = {
    500,  501,  502,  503,  505,  506,  507,  510,  512,  513,  1120, 1121, 1122, 1125, 1127, 1128, 1129,
    1504, 1506, 1509, 1510, 1514, 1522, 1526, 1527, 1711, 1712, 1724, 1783, 2503, 2514, 2603, 2643,
};

// Densely stacked small props: books, bones, scattered paper, desk clutter, small boxes on shelf.
constexpr std::array<int32_t, 5> CLUTTER_VIDS = {1765, 1532, 408, 1288, 428};

// objects.res "BLOCK:" entries: invisible air walls (Z dummies, monster/player/vehicle blockers).
// All of them carry the 0x20000 flag in the OBJ parameter block.
constexpr std::array<int32_t, 8> BLOCK_VIDS = {630, 631, 632, 633, 634, 635, 2648, 2706};

template <std::size_t Count>
bool contains(const std::array<int32_t, Count>& vids, int32_t vid) {
    return std::find(vids.begin(), vids.end(), vid) != vids.end();
}

} // namespace

bool draws_placeholder(int32_t vid) {
    if (is_invisible_block(vid)) {
        return false;
    }
    if (lies_on_ground(vid)) {
        return false;
    }
    if (contains(CLUTTER_VIDS, vid)) {
        return false;
    }
    return true;
}

bool lies_on_ground(int32_t vid) {
    if (contains(BLOOD_VIDS, vid)) {
        return true;
    }
    if (contains(SHADOW_VIDS, vid)) {
        return true;
    }
    if (contains(TERRAIN_VIDS, vid)) {
        return true;
    }
    return false;
}

bool is_invisible_block(int32_t vid) {
    return contains(BLOCK_VIDS, vid);
}

} // namespace auto_mapper::core::decoration_builder
