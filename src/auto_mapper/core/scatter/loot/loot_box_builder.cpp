/**
 * @file loot_box_builder.cpp
 * @brief Random piles of loot boxes and fields of alien eggs for the Loot Box brush.
 */

#include "auto_mapper/core/scatter/loot/loot_box_builder.h"

#include "auto_mapper/core/randomizer.h"
#include "auto_mapper/core/scatter/scatter_fill.h"

#include <algorithm>
#include <cmath>

namespace auto_mapper::core {
namespace {

constexpr double PI = 3.14159265358979323846;
constexpr int FULL_DIRECTION_RANGE = 256;
// Default brushes: a small box pile (about five boxes) and a wider egg patch.
constexpr float DEFAULT_BOX_RADIUS = 80.0f;
constexpr float DEFAULT_BOX_SPACING = 45.0f;
constexpr float DEFAULT_EGG_RADIUS = 150.0f;
constexpr float DEFAULT_EGG_SPACING = 35.0f;
constexpr double DEFAULT_STACK_CHANCE = 0.15;
constexpr double DEFAULT_EGG_BROKEN_CHANCE = 0.10;

float distance_between(float ax, float ay, float bx, float by) {
    return std::hypot(ax - bx, ay - by);
}

int32_t container_vid(LootBoxKind kind) {
    if (kind == LootBoxKind::Metal) {
        return LOOT_VID_METAL_BOX;
    }
    if (kind == LootBoxKind::Egg) {
        return LOOT_VID_EGG;
    }
    return LOOT_VID_WOOD_BOX;
}

float stack_height(LootBoxKind kind) {
    if (kind == LootBoxKind::Metal) {
        return LOOT_METAL_BOX_STACK_HEIGHT;
    }
    return LOOT_WOOD_BOX_STACK_HEIGHT;
}

uint32_t random_direction(LootBoxKind kind) {
    if (kind == LootBoxKind::Egg) {
        return static_cast<uint32_t>(Random::get(0, FULL_DIRECTION_RANGE - 1));
    }
    if (kind == LootBoxKind::Metal) {
        int pick = Random::get(0, static_cast<int>(std::size(LOOT_METAL_BOX_DIRECTIONS)) - 1);
        return LOOT_METAL_BOX_DIRECTIONS[pick];
    }
    int pick = Random::get(0, static_cast<int>(std::size(LOOT_WOOD_BOX_DIRECTIONS)) - 1);
    return LOOT_WOOD_BOX_DIRECTIONS[pick];
}

/**
 * Anchors of existing loot boxes close enough to the dab to matter for spacing.
 */
std::vector<ScatterPoint> nearby_anchors(const std::vector<LootBox>& existing, float center_x, float center_y, float reach) {
    std::vector<ScatterPoint> anchors;
    for (const LootBox& box : existing) {
        if (distance_between(box.x, box.y, center_x, center_y) <= reach) {
            anchors.push_back({box.x, box.y});
        }
    }
    return anchors;
}

/**
 * One box pile: a box on every ground spot, and on some of them one more box stacked on top.
 */
std::vector<LootBox> pile_boxes(const LootBrushSettings& settings, const std::vector<ScatterPoint>& spots) {
    int32_t vid = container_vid(settings.kind);
    std::vector<LootBox> result;
    for (const ScatterPoint& spot : spots) {
        LootBox box;
        box.vid = vid;
        box.x = spot.x;
        box.y = spot.y;
        box.direction = random_direction(settings.kind);
        result.push_back(box);

        if (Random::get_real(0.0, 1.0) >= settings.stack_chance) {
            continue;
        }
        double shift = Random::get_real(0.0, LOOT_STACK_OFFSET_MAX);
        double angle = Random::get_real(0.0, 2.0 * PI);
        LootBox top;
        top.vid = vid;
        top.x = box.x + static_cast<float>(shift * std::cos(angle));
        top.y = box.y + static_cast<float>(shift * std::sin(angle));
        top.z = stack_height(settings.kind);
        top.direction = random_direction(settings.kind);
        result.push_back(top);
    }
    return result;
}

/**
 * One egg patch: an egg on every ground spot, some of them the lootless broken egg.
 */
std::vector<LootBox> lay_eggs(const LootBrushSettings& settings, const std::vector<ScatterPoint>& spots) {
    std::vector<LootBox> result;
    for (const ScatterPoint& spot : spots) {
        LootBox egg;
        egg.vid = LOOT_VID_EGG;
        if (Random::get_real(0.0, 1.0) < settings.broken_chance) {
            egg.vid = LOOT_VID_BROKEN_EGG;
        }
        egg.x = spot.x;
        egg.y = spot.y;
        egg.direction = random_direction(LootBoxKind::Egg);
        result.push_back(egg);
    }
    return result;
}

} // namespace

LootBrushSettings default_loot_brush(LootBoxKind kind) {
    LootBrushSettings settings;
    settings.kind = kind;
    settings.radius = DEFAULT_BOX_RADIUS;
    settings.spacing = DEFAULT_BOX_SPACING;
    settings.stack_chance = DEFAULT_STACK_CHANCE;
    if (kind == LootBoxKind::Egg) {
        settings.radius = DEFAULT_EGG_RADIUS;
        settings.spacing = DEFAULT_EGG_SPACING;
        settings.broken_chance = DEFAULT_EGG_BROKEN_CHANCE;
    }
    return settings;
}

bool loot_box_belongs_to(int32_t vid, LootBoxKind kind) {
    if (kind == LootBoxKind::Egg) {
        return vid == LOOT_VID_EGG || vid == LOOT_VID_BROKEN_EGG;
    }
    return vid == container_vid(kind);
}

std::vector<int32_t> loot_table_vids(LootBoxKind kind) {
    return {container_vid(kind)};
}

std::vector<LootBox> scatter_loot_boxes(const LootBrushSettings& settings, float center_x, float center_y,
                                        const std::vector<LootBox>& existing) {
    float spacing = std::max(1.0f, settings.spacing);
    std::vector<ScatterPoint> occupied = nearby_anchors(existing, center_x, center_y, settings.radius + spacing);
    std::vector<ScatterPoint> spots = fill_from_center(center_x, center_y, settings.radius, spacing, occupied);
    std::vector<LootBox> result;
    if (settings.kind == LootBoxKind::Egg) {
        result = lay_eggs(settings, spots);
    } else {
        result = pile_boxes(settings, spots);
    }
    return result;
}

int erase_loot_boxes(std::vector<LootBox>& boxes, float center_x, float center_y, float radius, LootBoxKind kind) {
    std::vector<LootBox> kept;
    kept.reserve(boxes.size());
    for (const LootBox& box : boxes) {
        bool inside = distance_between(box.x, box.y, center_x, center_y) <= radius;
        if (inside && loot_box_belongs_to(box.vid, kind)) {
            continue;
        }
        kept.push_back(box);
    }
    int erased = static_cast<int>(boxes.size() - kept.size());
    boxes = kept;
    return erased;
}

std::vector<io::Sprite> build_loot_boxes(const std::vector<LootBox>& boxes) {
    std::vector<io::Sprite> sprites;
    sprites.reserve(boxes.size());
    for (const LootBox& box : boxes) {
        io::Sprite sprite(box.vid, box.x, box.y, box.z, box.direction);
        sprite.army = LOOT_CONTAINER_ARMY;
        if (box.vid == LOOT_VID_BROKEN_EGG) {
            sprite.army = LOOT_BROKEN_EGG_ARMY;
        }
        sprites.push_back(sprite);
    }
    return sprites;
}

} // namespace auto_mapper::core
