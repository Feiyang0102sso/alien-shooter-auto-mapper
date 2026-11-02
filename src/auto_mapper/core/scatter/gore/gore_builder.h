/**
 * @file gore_builder.h
 * @brief Gore brush: scatter corpses with blood around them, or bare blood pools, in Map Coordinates.
 *
 * One corpse dab fills the brush circle from its center outwards (the Loot Box engine, scatter_fill.h) with
 * corpses `spacing` apart, then splashes blood around every corpse. Unlike loot boxes, gore never avoids what is
 * already there. A blood pool dab only splashes blood.
 * The editor stores the concrete sprites; generation only turns them into sprite records.
 *
 * Numbers come from the original maps (AS2R: AlienShooter2 Reloaded\Maps, AS2C: Conscription\Maps):
 * blood lies within about 80 x 60 of its corpse (p90 of |dx| / |dy|), close to uniform over that ellipse;
 * corpse-less pools have a radius of about 45. Corpses are army 0 except the bone pile 1532 (army 2 in every
 * official map); blood is army 0. Directions use the whole 0..255 range as the AS2R maps do.
 */

#pragma once

#include "auto_mapper/core/vid_properties/vid_property_types.h"
#include "auto_mapper/io/map_writer.h"

#include <cstdint>
#include <set>
#include <vector>

namespace auto_mapper::core {

inline constexpr int32_t GORE_VID_BONES = 1532;

// Monster and human corpse pools of every AS2 Series game. Bosses, machines, named NPCs, loose parts and the
// HIDE: soldier (same frame as the MAPEDIT one, 1159) are left out.
inline constexpr int32_t GORE_MONSTER_VIDS[] = {1027, 1029, 1031, 1036, 1039, 1041, 1053, 1054,
                                                1063, 1065, 1082, 1094, 1096, 1801, 1812, 1822,
                                                1832, 1834, 1842, 1844, 1855, 1863, 1872};
inline constexpr int32_t GORE_HUMAN_VIDS[] = {430, 431, 1159, 1167, 1172};
// Corpses only Conscription's objects.res (shared by AS2OE) knows; AS2 / AS2R games have no such objects.
// They join a card's pool only when the configured AS2 game folder defines them (see gore_corpse_vids()).
// objects.res calls 3968 "Монстр1", but its frame is the heavy armour soldier with a minigun, so it is human.
inline constexpr int32_t GORE_MONSTER_VIDS_AS2C[] = {4019, 4023, 4025, 4029, 4033, 4035, 4039, 4041};
inline constexpr int32_t GORE_HUMAN_VIDS_AS2C[] = {3959, 3968, 3971};

// Ground blood: dried blood 110, small dried blood 120, spreading blood 916 (drawn 20 up), lying gibs 1319.
// The "vertical" blood 154 is deliberately not used.
inline constexpr int32_t GORE_VID_BLOOD = 110;
inline constexpr int32_t GORE_VID_BLOOD_SMALL = 120;
inline constexpr int32_t GORE_VID_BLOOD_SPREADING = 916;
inline constexpr int32_t GORE_VID_GIBS = 1319;
inline constexpr float GORE_SPREADING_BLOOD_Z = 20.0f;
// Blood around a monster or soldier corpse is lifted this much so it lies over the body, not under it;
// blood pools use the same height. Bone piles keep their blood on the floor (z 0).
inline constexpr float GORE_BLOOD_OVER_CORPSE_Z_MIN = 5.0f;
inline constexpr float GORE_BLOOD_OVER_CORPSE_Z_MAX = 15.0f;

// Half axes of the ellipse a corpse's blood falls in, and the y / x ratio of a blood pool's ellipse.
inline constexpr float GORE_BLOOD_SPREAD_X = 80.0f;
inline constexpr float GORE_BLOOD_SPREAD_Y = 60.0f;
inline constexpr float GORE_POOL_Y_RATIO = 0.75f;

inline constexpr VidArmy GORE_BONES_ARMY = 2;
inline constexpr VidArmy GORE_DEFAULT_ARMY = 0;

enum class GoreKind { Bones, Monster, Human, BloodPool };

/**
 * One placed corpse or blood sprite. Map Coordinates.
 */
struct GoreItem {
    int32_t vid = GORE_VID_BLOOD;
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    uint32_t direction = 0;
};

/**
 * Brush parameters of one card. Distances are Map Coordinates.
 */
struct GoreBrushSettings {
    GoreKind kind = GoreKind::Monster;
    // Corpse cards: corpses fill this circle. Blood pool: the pool's half width.
    float radius = 40.0f;
    // Corpse cards: smallest distance between two corpses. Unused by the blood pool.
    float spacing = 90.0f;
    // Blood sprites per corpse (blood pool: per dab); each dab takes a random count in [n/2, 3n/2].
    int blood_count = 8;
};

/**
 * Default brush of a card: a small bone field (radius 80, spacing 35, 4 blood), a lone corpse
 * (radius 40, spacing 90, 8 blood), or a pool (radius 50, 6 blood).
 */
GoreBrushSettings default_gore_brush(GoreKind kind);

/**
 * True for the blood VIDs the brush lays (110, 120, 916, 1319).
 */
bool is_gore_blood(int32_t vid);

/**
 * Every corpse VID any card may use: the shared pools plus the Conscription-only ones. The editor checks these
 * against the AS2 game folder once, when the folder opens.
 */
std::vector<int32_t> gore_corpse_candidates();

/**
 * Corpse pool of a card. `game_vids` holds the candidates the configured AS2 game folder defines; the pool is
 * the card's candidates found in it, so a Conscription folder adds its own corpses and an AS2R folder does not.
 * Null (no game folder), or a set holding none of the card's candidates, gives the shared pool every AS2 Series
 * game has. Empty for the blood pool card.
 */
std::vector<int32_t> gore_corpse_vids(GoreKind kind, const std::set<int32_t>* game_vids);

/**
 * VIDs a card owns: its corpses (from any game), plus all blood on every card.
 */
bool gore_item_belongs_to(int32_t vid, GoreKind kind);

/**
 * One brush dab around (center_x, center_y). Corpses of one dab keep `spacing` from each other, but nothing
 * already on the map blocks a dab: corpses and blood may pile up freely. `game_vids` picks the corpse pool
 * (see gore_corpse_vids()).
 */
std::vector<GoreItem> scatter_gore(const GoreBrushSettings& settings, const std::set<int32_t>* game_vids,
                                   float center_x, float center_y);

/**
 * Remove the items `kind` owns whose anchor lies within `radius` of the center. Returns how many went.
 */
int erase_gore(std::vector<GoreItem>& items, float center_x, float center_y, float radius, GoreKind kind);

/**
 * Items to sprites: blood first so a corpse at the same depth lies on top of it; army 2 for bone piles, 0 otherwise.
 */
std::vector<io::Sprite> build_gore(const std::vector<GoreItem>& items);

} // namespace auto_mapper::core
