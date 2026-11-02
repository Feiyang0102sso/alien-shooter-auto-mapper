#include <gtest/gtest.h>

#include "auto_mapper/core/scatter/gore/gore_builder.h"
#include "auto_mapper/core/randomizer.h"

#include <algorithm>
#include <cmath>
#include <set>
#include <vector>

using namespace auto_mapper::core;
using auto_mapper::io::Sprite;

namespace {

// Repeat random dabs so a broken rule shows up even if one dab happens to pass.
constexpr int RANDOM_REPEATS = 30;
constexpr float CENTER = 1000.0f;

std::vector<GoreItem> corpses_of(const std::vector<GoreItem>& items) {
    std::vector<GoreItem> corpses;
    for (const GoreItem& item : items) {
        if (!is_gore_blood(item.vid)) {
            corpses.push_back(item);
        }
    }
    return corpses;
}

bool in_set(const std::vector<int32_t>& vids, int32_t vid) {
    return std::find(vids.begin(), vids.end(), vid) != vids.end();
}

} // namespace

TEST(GoreBuilderTest, CorpsesKeepSpacingAndComeFromTheirPool) {
    GoreBrushSettings settings = default_gore_brush(GoreKind::Monster);
    settings.radius = 200.0f;
    std::vector<int32_t> pool = gore_corpse_vids(GoreKind::Monster, nullptr);

    for (int repeat = 0; repeat < RANDOM_REPEATS; ++repeat) {
        std::vector<GoreItem> corpses = corpses_of(scatter_gore(settings, nullptr, CENTER, CENTER));
        ASSERT_FALSE(corpses.empty());
        for (size_t i = 0; i < corpses.size(); ++i) {
            EXPECT_TRUE(in_set(pool, corpses[i].vid)) << corpses[i].vid;
            EXPECT_EQ(corpses[i].z, 0.0f);
            EXPECT_LT(corpses[i].direction, 256u);
            for (size_t j = i + 1; j < corpses.size(); ++j) {
                float distance = std::hypot(corpses[i].x - corpses[j].x, corpses[i].y - corpses[j].y);
                EXPECT_GE(distance, settings.spacing);
            }
        }
    }
}

TEST(GoreBuilderTest, CorpsePoolFollowsTheGameFolder) {
    // No game folder: the shared pool only.
    for (int32_t vid : GORE_HUMAN_VIDS_AS2C) {
        EXPECT_FALSE(in_set(gore_corpse_vids(GoreKind::Human, nullptr), vid));
    }
    // A Conscription folder defines every candidate: its own corpses join.
    std::vector<int32_t> candidates = gore_corpse_candidates();
    std::set<int32_t> conscription(candidates.begin(), candidates.end());
    for (int32_t vid : GORE_HUMAN_VIDS_AS2C) {
        EXPECT_TRUE(in_set(gore_corpse_vids(GoreKind::Human, &conscription), vid));
    }
    for (int32_t vid : GORE_MONSTER_VIDS_AS2C) {
        EXPECT_TRUE(in_set(gore_corpse_vids(GoreKind::Monster, &conscription), vid));
    }
    // An AS2R folder lacks them: they stay out.
    std::set<int32_t> reloaded(std::begin(GORE_HUMAN_VIDS), std::end(GORE_HUMAN_VIDS));
    reloaded.insert(std::begin(GORE_MONSTER_VIDS), std::end(GORE_MONSTER_VIDS));
    for (int32_t vid : GORE_HUMAN_VIDS_AS2C) {
        EXPECT_FALSE(in_set(gore_corpse_vids(GoreKind::Human, &reloaded), vid));
    }
    // A corpse the game lacks leaves the shared pool too; a folder with none of them keeps the shared pool.
    std::set<int32_t> soldier_only = {GORE_HUMAN_VIDS[0]};
    EXPECT_EQ(gore_corpse_vids(GoreKind::Human, &soldier_only), std::vector<int32_t>{GORE_HUMAN_VIDS[0]});
    std::set<int32_t> none;
    EXPECT_EQ(gore_corpse_vids(GoreKind::Human, &none).size(), std::size(GORE_HUMAN_VIDS));
    EXPECT_TRUE(gore_corpse_vids(GoreKind::BloodPool, &conscription).empty());
}

