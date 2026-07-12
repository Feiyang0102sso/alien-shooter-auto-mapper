#include <gtest/gtest.h>

#include "auto_mapper/core/indoor_decorations/incubator_builder.h"
#include "auto_mapper/io/map_writer.h"
#include "utils/test_utils.h"

#include <cmath>
#include <cstdint>
#include <vector>

using namespace auto_mapper;
using namespace auto_mapper::core::indoor_decorations;
using namespace auto_mapper::test;

float get_test_step_length(float step_x, float step_y, float spacing_scale) {
    return std::sqrt(step_x * step_x + step_y * step_y) * spacing_scale;
}

float get_test_center_offset_distance(float area_length, int slot_count, float step_length, float footprint_length) {
    if (slot_count <= 0) {
        return 0.0f;
    }

    float occupied_length = footprint_length + static_cast<float>(slot_count - 1) * step_length;
    float remaining_length = area_length - occupied_length;
    if (remaining_length <= 0.0f) {
        return 0.0f;
    }

    return remaining_length / 2.0f;
}

float get_test_axis_offset_x(float axis_x, float axis_y, float distance) {
    float axis_length = std::sqrt(axis_x * axis_x + axis_y * axis_y);
    if (axis_length <= 0.0f) {
        return 0.0f;
    }

    return axis_x / axis_length * distance;
}

float get_test_axis_offset_y(float axis_x, float axis_y, float distance) {
    float axis_length = std::sqrt(axis_x * axis_x + axis_y * axis_y);
    if (axis_length <= 0.0f) {
        return 0.0f;
    }

    return axis_y / axis_length * distance;
}

float get_expected_first_item_x(const IncubatorArray& array, int items_per_row, int row_count) {
    float item_step_length = get_test_step_length(
        INCUBATOR_DEFAULT_ROW_SPACING_X,
        INCUBATOR_DEFAULT_ROW_SPACING_Y,
        array.item_spacing_scale
    );
    float row_step_length = get_test_step_length(
        INCUBATOR_DEFAULT_COLUMN_SPACING_X,
        INCUBATOR_DEFAULT_COLUMN_SPACING_Y,
        array.row_spacing_scale
    );
    float item_center_distance = get_test_center_offset_distance(
        array.row_length,
        items_per_row,
        item_step_length,
        INCUBATOR_FOOTPRINT_ROW_LENGTH
    );
    float row_center_distance = get_test_center_offset_distance(
        array.column_length,
        row_count,
        row_step_length,
        INCUBATOR_FOOTPRINT_COLUMN_LENGTH
    );

    float expected_x = array.start_x;
    expected_x += get_test_axis_offset_x(
        INCUBATOR_DEFAULT_ROW_SPACING_X,
        INCUBATOR_DEFAULT_ROW_SPACING_Y,
        item_center_distance
    );
    expected_x += get_test_axis_offset_x(
        INCUBATOR_DEFAULT_COLUMN_SPACING_X,
        INCUBATOR_DEFAULT_COLUMN_SPACING_Y,
        row_center_distance
    );
    expected_x += get_test_axis_offset_x(
        INCUBATOR_DEFAULT_ROW_SPACING_X,
        INCUBATOR_DEFAULT_ROW_SPACING_Y,
        INCUBATOR_FOOTPRINT_ROW_LENGTH / 2.0f
    );
    expected_x += get_test_axis_offset_x(
        INCUBATOR_DEFAULT_COLUMN_SPACING_X,
        INCUBATOR_DEFAULT_COLUMN_SPACING_Y,
        INCUBATOR_FOOTPRINT_COLUMN_LENGTH / 2.0f
    );
    return expected_x;
}

float get_expected_first_item_y(const IncubatorArray& array, int items_per_row, int row_count) {
    float item_step_length = get_test_step_length(
        INCUBATOR_DEFAULT_ROW_SPACING_X,
        INCUBATOR_DEFAULT_ROW_SPACING_Y,
        array.item_spacing_scale
    );
    float row_step_length = get_test_step_length(
        INCUBATOR_DEFAULT_COLUMN_SPACING_X,
        INCUBATOR_DEFAULT_COLUMN_SPACING_Y,
        array.row_spacing_scale
    );
    float item_center_distance = get_test_center_offset_distance(
        array.row_length,
        items_per_row,
        item_step_length,
        INCUBATOR_FOOTPRINT_ROW_LENGTH
    );
    float row_center_distance = get_test_center_offset_distance(
        array.column_length,
        row_count,
        row_step_length,
        INCUBATOR_FOOTPRINT_COLUMN_LENGTH
    );

    float expected_y = array.start_y;
    expected_y += get_test_axis_offset_y(
        INCUBATOR_DEFAULT_ROW_SPACING_X,
        INCUBATOR_DEFAULT_ROW_SPACING_Y,
        item_center_distance
    );
    expected_y += get_test_axis_offset_y(
        INCUBATOR_DEFAULT_COLUMN_SPACING_X,
        INCUBATOR_DEFAULT_COLUMN_SPACING_Y,
        row_center_distance
    );
    expected_y += get_test_axis_offset_y(
        INCUBATOR_DEFAULT_ROW_SPACING_X,
        INCUBATOR_DEFAULT_ROW_SPACING_Y,
        INCUBATOR_FOOTPRINT_ROW_LENGTH / 2.0f
    );
    expected_y += get_test_axis_offset_y(
        INCUBATOR_DEFAULT_COLUMN_SPACING_X,
        INCUBATOR_DEFAULT_COLUMN_SPACING_Y,
        INCUBATOR_FOOTPRINT_COLUMN_LENGTH / 2.0f
    );
    return expected_y;
}

