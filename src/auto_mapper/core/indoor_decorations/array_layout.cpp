/**
 * @file array_layout.cpp
 * @brief Implements centered indoor decoration array layout.
 */

#include "auto_mapper/core/indoor_decorations/array_layout.h"

#include <cmath>

namespace auto_mapper::core::indoor_decorations {

namespace {

// cal the steps, used both items step and row step
// one central point to the other, no need to plus footprint
float get_base_step_length(const ArrayLayoutAxis& axis) {
    return std::sqrt(axis.step_x * axis.step_x + axis.step_y * axis.step_y);
}

float get_scaled_step_length(const ArrayLayoutAxis& axis) {
    return get_base_step_length(axis) * axis.spacing_scale;
}

// how many can be placed on the area axis (both)
int get_slot_count(float available_length, const ArrayLayoutAxis& axis) {
    if (available_length < 0.0f) {
        return 0;
    }

    if (axis.spacing_scale <= 0.0f) {
        return 0;
    }

    float step_length = get_scaled_step_length(axis);
    if (step_length <= 0.0f) {
        return 0;
    }

    // always one for an undersized area.
    if (available_length <= axis.footprint_length) {
        return 1;
    }

    float remaining_length_after_first_item = available_length - axis.footprint_length;
    return static_cast<int>(std::floor(remaining_length_after_first_item / step_length)) + 1;
}

// to make all items central, we have to adjust the first item
// the first item central point should not on the start point
float get_start_padding_for_centering(float available_length, int slot_count, const ArrayLayoutAxis& axis) {
    float occupied_length = axis.footprint_length; //half for first + half for last
    occupied_length += static_cast<float>(slot_count - 1) * get_scaled_step_length(axis);

    float remaining_length = available_length - occupied_length;
    if (remaining_length <= 0.0f) {
        return 0.0f;
    }

    return remaining_length / 2.0f;
}

// distance -> x += 100; y -= 50
ArrayItemAnchor project_axis_distance_to_xy(const ArrayLayoutAxis& axis, float distance) {
    float base_step_length = get_base_step_length(axis);

    ArrayItemAnchor offset = {
        .pos_x = axis.step_x / base_step_length * distance,
        .pos_y = axis.step_y / base_step_length * distance,
    };
    return offset;
}

ArrayItemAnchor get_first_array_item_anchor(const ArrayLayout& layout, int items_per_row, int row_count) {
    float item_start_padding = get_start_padding_for_centering(
        layout.row_length,
        items_per_row,
        layout.item_axis
    );
    float row_start_padding = get_start_padding_for_centering(
        layout.column_length,
        row_count,
        layout.row_axis
    );

    ArrayItemAnchor item_start_padding_offset = project_axis_distance_to_xy(
        layout.item_axis,
        item_start_padding
    );
    ArrayItemAnchor row_start_padding_offset = project_axis_distance_to_xy(
        layout.row_axis,
        row_start_padding
    );
    ArrayItemAnchor item_footprint_offset = project_axis_distance_to_xy(
        layout.item_axis,
        layout.item_axis.footprint_length / 2.0f
    );
    ArrayItemAnchor row_footprint_offset = project_axis_distance_to_xy(
        layout.row_axis,
        layout.row_axis.footprint_length / 2.0f
    );

    ArrayItemAnchor first_anchor = {
        .pos_x = layout.start_x,
        .pos_y = layout.start_y,
    };
    first_anchor.pos_x += item_start_padding_offset.pos_x;
    first_anchor.pos_x += row_start_padding_offset.pos_x;
    first_anchor.pos_x += item_footprint_offset.pos_x;
    first_anchor.pos_x += row_footprint_offset.pos_x;
    first_anchor.pos_y += item_start_padding_offset.pos_y;
    first_anchor.pos_y += row_start_padding_offset.pos_y;
    first_anchor.pos_y += item_footprint_offset.pos_y;
    first_anchor.pos_y += row_footprint_offset.pos_y;
    return first_anchor;
}

} // namespace

std::vector<ArrayItemAnchor> calculate_array_item_anchors(const ArrayLayout& layout) {
    std::vector<ArrayItemAnchor> anchors;

    int items_per_row = get_slot_count(layout.row_length, layout.item_axis);
    int row_count = get_slot_count(layout.column_length, layout.row_axis);
    if (items_per_row <= 0 || row_count <= 0) {
        return anchors;
    }

    ArrayItemAnchor first_anchor = get_first_array_item_anchor(layout, items_per_row, row_count);
    float item_step_x = layout.item_axis.step_x * layout.item_axis.spacing_scale;
    float item_step_y = layout.item_axis.step_y * layout.item_axis.spacing_scale;
    float row_step_x = layout.row_axis.step_x * layout.row_axis.spacing_scale;
    float row_step_y = layout.row_axis.step_y * layout.row_axis.spacing_scale;

    anchors.reserve(static_cast<size_t>(items_per_row * row_count));
    for (int row_index = 0; row_index < row_count; ++row_index) {
        ArrayItemAnchor row_start = first_anchor;
        row_start.pos_x += static_cast<float>(row_index) * row_step_x;
        row_start.pos_y += static_cast<float>(row_index) * row_step_y;

        for (int item_index = 0; item_index < items_per_row; ++item_index) {
            ArrayItemAnchor anchor = row_start;
            anchor.pos_x += static_cast<float>(item_index) * item_step_x;
            anchor.pos_y += static_cast<float>(item_index) * item_step_y;
            anchors.push_back(anchor);
        }
    }

    return anchors;
}

} // namespace auto_mapper::core::indoor_decorations
