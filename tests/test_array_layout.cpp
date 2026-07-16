#include <gtest/gtest.h>

#include "auto_mapper/core/indoor_decorations/array_layout.h"

using namespace auto_mapper::core::indoor_decorations;

namespace {

ArrayLayout make_layout() {
    ArrayLayout layout = {
        .start_x = 10.0f,
        .start_y = 20.0f,
        .row_length = 60.0f,
        .column_length = 40.0f,
        .item_axis = {
            .step_x = 30.0f,
            .step_y = 0.0f,
            .spacing_scale = 1.0f,
            .footprint_length = 10.0f,
        },
        .row_axis = {
            .step_x = 0.0f,
            .step_y = 20.0f,
            .spacing_scale = 1.0f,
            .footprint_length = 10.0f,
        },
    };
    return layout;
}

} // namespace

TEST(ArrayLayoutTest, BuildsCenteredAnchorsInRowMajorOrder) {
    ArrayLayout layout = make_layout();

    std::vector<ArrayItemAnchor> anchors = calculate_array_item_anchors(layout);

    ASSERT_EQ(anchors.size(), 4U);
    EXPECT_FLOAT_EQ(anchors[0].pos_x, 25.0f);
    EXPECT_FLOAT_EQ(anchors[0].pos_y, 30.0f);
    EXPECT_FLOAT_EQ(anchors[1].pos_x, 55.0f);
    EXPECT_FLOAT_EQ(anchors[1].pos_y, 30.0f);
    EXPECT_FLOAT_EQ(anchors[2].pos_x, 25.0f);
    EXPECT_FLOAT_EQ(anchors[2].pos_y, 50.0f);
    EXPECT_FLOAT_EQ(anchors[3].pos_x, 55.0f);
    EXPECT_FLOAT_EQ(anchors[3].pos_y, 50.0f);
}

TEST(ArrayLayoutTest, SpacingScaleChangesSlotCountAndStep) {
    ArrayLayout layout = make_layout();
    layout.row_length = 80.0f;
    layout.item_axis.spacing_scale = 2.0f;

    std::vector<ArrayItemAnchor> anchors = calculate_array_item_anchors(layout);

    ASSERT_EQ(anchors.size(), 4U);
    EXPECT_FLOAT_EQ(anchors[1].pos_x - anchors[0].pos_x, 60.0f);
}

TEST(ArrayLayoutTest, RowSpacingScaleChangesRowCountAndStep) {
    ArrayLayout layout = make_layout();
    layout.column_length = 70.0f;
    layout.row_axis.spacing_scale = 2.0f;

    std::vector<ArrayItemAnchor> anchors = calculate_array_item_anchors(layout);

    ASSERT_EQ(anchors.size(), 4U);
    EXPECT_FLOAT_EQ(anchors[2].pos_y - anchors[0].pos_y, 40.0f);
}

TEST(ArrayLayoutTest, FootprintChangesSlotCountAndStartingOffset) {
    ArrayLayout layout = make_layout();
    layout.row_length = 80.0f;
    std::vector<ArrayItemAnchor> compact_anchors = calculate_array_item_anchors(layout);

    layout.item_axis.footprint_length = 40.0f;
    std::vector<ArrayItemAnchor> wide_anchors = calculate_array_item_anchors(layout);

    ASSERT_EQ(compact_anchors.size(), 6U);
    ASSERT_EQ(wide_anchors.size(), 4U);
    EXPECT_GT(wide_anchors[0].pos_x, compact_anchors[0].pos_x);
}

TEST(ArrayLayoutTest, UsesBothFootprintsToOffsetTheFirstAnchor) {
    ArrayLayout layout = make_layout();
    layout.row_length = 0.0f;
    layout.column_length = 0.0f;

    std::vector<ArrayItemAnchor> anchors = calculate_array_item_anchors(layout);

    ASSERT_EQ(anchors.size(), 1U);
    EXPECT_FLOAT_EQ(anchors[0].pos_x, 15.0f);
    EXPECT_FLOAT_EQ(anchors[0].pos_y, 25.0f);
}

TEST(ArrayLayoutTest, SupportsDiagonalAxes) {
    ArrayLayout layout = make_layout();
    layout.row_length = 0.0f;
    layout.column_length = 0.0f;
    layout.item_axis.step_x = 3.0f;
    layout.item_axis.step_y = 4.0f;
    layout.row_axis.step_x = -4.0f;
    layout.row_axis.step_y = 3.0f;

    std::vector<ArrayItemAnchor> anchors = calculate_array_item_anchors(layout);

    ASSERT_EQ(anchors.size(), 1U);
    EXPECT_FLOAT_EQ(anchors[0].pos_x, 9.0f);
    EXPECT_FLOAT_EQ(anchors[0].pos_y, 27.0f);
}

TEST(ArrayLayoutTest, ReturnsEmptyForInvalidLayout) {
    ArrayLayout negative_area = make_layout();
    negative_area.row_length = -1.0f;
    EXPECT_TRUE(calculate_array_item_anchors(negative_area).empty());

    ArrayLayout invalid_scale = make_layout();
    invalid_scale.item_axis.spacing_scale = 0.0f;
    EXPECT_TRUE(calculate_array_item_anchors(invalid_scale).empty());

    ArrayLayout empty_axis = make_layout();
    empty_axis.row_axis.step_x = 0.0f;
    empty_axis.row_axis.step_y = 0.0f;
    EXPECT_TRUE(calculate_array_item_anchors(empty_axis).empty());
}
