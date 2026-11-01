/**
 * @file gore_builder.cpp
 * @brief Corpses with blood around them, and bare blood pools, for the Gore brush.
 */

#include "auto_mapper/core/scatter/gore/gore_builder.h"

#include "auto_mapper/core/randomizer.h"
#include "auto_mapper/core/scatter/scatter_fill.h"

#include <algorithm>
#include <cmath>
#include <iterator>
#include <span>

namespace auto_mapper::core {
namespace {

constexpr int FULL_DIRECTION_RANGE = 256;

// Default brushes: a small bone field, a lone corpse, and a pool the size of an official corpse-less one.
constexpr float DEFAULT_BONES_RADIUS = 80.0f;
constexpr float DEFAULT_BONES_SPACING = 35.0f;
constexpr int DEFAULT_BONES_BLOOD = 4;
constexpr float DEFAULT_CORPSE_RADIUS = 40.0f;
constexpr float DEFAULT_CORPSE_SPACING = 90.0f;
constexpr int DEFAULT_CORPSE_BLOOD = 8;
constexpr float DEFAULT_POOL_RADIUS = 50.0f;
constexpr int DEFAULT_POOL_BLOOD = 6;

/**
 * One blood VID's share of a recipe, in percent.
 */
struct BloodShare {
    int32_t vid = 0;
    int percent = 0;
};

// Blood around a corpse: the AS2R maps lean on 120, the AS2C maps on 110; this sits between them.
constexpr BloodShare CORPSE_BLOOD_RECIPE[] = {
    {GORE_VID_BLOOD, 45},
    {GORE_VID_BLOOD_SMALL, 35},
    {GORE_VID_BLOOD_SPREADING, 13},
    {GORE_VID_GIBS, 7},
};
// Corpse-less pools are mostly 110, sometimes with 120 or spreading blood.
constexpr BloodShare POOL_BLOOD_RECIPE[] = {
    {GORE_VID_BLOOD, 60},
    {GORE_VID_BLOOD_SMALL, 30},
    {GORE_VID_BLOOD_SPREADING, 10},
};

float distance_between(float ax, float ay, float bx, float by) {
    return std::hypot(ax - bx, ay - by);
}

bool contains(std::span<const int32_t> vids, int32_t vid) {
    return std::find(vids.begin(), vids.end(), vid) != vids.end();
}

bool uses_conscription_objects(io::MapFormat format) {
    return format == io::MapFormat::AS2C || format == io::MapFormat::AS2OE;
}

uint32_t random_direction() {
    return static_cast<uint32_t>(Random::get(0, FULL_DIRECTION_RANGE - 1));
}

template <std::size_t Count>
int32_t pick_blood_vid(const BloodShare (&recipe)[Count]) {
    int roll = Random::get(0, 99);
    for (const BloodShare& share : recipe) {
        if (roll < share.percent) {
            return share.vid;
        }
        roll -= share.percent;
    }
    return recipe[0].vid;
}

/**
 * A random count in [n/2, 3n/2], so neighbouring corpses do not all bleed alike.
 */
int random_blood_count(int blood_count) {
    int count = std::max(0, blood_count);
    return Random::get(count - count / 2, count + count / 2);
}

/**
 * `count` blood sprites uniform over the ellipse with half axes (spread_x, spread_y) around the center.
 * `lifted` raises them 5..15 (over a corpse, or a blood pool); the spreading blood 916 always sits at 20.
 */
template <std::size_t Count>
void splash_blood(std::vector<GoreItem>& result, float center_x, float center_y, float spread_x, float spread_y,
                  int count, const BloodShare (&recipe)[Count], bool lifted) {
    for (int index = 0; index < count; ++index) {
        ScatterPoint unit = random_point_in_circle(0.0f, 0.0f, 1.0f);
        GoreItem blood;
        blood.vid = pick_blood_vid(recipe);
        blood.x = center_x + unit.x * spread_x;
        blood.y = center_y + unit.y * spread_y;
        if (lifted) {
            blood.z = static_cast<float>(Random::get_real(GORE_BLOOD_OVER_CORPSE_Z_MIN, GORE_BLOOD_OVER_CORPSE_Z_MAX));
        }
        if (blood.vid == GORE_VID_BLOOD_SPREADING) {
            blood.z = GORE_SPREADING_BLOOD_Z;
        }
        blood.direction = random_direction();
        result.push_back(blood);
    }
}

} // namespace

GoreBrushSettings default_gore_brush(GoreKind kind) {
    GoreBrushSettings settings;
    settings.kind = kind;
    settings.radius = DEFAULT_CORPSE_RADIUS;
    settings.spacing = DEFAULT_CORPSE_SPACING;
    settings.blood_count = DEFAULT_CORPSE_BLOOD;
    if (kind == GoreKind::Bones) {
        settings.radius = DEFAULT_BONES_RADIUS;
        settings.spacing = DEFAULT_BONES_SPACING;
        settings.blood_count = DEFAULT_BONES_BLOOD;
    }
    if (kind == GoreKind::BloodPool) {
        settings.radius = DEFAULT_POOL_RADIUS;
        settings.blood_count = DEFAULT_POOL_BLOOD;
    }
    return settings;
}

bool is_gore_blood(int32_t vid) {
    return vid == GORE_VID_BLOOD || vid == GORE_VID_BLOOD_SMALL || vid == GORE_VID_BLOOD_SPREADING ||
           vid == GORE_VID_GIBS;
}

std::vector<int32_t> gore_corpse_vids(GoreKind kind, io::MapFormat format) {
    std::vector<int32_t> vids;
    if (kind == GoreKind::Bones) {
        vids.push_back(GORE_VID_BONES);
    }
    if (kind == GoreKind::Monster) {
        vids.assign(std::begin(GORE_MONSTER_VIDS), std::end(GORE_MONSTER_VIDS));
        if (uses_conscription_objects(format)) {
            vids.insert(vids.end(), std::begin(GORE_MONSTER_VIDS_AS2C), std::end(GORE_MONSTER_VIDS_AS2C));
        }
    }
    if (kind == GoreKind::Human) {
        vids.assign(std::begin(GORE_HUMAN_VIDS), std::end(GORE_HUMAN_VIDS));
        if (uses_conscription_objects(format)) {
            vids.insert(vids.end(), std::begin(GORE_HUMAN_VIDS_AS2C), std::end(GORE_HUMAN_VIDS_AS2C));
        }
    }
    return vids;
}

bool gore_vid_available(int32_t vid, io::MapFormat format) {
    if (uses_conscription_objects(format)) {
        return true;
    }
    return !contains(GORE_MONSTER_VIDS_AS2C, vid) && !contains(GORE_HUMAN_VIDS_AS2C, vid);
}

bool gore_item_belongs_to(int32_t vid, GoreKind kind) {
    if (is_gore_blood(vid)) {
        return true;
    }
    if (kind == GoreKind::Bones) {
        return vid == GORE_VID_BONES;
    }
    if (kind == GoreKind::Monster) {
        return contains(GORE_MONSTER_VIDS, vid) || contains(GORE_MONSTER_VIDS_AS2C, vid);
    }
    if (kind == GoreKind::Human) {
        return contains(GORE_HUMAN_VIDS, vid) || contains(GORE_HUMAN_VIDS_AS2C, vid);
    }
    return false;
}

std::vector<GoreItem> scatter_gore(const GoreBrushSettings& settings, io::MapFormat format, float center_x,
                                   float center_y) {
    std::vector<GoreItem> result;
    if (settings.kind == GoreKind::BloodPool) {
        float radius = std::max(1.0f, settings.radius);
        splash_blood(result, center_x, center_y, radius, radius * GORE_POOL_Y_RATIO,
                     random_blood_count(settings.blood_count), POOL_BLOOD_RECIPE, true);
        return result;
    }

    std::vector<int32_t> pool = gore_corpse_vids(settings.kind, format);
    float spacing = std::max(1.0f, settings.spacing);
    // Only this dab's own corpses keep apart: gore may land on top of anything already placed.
    std::vector<ScatterPoint> occupied;
    std::vector<ScatterPoint> spots = fill_from_center(center_x, center_y, settings.radius, spacing, occupied);

    // Blood first: build_gore() keeps that order anyway, this just keeps one dab readable in project JSON.
    // Monster and soldier corpses get their blood on top of them; bone piles keep it on the floor.
    bool over_corpse = settings.kind == GoreKind::Monster || settings.kind == GoreKind::Human;
    std::vector<GoreItem> corpses;
    for (const ScatterPoint& spot : spots) {
        splash_blood(result, spot.x, spot.y, GORE_BLOOD_SPREAD_X, GORE_BLOOD_SPREAD_Y,
                     random_blood_count(settings.blood_count), CORPSE_BLOOD_RECIPE, over_corpse);
        GoreItem corpse;
        corpse.vid = pool[Random::get(0, static_cast<int>(pool.size()) - 1)];
        corpse.x = spot.x;
        corpse.y = spot.y;
        corpse.direction = random_direction();
        corpses.push_back(corpse);
    }
    result.insert(result.end(), corpses.begin(), corpses.end());
    return result;
}

int erase_gore(std::vector<GoreItem>& items, float center_x, float center_y, float radius, GoreKind kind) {
    std::vector<GoreItem> kept;
    kept.reserve(items.size());
    for (const GoreItem& item : items) {
        bool inside = distance_between(item.x, item.y, center_x, center_y) <= radius;
        if (inside && gore_item_belongs_to(item.vid, kind)) {
            continue;
        }
        kept.push_back(item);
    }
    int erased = static_cast<int>(items.size() - kept.size());
    items = kept;
    return erased;
}

std::vector<io::Sprite> build_gore(const std::vector<GoreItem>& items) {
    std::vector<io::Sprite> blood;
    std::vector<io::Sprite> corpses;
    for (const GoreItem& item : items) {
        io::Sprite sprite(item.vid, item.x, item.y, item.z, item.direction);
        sprite.army = GORE_DEFAULT_ARMY;
        if (item.vid == GORE_VID_BONES) {
            sprite.army = GORE_BONES_ARMY;
        }
        if (is_gore_blood(item.vid)) {
            blood.push_back(sprite);
        } else {
            corpses.push_back(sprite);
        }
    }
    blood.insert(blood.end(), corpses.begin(), corpses.end());
    return blood;
}

} // namespace auto_mapper::core
