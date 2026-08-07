#include <gtest/gtest.h>
#include "../src/auto_mapper/core/door_builder/door_builder.h"
#include "../src/auto_mapper/core/door_builder/door_profiles_as1.h"
#include "auto_mapper/core/wall_builder/wall_builder.h"
#include "auto_mapper/io/map_writer.h"
#include "utils/test_utils.h"
#include <map>
#include <set>
#include <cmath>

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

    const std::string temp_output_path = get_test_output_path("current_door_builder.map");
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

TEST(DoorBuilderTest, AS2WallSet1DoorVariantsUseTwoStatesWithoutLights) {
    const std::string json_path = resolve_test_path("tests/golden/as2_wall_set1_door_variants.json");
    TestScene scene = load_test_scene(json_path);

    ASSERT_FLOAT_EQ(scene.map_size_x, 5000.0f);
    ASSERT_FLOAT_EQ(scene.map_size_y, 5000.0f);
    ASSERT_EQ(scene.doors.size(), 12u);

    DoorBuilder door_builder(scene.map_size_x, scene.map_size_y);
    std::vector<io::Sprite> door_sprites = door_builder.build(scene.doors);

    ASSERT_EQ(door_sprites.size(), 30u);

    std::map<int, int> vid_counts;
    std::map<int, std::set<uint32_t>> directions_by_vid;
    std::map<std::pair<int, uint32_t>, int> count_by_vid_and_direction;
    for (const io::Sprite& sprite : door_sprites) {
        vid_counts[sprite.vid] += 1;
        directions_by_vid[sprite.vid].insert(sprite.direction);
        count_by_vid_and_direction[{sprite.vid, sprite.direction}] += 1;
    }

    EXPECT_EQ(vid_counts[1779], 6);
    EXPECT_EQ(vid_counts[1777], 3);
    EXPECT_EQ(vid_counts[1778], 3);
    EXPECT_EQ(vid_counts[1788], 6);
    EXPECT_EQ(vid_counts[1785], 3);
    EXPECT_EQ(vid_counts[1786], 3);

    EXPECT_EQ(vid_counts[423], 0);
    EXPECT_EQ(vid_counts[424], 0);
    EXPECT_EQ(vid_counts[425], 0);
    EXPECT_EQ(vid_counts[1782], 6);

    std::pair<int, uint32_t> fixed1_pillar_key = {1782, 85u};
    std::pair<int, uint32_t> random_pillar_key = {1782, 170u};
    std::pair<int, uint32_t> fixed0_pillar_key = {1782, 0u};
    EXPECT_EQ(count_by_vid_and_direction[fixed1_pillar_key], 4);
    EXPECT_EQ(count_by_vid_and_direction[random_pillar_key], 2);
    EXPECT_EQ(count_by_vid_and_direction[fixed0_pillar_key], 0);

    EXPECT_TRUE(directions_by_vid[1779].count(0) > 0);
    EXPECT_TRUE(directions_by_vid[1779].count(51) > 0);
    EXPECT_TRUE(directions_by_vid[1777].count(0) > 0);
    EXPECT_TRUE(directions_by_vid[1777].count(128) > 0);
    EXPECT_TRUE(directions_by_vid[1778].count(0) > 0);
    EXPECT_TRUE(directions_by_vid[1778].count(128) > 0);
    EXPECT_TRUE(directions_by_vid[1788].count(0) > 0);
    EXPECT_TRUE(directions_by_vid[1788].count(128) > 0);
    EXPECT_TRUE(directions_by_vid[1785].count(0) > 0);
    EXPECT_TRUE(directions_by_vid[1785].count(128) > 0);
    EXPECT_TRUE(directions_by_vid[1786].count(0) > 0);
    EXPECT_TRUE(directions_by_vid[1786].count(128) > 0);
}

TEST(DoorBuilderTest, AS2WallSet1DoorVariantsWriteCompactLShapeMap) {
    const std::string json_path = resolve_test_path("tests/golden/as2_wall_set1_door_variants.json");
    TestScene scene = load_test_scene(json_path);

    std::vector<DoorExcavation> excavations;
    excavations.reserve(scene.doors.size());
    for (const DoorInstance& door : scene.doors) {
        excavations.push_back({
            door.pos,
            door.direction_type,
            door.size,
            door.wall_type
        });
    }

    WallBuilder wall_builder(scene.map_size_x, scene.map_size_y);
    std::vector<io::Sprite> sprites = wall_builder.build(scene.segments, false, false, excavations);

    DoorBuilder door_builder(scene.map_size_x, scene.map_size_y);
    std::vector<io::Sprite> door_sprites = door_builder.build(scene.doors);
    sprites.insert(sprites.end(), door_sprites.begin(), door_sprites.end());

    std::string output_path = get_test_output_path("as2_wall_set1_door_variants.map");
    bool write_success = io::write_map(
        sprites,
        output_path,
        io::MapFormat::AS2R,
        scene.map_size_x,
        scene.map_size_y
    );

    ASSERT_TRUE(write_success);
}

