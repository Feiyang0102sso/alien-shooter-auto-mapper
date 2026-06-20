#include "geometry.h"
#include <cmath>

namespace auto_mapper::core {

Point2D to_iso(const GridPoint& grid, const Point2D& shift) {
    float pos_x = (grid.x - grid.y) * STEP_X + shift.x;
    float pos_y = (grid.x + grid.y) * STEP_Y + shift.y;
    return {pos_x, pos_y};
}

Point2D calculate_shift(const BoundingBox& bbox, float map_size_x, float map_size_y) {
    if (!bbox.initialized) {
        return {0.0f, 0.0f};
    }

    float width = bbox.max_x - bbox.min_x;
    float height = bbox.max_y - bbox.min_y;

    // 纯数学的居中理论偏置
    float raw_shift_x = (map_size_x - width) / 2.0f - bbox.min_x;
    float raw_shift_y = (map_size_y - height) / 2.0f - bbox.min_y;

    // 网格吸附：强制让 shiftX 满足 = K * 40 + 20，shiftY 满足 = J * 28 + 14
    float grid_x = std::round((raw_shift_x - 20.0f) / STEP_X);
    float shift_x = grid_x * STEP_X + 20.0f;

    float grid_y = std::round((raw_shift_y - 14.0f) / STEP_Y);
    float shift_y = grid_y * STEP_Y + 14.0f;

    return {shift_x, shift_y};
}

} // namespace auto_mapper::core
