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

bool computer_directions_are_uniform(const std::vector<io::Sprite>& sprites) {
    uint32_t first_direction = 0;
    bool has_first_direction = false;

    for (const io::Sprite& sprite : sprites) {
        if (sprite.vid != INCUBATOR_BIG_COMPUTER_VID) {
            continue;
        }

        if (!contains_direction(INCUBATOR_BIG_COMPUTER_DIRECTIONS, sprite.direction)) {
            return false;
        }

        if (!has_first_direction) {
            first_direction = sprite.direction;
            has_first_direction = true;
            continue;
        }

        if (sprite.direction != first_direction) {
            return false;
        }
    }

    return has_first_direction;
}

TEST(IncubatorBuilderTest, BuildsRequiredIncubatorSprites) {
    IncubatorUnit unit = {
        .pos_x = 180.0f,
        .pos_y = 335.0f
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
        .options = {
            .pos_z = 0.0f,
            .with_big_computer = true,
            .computer_offset_x = 23.0f,
            .computer_offset_y = -14.0f
        }
    };

    IncubatorBuilder builder;
    std::vector<io::Sprite> sprites = builder.build(unit);

    ASSERT_EQ(sprites.size(), 3u);

    EXPECT_EQ(sprites[2].vid, INCUBATOR_BIG_COMPUTER_VID);
    EXPECT_FLOAT_EQ(sprites[2].posX, 203.0f);
    EXPECT_FLOAT_EQ(sprites[2].posY, 321.0f);
    EXPECT_TRUE(contains_direction(INCUBATOR_BIG_COMPUTER_DIRECTIONS, sprites[2].direction));
}

TEST(IncubatorBuilderTest, BuildsArrayAsSingleRowForFlatArea) {
    IncubatorArray array = {
        .start_x = 120.0f,
        .start_y = 780.0f,
        .row_length = 500.0f,
        .column_length = 0.0f,
        .item_spacing_scale = 1.0f,
        .row_spacing_scale = 1.0f
    };

    IncubatorBuilder builder;
    std::vector<io::Sprite> sprites = builder.build_array(array);

    ASSERT_EQ(sprites.size(), 6u);

    EXPECT_FLOAT_EQ(sprites[0].posX, 120.0f);
    EXPECT_FLOAT_EQ(sprites[0].posY, 780.0f);

    EXPECT_FLOAT_EQ(sprites[2].posX, 120.0f + INCUBATOR_DEFAULT_ROW_SPACING_X);
    EXPECT_FLOAT_EQ(sprites[2].posY, 780.0f + INCUBATOR_DEFAULT_ROW_SPACING_Y);

    EXPECT_FLOAT_EQ(sprites[4].posX, 120.0f + INCUBATOR_DEFAULT_ROW_SPACING_X * 2.0f);
    EXPECT_FLOAT_EQ(sprites[4].posY, 780.0f + INCUBATOR_DEFAULT_ROW_SPACING_Y * 2.0f);
}

TEST(IncubatorBuilderTest, BuildsArraySecondRowWhenAreaGetsDeeper) {
    IncubatorArray array = {
        .start_x = 120.0f,
        .start_y = 780.0f,
        .row_length = 500.0f,
        .column_length = 250.0f,
        .item_spacing_scale = 1.0f,
        .row_spacing_scale = 1.0f
    };

    IncubatorBuilder builder;
    std::vector<io::Sprite> sprites = builder.build_array(array);

    ASSERT_EQ(sprites.size(), 12u);

    EXPECT_FLOAT_EQ(sprites[6].posX, 120.0f + INCUBATOR_DEFAULT_COLUMN_SPACING_X);
    EXPECT_FLOAT_EQ(sprites[6].posY, 780.0f + INCUBATOR_DEFAULT_COLUMN_SPACING_Y);
}

TEST(IncubatorBuilderTest, LargerItemSpacingReducesItemsPerRow) {
    IncubatorArray compact_array = {
        .start_x = 120.0f,
        .start_y = 780.0f,
        .row_length = 500.0f,
        .column_length = 0.0f,
        .item_spacing_scale = 1.0f,
        .row_spacing_scale = 1.0f
    };

    IncubatorArray wide_spacing_array = {
        .start_x = 120.0f,
        .start_y = 780.0f,
        .row_length = 500.0f,
        .column_length = 0.0f,
        .item_spacing_scale = 2.0f,
        .row_spacing_scale = 1.0f
    };

    IncubatorBuilder builder;
    std::vector<io::Sprite> compact_sprites = builder.build_array(compact_array);
    std::vector<io::Sprite> wide_spacing_sprites = builder.build_array(wide_spacing_array);

    EXPECT_EQ(compact_sprites.size(), 6u);
    EXPECT_EQ(wide_spacing_sprites.size(), 4u);
}

TEST(IncubatorBuilderTest, LargerRowSpacingReducesRows) {
    IncubatorArray compact_array = {
        .start_x = 120.0f,
        .start_y = 780.0f,
        .row_length = 100.0f,
        .column_length = 500.0f,
        .item_spacing_scale = 1.0f,
        .row_spacing_scale = 1.0f
    };

    IncubatorArray wide_spacing_array = {
        .start_x = 120.0f,
        .start_y = 780.0f,
        .row_length = 100.0f,
        .column_length = 500.0f,
        .item_spacing_scale = 1.0f,
        .row_spacing_scale = 2.0f
    };

    IncubatorBuilder builder;
    std::vector<io::Sprite> compact_sprites = builder.build_array(compact_array);
    std::vector<io::Sprite> wide_spacing_sprites = builder.build_array(wide_spacing_array);

    EXPECT_EQ(compact_sprites.size(), 6u);
    EXPECT_EQ(wide_spacing_sprites.size(), 4u);
}