TEST(DoorBuilderTest, AS2WallSet2LargeDoorUsesTwoFrameParts) {
    DoorBuilder builder(5000.0f, 5000.0f);

    std::vector<DoorInstance> doors = {
        {
            {0, 0},
            WALL_TYPE_AS2_WALL_SET2_RANDOM,
            0,
            2,
            DOOR_STATE_CLOSED,
            LIGHT_STATE_RED,
            0.0f
        },
        {
            {0, 4},
            WALL_TYPE_AS2_WALL_SET2_RANDOM,
            0,
            2,
            DOOR_STATE_OPEN,
            LIGHT_STATE_GREEN,
            0.0f
        },
        {
            {4, 0},
            WALL_TYPE_AS2_WALL_SET2_RANDOM,
            1,
            2,
            DOOR_STATE_CLOSED,
            LIGHT_STATE_RED,
            0.0f
        },
        {
            {8, 0},
            WALL_TYPE_AS2_WALL_SET2_RANDOM,
            1,
            2,
            DOOR_STATE_OPEN,
            LIGHT_STATE_GREEN,
            0.0f
        }
    };

    std::vector<io::Sprite> sprites = builder.build(doors);
    ASSERT_EQ(sprites.size(), 12u);

    std::map<std::pair<int, uint32_t>, int> count_by_vid_and_direction;
    for (const io::Sprite& sprite : sprites) {
        count_by_vid_and_direction[{sprite.vid, sprite.direction}] += 1;
    }

    std::pair<int, uint32_t> frame_dir_b_left = {1703, 0u};
    std::pair<int, uint32_t> frame_dir_b_right = {1703, 64u};
    std::pair<int, uint32_t> frame_dir_a_left = {1703, 128u};
    std::pair<int, uint32_t> frame_dir_a_right = {1703, 192u};
    std::pair<int, uint32_t> closed_panel_dir_b = {1785, 0u};
    std::pair<int, uint32_t> open_panel_dir_b = {1786, 0u};
    std::pair<int, uint32_t> closed_panel_dir_a = {1785, 128u};
    std::pair<int, uint32_t> open_panel_dir_a = {1786, 128u};

    EXPECT_EQ(count_by_vid_and_direction[frame_dir_b_left], 2);
    EXPECT_EQ(count_by_vid_and_direction[frame_dir_b_right], 2);
    EXPECT_EQ(count_by_vid_and_direction[frame_dir_a_left], 2);
    EXPECT_EQ(count_by_vid_and_direction[frame_dir_a_right], 2);
    EXPECT_EQ(count_by_vid_and_direction[closed_panel_dir_b], 1);
    EXPECT_EQ(count_by_vid_and_direction[open_panel_dir_b], 1);
    EXPECT_EQ(count_by_vid_and_direction[closed_panel_dir_a], 1);
    EXPECT_EQ(count_by_vid_and_direction[open_panel_dir_a], 1);
}

TEST(DoorBuilderTest, AS2WallSet2LargeDoorVariantsWriteCompactLShapeMap) {
    const float map_size_x = 5000.0f;
    const float map_size_y = 5000.0f;

    std::vector<Segment> segments = {
        {{2, 10}, {2, 22}, WALL_TYPE_AS2_WALL_SET2_RANDOM},
        {{2, 22}, {14, 22}, WALL_TYPE_AS2_WALL_SET2_RANDOM},
        {{18, 10}, {18, 22}, WALL_TYPE_AS2_WALL_SET2_RANDOM},
        {{18, 22}, {30, 22}, WALL_TYPE_AS2_WALL_SET2_RANDOM}
    };

    std::vector<DoorInstance> doors = {
        {
            {2, 13},
            WALL_TYPE_AS2_WALL_SET2_RANDOM,
            0,
            2,
            DOOR_STATE_CLOSED,
            LIGHT_STATE_RED,
            0.0f
        },
        {
            {2, 18},
            WALL_TYPE_AS2_WALL_SET2_RANDOM,
            0,
            2,
            DOOR_STATE_OPEN,
            LIGHT_STATE_GREEN,
            0.0f
        },
        {
            {5, 22},
            WALL_TYPE_AS2_WALL_SET2_RANDOM,
            1,
            2,
            DOOR_STATE_CLOSED,
            LIGHT_STATE_RED,
            0.0f
        },
        {
            {10, 22},
            WALL_TYPE_AS2_WALL_SET2_RANDOM,
            1,
            2,
            DOOR_STATE_OPEN,
            LIGHT_STATE_GREEN,
            0.0f
        },
        {
            {18, 15},
            WALL_TYPE_AS2_WALL_SET2_RANDOM,
            0,
            2,
            DOOR_STATE_OPEN,
            LIGHT_STATE_GREEN,
            0.0f
        },
        {
            {23, 22},
            WALL_TYPE_AS2_WALL_SET2_RANDOM,
            1,
            2,
            DOOR_STATE_CLOSED,
            LIGHT_STATE_RED,
            0.0f
        }
    };

    std::vector<DoorExcavation> excavations;
    excavations.reserve(doors.size());
    for (const DoorInstance& door : doors) {
        excavations.push_back({
            door.pos,
            door.direction_type,
            door.size,
            door.wall_type
        });
    }

    WallBuilder wall_builder(map_size_x, map_size_y);
    std::vector<io::Sprite> sprites = wall_builder.build(segments, false, false, excavations);

    DoorBuilder door_builder(map_size_x, map_size_y);
    std::vector<io::Sprite> door_sprites = door_builder.build(doors);
    sprites.insert(sprites.end(), door_sprites.begin(), door_sprites.end());

    std::string output_path = get_test_output_path("as2_wall_set2_door_variants.map");
    bool write_success = io::write_map(
        sprites,
        output_path,
        io::MapFormat::AS2R,
        map_size_x,
        map_size_y
    );

    ASSERT_TRUE(write_success);
}

