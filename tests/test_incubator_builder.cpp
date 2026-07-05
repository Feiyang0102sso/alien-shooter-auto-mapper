#include <gtest/gtest.h>

#include "auto_mapper/core/indoor_decorations/incubator_builder.h"
#include "auto_mapper/io/map_writer.h"
#include "utils/test_utils.h"

#include <cstdint>
#include <vector>

using namespace auto_mapper;
using namespace auto_mapper::core::indoor_decorations;
using namespace auto_mapper::test;

bool contains_direction(const std::vector<uint32_t>& directions, uint32_t direction) {
    for (uint32_t allowed_direction : directions) {
        if (allowed_direction == direction) {
            return true;
        }
    }

    return false;
}

TEST(IncubatorBuilderTest, BuildsRequiredIncubatorSprites) {
    IncubatorUnit unit = {
        .pos_x = 180.0f,
        .pos_y = 335.0f,
        .pos_z = 0.0f,
        .with_big_computer = false
    };

    IncubatorBuilder builder;
    std::vector<io::Sprite> sprites = builder.build(unit);

    ASSERT_EQ(sprites.size(), 2u);

    EXPECT_EQ(sprites[0].vid, INCUBATOR_BODY_VID);
    EXPECT_FLOAT_EQ(sprites[0].posX, unit.pos_x);
    EXPECT_FLOAT_EQ(sprites[0].posY, unit.pos_y);
    EXPECT_EQ(sprites[0].direction, INCUBATOR_BODY_DIRECTION);

    EXPECT_EQ(sprites[1].vid, INCUBATOR_AIR_WALL_VID);
    EXPECT_FLOAT_EQ(sprites[1].posX, unit.pos_x);
    EXPECT_FLOAT_EQ(sprites[1].posY, unit.pos_y);
    EXPECT_EQ(sprites[1].direction, INCUBATOR_AIR_WALL_DIRECTION);
}

TEST(IncubatorBuilderTest, AddsOptionalBigComputer) {
    IncubatorUnit unit = {
        .pos_x = 180.0f,
        .pos_y = 335.0f,
        .pos_z = 0.0f,
        .with_big_computer = true,
        .computer_offset_x = 23.0f,
        .computer_offset_y = -14.0f
    };

    IncubatorBuilder builder;
    std::vector<io::Sprite> sprites = builder.build(unit);

    ASSERT_EQ(sprites.size(), 3u);

    EXPECT_EQ(sprites[2].vid, INCUBATOR_BIG_COMPUTER_VID);
    EXPECT_FLOAT_EQ(sprites[2].posX, 203.0f);
    EXPECT_FLOAT_EQ(sprites[2].posY, 321.0f);
    EXPECT_TRUE(contains_direction(INCUBATOR_BIG_COMPUTER_DIRECTIONS, sprites[2].direction));
}

TEST(IncubatorBuilderTest, BuildsHorizontalArrayWithDefaultSpacing) {
    IncubatorArray array = {
        .start_x = 180.0f,
        .start_y = 220.0f,
        .pos_z = 0.0f,
        .count = 3,
        .direction = INCUBATOR_ARRAY_HORIZONTAL,
        .spacing_x = INCUBATOR_DEFAULT_SPACING_X,
        .spacing_y = INCUBATOR_DEFAULT_SPACING_Y,
        .with_big_computer = false
    };

    IncubatorBuilder builder;
    std::vector<io::Sprite> sprites = builder.build_array(array);

    ASSERT_EQ(sprites.size(), 6u);

    EXPECT_FLOAT_EQ(sprites[0].posX, 180.0f);
    EXPECT_FLOAT_EQ(sprites[0].posY, 220.0f);

    EXPECT_FLOAT_EQ(sprites[2].posX, 330.0f);
    EXPECT_FLOAT_EQ(sprites[2].posY, 90.0f);

    EXPECT_FLOAT_EQ(sprites[4].posX, 480.0f);
    EXPECT_FLOAT_EQ(sprites[4].posY, -40.0f);
}

