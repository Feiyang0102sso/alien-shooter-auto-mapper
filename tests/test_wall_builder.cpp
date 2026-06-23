#include <gtest/gtest.h>
#include "auto_mapper/core/wall_builder.h"
#include "auto_mapper/io/map_writer.h"
#include <vector>

using namespace auto_mapper;
using namespace auto_mapper::core;

TEST(WallBuilderTest, BuildCrossShapeStandard) {
    // Use standard wall profile (601/602/604, step 40x28)
    WallBuilder builder(WALL_STANDARD, 20, 600.0f, 600.0f);

    std::vector<Segment> segments = {
        {{0, 5}, {5, 5}},
        {{2, 0}, {2, 10}}
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
    // Use lab wall profile (651/650/652, step 90x64)
    WallBuilder builder(WALL_LAB, 20, 600.0f, 600.0f);

    // Simple horizontal line: 3 wall units
    std::vector<Segment> segments = {
        {{0, 0}, {3, 0}}
    };

    std::vector<io::Sprite> sprites = builder.build(segments);

    int num_walls = 0;
    int num_pillars = 0;

    for (const auto& spr : sprites) {
        if (spr.vid == WALL_LAB.id_dir_b) num_walls++;
        else if (spr.vid == WALL_LAB.id_pillar) num_pillars++;
    }

    // 3 wall segments + 2 endpoint pillars
    EXPECT_EQ(num_walls, 3);
    EXPECT_EQ(num_pillars, 2);
}
