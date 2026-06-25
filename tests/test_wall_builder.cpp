#include <gtest/gtest.h>
#include "auto_mapper/core/wall_builder.h"
#include "auto_mapper/io/map_writer.h"
#include <vector>

using namespace auto_mapper;
using namespace auto_mapper::core;

TEST(WallBuilderTest, BuildCrossShapeStandard) {
    WallBuilder builder(600.0f, 600.0f);

    std::vector<Segment> segments = {
        {{0, 5}, {5, 5}, WALL_TYPE_STANDARD},
        {{2, 0}, {2, 10}, WALL_TYPE_STANDARD}
    };

    std::vector<io::Sprite> sprites = builder.build(segments);

    int num_pillars = 0;
    int num_wall_a = 0;
    int num_wall_b = 0;

    for (const auto& spr : sprites) {
        if (spr.vid == WALL_STANDARD.id_pillar) num_pillars++;
        else if (spr.vid == WALL_STANDARD.id_dir_a) num_wall_a++;
        else if (spr.vid == WALL_STANDARD.id_dir_b) num_wall_b++;
    }

    EXPECT_EQ(num_wall_a, 10);
    EXPECT_EQ(num_wall_b, 5);
    EXPECT_EQ(num_pillars, 5);

    bool success = io::write_map(sprites, "test_wall_builder_output.map");
    EXPECT_TRUE(success);
}

TEST(WallBuilderTest, BuildLineLabWall) {
    WallBuilder builder(600.0f, 600.0f);

    std::vector<Segment> segments = {
        {{0, 0}, {3, 0}, WALL_TYPE_LAB}
    };

    std::vector<io::Sprite> sprites = builder.build(segments);

    int num_walls = 0;
    int num_pillars = 0;

    for (const auto& spr : sprites) {
        if (spr.vid == WALL_LAB.id_dir_b) num_walls++;
        else if (spr.vid == WALL_LAB.id_pillar) num_pillars++;
    }

    EXPECT_EQ(num_walls, 3);
    EXPECT_EQ(num_pillars, 2);
}

TEST(WallBuilderTest, BuildMixedWallTypes) {
    // Mixed: standard + lab walls in the same build
    WallBuilder builder(600.0f, 600.0f);

    std::vector<Segment> segments = {
        {{0, 0}, {3, 0}, WALL_TYPE_STANDARD},  // 3 standard walls
        {{0, 0}, {0, 2}, WALL_TYPE_LAB}         // 2 lab walls
    };

    std::vector<io::Sprite> sprites = builder.build(segments);

    int num_standard = 0;
    int num_lab = 0;

    for (const auto& spr : sprites) {
        if (spr.vid == WALL_STANDARD.id_dir_b) num_standard++;
        if (spr.vid == WALL_LAB.id_dir_a) num_lab++;
    }

    EXPECT_EQ(num_standard, 3);
    EXPECT_EQ(num_lab, 2);
    // Total sprites should include both types + their respective pillars
    EXPECT_GT(sprites.size(), 5u);
}
