/**
 * @file array_layout.h
 * @brief Calculate centered anchor points for indoor decoration arrays.
 */

#pragma once

#include <vector>

namespace auto_mapper::core::indoor_decorations {

/**
 * @brief single axis for the array.
 */
struct ArrayLayoutAxis {
    float step_x; // when move to next item, how many x increase on axis
    float step_y; // when move to next item, how many y increase on axis
    float spacing_scale = 1.0f;
    float footprint_length; // single item footprint increase on the corresponding axis
};

/**
 * @brief double axis for the array
 */
struct ArrayLayout {
    float start_x;
    float start_y;
    float row_length;
    float column_length;
    ArrayLayoutAxis item_axis;
    ArrayLayoutAxis row_axis;
};

/**
 * @brief The center position of one built decoration unit.
 */
struct ArrayItemAnchor {
    float pos_x;
    float pos_y;
};

/**
 * @brief Calculate anchors in row-major order.
 *
 * Returns an empty vector when an area length is negative, an axis has a
 * non-positive spacing scale, or an axis has no length.
 */
std::vector<ArrayItemAnchor> calculate_array_item_anchors(const ArrayLayout& layout);

} // namespace auto_mapper::core::indoor_decorations
