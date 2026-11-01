/**
 * @file loot_box_builder.h
 * @brief Loot Box brush: scatter wood boxes, metal boxes and alien eggs in Map Coordinates.
 *
 * One brush dab is one pile (or one egg patch): items fill the brush circle from its center outwards,
 * keeping `spacing` from each other and from what is already there. Unlike a Decoration Stamp the layout
 * is new every time. The editor stores the concrete instances; generation only turns them into sprites.
 * Loot texts are not part of an instance: generate_map() rolls them from the Loot Tables (see loot_table.h).
 *
 * Defaults come from the original AS2R maps (AlienShooter2 Reloaded\Maps): neighbouring boxes 38..53
 * apart (p25..p75), about 1.4% of boxes stacked on another box 25 (wood) / 30 (metal) higher with at most
 * 30 sideways; AS2C egg fields have spacing ~35.
 */

#pragma once

#include "auto_mapper/core/vid_properties/vid_property_types.h"
#include "auto_mapper/io/map_writer.h"

#include <cstdint>
#include <vector>

namespace auto_mapper::core {

inline constexpr int32_t LOOT_VID_WOOD_BOX = 1225;
inline constexpr int32_t LOOT_VID_METAL_BOX = 2332;
inline constexpr int32_t LOOT_VID_EGG = 2750;
inline constexpr int32_t LOOT_VID_BROKEN_EGG = 2751;

// Box directions the official AS2R maps use, the only ones with a drawable frame. objects.res says noDir 9 / 10,
// but 2332 has no frame for 204 / 230, so the lists are taken from the maps rather than computed from noDir.
// Eggs use the whole 0..255 range as the official egg fields do.
inline constexpr uint32_t LOOT_WOOD_BOX_DIRECTIONS[] = {0, 28, 56, 85, 113, 142, 170, 199, 227};
inline constexpr uint32_t LOOT_METAL_BOX_DIRECTIONS[] = {0, 25, 51, 76, 102, 128, 153, 179};

// Breakable containers are army 2 in every official map; a broken egg is scenery (army 0).
inline constexpr VidArmy LOOT_CONTAINER_ARMY = 2;
inline constexpr VidArmy LOOT_BROKEN_EGG_ARMY = 0;

// Height of a box standing on another box of the same kind, and its largest sideways shift.
inline constexpr float LOOT_WOOD_BOX_STACK_HEIGHT = 25.0f;
inline constexpr float LOOT_METAL_BOX_STACK_HEIGHT = 30.0f;
inline constexpr float LOOT_STACK_OFFSET_MAX = 30.0f;

enum class LootBoxKind { Wood, Metal, Egg };

/**
 * One placed loot container. Map Coordinates; z > 0 means it stands on another box.
 */
struct LootBox {
    int32_t vid = LOOT_VID_WOOD_BOX;
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    uint32_t direction = 0;
};

/**
 * Brush parameters of one card. Distances are Map Coordinates, chances are 0..1.
 */
struct LootBrushSettings {
    LootBoxKind kind = LootBoxKind::Wood;
    // The pile (or egg patch) one dab lays out fills this circle.
    float radius = 80.0f;
    // Smallest distance between two ground items.
    float spacing = 45.0f;
    // Boxes: chance a ground box gets one more box on top.
    double stack_chance = 0.15;
    // Eggs: chance an egg is the broken 2751.
    double broken_chance = 0.10;
};

/**
 * Default brush of a card: a small box pile (radius 80, spacing 45, 15% stacked), or a wider egg patch
 * (radius 150, spacing 35, 10% broken).
 */
LootBrushSettings default_loot_brush(LootBoxKind kind);

/**
 * VIDs a card owns: its own container, plus the broken egg on the egg card.
 */
bool loot_box_belongs_to(int32_t vid, LootBoxKind kind);

/**
 * VIDs whose nameText the card's Loot Table fills (never the broken egg).
 */
std::vector<int32_t> loot_table_vids(LootBoxKind kind);

/**
 * One brush dab around (center_x, center_y): a new pile filled from the center outwards; `existing` is left
 * untouched. New items keep their spacing from every existing loot box, so dabbing twice on one spot adds nothing.
 */
std::vector<LootBox> scatter_loot_boxes(const LootBrushSettings& settings, float center_x, float center_y,
                                        const std::vector<LootBox>& existing);

/**
 * Remove the boxes of `kind` whose anchor lies within `radius` of the center. Returns how many went.
 */
int erase_loot_boxes(std::vector<LootBox>& boxes, float center_x, float center_y, float radius, LootBoxKind kind);

/**
 * Instances to sprites: army 2 (broken egg 0), default gamma and scale, empty nameText.
 */
std::vector<io::Sprite> build_loot_boxes(const std::vector<LootBox>& boxes);

} // namespace auto_mapper::core
