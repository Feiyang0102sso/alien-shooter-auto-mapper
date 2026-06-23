/**
 * @file geometry.cpp
 * @brief code for isometric projection coordinate transformation
 * and geometric bounding boxes
 */

#include "geometry.h"
#include <cmath>

namespace auto_mapper::core {

MapPoint to_iso(const GridPoint& grid, float step_x, float step_y, const MapPoint& shift) {
    float map_x = (grid.x - grid.y) * step_x + shift.x;
    float map_y = (grid.x + grid.y) * step_y + shift.y;
    return {map_x, map_y};
}

MapPoint to_iso(float gx, float gy, float step_x, float step_y, const MapPoint& shift) {
    MapPoint pt;
    pt.x = (gx - gy) * step_x + shift.x;
    pt.y = (gx + gy) * step_y + shift.y;
    return pt;
}

} // namespace auto_mapper::core
