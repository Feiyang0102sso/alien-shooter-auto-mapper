#include <gtest/gtest.h>

#include "auto_mapper/core/loot/loot_box_builder.h"
#include "auto_mapper/core/loot/loot_table.h"

#include <cmath>
#include <iterator>
#include <set>
#include <vector>

using namespace auto_mapper::core;
using auto_mapper::io::Sprite;

namespace {

// Repeat random dabs so a broken rule shows up even if one dab happens to pass.
constexpr int RANDOM_REPEATS = 30;

float anchor_distance(const LootBox& a, const LootBox& b) {
    return std::hypot(a.x - b.x, a.y - b.y);
}

LootTable single_line_table(int32_t vid, const std::string& text, double percent) {
    LootTable table;
    table.vids = {vid};
    table.lines.push_back({text, percent});
    return table;
}

} // namespace

TEST(LootTableTest, ValidatesPercents) {
    LootTable table;
    table.lines = {{"Obj_money1", 60.0}, {"Obj_money2", 40.0}};
    EXPECT_TRUE(is_valid_loot_table(table));

    table.lines.push_back({"Obj_money3", 0.5});
    EXPECT_FALSE(is_valid_loot_table(table));

    table.lines = {{"Obj_money1", -1.0}};
    EXPECT_FALSE(is_valid_loot_table(table));
}

TEST(LootTableTest, HundredPercentLineFillsEveryListedSprite) {
    const std::string text = "EMPTY EMPTY Ammo_shotgun Ammo_minigun Obj_money3";
    std::vector<Sprite> sprites = {Sprite(LOOT_VID_WOOD_BOX, 0, 0), Sprite(601, 0, 0), Sprite(LOOT_VID_WOOD_BOX, 9, 9),
                                   Sprite(LOOT_VID_BROKEN_EGG, 0, 0)};
    assign_loot_text(sprites, {single_line_table(LOOT_VID_WOOD_BOX, text, 100.0)});

    EXPECT_EQ(sprites[0].name_text, text);
    EXPECT_EQ(sprites[2].name_text, text);
    // Unlisted VIDs (a wall, the broken egg) keep an empty nameText.
    EXPECT_TRUE(sprites[1].name_text.empty());
    EXPECT_TRUE(sprites[3].name_text.empty());
}

TEST(LootTableTest, ZeroPercentAndEmptyTablesWriteNothing) {
    std::vector<Sprite> sprites = {Sprite(LOOT_VID_WOOD_BOX, 0, 0), Sprite(LOOT_VID_METAL_BOX, 0, 0)};
    LootTable empty_metal;
    empty_metal.vids = {LOOT_VID_METAL_BOX};
    assign_loot_text(sprites, {single_line_table(LOOT_VID_WOOD_BOX, "Obj_money1", 0.0), empty_metal});

    EXPECT_TRUE(sprites[0].name_text.empty());
    EXPECT_TRUE(sprites[1].name_text.empty());
}

TEST(LootBoxBuilderTest, BoxPilesKeepSpacingStackHeightAndDirections) {
    LootBrushSettings settings = default_loot_brush(LootBoxKind::Wood);
    settings.radius = 150.0f;
    settings.stack_chance = 0.5;

    std::set<uint32_t> allowed_directions(std::begin(LOOT_WOOD_BOX_DIRECTIONS), std::end(LOOT_WOOD_BOX_DIRECTIONS));

    bool saw_stack = false;
    for (int repeat = 0; repeat < RANDOM_REPEATS; ++repeat) {
        std::vector<LootBox> boxes = scatter_loot_boxes(settings, 1000.0f, 1000.0f, {});
        ASSERT_FALSE(boxes.empty());
        for (size_t i = 0; i < boxes.size(); ++i) {
            const LootBox& box = boxes[i];
            EXPECT_EQ(box.vid, LOOT_VID_WOOD_BOX);
            EXPECT_TRUE(allowed_directions.count(box.direction)) << box.direction;
            // Only one stacked level.
            EXPECT_TRUE(box.z == 0.0f || box.z == LOOT_WOOD_BOX_STACK_HEIGHT) << box.z;
            if (box.z > 0.0f) {
                saw_stack = true;
                continue;
            }
            // Ground boxes never crowd each other below the smallest box gap.
            for (size_t j = i + 1; j < boxes.size(); ++j) {
                if (boxes[j].z == 0.0f) {
                    EXPECT_GE(anchor_distance(box, boxes[j]), settings.spacing - 0.01f);
                }
            }
        }
    }
    EXPECT_TRUE(saw_stack);
}