TEST(GoreBuilderTest, BloodStaysInTheEllipseOverTheCorpse) {
    GoreBrushSettings settings = default_gore_brush(GoreKind::Human);
    settings.blood_count = 40;
    std::set<int32_t> allowed_blood = {GORE_VID_BLOOD, GORE_VID_BLOOD_SMALL, GORE_VID_BLOOD_SPREADING, GORE_VID_GIBS};

    for (int repeat = 0; repeat < RANDOM_REPEATS; ++repeat) {
        std::vector<GoreItem> items = scatter_gore(settings, nullptr, CENTER, CENTER);
        // One lone corpse at the center: the radius is smaller than the spacing.
        std::vector<GoreItem> corpses = corpses_of(items);
        ASSERT_EQ(corpses.size(), 1u);
        EXPECT_EQ(corpses[0].x, CENTER);
        EXPECT_EQ(corpses[0].y, CENTER);
        int blood = static_cast<int>(items.size()) - 1;
        EXPECT_GE(blood, 20);
        EXPECT_LE(blood, 60);
        for (const GoreItem& item : items) {
            if (!is_gore_blood(item.vid)) {
                continue;
            }
            EXPECT_TRUE(allowed_blood.count(item.vid) == 1) << item.vid;
            float nx = (item.x - CENTER) / GORE_BLOOD_SPREAD_X;
            float ny = (item.y - CENTER) / GORE_BLOOD_SPREAD_Y;
            EXPECT_LE(nx * nx + ny * ny, 1.0001f);
            // Soldier corpse blood lies over the body; the spreading blood keeps its own height.
            if (item.vid == GORE_VID_BLOOD_SPREADING) {
                EXPECT_EQ(item.z, GORE_SPREADING_BLOOD_Z);
            } else {
                EXPECT_GE(item.z, GORE_BLOOD_OVER_CORPSE_Z_MIN);
                EXPECT_LE(item.z, GORE_BLOOD_OVER_CORPSE_Z_MAX);
            }
        }
    }
}

TEST(GoreBuilderTest, BonePileBloodStaysOnTheFloor) {
    GoreBrushSettings settings = default_gore_brush(GoreKind::Bones);
    for (int repeat = 0; repeat < RANDOM_REPEATS; ++repeat) {
        for (const GoreItem& item : scatter_gore(settings, nullptr, CENTER, CENTER)) {
            if (item.vid == GORE_VID_BLOOD_SPREADING) {
                EXPECT_EQ(item.z, GORE_SPREADING_BLOOD_Z);
            } else {
                EXPECT_EQ(item.z, 0.0f);
            }
        }
    }
}

TEST(GoreBuilderTest, HeavyArmourSoldierIsHuman) {
    EXPECT_TRUE(gore_item_belongs_to(3968, GoreKind::Human));
    EXPECT_FALSE(gore_item_belongs_to(3968, GoreKind::Monster));
    std::vector<int32_t> candidates = gore_corpse_candidates();
    std::set<int32_t> conscription(candidates.begin(), candidates.end());
    std::vector<int32_t> soldiers = gore_corpse_vids(GoreKind::Human, &conscription);
    EXPECT_TRUE(in_set(soldiers, 3968));
}

TEST(GoreBuilderTest, BloodPoolHasNoCorpseAndStaysInItsEllipse) {
    GoreBrushSettings settings = default_gore_brush(GoreKind::BloodPool);
    for (int repeat = 0; repeat < RANDOM_REPEATS; ++repeat) {
        std::vector<GoreItem> items = scatter_gore(settings, nullptr, CENTER, CENTER);
        ASSERT_FALSE(items.empty());
        for (const GoreItem& item : items) {
            EXPECT_TRUE(is_gore_blood(item.vid));
            EXPECT_NE(item.vid, GORE_VID_GIBS);
            // Pool blood is lifted like blood over a corpse; the spreading blood keeps its own height.
            if (item.vid == GORE_VID_BLOOD_SPREADING) {
                EXPECT_EQ(item.z, GORE_SPREADING_BLOOD_Z);
            } else {
                EXPECT_GE(item.z, GORE_BLOOD_OVER_CORPSE_Z_MIN);
                EXPECT_LE(item.z, GORE_BLOOD_OVER_CORPSE_Z_MAX);
            }
            float nx = (item.x - CENTER) / settings.radius;
            float ny = (item.y - CENTER) / (settings.radius * GORE_POOL_Y_RATIO);
            EXPECT_LE(nx * nx + ny * ny, 1.0001f);
        }
    }
}

TEST(GoreBuilderTest, SecondDabOnTheSameSpotAddsMoreCorpses) {
    // Gore never avoids what is already placed: a second click on a lone corpse lays another one.
    GoreBrushSettings lone = default_gore_brush(GoreKind::Human);
    for (int repeat = 0; repeat < RANDOM_REPEATS; ++repeat) {
        EXPECT_EQ(corpses_of(scatter_gore(lone, nullptr, CENTER, CENTER)).size(), 1u);
    }
}

