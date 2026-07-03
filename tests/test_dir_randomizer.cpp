#include <gtest/gtest.h>

#include "auto_mapper/core/dir_randomizer.h"
#include "auto_mapper/core/door_builder.h"
#include "auto_mapper/core/wall_builder.h"
#include "auto_mapper/io/map_writer.h"
#include "utils/test_utils.h"

#include <vector>

using namespace auto_mapper;
using namespace auto_mapper::core;
using namespace auto_mapper::test;

TEST(SpriteDirectionRandomizerTest, RandomizesExpectedSprites) {
    float map_size_x = 1200.0f;
    float map_size_y = 900.0f;

    std::vector<Segment> segments = {
        {{0, 0}, {6, 0}, WALL_TYPE_STANDARD},
        {{6, 0}, {6, 5}, WALL_TYPE_STANDARD},
        {{6, 5}, {0, 5}, WALL_TYPE_STANDARD},
        {{0, 5}, {0, 0}, WALL_TYPE_STANDARD},
    };

    std::vector<DoorInstance> doors = {
        {{2, 0}, WALL_TYPE_STANDARD, 1, 1, DOOR_STATE_CLOSED, LIGHT_STATE_RED, 0.0f},
    };

    std::vector<DoorExcavation> excavations = {
        {{2, 0}, 1, 1, WALL_TYPE_STANDARD},
    };

    WallBuilder wall_builder(map_size_x, map_size_y);
    std::vector<io::Sprite> sprites = wall_builder.build(segments, true, false, excavations);

    DoorBuilder door_builder(map_size_x, map_size_y);
    std::vector<io::Sprite> door_sprites = door_builder.build(doors);
    size_t door_start_index = sprites.size();
    sprites.insert(sprites.end(), door_sprites.begin(), door_sprites.end());

    randomize_wall_and_floor_directions(sprites);

    bool all_randomized_directions_same = true;
    uint32_t first_direction = sprites[0].direction;
    for (size_t index = 0; index < door_start_index; ++index) {
        EXPECT_LE(sprites[index].direction, 255u);
        if (sprites[index].direction != first_direction) {
            all_randomized_directions_same = false;
        }
    }

    EXPECT_FALSE(all_randomized_directions_same);

    for (size_t index = door_start_index; index < sprites.size(); ++index) {
        size_t door_index = index - door_start_index;
        EXPECT_EQ(sprites[index].direction, door_sprites[door_index].direction);
    }

    // in case there need to be manually verified
    std::string out_map_path = get_project_root() + "/random_direction_manual_test.map";
    ASSERT_TRUE(io::write_map(sprites, out_map_path, map_size_x, map_size_y));
}