TEST(LootBoxBuilderTest, MetalBoxesOnlyUseDrawableDirections) {
    // 2332 claims 10 directions but has no frame for 204 / 230.
    LootBrushSettings settings = default_loot_brush(LootBoxKind::Metal);
    settings.radius = 200.0f;
    std::set<uint32_t> allowed(std::begin(LOOT_METAL_BOX_DIRECTIONS), std::end(LOOT_METAL_BOX_DIRECTIONS));
    for (int repeat = 0; repeat < RANDOM_REPEATS; ++repeat) {
        for (const LootBox& box : scatter_loot_boxes(settings, 0.0f, 0.0f, {})) {
            EXPECT_TRUE(allowed.count(box.direction)) << box.direction;
        }
    }
}

TEST(LootBoxBuilderTest, DabFillsFromTheCenterOutwards) {
    // On empty ground the first item sits on the dab center and every item stays inside the brush circle.
    for (LootBoxKind kind : {LootBoxKind::Wood, LootBoxKind::Egg}) {
        LootBrushSettings settings = default_loot_brush(kind);
        settings.stack_chance = 0.0;
        std::vector<LootBox> items = scatter_loot_boxes(settings, 300.0f, 400.0f, {});
        ASSERT_GT(items.size(), 2u);
        EXPECT_EQ(items[0].x, 300.0f);
        EXPECT_EQ(items[0].y, 400.0f);
        for (const LootBox& item : items) {
            EXPECT_LE(std::hypot(item.x - 300.0f, item.y - 400.0f), settings.radius + 0.01f);
        }
    }
}

TEST(LootBoxBuilderTest, DabOnFullSpotAddsNothing) {
    LootBrushSettings settings = default_loot_brush(LootBoxKind::Metal);
    // One box sits on the dab center and the whole brush circle lies within its spacing.
    settings.radius = settings.spacing * 0.5f;
    LootBox existing;
    existing.vid = LOOT_VID_METAL_BOX;
    existing.x = 500.0f;
    existing.y = 500.0f;

    for (int repeat = 0; repeat < RANDOM_REPEATS; ++repeat) {
        EXPECT_TRUE(scatter_loot_boxes(settings, 500.0f, 500.0f, {existing}).empty());
    }
}

TEST(LootBoxBuilderTest, EggFieldsKeepSpacingAndBrokenChance) {
    LootBrushSettings settings = default_loot_brush(LootBoxKind::Egg);

    settings.broken_chance = 0.0;
    std::vector<LootBox> eggs = scatter_loot_boxes(settings, 0.0f, 0.0f, {});
    ASSERT_GT(eggs.size(), 5u);
    for (size_t i = 0; i < eggs.size(); ++i) {
        EXPECT_EQ(eggs[i].vid, LOOT_VID_EGG);
        for (size_t j = i + 1; j < eggs.size(); ++j) {
            EXPECT_GE(anchor_distance(eggs[i], eggs[j]), settings.spacing - 0.01f);
        }
    }

    settings.broken_chance = 1.0;
    for (const LootBox& egg : scatter_loot_boxes(settings, 0.0f, 0.0f, {})) {
        EXPECT_EQ(egg.vid, LOOT_VID_BROKEN_EGG);
    }
}

TEST(LootBoxBuilderTest, EraseRemovesOnlyTheCardKindInsideTheCircle) {
    std::vector<LootBox> boxes = {
        {LOOT_VID_WOOD_BOX, 0.0f, 0.0f, 0.0f, 0},     {LOOT_VID_METAL_BOX, 10.0f, 0.0f, 0.0f, 0},
        {LOOT_VID_WOOD_BOX, 500.0f, 0.0f, 0.0f, 0},   {LOOT_VID_EGG, 0.0f, 10.0f, 0.0f, 0},
        {LOOT_VID_BROKEN_EGG, 0.0f, 20.0f, 0.0f, 0},
    };

    EXPECT_EQ(erase_loot_boxes(boxes, 0.0f, 0.0f, 100.0f, LootBoxKind::Wood), 1);
    EXPECT_EQ(boxes.size(), 4u);
    // The egg card owns the broken egg too.
    EXPECT_EQ(erase_loot_boxes(boxes, 0.0f, 0.0f, 100.0f, LootBoxKind::Egg), 2);
    EXPECT_EQ(boxes.size(), 2u);
}

TEST(LootBoxBuilderTest, SpritesCarryContainerArmy) {
    std::vector<Sprite> sprites = build_loot_boxes({{LOOT_VID_WOOD_BOX, 1.0f, 2.0f, 25.0f, 28},
                                                    {LOOT_VID_BROKEN_EGG, 3.0f, 4.0f, 0.0f, 7}});
    ASSERT_EQ(sprites.size(), 2u);
    EXPECT_EQ(sprites[0].army, LOOT_CONTAINER_ARMY);
    EXPECT_EQ(sprites[0].posZ, 25.0f);
    EXPECT_EQ(sprites[0].direction, 28u);
    EXPECT_EQ(sprites[1].army, LOOT_BROKEN_EGG_ARMY);
    EXPECT_TRUE(sprites[0].name_text.empty());
}