static void expect_as2_wall_set3_or_set4_large_door_parts(int wall_type) {
    DoorBuilder builder(5000.0f, 5000.0f);

    std::vector<DoorInstance> doors = {
        {
            {0, 0},
            wall_type,
            0,
            2,
            DOOR_STATE_CLOSED,
            LIGHT_STATE_RED,
            0.0f
        },
        {
            {0, 4},
            wall_type,
            0,
            2,
            DOOR_STATE_OPEN,
            LIGHT_STATE_GREEN,
            0.0f
        },
        {
            {4, 0},
            wall_type,
            1,
            2,
            DOOR_STATE_CLOSED,
            LIGHT_STATE_RED,
            0.0f
        },
        {
            {8, 0},
            wall_type,
            1,
            2,
            DOOR_STATE_OPEN,
            LIGHT_STATE_GREEN,
            0.0f
        }
    };

    std::vector<io::Sprite> sprites = builder.build(doors);
    ASSERT_EQ(sprites.size(), 12u);

    std::map<std::pair<int, uint32_t>, int> count_by_vid_and_direction;
    for (const io::Sprite& sprite : sprites) {
        count_by_vid_and_direction[{sprite.vid, sprite.direction}] += 1;
    }

    std::pair<int, uint32_t> frame_dir_b_left = {1103, 0u};
    std::pair<int, uint32_t> frame_dir_b_right = {1103, 128u};
    std::pair<int, uint32_t> frame_dir_a_left = {1103, 64u};
    std::pair<int, uint32_t> frame_dir_a_right = {1103, 192u};
    std::pair<int, uint32_t> closed_panel_dir_b = {1785, 0u};
    std::pair<int, uint32_t> open_panel_dir_b = {1786, 0u};
    std::pair<int, uint32_t> closed_panel_dir_a = {1785, 128u};
    std::pair<int, uint32_t> open_panel_dir_a = {1786, 128u};

    EXPECT_EQ(count_by_vid_and_direction[frame_dir_b_left], 2);
    EXPECT_EQ(count_by_vid_and_direction[frame_dir_b_right], 2);
    EXPECT_EQ(count_by_vid_and_direction[frame_dir_a_left], 2);
    EXPECT_EQ(count_by_vid_and_direction[frame_dir_a_right], 2);
    EXPECT_EQ(count_by_vid_and_direction[closed_panel_dir_b], 1);
    EXPECT_EQ(count_by_vid_and_direction[open_panel_dir_b], 1);
    EXPECT_EQ(count_by_vid_and_direction[closed_panel_dir_a], 1);
    EXPECT_EQ(count_by_vid_and_direction[open_panel_dir_a], 1);
}

TEST(DoorBuilderTest, AS2WallSet3AndSet4LargeDoorUseTwoFrameParts) {
    expect_as2_wall_set3_or_set4_large_door_parts(WALL_TYPE_AS2_WALL_SET3_RANDOM);
    expect_as2_wall_set3_or_set4_large_door_parts(WALL_TYPE_AS2_WALL_SET4_RANDOM);
}

static void write_as2_wall_set3_or_set4_large_door_map(int wall_type, const std::string& output_name) {
    const float map_size_x = 5000.0f;
    const float map_size_y = 5000.0f;

    std::vector<Segment> segments = {
        {{2, 10}, {2, 22}, wall_type},
        {{2, 22}, {14, 22}, wall_type},
        {{18, 10}, {18, 22}, wall_type},
        {{18, 22}, {30, 22}, wall_type}
    };

    std::vector<DoorInstance> doors = {
        {
            {2, 13},
            wall_type,
            0,
            2,
            DOOR_STATE_CLOSED,
            LIGHT_STATE_RED,
            0.0f
        },
        {
            {2, 18},
            wall_type,
            0,
            2,
            DOOR_STATE_OPEN,
            LIGHT_STATE_GREEN,
            0.0f
        },
        {
            {5, 22},
            wall_type,
            1,
            2,
            DOOR_STATE_CLOSED,
            LIGHT_STATE_RED,
            0.0f
        },
        {
            {10, 22},
            wall_type,
            1,
            2,
            DOOR_STATE_OPEN,
            LIGHT_STATE_GREEN,
            0.0f
        },
        {
            {18, 15},
            wall_type,
            0,
            2,
            DOOR_STATE_OPEN,
            LIGHT_STATE_GREEN,
            0.0f
        },
        {
            {23, 22},
            wall_type,
            1,
            2,
            DOOR_STATE_CLOSED,
            LIGHT_STATE_RED,
            0.0f
        }
    };

    std::vector<DoorExcavation> excavations;
    excavations.reserve(doors.size());
    for (const DoorInstance& door : doors) {
        excavations.push_back({
            door.pos,
            door.direction_type,
            door.size,
            door.wall_type
        });
    }

    WallBuilder wall_builder(map_size_x, map_size_y);
    std::vector<io::Sprite> sprites = wall_builder.build(segments, false, false, excavations);

    DoorBuilder door_builder(map_size_x, map_size_y);
    std::vector<io::Sprite> door_sprites = door_builder.build(doors);
    sprites.insert(sprites.end(), door_sprites.begin(), door_sprites.end());

    std::string output_path = get_test_output_path(output_name);
    bool write_success = io::write_map(
        sprites,
        output_path,
        io::MapFormat::AS2R,
        map_size_x,
        map_size_y
    );

    ASSERT_TRUE(write_success);
}

