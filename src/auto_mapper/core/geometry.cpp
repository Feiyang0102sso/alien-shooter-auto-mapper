/**
 * @file geometry.cpp
 * @brief code for isometric projection coordinate transformation
 * and geometric bounding boxes
 */

#include "geometry.h"
#include <cmath>

namespace auto_mapper::core {

MapPoint to_iso(const GridPoint& grid, const MapPoint& shift) {
    float pos_x = (grid.x - grid.y) * STEP_X + shift.x;
    float pos_y = (grid.x + grid.y) * STEP_Y + shift.y;
    return {pos_x, pos_y};
}

MapPoint calculate_shift(const BoundingBox& bbox, float map_size_x, float map_size_y) {
    if (!bbox.initialized) {
        return {0.0f, 0.0f};
    }

    float width = bbox.max_x - bbox.min_x;
    float height = bbox.max_y - bbox.min_y;

    // centering theory bias
    float raw_shift_x = (map_size_x - width) / 2.0f - bbox.min_x;
    float raw_shift_y = (map_size_y - height) / 2.0f - bbox.min_y;

    // align to the top-left? seems not good ...
    // float raw_shift_x = -bbox.min_x;
    // float raw_shift_y = -bbox.min_y;

    // Snap to grid: force shiftX = K * STEP_X + 20, shiftY = J * STEP_Y + 14
    float grid_x = std::round((raw_shift_x - 20.0f) / STEP_X);
    float shift_x = grid_x * STEP_X + 20.0f;

    float grid_y = std::round((raw_shift_y - 14.0f) / STEP_Y);
    float shift_y = grid_y * STEP_Y + 14.0f;

    // Add a small 2-tile padding to prevent the structure from hugging the map boundary too tightly.
    // Since we add integer multiples of STEP, the grid snap remains perfectly valid.
    shift_x += STEP_X * 2;
    shift_y += STEP_Y * 2;

    return {shift_x, shift_y};
}

} // namespace auto_mapper::core
