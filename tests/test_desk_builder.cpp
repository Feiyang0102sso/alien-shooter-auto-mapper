#include <gtest/gtest.h>

#include "auto_mapper/core/indoor_decorations/desk_builder.h"
#include "auto_mapper/io/map_writer.h"
#include "utils/test_utils.h"

#include <cmath>
#include <string>
#include <vector>

using namespace auto_mapper;
using namespace auto_mapper::core::indoor_decorations;
using namespace auto_mapper::test;

namespace {

DeskUnit get_fixed_test_unit() {
    return {
        .pos_x = 470.0f,
        .pos_y = 430.0f,
        .pos_z = 0.0f,
    };
}

bool is_computer_vid(int vid) {
    if (vid == DESK_BROKEN_COMPUTER_VID) {
        return true;
    }

    if (vid == DESK_ENABLED_COMPUTER_VID) {
        return true;
    }

    return false;
}

bool is_valid_computer_direction(uint32_t direction) {
    if (direction == 0u) {
        return true;
    }

    if (direction == 128u) {
        return true;
    }

    return false;
}

bool is_valid_desk_direction(uint32_t direction) {
    if (direction == 0u) {
        return true;
    }

    if (direction == 85u) {
        return true;
    }

    if (direction == 170u) {
        return true;
    }

    return false;
}

bool is_default_template_direction(uint32_t direction) {
    return is_valid_desk_direction(direction);
}

bool is_up_left_template_direction(uint32_t direction) {
    if (direction == 42u) {
        return true;
    }

    if (direction == 128u) {
        return true;
    }

    if (direction == 213u) {
        return true;
    }

    return false;
}

bool is_down_left_template_direction(uint32_t direction) {
    if (direction == 21u) {
        return true;
    }

    if (direction == 106u) {
        return true;
    }

    if (direction == 192u) {
        return true;
    }

    return false;
}

bool is_up_right_template_direction(uint32_t direction) {
    if (direction == 64u) {
        return true;
    }

    if (direction == 149u) {
        return true;
    }

    if (direction == 234u) {
        return true;
    }

    return false;
}

bool is_valid_array_desk_direction(uint32_t direction) {
    if (is_default_template_direction(direction)) {
        return true;
    }

    if (is_up_left_template_direction(direction)) {
        return true;
    }

    if (is_down_left_template_direction(direction)) {
        return true;
    }

    if (is_up_right_template_direction(direction)) {
        return true;
    }

    return false;
}

bool is_up_left_computer_direction(uint32_t direction) {
    if (direction == 64u) {
        return true;
    }

    if (direction == 192u) {
        return true;
    }

    return false;
}

bool is_down_left_computer_direction(uint32_t direction) {
    if (direction == 32u) {
        return true;
    }

    if (direction == 160u) {
        return true;
    }

    return false;
}

bool is_up_right_computer_direction(uint32_t direction) {
    if (direction == 96u) {
        return true;
    }

    if (direction == 224u) {
        return true;
    }

    return false;
}

bool is_sideways_template_direction(uint32_t direction) {
    if (is_down_left_template_direction(direction)) {
        return true;
    }

    if (is_up_right_template_direction(direction)) {
        return true;
    }

    return false;
}

float get_test_step_length(float step_x, float step_y, float spacing_scale) {
    return std::sqrt(step_x * step_x + step_y * step_y) * spacing_scale;
}

int get_test_slot_count(float area_length, float step_x, float step_y, float spacing_scale, float footprint_length) {
    if (area_length < 0.0f) {
        return 0;
    }

    if (spacing_scale <= 0.0f) {
        return 0;
    }

    float step_length = get_test_step_length(step_x, step_y, spacing_scale);
    if (step_length <= 0.0f) {
        return 0;
    }

    if (area_length <= footprint_length) {
        return 1;
    }

    float anchor_area_length = area_length - footprint_length;
    return static_cast<int>(std::floor(anchor_area_length / step_length)) + 1;
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

float get_expected_first_desk_x(
    const DeskArray& array,
    int items_per_row,
    int row_count,
    float footprint_row_length = DESK_FOOTPRINT_ROW_LENGTH,
    float footprint_column_length = DESK_FOOTPRINT_COLUMN_LENGTH
) {
    float item_step_length = get_test_step_length(
        DESK_DEFAULT_ROW_SPACING_X,
        DESK_DEFAULT_ROW_SPACING_Y,
        array.item_spacing_scale
    );
    float row_step_length = get_test_step_length(
        DESK_DEFAULT_COLUMN_SPACING_X,
        DESK_DEFAULT_COLUMN_SPACING_Y,
        array.row_spacing_scale
    );
    float item_center_distance = get_test_center_offset_distance(
        array.row_length,
        items_per_row,
        item_step_length,
        footprint_row_length
    );
    float row_center_distance = get_test_center_offset_distance(
        array.column_length,
        row_count,
        row_step_length,
        footprint_column_length
    );

    float expected_x = array.start_x;
    expected_x += get_test_axis_offset_x(
        DESK_DEFAULT_ROW_SPACING_X,
        DESK_DEFAULT_ROW_SPACING_Y,
        item_center_distance
    );
    expected_x += get_test_axis_offset_x(
        DESK_DEFAULT_COLUMN_SPACING_X,
        DESK_DEFAULT_COLUMN_SPACING_Y,
        row_center_distance
    );
    expected_x += get_test_axis_offset_x(
        DESK_DEFAULT_ROW_SPACING_X,
        DESK_DEFAULT_ROW_SPACING_Y,
        footprint_row_length / 2.0f
    );
    expected_x += get_test_axis_offset_x(
        DESK_DEFAULT_COLUMN_SPACING_X,
        DESK_DEFAULT_COLUMN_SPACING_Y,
        footprint_column_length / 2.0f
    );
    return expected_x;
}

float get_expected_first_desk_y(
    const DeskArray& array,
    int items_per_row,
    int row_count,
    float footprint_row_length = DESK_FOOTPRINT_ROW_LENGTH,
    float footprint_column_length = DESK_FOOTPRINT_COLUMN_LENGTH
) {
    float item_step_length = get_test_step_length(
        DESK_DEFAULT_ROW_SPACING_X,
        DESK_DEFAULT_ROW_SPACING_Y,
        array.item_spacing_scale
    );
    float row_step_length = get_test_step_length(
        DESK_DEFAULT_COLUMN_SPACING_X,
        DESK_DEFAULT_COLUMN_SPACING_Y,
        array.row_spacing_scale
    );
    float item_center_distance = get_test_center_offset_distance(
        array.row_length,
        items_per_row,
        item_step_length,
        footprint_row_length
    );
    float row_center_distance = get_test_center_offset_distance(
        array.column_length,
        row_count,
        row_step_length,
        footprint_column_length
    );

    float expected_y = array.start_y;
    expected_y += get_test_axis_offset_y(
        DESK_DEFAULT_ROW_SPACING_X,
        DESK_DEFAULT_ROW_SPACING_Y,
        item_center_distance
    );
    expected_y += get_test_axis_offset_y(
        DESK_DEFAULT_COLUMN_SPACING_X,
        DESK_DEFAULT_COLUMN_SPACING_Y,
        row_center_distance
    );
    expected_y += get_test_axis_offset_y(
        DESK_DEFAULT_ROW_SPACING_X,
        DESK_DEFAULT_ROW_SPACING_Y,
        footprint_row_length / 2.0f
    );
    expected_y += get_test_axis_offset_y(
        DESK_DEFAULT_COLUMN_SPACING_X,
        DESK_DEFAULT_COLUMN_SPACING_Y,
        footprint_column_length / 2.0f
    );
    return expected_y;
}

float get_template_footprint_row_length(uint32_t body_direction) {
    if (is_sideways_template_direction(body_direction)) {
        return DESK_SIDEWAYS_FOOTPRINT_ROW_LENGTH;
    }

    return DESK_FOOTPRINT_ROW_LENGTH;
}

float get_template_footprint_column_length(uint32_t body_direction) {
    if (is_sideways_template_direction(body_direction)) {
        return DESK_SIDEWAYS_FOOTPRINT_COLUMN_LENGTH;
    }

    return DESK_FOOTPRINT_COLUMN_LENGTH;
}

} // namespace

TEST(DeskBuilderTest, DefaultTemplateUsesThreeDeskDirections) {
    const DeskTemplate& desk_template = get_default_desk_template();

    EXPECT_EQ(desk_template.body_vid, DESK_BODY_VID);
    ASSERT_EQ(desk_template.body_directions.size(), 3u);
    EXPECT_EQ(desk_template.body_directions[0], 0u);
    EXPECT_EQ(desk_template.body_directions[1], 85u);
    EXPECT_EQ(desk_template.body_directions[2], 170u);

    ASSERT_EQ(desk_template.computer_vids.size(), 2u);
    EXPECT_EQ(desk_template.computer_vids[0], DESK_BROKEN_COMPUTER_VID);
    EXPECT_EQ(desk_template.computer_vids[1], DESK_ENABLED_COMPUTER_VID);

    ASSERT_EQ(desk_template.computer_directions.size(), 2u);
    EXPECT_EQ(desk_template.computer_directions[0], 0u);
    EXPECT_EQ(desk_template.computer_directions[1], 128u);
}

TEST(DeskBuilderTest, BuildsDeskComputerAndChairCombo) {
    DeskBuilder builder;
    DeskUnit unit = get_fixed_test_unit();

    for (int iteration = 0; iteration < 30; ++iteration) {
        std::vector<io::Sprite> sprites = builder.build(unit);

        ASSERT_EQ(sprites.size(), 3u);

        const io::Sprite& desk = sprites[0];
        EXPECT_EQ(desk.vid, DESK_BODY_VID);
        EXPECT_EQ(desk.posX, unit.pos_x);
        EXPECT_EQ(desk.posY, unit.pos_y);
        EXPECT_EQ(desk.posZ, unit.pos_z);
        EXPECT_TRUE(is_valid_desk_direction(desk.direction));

        const io::Sprite& computer = sprites[1];
        EXPECT_TRUE(is_computer_vid(computer.vid));
        EXPECT_GE(computer.posX, unit.pos_x - 13.0f);
        EXPECT_LE(computer.posX, unit.pos_x - 3.0f);
        EXPECT_GE(computer.posY, unit.pos_y);
        EXPECT_LE(computer.posY, unit.pos_y + 4.0f);
        EXPECT_EQ(computer.posZ, DESK_COMPUTER_POS_Z);
        EXPECT_TRUE(is_valid_computer_direction(computer.direction));

        const io::Sprite& chair = sprites[2];
        EXPECT_EQ(chair.vid, DESK_CHAIR_VID);
        EXPECT_GE(chair.posX, unit.pos_x + 25.0f);
        EXPECT_LE(chair.posX, unit.pos_x + 39.0f);
        EXPECT_GE(chair.posY, unit.pos_y + 28.0f);
        EXPECT_LE(chair.posY, unit.pos_y + 42.0f);
        EXPECT_EQ(chair.posZ, DESK_CHAIR_POS_Z);
        EXPECT_LE(chair.direction, 255u);
    }
}

TEST(DeskBuilderTest, UpLeftTemplateUsesExtractedDirections) {
    const DeskTemplate& desk_template = get_up_left_desk_template();

    EXPECT_EQ(desk_template.body_vid, DESK_BODY_VID);
    ASSERT_EQ(desk_template.body_directions.size(), 3u);
    EXPECT_EQ(desk_template.body_directions[0], 42u);
    EXPECT_EQ(desk_template.body_directions[1], 128u);
    EXPECT_EQ(desk_template.body_directions[2], 213u);

    ASSERT_EQ(desk_template.computer_vids.size(), 2u);
    EXPECT_EQ(desk_template.computer_vids[0], DESK_BROKEN_COMPUTER_VID);
    EXPECT_EQ(desk_template.computer_vids[1], DESK_ENABLED_COMPUTER_VID);

    ASSERT_EQ(desk_template.computer_directions.size(), 2u);
    EXPECT_EQ(desk_template.computer_directions[0], 64u);
    EXPECT_EQ(desk_template.computer_directions[1], 192u);

    ASSERT_EQ(desk_template.chair_directions.size(), 3u);
    EXPECT_EQ(desk_template.chair_directions[0], 85u);
    EXPECT_EQ(desk_template.chair_directions[1], 128u);
    EXPECT_EQ(desk_template.chair_directions[2], 213u);

    EXPECT_EQ(desk_template.chair_offset_range.min_x, -60.0f);
    EXPECT_EQ(desk_template.chair_offset_range.max_x, -31.0f);
    EXPECT_EQ(desk_template.chair_offset_range.min_y, -38.0f);
    EXPECT_EQ(desk_template.chair_offset_range.max_y, -26.0f);
}

TEST(DeskBuilderTest, DownLeftTemplateUsesExtractedDirectionsAndSidewaysFootprint) {
    const DeskTemplate& desk_template = get_down_left_desk_template();

    EXPECT_EQ(desk_template.body_vid, DESK_BODY_VID);
    ASSERT_EQ(desk_template.body_directions.size(), 3u);
    EXPECT_EQ(desk_template.body_directions[0], 21u);
    EXPECT_EQ(desk_template.body_directions[1], 106u);
    EXPECT_EQ(desk_template.body_directions[2], 192u);

    ASSERT_EQ(desk_template.computer_vids.size(), 2u);
    EXPECT_EQ(desk_template.computer_vids[0], DESK_BROKEN_COMPUTER_VID);
    EXPECT_EQ(desk_template.computer_vids[1], DESK_ENABLED_COMPUTER_VID);

    ASSERT_EQ(desk_template.computer_directions.size(), 2u);
    EXPECT_EQ(desk_template.computer_directions[0], 32u);
    EXPECT_EQ(desk_template.computer_directions[1], 160u);

    ASSERT_EQ(desk_template.chair_directions.size(), 4u);
    EXPECT_EQ(desk_template.chair_directions[0], 0u);
    EXPECT_EQ(desk_template.chair_directions[1], 42u);
    EXPECT_EQ(desk_template.chair_directions[2], 85u);
    EXPECT_EQ(desk_template.chair_directions[3], 128u);

    EXPECT_EQ(desk_template.computer_offset_range.min_x, -7.0f);
    EXPECT_EQ(desk_template.computer_offset_range.max_x, 2.0f);
    EXPECT_EQ(desk_template.computer_offset_range.min_y, -4.0f);
    EXPECT_EQ(desk_template.computer_offset_range.max_y, 3.0f);

    EXPECT_EQ(desk_template.chair_offset_range.min_x, -55.0f);
    EXPECT_EQ(desk_template.chair_offset_range.max_x, -8.0f);
    EXPECT_EQ(desk_template.chair_offset_range.min_y, 28.0f);
    EXPECT_EQ(desk_template.chair_offset_range.max_y, 53.0f);

    EXPECT_EQ(desk_template.footprint_row_length, DESK_SIDEWAYS_FOOTPRINT_ROW_LENGTH);
    EXPECT_EQ(desk_template.footprint_column_length, DESK_SIDEWAYS_FOOTPRINT_COLUMN_LENGTH);
}

TEST(DeskBuilderTest, UpRightTemplateUsesExtractedDirectionsAndSidewaysFootprint) {
    const DeskTemplate& desk_template = get_up_right_desk_template();

    EXPECT_EQ(desk_template.body_vid, DESK_BODY_VID);
    ASSERT_EQ(desk_template.body_directions.size(), 3u);
    EXPECT_EQ(desk_template.body_directions[0], 64u);
    EXPECT_EQ(desk_template.body_directions[1], 149u);
    EXPECT_EQ(desk_template.body_directions[2], 234u);

    ASSERT_EQ(desk_template.computer_vids.size(), 2u);
    EXPECT_EQ(desk_template.computer_vids[0], DESK_BROKEN_COMPUTER_VID);
    EXPECT_EQ(desk_template.computer_vids[1], DESK_ENABLED_COMPUTER_VID);

    ASSERT_EQ(desk_template.computer_directions.size(), 2u);
    EXPECT_EQ(desk_template.computer_directions[0], 96u);
    EXPECT_EQ(desk_template.computer_directions[1], 224u);

    ASSERT_EQ(desk_template.chair_directions.size(), 4u);
    EXPECT_EQ(desk_template.chair_directions[0], 42u);
    EXPECT_EQ(desk_template.chair_directions[1], 85u);
    EXPECT_EQ(desk_template.chair_directions[2], 128u);
    EXPECT_EQ(desk_template.chair_directions[3], 170u);

    EXPECT_EQ(desk_template.computer_offset_range.min_x, -7.0f);
    EXPECT_EQ(desk_template.computer_offset_range.max_x, 2.0f);
    EXPECT_EQ(desk_template.computer_offset_range.min_y, -4.0f);
    EXPECT_EQ(desk_template.computer_offset_range.max_y, 3.0f);

    EXPECT_EQ(desk_template.chair_offset_range.min_x, 37.0f);
    EXPECT_EQ(desk_template.chair_offset_range.max_x, 53.0f);
    EXPECT_EQ(desk_template.chair_offset_range.min_y, -39.0f);
    EXPECT_EQ(desk_template.chair_offset_range.max_y, -17.0f);

    EXPECT_EQ(desk_template.footprint_row_length, DESK_SIDEWAYS_FOOTPRINT_ROW_LENGTH);
    EXPECT_EQ(desk_template.footprint_column_length, DESK_SIDEWAYS_FOOTPRINT_COLUMN_LENGTH);
}

TEST(DeskBuilderTest, RandomDeskDirectionUsesAllowedDirections) {
    DeskBuilder builder;

    for (int iteration = 0; iteration < 30; ++iteration) {
        DeskUnit unit = {
            .pos_x = 390.0f,
            .pos_y = 410.0f,
        };

        std::vector<io::Sprite> sprites = builder.build(unit);

        ASSERT_EQ(sprites.size(), 3u);
        EXPECT_TRUE(is_valid_desk_direction(sprites[0].direction));
    }
}

TEST(DeskBuilderTest, BuildsArrayAsSingleRowForFlatArea) {
    DeskArray array = {
        .start_x = 620.0f,
        .start_y = 220.0f,
        .row_length = 500.0f,
        .column_length = 0.0f,
        .item_spacing_scale = 1.0f,
        .row_spacing_scale = 1.0f
    };

    DeskBuilder builder;
    std::vector<io::Sprite> sprites = builder.build_array(array);

    ASSERT_FALSE(sprites.empty());
    uint32_t array_body_direction = sprites[0].direction;
    float footprint_row_length = get_template_footprint_row_length(array_body_direction);
    float footprint_column_length = get_template_footprint_column_length(array_body_direction);
    int items_per_row = get_test_slot_count(
        array.row_length,
        DESK_DEFAULT_ROW_SPACING_X,
        DESK_DEFAULT_ROW_SPACING_Y,
        array.item_spacing_scale,
        footprint_row_length
    );
    int row_count = get_test_slot_count(
        array.column_length,
        DESK_DEFAULT_COLUMN_SPACING_X,
        DESK_DEFAULT_COLUMN_SPACING_Y,
        array.row_spacing_scale,
        footprint_column_length
    );

    ASSERT_EQ(sprites.size(), static_cast<std::size_t>(items_per_row * row_count * 3));

    float expected_first_x = get_expected_first_desk_x(
        array,
        items_per_row,
        row_count,
        footprint_row_length,
        footprint_column_length
    );
    float expected_first_y = get_expected_first_desk_y(
        array,
        items_per_row,
        row_count,
        footprint_row_length,
        footprint_column_length
    );

    EXPECT_NEAR(sprites[0].posX, expected_first_x, 0.001f);
    EXPECT_NEAR(sprites[0].posY, expected_first_y, 0.001f);

    ASSERT_GE(items_per_row, 2);

    EXPECT_NEAR(sprites[3].posX, expected_first_x + DESK_DEFAULT_ROW_SPACING_X, 0.001f);
    EXPECT_NEAR(sprites[3].posY, expected_first_y + DESK_DEFAULT_ROW_SPACING_Y, 0.001f);

    int last_item_index = (items_per_row - 1) * 3;
    float last_item_offset = static_cast<float>(items_per_row - 1);
    EXPECT_NEAR(sprites[last_item_index].posX, expected_first_x + DESK_DEFAULT_ROW_SPACING_X * last_item_offset, 0.001f);
    EXPECT_NEAR(sprites[last_item_index].posY, expected_first_y + DESK_DEFAULT_ROW_SPACING_Y * last_item_offset, 0.001f);
}

TEST(DeskBuilderTest, BuildsArraySecondRowWhenAreaGetsDeeper) {
    DeskArray array = {
        .start_x = 620.0f,
        .start_y = 220.0f,
        .row_length = 500.0f,
        .column_length = 300.0f,
        .item_spacing_scale = 1.0f,
        .row_spacing_scale = 1.0f
    };

    DeskBuilder builder;
    std::vector<io::Sprite> sprites = builder.build_array(array);

    ASSERT_FALSE(sprites.empty());
    uint32_t array_body_direction = sprites[0].direction;
    float footprint_row_length = get_template_footprint_row_length(array_body_direction);
    float footprint_column_length = get_template_footprint_column_length(array_body_direction);
    int items_per_row = get_test_slot_count(
        array.row_length,
        DESK_DEFAULT_ROW_SPACING_X,
        DESK_DEFAULT_ROW_SPACING_Y,
        array.item_spacing_scale,
        footprint_row_length
    );
    int row_count = get_test_slot_count(
        array.column_length,
        DESK_DEFAULT_COLUMN_SPACING_X,
        DESK_DEFAULT_COLUMN_SPACING_Y,
        array.row_spacing_scale,
        footprint_column_length
    );

    ASSERT_EQ(sprites.size(), static_cast<std::size_t>(items_per_row * row_count * 3));
    ASSERT_GE(row_count, 2);

    float expected_first_x = get_expected_first_desk_x(
        array,
        items_per_row,
        row_count,
        footprint_row_length,
        footprint_column_length
    );
    float expected_first_y = get_expected_first_desk_y(
        array,
        items_per_row,
        row_count,
        footprint_row_length,
        footprint_column_length
    );

    int second_row_first_index = items_per_row * 3;
    EXPECT_NEAR(sprites[second_row_first_index].posX, expected_first_x + DESK_DEFAULT_COLUMN_SPACING_X, 0.001f);
    EXPECT_NEAR(sprites[second_row_first_index].posY, expected_first_y + DESK_DEFAULT_COLUMN_SPACING_Y, 0.001f);
}

TEST(DeskBuilderTest, LargerArraySpacingReducesDeskCount) {
    DeskArray compact_array = {
        .start_x = 620.0f,
        .start_y = 220.0f,
        .row_length = 500.0f,
        .column_length = 300.0f,
        .item_spacing_scale = 1.0f,
        .row_spacing_scale = 1.0f
    };

    DeskArray wide_spacing_array = {
        .start_x = 620.0f,
        .start_y = 220.0f,
        .row_length = 500.0f,
        .column_length = 300.0f,
        .item_spacing_scale = 2.0f,
        .row_spacing_scale = 2.0f
    };

    DeskBuilder builder;
    std::vector<io::Sprite> compact_sprites = builder.build_array(compact_array);
    std::vector<io::Sprite> wide_spacing_sprites = builder.build_array(wide_spacing_array);

    EXPECT_TRUE(compact_sprites.size() == 18u || compact_sprites.size() == 24u);
    EXPECT_EQ(wide_spacing_sprites.size(), 6u);
}

TEST(DeskBuilderTest, ArrayUsesOneRandomDeskDirection) {
    DeskArray array = {
        .start_x = 620.0f,
        .start_y = 220.0f,
        .row_length = 500.0f,
        .column_length = 300.0f,
        .item_spacing_scale = 1.0f,
        .row_spacing_scale = 1.0f
    };

    DeskBuilder builder;
    std::vector<io::Sprite> sprites = builder.build_array(array);

    ASSERT_FALSE(sprites.empty());
    uint32_t array_body_direction = sprites[0].direction;
    EXPECT_TRUE(is_valid_array_desk_direction(array_body_direction));

    for (std::size_t index = 0; index < sprites.size(); index += 3) {
        EXPECT_EQ(sprites[index].vid, DESK_BODY_VID);
        EXPECT_EQ(sprites[index].direction, array_body_direction);
    }
}

TEST(DeskBuilderTest, ArrayUsesOneRandomDeskTemplate) {
    DeskArray array = {
        .start_x = 620.0f,
        .start_y = 220.0f,
        .row_length = 500.0f,
        .column_length = 300.0f,
        .item_spacing_scale = 1.0f,
        .row_spacing_scale = 1.0f
    };

    DeskBuilder builder;

    for (int iteration = 0; iteration < 30; ++iteration) {
        std::vector<io::Sprite> sprites = builder.build_array(array);

        ASSERT_FALSE(sprites.empty());
        uint32_t array_body_direction = sprites[0].direction;
        bool uses_default_template = is_default_template_direction(array_body_direction);
        bool uses_up_left_template = is_up_left_template_direction(array_body_direction);
        bool uses_down_left_template = is_down_left_template_direction(array_body_direction);
        bool uses_up_right_template = is_up_right_template_direction(array_body_direction);
        EXPECT_TRUE(
            uses_default_template ||
            uses_up_left_template ||
            uses_down_left_template ||
            uses_up_right_template
        );

        for (std::size_t index = 0; index < sprites.size(); index += 3) {
            const io::Sprite& desk = sprites[index];
            const io::Sprite& computer = sprites[index + 1];
            const io::Sprite& chair = sprites[index + 2];

            EXPECT_EQ(desk.direction, array_body_direction);

            if (uses_default_template) {
                EXPECT_TRUE(is_valid_computer_direction(computer.direction));
                EXPECT_GT(chair.posX, desk.posX);
                EXPECT_GT(chair.posY, desk.posY);
            }

            if (uses_up_left_template) {
                EXPECT_TRUE(is_up_left_computer_direction(computer.direction));
                EXPECT_LT(chair.posX, desk.posX);
                EXPECT_LT(chair.posY, desk.posY);
            }

            if (uses_down_left_template) {
                EXPECT_TRUE(is_down_left_computer_direction(computer.direction));
                EXPECT_LT(chair.posX, desk.posX);
                EXPECT_GT(chair.posY, desk.posY);
            }

            if (uses_up_right_template) {
                EXPECT_TRUE(is_up_right_computer_direction(computer.direction));
                EXPECT_GT(chair.posX, desk.posX);
                EXPECT_LT(chair.posY, desk.posY);
            }
        }
    }
}

TEST(DeskBuilderTest, EmptyArrayReturnsNoSprites) {
    DeskArray array = {
        .start_x = 620.0f,
        .start_y = 220.0f,
        .row_length = -1.0f,
        .column_length = 0.0f
    };

    DeskBuilder builder;
    std::vector<io::Sprite> sprites = builder.build_array(array);

    EXPECT_TRUE(sprites.empty());
}

TEST(DeskBuilderTest, WritesRandomManualPreviewMap) {
    DeskBuilder builder;
    DeskArray array = {
        .start_x = 620.0f,
        .start_y = 220.0f,
        .row_length = 500.0f,
        .column_length = 300.0f,
        .item_spacing_scale = 1.0f,
        .row_spacing_scale = 1.0f
    };

    std::vector<io::Sprite> sprites = builder.build_array(array);

    std::string output_path = get_project_root() + "/desk_builder_manual_test.map";
    ASSERT_TRUE(io::write_map(sprites, output_path, 800.0f, 800.0f));
}