TEST(DoorBuilderTest, AS2WallSet3AndSet4LargeDoorVariantsWriteCompactLShapeMaps) {
    write_as2_wall_set3_or_set4_large_door_map(
        WALL_TYPE_AS2_WALL_SET3_RANDOM,
        "as2_wall_set3_door_variants.map"
    );
    write_as2_wall_set3_or_set4_large_door_map(
        WALL_TYPE_AS2_WALL_SET4_RANDOM,
        "as2_wall_set4_door_variants.map"
    );
}

TEST(DoorBuilderTest, AS2WallSet5LargeDoorUsesSingleFrame) {
    DoorBuilder builder(5000.0f, 5000.0f);

    std::vector<DoorInstance> doors = {
        {
            {0, 0},
            WALL_TYPE_AS2_WALL_SET5_RANDOM,
            0,
            2,
            DOOR_STATE_CLOSED,
            LIGHT_STATE_RED,
            0.0f
        },
        {
            {0, 4},
            WALL_TYPE_AS2_WALL_SET5_RANDOM,
            0,
            2,
            DOOR_STATE_OPEN,
            LIGHT_STATE_GREEN,
            0.0f
        },
        {
            {4, 0},
            WALL_TYPE_AS2_WALL_SET5_RANDOM,
            1,
            2,
            DOOR_STATE_CLOSED,
            LIGHT_STATE_RED,
            0.0f
        },
        {
            {8, 0},
            WALL_TYPE_AS2_WALL_SET5_RANDOM,
            1,
            2,
            DOOR_STATE_OPEN,
            LIGHT_STATE_GREEN,
            0.0f
        }
    };

    // 4 doors x (1 frame + 1 panel + 2 air-wall VID 631 [one left, one right,
    //   FAR only — near-frame air-walls are intentionally omitted]) = 4 x 4 = 16
    std::vector<io::Sprite> sprites = builder.build(doors);
    ASSERT_EQ(sprites.size(), 16u);

    std::map<std::pair<int, uint32_t>, int> count_by_vid_and_direction;
    for (const io::Sprite& sprite : sprites) {
        count_by_vid_and_direction[{sprite.vid, sprite.direction}] += 1;
    }

    std::pair<int, uint32_t> frame_dir_b = {2504, 0u};
    std::pair<int, uint32_t> frame_dir_a = {2504, 128u};
    std::pair<int, uint32_t> closed_panel_dir_b = {2506, 0u};
    std::pair<int, uint32_t> open_panel_dir_b = {2505, 0u};
    std::pair<int, uint32_t> closed_panel_dir_a = {2506, 128u};
    std::pair<int, uint32_t> open_panel_dir_a = {2505, 128u};
    // Air-wall VID 631 appended by DoorBuilder AFTER excavations.
    // Per calibration map _wall_set_5_doors.json the 631 direction is
    // OPPOSITE to the 2504 frame direction, and there are 2 (far) per door
    // (one on each side, i.e. left-and-right layout, near-frame omitted):
    //   direction_type A (2504 dir = 128, "\" visual): 2 x 631 dir=0
    //       per door x 2 doors = 4
    //   direction_type B (2504 dir = 0,   "/" visual): 2 x 631 dir=128
    //       per door x 2 doors = 4
    std::pair<int, uint32_t> air_wall_for_2504_dir_128 = {631, 0u};
    std::pair<int, uint32_t> air_wall_for_2504_dir_0 = {631, 128u};

    EXPECT_EQ(count_by_vid_and_direction[frame_dir_b], 2);
    EXPECT_EQ(count_by_vid_and_direction[frame_dir_a], 2);
    EXPECT_EQ(count_by_vid_and_direction[closed_panel_dir_b], 1);
    EXPECT_EQ(count_by_vid_and_direction[open_panel_dir_b], 1);
    EXPECT_EQ(count_by_vid_and_direction[closed_panel_dir_a], 1);
    EXPECT_EQ(count_by_vid_and_direction[open_panel_dir_a], 1);
    EXPECT_EQ(count_by_vid_and_direction[air_wall_for_2504_dir_128], 4);
    EXPECT_EQ(count_by_vid_and_direction[air_wall_for_2504_dir_0], 4);
}