bool contains_direction(const std::vector<int>& directions, uint32_t direction) {
    for (int allowed_direction : directions) {
        if (static_cast<uint32_t>(allowed_direction) == direction) {
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

    float expected_first_x = get_expected_first_item_x(array, 3, 1);
    float expected_first_y = get_expected_first_item_y(array, 3, 1);

    EXPECT_NEAR(sprites[0].posX, expected_first_x, 0.001f);
    EXPECT_NEAR(sprites[0].posY, expected_first_y, 0.001f);

    EXPECT_NEAR(sprites[2].posX, expected_first_x + INCUBATOR_DEFAULT_ROW_SPACING_X, 0.001f);
    EXPECT_NEAR(sprites[2].posY, expected_first_y + INCUBATOR_DEFAULT_ROW_SPACING_Y, 0.001f);

    EXPECT_NEAR(sprites[4].posX, expected_first_x + INCUBATOR_DEFAULT_ROW_SPACING_X * 2.0f, 0.001f);
    EXPECT_NEAR(sprites[4].posY, expected_first_y + INCUBATOR_DEFAULT_ROW_SPACING_Y * 2.0f, 0.001f);
}

TEST(IncubatorBuilderTest, BuildsArraySecondRowWhenAreaGetsDeeper) {
    IncubatorArray array = {
        .start_x = 120.0f,
        .start_y = 780.0f,
        .row_length = 500.0f,
        .column_length = 300.0f,
        .item_spacing_scale = 1.0f,
        .row_spacing_scale = 1.0f
    };

    IncubatorBuilder builder;
    std::vector<io::Sprite> sprites = builder.build_array(array);

    ASSERT_EQ(sprites.size(), 12u);

    float expected_first_x = get_expected_first_item_x(array, 3, 2);
    float expected_first_y = get_expected_first_item_y(array, 3, 2);

    EXPECT_NEAR(sprites[6].posX, expected_first_x + INCUBATOR_DEFAULT_COLUMN_SPACING_X, 0.001f);
    EXPECT_NEAR(sprites[6].posY, expected_first_y + INCUBATOR_DEFAULT_COLUMN_SPACING_Y, 0.001f);
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

TEST(IncubatorBuilderTest, CentersArrayInsideSelectedArea) {
    IncubatorArray array = {
        .start_x = 120.0f,
        .start_y = 780.0f,
        .row_length = 500.0f,
        .column_length = 500.0f,
        .item_spacing_scale = 1.0f,
        .row_spacing_scale = 1.0f
    };

    IncubatorBuilder builder;
    std::vector<io::Sprite> sprites = builder.build_array(array);

    ASSERT_EQ(sprites.size(), 18u);

    float expected_first_x = get_expected_first_item_x(array, 3, 3);
    float expected_first_y = get_expected_first_item_y(array, 3, 3);

    EXPECT_GT(expected_first_x, array.start_x);
    EXPECT_NE(expected_first_y, array.start_y);
    EXPECT_NEAR(sprites[0].posX, expected_first_x, 0.001f);
    EXPECT_NEAR(sprites[0].posY, expected_first_y, 0.001f);
}

TEST(IncubatorBuilderTest, ArrayComputersUseSameDirection) {
    IncubatorArray array = {
        .start_x = 120.0f,
        .start_y = 780.0f,
        .row_length = 500.0f,
        .column_length = 300.0f,
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
        .column_length = 300.0f,
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
        .column_length = 300.0f,
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

TEST(IncubatorBuilderTest, IncubatorSceneGolden) {
    const std::string json_path = resolve_test_path("tests/golden/incubator_builder_gold.json");
    TestScene scene = load_test_scene(json_path);

    ASSERT_GT(scene.segments.size(), 0u);
    ASSERT_GT(scene.decorations.size(), 0u);

    // 1. Build walls and floors (Floor builder style: gen_floor=false, gen_ceiling=false)
    core::WallBuilder wall_builder(scene.map_size_x, scene.map_size_y);
    std::vector<io::Sprite> sprites = wall_builder.build(scene.segments, false, false);

    // 2. Build incubator arrays
    IncubatorBuilder incubator_builder;
    for (const auto& dec : scene.decorations) {
        std::vector<io::Sprite> incubator_sprites = incubator_builder.build_array(dec);
        sprites.insert(sprites.end(), incubator_sprites.begin(), incubator_sprites.end());
    }

    const std::string temp_output_path = "current_incubator_builder.map";
    TempFileCleaner cleaner(temp_output_path);

    // manual verify
    // const std::string temp_output_path = get_project_root() + "/current_incubator_builder.map";
    // TempFileCleaner cleaner(temp_output_path);

    bool write_success = io::write_map(sprites, temp_output_path, scene.map_size_x, scene.map_size_y);
    ASSERT_TRUE(write_success);

    const std::string golden_map_path = resolve_test_path("tests/golden/incubator_builder_gold.map");
    bool files_match = compare_binary_files(temp_output_path, golden_map_path);

    EXPECT_TRUE(files_match);
}
