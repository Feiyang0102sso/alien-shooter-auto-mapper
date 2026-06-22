#include <gtest/gtest.h>
#include "auto_mapper/core/wall_builder.h"
#include "auto_mapper/io/map_writer.h"
#include <vector>

using namespace auto_mapper;
using namespace auto_mapper::core;

TEST(WallBuilderTest, BuildCrossShape) {
    WallBuilder builder(20, 600.0f, 600.0f);

    std::vector<Segment> segments = {
        {{0, 5}, {5, 5}},
        {{2, 0}, {2, 10}}
    };

    std::vector<io::Sprite> sprites = builder.build(segments);

    int num_pillars = 0;
    int num_wall_a = 0;
    int num_wall_b = 0;

    for (const auto& spr : sprites) {
        if (spr.vid == 604) num_pillars++;
        else if (spr.vid == 601) num_wall_a++;
        else if (spr.vid == 602) num_wall_b++;
    }

    EXPECT_EQ(num_wall_a, 10);
    EXPECT_EQ(num_wall_b, 5);
    EXPECT_EQ(num_pillars, 5);

    bool success = io::write_map(sprites, "test_wall_builder_output.map");
    EXPECT_TRUE(success);
}