TEST(DoorBuilderTest, AS2WallSet5LargeDoorVariantsWriteCompactLShapeMap) {
    const float map_size_x = 5000.0f;
    const float map_size_y = 5000.0f;

    std::vector<Segment> segments = {
        {{2, 10}, {2, 22}, WALL_TYPE_AS2_WALL_SET5_RANDOM},
        {{2, 22}, {14, 22}, WALL_TYPE_AS2_WALL_SET5_RANDOM},
        {{18, 10}, {18, 22}, WALL_TYPE_AS2_WALL_SET5_RANDOM},
        {{18, 22}, {30, 22}, WALL_TYPE_AS2_WALL_SET5_RANDOM}
    };

    std::vector<DoorInstance> doors = {
        {
            {2, 13},
            WALL_TYPE_AS2_WALL_SET5_RANDOM,
            0,
            2,
            DOOR_STATE_CLOSED,
            LIGHT_STATE_RED,
            0.0f
        },
        {
            {2, 18},
            WALL_TYPE_AS2_WALL_SET5_RANDOM,
            0,
            2,
            DOOR_STATE_OPEN,
            LIGHT_STATE_GREEN,
            0.0f
        },
        {
            {5, 22},
            WALL_TYPE_AS2_WALL_SET5_RANDOM,
            1,
            2,
            DOOR_STATE_CLOSED,
            LIGHT_STATE_RED,
            0.0f
        },
        {
            {10, 22},
            WALL_TYPE_AS2_WALL_SET5_RANDOM,
            1,
            2,
            DOOR_STATE_OPEN,
            LIGHT_STATE_GREEN,
            0.0f
        },
        {
            {18, 15},
            WALL_TYPE_AS2_WALL_SET5_RANDOM,
            0,
            2,
            DOOR_STATE_OPEN,
            LIGHT_STATE_GREEN,
            0.0f
        },
        {
            {23, 22},
            WALL_TYPE_AS2_WALL_SET5_RANDOM,
            1,
            2,
            DOOR_STATE_CLOSED,
            LIGHT_STATE_RED,
            0.0f
        }
    };

    std::vector<DoorExcavation> excavations;
    excavations.reserve(doors.size());
    for (const DoorInstance& door : doors) {
        excavations.push_back({
            door.pos,
            door.direction_type,
            door.size,
            door.wall_type
        });
    }

    WallBuilder wall_builder(map_size_x, map_size_y);
    std::vector<io::Sprite> sprites = wall_builder.build(segments, false, false, excavations);

    DoorBuilder door_builder(map_size_x, map_size_y);
    std::vector<io::Sprite> door_sprites = door_builder.build(doors);
    sprites.insert(sprites.end(), door_sprites.begin(), door_sprites.end());

    std::string output_path = get_test_output_path("as2_wall_set5_door_variants.map");
    bool write_success = io::write_map(
        sprites,
        output_path,
        io::MapFormat::AS2R,
        map_size_x,
        map_size_y
    );

    ASSERT_TRUE(write_success);
}

static void expect_as2_manual_single_frame_large_door_parts(
    int wall_type,
    int frame_vid,
    int closed_panel_vid,
    int open_panel_vid,
    uint32_t dir_a,
    uint32_t dir_b,
    bool open_has_panel,
    int expected_flank_per_door = 0
) {
    DoorBuilder builder(5000.0f, 5000.0f);

    std::vector<DoorInstance> doors = {
        {
            {0, 0},
            wall_type,
            0,
            2,
            DOOR_STATE_CLOSED,
            LIGHT_STATE_RED,
            0.0f
        },
        {
            {0, 4},
            wall_type,
            0,
            2,
            DOOR_STATE_OPEN,
            LIGHT_STATE_GREEN,
            0.0f
        },
        {
            {4, 0},
            wall_type,
            1,
            2,
            DOOR_STATE_CLOSED,
            LIGHT_STATE_RED,
            0.0f
        },
        {
            {8, 0},
            wall_type,
            1,
            2,
            DOOR_STATE_OPEN,
            LIGHT_STATE_GREEN,
            0.0f
        }
    };

    std::vector<io::Sprite> sprites = builder.build(doors);

    std::map<std::pair<int, uint32_t>, int> count_by_vid_and_direction;
    for (const io::Sprite& sprite : sprites) {
        count_by_vid_and_direction[{sprite.vid, sprite.direction}] += 1;
    }

    // Each door produces `expected_flank_per_door` extra flank-wall sprites.
    int total_flank = expected_flank_per_door * 4;

    std::pair<int, uint32_t> frame_dir_a = {frame_vid, dir_a};
    std::pair<int, uint32_t> frame_dir_b = {frame_vid, dir_b};
    std::pair<int, uint32_t> closed_panel_dir_a = {closed_panel_vid, dir_a};
    std::pair<int, uint32_t> closed_panel_dir_b = {closed_panel_vid, dir_b};
    std::pair<int, uint32_t> open_panel_dir_a = {open_panel_vid, dir_a};
    std::pair<int, uint32_t> open_panel_dir_b = {open_panel_vid, dir_b};

    if (!open_has_panel) {
        // 4 frames + 2 closed panels + flank walls
        ASSERT_EQ(sprites.size(), static_cast<size_t>(6 + total_flank));
        EXPECT_EQ(count_by_vid_and_direction[frame_dir_a], 2);
        EXPECT_EQ(count_by_vid_and_direction[frame_dir_b], 2);
        EXPECT_EQ(count_by_vid_and_direction[closed_panel_dir_a], 1);
        EXPECT_EQ(count_by_vid_and_direction[closed_panel_dir_b], 1);
        EXPECT_EQ(count_by_vid_and_direction[open_panel_dir_a], 0);
        EXPECT_EQ(count_by_vid_and_direction[open_panel_dir_b], 0);
        return;
    }

    // 4 frames + 4 panels + flank walls
    ASSERT_EQ(sprites.size(), static_cast<size_t>(8 + total_flank));
    EXPECT_EQ(count_by_vid_and_direction[frame_dir_a], 2);
    EXPECT_EQ(count_by_vid_and_direction[frame_dir_b], 2);
    EXPECT_EQ(count_by_vid_and_direction[closed_panel_dir_a], 1);
    EXPECT_EQ(count_by_vid_and_direction[closed_panel_dir_b], 1);
    EXPECT_EQ(count_by_vid_and_direction[open_panel_dir_a], 1);
    EXPECT_EQ(count_by_vid_and_direction[open_panel_dir_b], 1);
}

TEST(DoorBuilderTest, AS2WallSet6LargeDoorUsesSingleFrame) {
    expect_as2_manual_single_frame_large_door_parts(
        WALL_TYPE_AS2_WALL_SET6_RANDOM,
        2604,
        2605,
        2606,
        128,
        0,
        true
    );
}

