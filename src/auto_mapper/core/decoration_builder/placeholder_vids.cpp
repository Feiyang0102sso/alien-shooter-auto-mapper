/**
 * @file placeholder_vids.cpp
 * @brief Hard-coded AS1 and AS2 Series nvid categories for the editor stamp preview.
 *
 * Without a game folder the editor cannot read objects.res, so the categories
 * below were extracted from objects.res once and written down here:
 * AS1 from Alien Shooter 1.22, AS2 from Conscription. The two games number their
 * objects independently, so each list is complete on its own even where nvids repeat.
 */

#include "auto_mapper/core/decoration_builder/placeholder_vids.h"

#include <algorithm>
#include <array>

namespace auto_mapper::core::decoration_builder {
namespace {

// ---------------------------------------------------------------------------
// AS1 (Alien Shooter 1 objects.res)
// ---------------------------------------------------------------------------

// Dried blood (flag 0x40000000) and lying gibs.
constexpr std::array<int32_t, 9> AS1_BLOOD_VIDS = {110, 120, 152, 154, 166, 167, 198, 916, 918};

// Small and large shadows, embedded shadow, magma / monster gun shadows.
constexpr std::array<int32_t, 5> AS1_SHADOW_VIDS = {108, 144, 158, 191, 883};

// Floors, ground and grass.
constexpr std::array<int32_t, 9> AS1_TERRAIN_VIDS = {500, 501, 502, 503, 504, 505, 506, 507, 508};

// Battle litter lying on the ground: shells, cannon shells, blast pit, ice shards,
// computer wreckage, wood chips, monster armor fragments, rocks.
constexpr std::array<int32_t, 13> AS1_DEBRIS_VIDS = {
    103, 354, 107, 148, 161, 183, 764, 766, 768, 770, 778, 780, 783,
};

// Densely stacked small props: scattered paper, small boxes on shelves, beakers, books.
constexpr std::array<int32_t, 4> AS1_CLUTTER_VIDS = {408, 428, 426, 667};

// Invisible dummies carrying the 0x20000 flag: secret dummy, Z dummies.
// 908 also has the flag but is a scripted monster generator, not an air wall.
constexpr std::array<int32_t, 3> AS1_BLOCK_VIDS = {360, 365, 631};

// ---------------------------------------------------------------------------
// AS2 Series (Conscription objects.res)
// ---------------------------------------------------------------------------

// objects.res "BLOOD:" entries.
constexpr std::array<int32_t, 9> AS2_BLOOD_VIDS = {110, 120, 152, 154, 166, 167, 198, 918, 1319};

// objects.res "SHADOWS:" entries.
constexpr std::array<int32_t, 14> AS2_SHADOW_VIDS = {
    108, 133, 135, 144, 1205, 1300, 1301, 1302, 1303, 1507, 1879, 3009, 4045, 4066,
};

// objects.res "TERRAIN:" entries (floors, ground, grass, trees, fences).
constexpr std::array<int32_t, 33> AS2_TERRAIN_VIDS = {
    500,  501,  502,  503,  505,  506,  507,  510,  512,  513,  1120, 1121, 1122, 1125, 1127, 1128, 1129,
    1504, 1506, 1509, 1510, 1514, 1522, 1526, 1527, 1711, 1712, 1724, 1783, 2503, 2514, 2603, 2643,
};

// Densely stacked small props: books, bones, scattered paper, desk clutter, small boxes on shelf.
constexpr std::array<int32_t, 5> AS2_CLUTTER_VIDS = {1765, 1532, 408, 1288, 428};

// objects.res "BLOCK:" entries: invisible air walls (Z dummies, monster/player/vehicle blockers).
// All of them carry the 0x20000 flag in the OBJ parameter block.
constexpr std::array<int32_t, 8> AS2_BLOCK_VIDS = {630, 631, 632, 633, 634, 635, 2648, 2706};

template <std::size_t Count>
bool contains(const std::array<int32_t, Count>& vids, int32_t vid) {
    return std::find(vids.begin(), vids.end(), vid) != vids.end();
}

bool is_as1(io::MapFormat format) {
    return format == io::MapFormat::AS1;
}

bool is_clutter(io::MapFormat format, int32_t vid) {
    if (is_as1(format)) {
        return contains(AS1_CLUTTER_VIDS, vid);
    }
    return contains(AS2_CLUTTER_VIDS, vid);
}

} // namespace

bool draws_placeholder(io::MapFormat format, int32_t vid) {
    if (is_invisible_block(format, vid)) {
        return false;
    }
    if (lies_on_ground(format, vid)) {
        return false;
    }
    if (is_clutter(format, vid)) {
        return false;
    }
    return true;
}

bool lies_on_ground(io::MapFormat format, int32_t vid) {
    if (is_as1(format)) {
        if (contains(AS1_BLOOD_VIDS, vid)) {
            return true;
        }
        if (contains(AS1_SHADOW_VIDS, vid)) {
            return true;
        }
        if (contains(AS1_TERRAIN_VIDS, vid)) {
            return true;
        }
        if (contains(AS1_DEBRIS_VIDS, vid)) {
            return true;
        }
        return false;
    }

    if (contains(AS2_BLOOD_VIDS, vid)) {
        return true;
    }
    if (contains(AS2_SHADOW_VIDS, vid)) {
        return true;
    }
    if (contains(AS2_TERRAIN_VIDS, vid)) {
        return true;
    }
    return false;
}

bool is_invisible_block(io::MapFormat format, int32_t vid) {
    if (is_as1(format)) {
        return contains(AS1_BLOCK_VIDS, vid);
    }
    return contains(AS2_BLOCK_VIDS, vid);
}

} // namespace auto_mapper::core::decoration_builder
