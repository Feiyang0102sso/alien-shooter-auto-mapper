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



} // namespace auto_mapper::core