TEST(DoorBuilderTest, AS2WallSet7OpenDoorKeepsFrameAndOmitsPanel) {
    expect_as2_manual_single_frame_large_door_parts(
        WALL_TYPE_AS2_WALL_SET7_RANDOM,
        2624,
        2645,
        0,
        128,
        51,
        false,
        2
    );
}

TEST(DoorBuilderTest, AS2WallSet7ClosedPanelUsesManualOffsets) {
    DoorBuilder builder(5000.0f, 5000.0f);

    DoorInstance dir_a_door = {
        {0, 0},
        WALL_TYPE_AS2_WALL_SET7_RANDOM,
        0,
        2,
        DOOR_STATE_CLOSED,
        LIGHT_STATE_RED,
        0.0f
    };

    // 1 frame + 1 panel + 2 flank walls = 4 sprites
    std::vector<io::Sprite> sprites = builder.build({dir_a_door});
    ASSERT_EQ(sprites.size(), 4u);
    EXPECT_EQ(sprites[0].vid, 2624);
    EXPECT_EQ(sprites[1].vid, 2645);
    EXPECT_EQ(sprites[0].direction, 128u);
    EXPECT_EQ(sprites[1].direction, 128u);
    EXPECT_FLOAT_EQ(sprites[1].posX - sprites[0].posX, -79.0f);
    EXPECT_FLOAT_EQ(sprites[1].posY - sprites[0].posY, 55.0f);

    DoorInstance dir_b_door = dir_a_door;
    dir_b_door.direction_type = 1;

    std::vector<io::Sprite> dir_b_sprites = builder.build({dir_b_door});
    ASSERT_EQ(dir_b_sprites.size(), 4u);
    EXPECT_EQ(dir_b_sprites[0].vid, 2624);
    EXPECT_EQ(dir_b_sprites[1].vid, 2645);
    EXPECT_EQ(dir_b_sprites[0].direction, 51u);
    EXPECT_EQ(dir_b_sprites[1].direction, 51u);
    EXPECT_FLOAT_EQ(dir_b_sprites[1].posX - dir_b_sprites[0].posX, 6.0f);
    EXPECT_FLOAT_EQ(dir_b_sprites[1].posY - dir_b_sprites[0].posY, -5.0f);
}

TEST(DoorBuilderTest, AS2WallSet9UsesMonolithicDoorNoSeparateFrame) {
    DoorBuilder builder(5000.0f, 5000.0f);

    // 4 doors: dir_a closed, dir_a open, dir_b closed, dir_b open.
    // Each door is a single monolithic sprite (no separate frame) + 2 flank walls.
    std::vector<DoorInstance> doors = {
        {
            {0, 0},
            WALL_TYPE_AS2_WALL_SET9_RANDOM,
            0,
            2,
            DOOR_STATE_CLOSED,
            LIGHT_STATE_RED,
            0.0f
        },
        {
            {0, 4},
            WALL_TYPE_AS2_WALL_SET9_RANDOM,
            0,
            2,
            DOOR_STATE_OPEN,
            LIGHT_STATE_GREEN,
            0.0f
        },
        {
            {4, 0},
            WALL_TYPE_AS2_WALL_SET9_RANDOM,
            1,
            2,
            DOOR_STATE_CLOSED,
            LIGHT_STATE_RED,
            0.0f
        },
        {
            {8, 0},
            WALL_TYPE_AS2_WALL_SET9_RANDOM,
            1,
            2,
            DOOR_STATE_OPEN,
            LIGHT_STATE_GREEN,
            0.0f
        }
    };

    // 4 monolithic door sprites + 4 doors x 2 flank walls = 12
    std::vector<io::Sprite> sprites = builder.build(doors);
    ASSERT_EQ(sprites.size(), 12u);

    std::map<std::pair<int, uint32_t>, int> count_by_vid_and_direction;
    for (const io::Sprite& sprite : sprites) {
        count_by_vid_and_direction[{sprite.vid, sprite.direction}] += 1;
    }

    // Closed door: VID 1731. Open door: VID 1732.
    // dir_a (direction_type 0, "/") -> direction 128.
    // dir_b (direction_type 1, "\") -> direction 0.
    std::pair<int, uint32_t> closed_dir_a = {1731, 128u};
    std::pair<int, uint32_t> open_dir_a   = {1732, 128u};
    std::pair<int, uint32_t> closed_dir_b = {1731,   0u};
    std::pair<int, uint32_t> open_dir_b   = {1732,   0u};

    EXPECT_EQ(count_by_vid_and_direction[closed_dir_a], 1);
    EXPECT_EQ(count_by_vid_and_direction[open_dir_a],   1);
    EXPECT_EQ(count_by_vid_and_direction[closed_dir_b], 1);
    EXPECT_EQ(count_by_vid_and_direction[open_dir_b],   1);
}

TEST(DoorBuilderTest, AS2WallSet9PanelOffsetsAreZeroRelativeToAnchor) {
    DoorBuilder builder(5000.0f, 5000.0f);

    DoorInstance dir_a_door = {
        {0, 0},
        WALL_TYPE_AS2_WALL_SET9_RANDOM,
        0,
        2,
        DOOR_STATE_CLOSED,
        LIGHT_STATE_RED,
        0.0f
    };

    // Monolithic door: first sprite is the door, remaining 2 are flank walls.
    std::vector<io::Sprite> sprites_a = builder.build({dir_a_door});
    ASSERT_EQ(sprites_a.size(), 3u);
    EXPECT_EQ(sprites_a[0].vid, 1731);
    EXPECT_EQ(sprites_a[0].direction, 128u);
    EXPECT_FLOAT_EQ(sprites_a[0].posZ, 0.0f);

    DoorInstance dir_b_door = dir_a_door;
    dir_b_door.direction_type = 1;

    std::vector<io::Sprite> sprites_b = builder.build({dir_b_door});
    ASSERT_EQ(sprites_b.size(), 3u);
    EXPECT_EQ(sprites_b[0].vid, 1731);
    EXPECT_EQ(sprites_b[0].direction, 0u);
    EXPECT_FLOAT_EQ(sprites_b[0].posZ, 0.0f);
}

