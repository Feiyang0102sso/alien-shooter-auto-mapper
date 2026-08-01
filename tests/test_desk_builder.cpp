#include <gtest/gtest.h>

#include "auto_mapper/core/indoor_decorations/array_layout.h"
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

std::vector<ArrayItemAnchor> get_desk_anchors(
    const DeskArray& array,
    float footprint_row_length,
    float footprint_column_length
) {
    ArrayLayout layout = {
        .start_x = array.start_x,
        .start_y = array.start_y,
        .row_length = array.row_length,
        .column_length = array.column_length,
        .item_axis = {
            .step_x = DESK_DEFAULT_ROW_SPACING_X,
            .step_y = DESK_DEFAULT_ROW_SPACING_Y,
            .spacing_scale = array.item_spacing_scale,
            .footprint_length = footprint_row_length,
        },
        .row_axis = {
            .step_x = DESK_DEFAULT_COLUMN_SPACING_X,
            .step_y = DESK_DEFAULT_COLUMN_SPACING_Y,
            .spacing_scale = array.row_spacing_scale,
            .footprint_length = footprint_column_length,
        },
    };
    return calculate_array_item_anchors(layout);
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
    std::vector<ArrayItemAnchor> anchors = get_desk_anchors(
        array,
        footprint_row_length,
        footprint_column_length
    );

    ASSERT_GE(anchors.size(), 2U);
    ASSERT_EQ(sprites.size(), anchors.size() * 3);
    for (std::size_t anchor_index = 0; anchor_index < anchors.size(); ++anchor_index) {
        std::size_t sprite_index = anchor_index * 3;
        EXPECT_NEAR(sprites[sprite_index].posX, anchors[anchor_index].pos_x, 0.001f);
        EXPECT_NEAR(sprites[sprite_index].posY, anchors[anchor_index].pos_y, 0.001f);
    }
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
    std::vector<ArrayItemAnchor> anchors = get_desk_anchors(
        array,
        footprint_row_length,
        footprint_column_length
    );

    ASSERT_EQ(sprites.size(), anchors.size() * 3);
    bool found_second_row = false;
    for (std::size_t anchor_index = 0; anchor_index < anchors.size(); ++anchor_index) {
        std::size_t sprite_index = anchor_index * 3;
        EXPECT_NEAR(sprites[sprite_index].posX, anchors[anchor_index].pos_x, 0.001f);
        EXPECT_NEAR(sprites[sprite_index].posY, anchors[anchor_index].pos_y, 0.001f);

        float expected_second_row_x = anchors[0].pos_x + DESK_DEFAULT_COLUMN_SPACING_X;
        float expected_second_row_y = anchors[0].pos_y + DESK_DEFAULT_COLUMN_SPACING_Y;
        bool same_x = std::abs(anchors[anchor_index].pos_x - expected_second_row_x) < 0.001f;
        bool same_y = std::abs(anchors[anchor_index].pos_y - expected_second_row_y) < 0.001f;
        if (same_x && same_y) {
            found_second_row = true;
        }
    }
    EXPECT_TRUE(found_second_row);
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

    std::string output_path = get_test_output_path("desk_builder_manual_test.map");
    ASSERT_TRUE(io::write_map(sprites, output_path, 800.0f, 800.0f));
}
