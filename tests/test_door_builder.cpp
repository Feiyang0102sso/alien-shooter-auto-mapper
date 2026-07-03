#include <gtest/gtest.h>
#include "auto_mapper/core/door_builder.h"
#include "auto_mapper/core/wall_builder.h"
#include "auto_mapper/io/map_writer.h"
#include "utils/test_utils.h"
#include <set>

using namespace auto_mapper;
using namespace auto_mapper::core;
using namespace auto_mapper::test;


/**
 * Tests for AS1 door builder
 * normal door + lab door mixed scene
 * make sure all doors are aligned and some door can be open
 * no celling or floor in the map
 * jammed door removed, as it is now using random value
 */
TEST(DoorBuilderTest, DoorSceneGolden) {
    const std::string json_path = resolve_test_path("tests/golden/door_builder.gold.json");
    TestScene scene = load_test_scene(json_path);

    ASSERT_GT(scene.segments.size(), 0u);
    ASSERT_GT(scene.doors.size(), 0u);

    std::vector<DoorExcavation> excavations;
    excavations.reserve(scene.doors.size());
    for (const auto& door : scene.doors) {
        int excavation_size = door.size;
        if (door.wall_type == WALL_TYPE_LAB) {
            excavation_size = 1;
        }

        excavations.push_back({
            door.pos,
            door.direction_type,
            excavation_size,
            door.wall_type
        });
    }

    WallBuilder wall_builder(scene.map_size_x, scene.map_size_y);
    std::vector<io::Sprite> sprites = wall_builder.build(scene.segments, false, false, excavations);

    DoorBuilder door_builder(scene.map_size_x, scene.map_size_y);
    std::vector<io::Sprite> door_sprites = door_builder.build(scene.doors);
    sprites.insert(sprites.end(), door_sprites.begin(), door_sprites.end());

    const std::string temp_output_path = "current_door_builder.map";
    TempFileCleaner cleaner(temp_output_path);

    bool write_success = io::write_map(sprites, temp_output_path, scene.map_size_x, scene.map_size_y);
    ASSERT_TRUE(write_success);

    const std::string golden_map_path = resolve_test_path("tests/golden/door_builder.gold.map");
    bool files_match = compare_binary_files(temp_output_path, golden_map_path);

    EXPECT_TRUE(files_match);
}

TEST(DoorBuilderTest, StandardActiveDoorUsesOpenPanelVid) {
    DoorBuilder builder(600.0f, 600.0f);

    DoorInstance small_open_door = {
        {0, 0},
        WALL_TYPE_STANDARD,
        0,
        1,
        DOOR_STATE_OPEN,
        LIGHT_STATE_RED,
        0.0f
    };

    DoorInstance large_open_door = small_open_door;
    large_open_door.size = 2;

    auto small_sprites = builder.build({small_open_door});
    auto large_sprites = builder.build({large_open_door});

    ASSERT_GE(small_sprites.size(), 2);
    ASSERT_GE(large_sprites.size(), 2);
    EXPECT_EQ(small_sprites[1].vid, 605);
    EXPECT_FLOAT_EQ(small_sprites[1].posZ, 0.0f);
    EXPECT_EQ(large_sprites[1].vid, 607);
    EXPECT_FLOAT_EQ(large_sprites[1].posZ, 0.0f);
}

TEST(DoorBuilderTest, StandardActiveDoorIgnoresDoorStateForPanelVid) {
    DoorBuilder builder(600.0f, 600.0f);

    DoorInstance small_door = {
        {0, 0},
        WALL_TYPE_STANDARD,
        0,
        1,
        DOOR_STATE_CLOSED,
        LIGHT_STATE_GREEN,
        0.0f
    };

    DoorInstance large_door = small_door;
    large_door.size = 2;

    auto small_sprites = builder.build({small_door});
    auto large_sprites = builder.build({large_door});

    ASSERT_GE(small_sprites.size(), 2);
    ASSERT_GE(large_sprites.size(), 2);
    EXPECT_EQ(small_sprites[1].vid, 605);
    EXPECT_FLOAT_EQ(small_sprites[1].posZ, 0.0f);
    EXPECT_EQ(large_sprites[1].vid, 607);
    EXPECT_FLOAT_EQ(large_sprites[1].posZ, 0.0f);
}