static void write_as2_manual_single_frame_large_door_map(int wall_type, const std::string& output_name) {
    const float map_size_x = 5000.0f;
    const float map_size_y = 5000.0f;

    std::vector<Segment> segments = {
        {{2, 10}, {2, 22}, wall_type},
        {{2, 22}, {14, 22}, wall_type},
        {{18, 10}, {18, 22}, wall_type},
        {{18, 22}, {30, 22}, wall_type}
    };

    std::vector<DoorInstance> doors = {
        {
            {2, 13},
            wall_type,
            0,
            2,
            DOOR_STATE_CLOSED,
            LIGHT_STATE_RED,
            0.0f
        },
        {
            {2, 18},
            wall_type,
            0,
            2,
            DOOR_STATE_OPEN,
            LIGHT_STATE_GREEN,
            0.0f
        },
        {
            {5, 22},
            wall_type,
            1,
            2,
            DOOR_STATE_CLOSED,
            LIGHT_STATE_RED,
            0.0f
        },
        {
            {10, 22},
            wall_type,
            1,
            2,
            DOOR_STATE_OPEN,
            LIGHT_STATE_GREEN,
            0.0f
        },
        {
            {18, 15},
            wall_type,
            0,
            2,
            DOOR_STATE_OPEN,
            LIGHT_STATE_GREEN,
            0.0f
        },
        {
            {23, 22},
            wall_type,
            1,
            2,
            DOOR_STATE_CLOSED,
            LIGHT_STATE_RED,
            0.0f
        }
    };

    std::vector<DoorExcavation> excavations;
    excavations.reserve(doors.size());
    for (const DoorInstance& door : doors) {
        excavations.push_back({
            door.pos,
            door.direction_type,
            door.size,
            door.wall_type
        });
    }

    WallBuilder wall_builder(map_size_x, map_size_y);
    std::vector<io::Sprite> sprites = wall_builder.build(segments, false, false, excavations);

    DoorBuilder door_builder(map_size_x, map_size_y);
    std::vector<io::Sprite> door_sprites = door_builder.build(doors);
    sprites.insert(sprites.end(), door_sprites.begin(), door_sprites.end());

    std::string output_path = get_test_output_path(output_name);
    bool write_success = io::write_map(
        sprites,
        output_path,
        io::MapFormat::AS2R,
        map_size_x,
        map_size_y
    );

    ASSERT_TRUE(write_success);
}

TEST(DoorBuilderTest, AS2WallSet6AndSet7LargeDoorVariantsWriteCompactLShapeMaps) {
    write_as2_manual_single_frame_large_door_map(
        WALL_TYPE_AS2_WALL_SET6_RANDOM,
        "as2_wall_set6_door_variants.map"
    );
    write_as2_manual_single_frame_large_door_map(
        WALL_TYPE_AS2_WALL_SET7_RANDOM,
        "as2_wall_set7_door_variants.map"
    );
}

TEST(DoorBuilderTest, AS2WallSet8LargeDoorVariantsWriteCompactLShapeMap) {
    write_as2_manual_single_frame_large_door_map(
        WALL_TYPE_AS2_WALL_SET8_RANDOM,
        "as2_wall_set8_door_variants.map"
    );
}

TEST(DoorBuilderTest, AS2WallSet9LargeDoorVariantsWriteCompactLShapeMap) {
    write_as2_manual_single_frame_large_door_map(
        WALL_TYPE_AS2_WALL_SET9_RANDOM,
        "as2_wall_set9_door_variants.map"
    );
}

// --- Door Flank Wall tests ---

/// Set7 door flank walls use variant[1]: dir_a VID=2621 dir=102, dir_b VID=2620 dir=102.
TEST(DoorBuilderTest, AS2WallSet7DoorFlankWallsUseFixedDirection102) {
    DoorBuilder builder(5000.0f, 5000.0f);

    DoorInstance dir_a_door = {
        {0, 0},
        WALL_TYPE_AS2_WALL_SET7_RANDOM,
        0,
        2,
        DOOR_STATE_CLOSED,
        LIGHT_STATE_RED,
        0.0f
    };

    std::vector<io::Sprite> sprites_a = builder.build({dir_a_door});
    // 1 frame + 1 panel + 2 flank walls
    ASSERT_EQ(sprites_a.size(), 4u);

    // Flank walls are the last 2 sprites
    for (int i = 2; i < 4; ++i) {
        EXPECT_EQ(sprites_a[i].vid, 2621)
            << "dir_a flank wall VID should be 2621";
        EXPECT_EQ(sprites_a[i].direction, 102u)
            << "dir_a flank wall direction should be 102";
    }

    DoorInstance dir_b_door = dir_a_door;
    dir_b_door.direction_type = 1;

    std::vector<io::Sprite> sprites_b = builder.build({dir_b_door});
    ASSERT_EQ(sprites_b.size(), 4u);

    for (int i = 2; i < 4; ++i) {
        EXPECT_EQ(sprites_b[i].vid, 2620)
            << "dir_b flank wall VID should be 2620";
        EXPECT_EQ(sprites_b[i].direction, 102u)
            << "dir_b flank wall direction should be 102";
    }
}