TEST(GoreBuilderTest, SameSeedGivesTheSameDab) {
    // The brush ghost and the click that places it run from one seed and must agree.
    GoreBrushSettings settings = default_gore_brush(GoreKind::Monster);
    settings.radius = 200.0f;
    std::vector<GoreItem> ghost;
    std::vector<GoreItem> placed;
    {
        RandomSeedScope seed(12345);
        ghost = scatter_gore(settings, nullptr, CENTER, CENTER);
    }
    {
        RandomSeedScope seed(12345);
        placed = scatter_gore(settings, nullptr, CENTER, CENTER);
    }
    ASSERT_EQ(ghost.size(), placed.size());
    for (size_t i = 0; i < ghost.size(); ++i) {
        EXPECT_EQ(ghost[i].vid, placed[i].vid);
        EXPECT_EQ(ghost[i].x, placed[i].x);
        EXPECT_EQ(ghost[i].y, placed[i].y);
        EXPECT_EQ(ghost[i].z, placed[i].z);
        EXPECT_EQ(ghost[i].direction, placed[i].direction);
    }
}

TEST(GoreBuilderTest, SeedScopeLeavesTheSharedSequenceAlone) {
    // Drawing a ghost must not change what the rest of the program draws next.
    std::vector<int> expected;
    std::vector<int> actual;
    {
        RandomSeedScope outer(777);
        for (int i = 0; i < 5; ++i) {
            expected.push_back(Random::get(0, 1000000));
        }
    }
    {
        RandomSeedScope outer(777);
        actual.push_back(Random::get(0, 1000000));
        {
            RandomSeedScope ghost(1);
            Random::get(0, 1000000);
        }
        for (int i = 1; i < 5; ++i) {
            actual.push_back(Random::get(0, 1000000));
        }
    }
    EXPECT_EQ(actual, expected);
}

TEST(GoreBuilderTest, EraseRemovesOnlyTheCardsItems) {
    std::vector<GoreItem> items = {
        {GORE_VID_BONES, 0, 0, 0, 0},     {GORE_MONSTER_VIDS[0], 10, 0, 0, 0}, {GORE_VID_BLOOD, 0, 10, 0, 0},
        {GORE_HUMAN_VIDS[0], 0, -10, 0, 0}, {GORE_VID_BONES, 500, 0, 0, 0},
    };
    std::vector<GoreItem> bones_erase = items;
    EXPECT_EQ(erase_gore(bones_erase, 0, 0, 50, GoreKind::Bones), 2);
    EXPECT_EQ(bones_erase.size(), 3u);

    std::vector<GoreItem> pool_erase = items;
    EXPECT_EQ(erase_gore(pool_erase, 0, 0, 50, GoreKind::BloodPool), 1);

    std::vector<GoreItem> human_erase = items;
    EXPECT_EQ(erase_gore(human_erase, 0, 0, 50, GoreKind::Human), 2);
}

TEST(GoreBuilderTest, BuildWritesBloodFirstAndBonesAsArmyTwo) {
    std::vector<GoreItem> items = {
        {GORE_VID_BONES, 1, 2, 0, 9}, {GORE_VID_BLOOD, 3, 4, 0, 7}, {GORE_HUMAN_VIDS[0], 5, 6, 0, 5},
        {GORE_VID_BLOOD_SPREADING, 7, 8, GORE_SPREADING_BLOOD_Z, 3},
    };
    std::vector<Sprite> sprites = build_gore(items);
    ASSERT_EQ(sprites.size(), 4u);
    EXPECT_EQ(sprites[0].vid, GORE_VID_BLOOD);
    EXPECT_EQ(sprites[1].vid, GORE_VID_BLOOD_SPREADING);
    EXPECT_EQ(sprites[1].posZ, GORE_SPREADING_BLOOD_Z);
    EXPECT_EQ(sprites[2].vid, GORE_VID_BONES);
    EXPECT_EQ(sprites[2].army, GORE_BONES_ARMY);
    EXPECT_EQ(sprites[2].direction, 9u);
    EXPECT_EQ(sprites[3].vid, GORE_HUMAN_VIDS[0]);
    EXPECT_EQ(sprites[3].army, GORE_DEFAULT_ARMY);
    EXPECT_EQ(sprites[0].army, GORE_DEFAULT_ARMY);
}