TEST(IncubatorBuilderTest, BuildsVerticalArrayWithCustomSpacing) {
    IncubatorArray array = {
        .start_x = 180.0f,
        .start_y = 220.0f,
        .pos_z = 0.0f,
        .count = 3,
        .direction = INCUBATOR_ARRAY_VERTICAL,
        .spacing_x = 150.0f,
        .spacing_y = 160.0f,
        .with_big_computer = false
    };

    IncubatorBuilder builder;
    std::vector<io::Sprite> sprites = builder.build_array(array);

    ASSERT_EQ(sprites.size(), 6u);

    EXPECT_FLOAT_EQ(sprites[0].posX, 180.0f);
    EXPECT_FLOAT_EQ(sprites[0].posY, 220.0f);

    EXPECT_FLOAT_EQ(sprites[2].posX, 330.0f);
    EXPECT_FLOAT_EQ(sprites[2].posY, 380.0f);

    EXPECT_FLOAT_EQ(sprites[4].posX, 480.0f);
    EXPECT_FLOAT_EQ(sprites[4].posY, 540.0f);
}

TEST(IncubatorBuilderTest, EmptyArrayReturnsNoSprites) {
    IncubatorArray array = {
        .start_x = 180.0f,
        .start_y = 220.0f,
        .pos_z = 0.0f,
        .count = 0
    };

    IncubatorBuilder builder;
    std::vector<io::Sprite> sprites = builder.build_array(array);

    EXPECT_TRUE(sprites.empty());
}

TEST(IncubatorBuilderTest, ComputerDirectionsAreNotAlwaysSame) {
    IncubatorBuilder builder;
    std::vector<io::Sprite> review_sprites;
    bool found_different_direction = false;

    for (int attempt = 0; attempt < 5; ++attempt) {
        uint32_t first_direction = 0;
        bool has_first_direction = false;
        IncubatorArray row_array = {
            .start_x = 120.0f,
            .start_y = 780.0f,
            .pos_z = 0.0f,
            .count = 5,
            .direction = INCUBATOR_ARRAY_HORIZONTAL,
            .spacing_x = INCUBATOR_DEFAULT_SPACING_X,
            .spacing_y = INCUBATOR_DEFAULT_SPACING_Y,
            .with_big_computer = true,
            .computer_offset_x = 23.0f,
            .computer_offset_y = -14.0f
        };

        IncubatorArray second_row_array = {
            .start_x = 270.0f,
            .start_y = 910.0f,
            .pos_z = 0.0f,
            .count = 5,
            .direction = INCUBATOR_ARRAY_HORIZONTAL,
            .spacing_x = INCUBATOR_DEFAULT_SPACING_X,
            .spacing_y = INCUBATOR_DEFAULT_SPACING_Y,
            .with_big_computer = true,
            .computer_offset_x = 23.0f,
            .computer_offset_y = -14.0f
        };

        std::vector<io::Sprite> attempt_sprites = builder.build_array(row_array);
        std::vector<io::Sprite> second_row_sprites = builder.build_array(second_row_array);
        attempt_sprites.insert(attempt_sprites.end(), second_row_sprites.begin(), second_row_sprites.end());

        ASSERT_EQ(attempt_sprites.size(), 30u);

        for (size_t sprite_index = 0; sprite_index < attempt_sprites.size(); ++sprite_index) {
            if (attempt_sprites[sprite_index].vid != INCUBATOR_BIG_COMPUTER_VID) {
                continue;
            }

            EXPECT_TRUE(contains_direction(INCUBATOR_BIG_COMPUTER_DIRECTIONS, attempt_sprites[sprite_index].direction));

            if (!has_first_direction) {
                first_direction = attempt_sprites[sprite_index].direction;
                has_first_direction = true;
                continue;
            }

            if (attempt_sprites[sprite_index].direction != first_direction) {
                found_different_direction = true;
            }
        }

        if (found_different_direction) {
            review_sprites = attempt_sprites;
            break;
        }
    }

    EXPECT_TRUE(found_different_direction);
    ASSERT_FALSE(review_sprites.empty());

    std::string out_map_path = get_project_root() + "/incubator_builder_manual_test.map";
    ASSERT_TRUE(io::write_map(review_sprites, out_map_path, 1200.0f, 1400.0f));
}