/// Set9 door flank walls use variant[2]: dir_a VID=1721 dir=102, dir_b VID=1720 dir=102.
TEST(DoorBuilderTest, AS2WallSet9DoorFlankWallsUseFixedDirection102) {
    DoorBuilder builder(5000.0f, 5000.0f);

    DoorInstance dir_a_door = {
        {0, 0},
        WALL_TYPE_AS2_WALL_SET9_RANDOM,
        0,
        2,
        DOOR_STATE_CLOSED,
        LIGHT_STATE_RED,
        0.0f
    };

    std::vector<io::Sprite> sprites_a = builder.build({dir_a_door});
    // 1 monolithic door + 2 flank walls
    ASSERT_EQ(sprites_a.size(), 3u);

    for (int i = 1; i < 3; ++i) {
        EXPECT_EQ(sprites_a[i].vid, 1721)
            << "dir_a flank wall VID should be 1721";
        EXPECT_EQ(sprites_a[i].direction, 102u)
            << "dir_a flank wall direction should be 102";
    }

    DoorInstance dir_b_door = dir_a_door;
    dir_b_door.direction_type = 1;

    std::vector<io::Sprite> sprites_b = builder.build({dir_b_door});
    ASSERT_EQ(sprites_b.size(), 3u);

    for (int i = 1; i < 3; ++i) {
        EXPECT_EQ(sprites_b[i].vid, 1720)
            << "dir_b flank wall VID should be 1720";
        EXPECT_EQ(sprites_b[i].direction, 102u)
            << "dir_b flank wall direction should be 102";
    }
}

/// WallBuilder excavation should clear flank wall slots (DirA/DirB) but not
/// extra pillars, so DoorBuilder can re-place them with fixed direction.
TEST(DoorBuilderTest, AS2WallSet7ExcavationClearsFlankWallSlots) {
    const float map_size_x = 5000.0f;
    const float map_size_y = 5000.0f;

    // A vertical wall segment long enough to have walls on both sides of the door.
    std::vector<Segment> segments = {
        {{2, 0}, {2, 10}, WALL_TYPE_AS2_WALL_SET7_RANDOM}
    };

    // Large door at (2, 4), direction A (vertical), size 2.
    // Excavation clears (2,4) and (2,5) for the door itself.
    // Flank clear=1 also clears (2,3) and (2,6) DirA wall segments.
    DoorInstance door = {
        {2, 4},
        WALL_TYPE_AS2_WALL_SET7_RANDOM,
        0,
        2,
        DOOR_STATE_CLOSED,
        LIGHT_STATE_RED,
        0.0f
    };

    std::vector<DoorExcavation> excavations = {
        {door.pos, door.direction_type, door.size, door.wall_type}
    };

    WallBuilder wall_builder(map_size_x, map_size_y);
    std::vector<io::Sprite> wall_sprites = wall_builder.build(segments, false, false, excavations);

    // Count DirA wall sprites (VID 2621) at the flank positions.
    // Positions (2,3) and (2,6) should be excavated, so no wall sprite
    // from WallBuilder should appear at those grid coordinates.
    const WallProfile& profile = WallBuilder::get_wall_profile(WALL_TYPE_AS2_WALL_SET7_RANDOM);
    MapPoint shift = WallBuilder::get_wall_shift(map_size_x, profile);

    // Calculate expected positions for flank wall grid points
    MapPoint flank_above = to_iso({2, 3}, profile.step_x, profile.step_y, shift);
    flank_above.x += profile.offset_a_x;
    flank_above.y += profile.offset_a_y;

    MapPoint flank_below = to_iso({2, 6}, profile.step_x, profile.step_y, shift);
    flank_below.x += profile.offset_a_x;
    flank_below.y += profile.offset_a_y;

    // No wall sprite from WallBuilder should be at the flank positions
    // (they were excavated). We check by looking for sprites with VID 2621
    // near those positions.
    for (const io::Sprite& sprite : wall_sprites) {
        if (sprite.vid != 2621) {
            continue;
        }

        float dx_above = std::abs(sprite.posX - flank_above.x);
        float dy_above = std::abs(sprite.posY - flank_above.y);
        float dx_below = std::abs(sprite.posX - flank_below.x);
        float dy_below = std::abs(sprite.posY - flank_below.y);

        bool is_at_flank_above = (dx_above < 1.0f && dy_above < 1.0f);
        bool is_at_flank_below = (dx_below < 1.0f && dy_below < 1.0f);

        EXPECT_FALSE(is_at_flank_above)
            << "WallBuilder should not place wall at flank-above position";
        EXPECT_FALSE(is_at_flank_below)
            << "WallBuilder should not place wall at flank-below position";
    }

    // DoorBuilder should re-place those 2 flank walls with direction 102.
    DoorBuilder door_builder(map_size_x, map_size_y);
    std::vector<io::Sprite> door_sprites = door_builder.build({door});

    int flank_wall_count = 0;
    for (const io::Sprite& sprite : door_sprites) {
        if (sprite.vid == 2621 && sprite.direction == 102u) {
            flank_wall_count += 1;
        }
    }
    EXPECT_EQ(flank_wall_count, 2);
}
