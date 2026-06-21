/**
 * @file geometry.cpp
 * @brief code for isometric projection coordinate transformation
 * and geometric bounding boxes
 */

#include "geometry.h"
#include <cmath>

namespace auto_mapper::core {

MapPoint to_iso(const GridPoint& grid, const MapPoint& shift) {
    float map_x = (grid.x - grid.y) * STEP_X + shift.x;
    float map_y = (grid.x + grid.y) * STEP_Y + shift.y;
    return {map_x, map_y};
}



} // namespace auto_mapper::core