TEST(IncubatorBuilderTest, ArrayComputersUseSameDirection) {
    IncubatorArray array = {
        .start_x = 120.0f,
        .start_y = 780.0f,
        .row_length = 500.0f,
        .column_length = 250.0f,
        .item_spacing_scale = 1.0f,
        .row_spacing_scale = 1.0f,
        .options = {
            .pos_z = 0.0f,
            .with_big_computer = true,
            .computer_offset_x = 23.0f,
            .computer_offset_y = -14.0f
        }
    };

    IncubatorBuilder builder;
    std::vector<io::Sprite> sprites = builder.build_array(array);

    ASSERT_EQ(sprites.size(), 18u);

    uint32_t first_computer_direction = 0;
    bool has_first_computer_direction = false;

    for (const io::Sprite& sprite : sprites) {
        if (sprite.vid != INCUBATOR_BIG_COMPUTER_VID) {
            continue;
        }

        EXPECT_TRUE(contains_direction(INCUBATOR_BIG_COMPUTER_DIRECTIONS, sprite.direction));

        if (!has_first_computer_direction) {
            first_computer_direction = sprite.direction;
            has_first_computer_direction = true;
            continue;
        }

        EXPECT_EQ(sprite.direction, first_computer_direction);
    }

    EXPECT_TRUE(has_first_computer_direction);
}

TEST(IncubatorBuilderTest, RandomizesComputerPresenceForWholeArray) {
    IncubatorArray array = {
        .start_x = 120.0f,
        .start_y = 780.0f,
        .row_length = 500.0f,
        .column_length = 250.0f,
        .item_spacing_scale = 1.0f,
        .row_spacing_scale = 1.0f,
        .randomize_big_computer = true,
        .options = {
            .pos_z = 0.0f,
            .with_big_computer = false,
            .computer_offset_x = 23.0f,
            .computer_offset_y = -14.0f
        }
    };

    IncubatorBuilder builder;
    bool found_array_without_computers = false;
    bool found_array_with_computers = false;

    for (int attempt = 0; attempt < 20; ++attempt) {
        std::vector<io::Sprite> sprites = builder.build_array(array);

        size_t computer_count = 0;
        uint32_t first_computer_direction = 0;
        bool has_first_computer_direction = false;

        for (const io::Sprite& sprite : sprites) {
            if (sprite.vid != INCUBATOR_BIG_COMPUTER_VID) {
                continue;
            }

            ++computer_count;

            if (!has_first_computer_direction) {
                first_computer_direction = sprite.direction;
                has_first_computer_direction = true;
                continue;
            }

            EXPECT_EQ(sprite.direction, first_computer_direction);
        }

        if (computer_count == 0u) {
            found_array_without_computers = true;
        }

        if (computer_count == 6u) {
            found_array_with_computers = true;
        }
    }

    EXPECT_TRUE(found_array_without_computers);
    EXPECT_TRUE(found_array_with_computers);
}

TEST(IncubatorBuilderTest, EmptyArrayReturnsNoSprites) {
    IncubatorArray array = {
        .start_x = 120.0f,
        .start_y = 780.0f,
        .row_length = -1.0f,
        .column_length = 0.0f
    };

    IncubatorBuilder builder;
    std::vector<io::Sprite> sprites = builder.build_array(array);

    EXPECT_TRUE(sprites.empty());
}

TEST(IncubatorBuilderTest, WritesManualSpacingPreviewMap) {
    IncubatorBuilder builder;

    IncubatorArray compact_array = {
        .start_x = 120.0f,
        .start_y = 780.0f,
        .row_length = 900.0f,
        .column_length = 250.0f,
        .item_spacing_scale = 1.0f,
        .row_spacing_scale = 1.0f,
        .options = {
            .pos_z = 0.0f,
            .with_big_computer = true,
            .computer_offset_x = 23.0f,
            .computer_offset_y = -14.0f
        }
    };

    IncubatorArray wide_spacing_array = {
        .start_x = 120.0f,
        .start_y = 1180.0f,
        .row_length = 900.0f,
        .column_length = 250.0f,
        .item_spacing_scale = 2.0f,
        .row_spacing_scale = 2.0f,
        .options = {
            .pos_z = 0.0f,
            .with_big_computer = true,
            .computer_offset_x = 23.0f,
            .computer_offset_y = -14.0f
        }
    };

    std::vector<io::Sprite> compact_sprites = builder.build_array(compact_array);
    std::vector<io::Sprite> wide_spacing_sprites = builder.build_array(wide_spacing_array);

    ASSERT_EQ(compact_sprites.size(), 30u);
    ASSERT_EQ(wide_spacing_sprites.size(), 9u);
    EXPECT_TRUE(computer_directions_are_uniform(compact_sprites));
    EXPECT_TRUE(computer_directions_are_uniform(wide_spacing_sprites));

    compact_sprites.insert(compact_sprites.end(), wide_spacing_sprites.begin(), wide_spacing_sprites.end());

    std::string out_map_path = get_project_root() + "/incubator_builder_manual_test.map";
    ASSERT_TRUE(io::write_map(compact_sprites, out_map_path, 1200.0f, 1400.0f));
}