TEST(DoorBuilderTest, StandardDeadDoorKeepsClosedPanelVid) {
    DoorBuilder builder(600.0f, 600.0f);

    DoorInstance small_jammed_door = {
        {0, 0},
        WALL_TYPE_STANDARD,
        1,
        1,
        DOOR_STATE_CLOSED,
        LIGHT_STATE_BROKEN,
        -6.0f
    };

    DoorInstance large_open_door = small_jammed_door;
    large_open_door.size = 2;
    large_open_door.z_offset = -10.0f;

    auto small_sprites = builder.build({small_jammed_door});
    auto large_sprites = builder.build({large_open_door});

    ASSERT_GE(small_sprites.size(), 2);
    ASSERT_GE(large_sprites.size(), 2);
    EXPECT_EQ(small_sprites[1].vid, 617);
    EXPECT_EQ(small_sprites[1].direction, 64);
    EXPECT_FLOAT_EQ(small_sprites[1].posZ, -6.0f);
    EXPECT_EQ(large_sprites[1].vid, 611);
    EXPECT_EQ(large_sprites[1].direction, 64);
    EXPECT_FLOAT_EQ(large_sprites[1].posZ, -10.0f);
}

TEST(DoorBuilderTest, StandardJammedDoorRandomZOffsetsStayInRangeAndVary) {
    DoorBuilder builder(600.0f, 600.0f);

    std::vector<DoorInstance> doors;
    doors.reserve(20);

    for (int index = 0; index < 10; ++index) {
        doors.push_back({
            {index, 0},
            WALL_TYPE_STANDARD,
            0,
            1,
            DOOR_STATE_CLOSED,
            LIGHT_STATE_BROKEN,
            get_random_standard_jam_z_offset(1)
        });
    }

    for (int index = 0; index < 10; ++index) {
        doors.push_back({
            {index, 2},
            WALL_TYPE_STANDARD,
            0,
            2,
            DOOR_STATE_CLOSED,
            LIGHT_STATE_BROKEN,
            get_random_standard_jam_z_offset(2)
        });
    }

    std::vector<io::Sprite> sprites = builder.build(doors);
    ASSERT_EQ(sprites.size(), doors.size() * 3);

    std::set<float> small_offsets;
    std::set<float> large_offsets;

    for (int index = 0; index < 10; ++index) {
        float panel_z = sprites[index * 3 + 1].posZ;
        EXPECT_GE(panel_z, DOOR_STANDARD.small.jam_z_range.min_z);
        EXPECT_LE(panel_z, DOOR_STANDARD.small.jam_z_range.max_z);
        small_offsets.insert(panel_z);
    }

    for (int index = 10; index < 20; ++index) {
        float panel_z = sprites[index * 3 + 1].posZ;
        EXPECT_GE(panel_z, DOOR_STANDARD.large.jam_z_range.min_z);
        EXPECT_LE(panel_z, DOOR_STANDARD.large.jam_z_range.max_z);
        large_offsets.insert(panel_z);
    }

    EXPECT_NE(small_offsets.size(), 1u);
    EXPECT_NE(large_offsets.size(), 1u);
}

TEST(DoorBuilderTest, StandardDeadOpenUsesDeadOpenPanelVid) {
    DoorBuilder builder(600.0f, 600.0f);

    DoorInstance small_dead_open_door = {
        {0, 0},
        WALL_TYPE_STANDARD,
        1,
        1,
        DOOR_STATE_OPEN,
        LIGHT_STATE_BROKEN,
        0.0f
    };

    auto sprites = builder.build({small_dead_open_door});

    ASSERT_GE(sprites.size(), 2);
    EXPECT_EQ(sprites[1].vid, 617);
    EXPECT_EQ(sprites[1].direction, 64);
    EXPECT_FLOAT_EQ(sprites[1].posZ, DOOR_STANDARD.small.dead_open_z_offset.min_z);
}

TEST(DoorBuilderTest, StandardDarkDoorUsesDarkFramesAndStandardPanels) {
    DoorBuilder builder(600.0f, 600.0f);

    DoorInstance small_door = {
        {0, 0},
        WALL_TYPE_STANDARD_DARK,
        0,
        1,
        DOOR_STATE_CLOSED,
        LIGHT_STATE_RED,
        0.0f
    };

    DoorInstance large_door = small_door;
    large_door.size = 2;

    auto small_sprites = builder.build({small_door});
    auto large_sprites = builder.build({large_door});

    ASSERT_EQ(small_sprites.size(), 3);
    ASSERT_EQ(large_sprites.size(), 3);

    EXPECT_EQ(small_sprites[0].vid, 623);
    EXPECT_EQ(small_sprites[1].vid, 605);
    EXPECT_EQ(small_sprites[2].vid, 424);

    EXPECT_EQ(large_sprites[0].vid, 624);
    EXPECT_EQ(large_sprites[1].vid, 607);
    EXPECT_EQ(large_sprites[2].vid, 424);
}

TEST(DoorBuilderTest, LabLaserDoorUsesFrameAndPillar) {
    DoorBuilder builder(1200.0f, 1200.0f);

    DoorInstance door = {
        {0, 0},
        WALL_TYPE_LAB,
        1,
        1,
        DOOR_STATE_CLOSED,
        LIGHT_STATE_RED,
        0.0f
    };

    auto sprites = builder.build({door});

    ASSERT_EQ(sprites.size(), 2);
    EXPECT_EQ(sprites[0].vid, 653);
    EXPECT_EQ(sprites[0].direction, 0);
    EXPECT_EQ(sprites[1].vid, 164);
    EXPECT_EQ(sprites[1].direction, 0);
    EXPECT_FLOAT_EQ(sprites[1].posX - sprites[0].posX, 0.0f);
    EXPECT_FLOAT_EQ(sprites[1].posY - sprites[0].posY, -18.0f);
}

TEST(DoorBuilderTest, LabDeadDoorUsesSingleDecoration) {
    DoorBuilder builder(1200.0f, 1200.0f);

    DoorInstance door = {
        {0, 0},
        WALL_TYPE_LAB,
        0,
        1,
        DOOR_STATE_CLOSED,
        LIGHT_STATE_BROKEN,
        0.0f
    };

    auto sprites = builder.build({door});

    ASSERT_EQ(sprites.size(), 1);
    EXPECT_EQ(sprites[0].vid, 654);
    EXPECT_EQ(sprites[0].direction, 64);
}

TEST(DoorBuilderTest, LabOpenLaserDoorUsesFrameOnly) {
    DoorBuilder builder(1200.0f, 1200.0f);

    DoorInstance door = {
        {0, 0},
        WALL_TYPE_LAB,
        0,
        1,
        DOOR_STATE_OPEN,
        LIGHT_STATE_RED,
        0.0f
    };

    auto sprites = builder.build({door});

    ASSERT_EQ(sprites.size(), 1);
    EXPECT_EQ(sprites[0].vid, 653);
    EXPECT_EQ(sprites[0].direction, 64);
}

TEST(DoorBuilderTest, LabDoorIgnoresTwoTileSize) {
    DoorBuilder builder(1200.0f, 1200.0f);

    DoorInstance one_tile_door = {
        {0, 0},
        WALL_TYPE_LAB,
        1,
        1,
        DOOR_STATE_OPEN,
        LIGHT_STATE_RED,
        0.0f
    };

    DoorInstance two_tile_door = one_tile_door;
    two_tile_door.size = 2;

    auto one_tile_sprites = builder.build({one_tile_door});
    auto two_tile_sprites = builder.build({two_tile_door});

    ASSERT_EQ(one_tile_sprites.size(), 1);
    ASSERT_EQ(two_tile_sprites.size(), 1);
    EXPECT_FLOAT_EQ(two_tile_sprites[0].posX, one_tile_sprites[0].posX);
    EXPECT_FLOAT_EQ(two_tile_sprites[0].posY, one_tile_sprites[0].posY);
}
