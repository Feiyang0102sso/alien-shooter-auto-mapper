/**
 * @file geometry.h
 * @brief code for isometric projection coordinate transformation
 * and geometric bounding boxes
 */

#pragma once
#include <algorithm>

namespace auto_mapper::core {

// pixels that draw on Map Editor
struct MapPoint {
    float x;
    float y;
};

// grid user draw on canvas
struct GridPoint {
    int x;
    int y;
};

/**
 * Project grid to map point using given step sizes.
 * Formula:
 *   map_point.x = (grid.x - grid.y) * step_x + shift.x
 *   map_point.y = (grid.x + grid.y) * step_y + shift.y
 *
 * @param step_x  X-axis step size (determined by wall type, e.g. 40.0 for standard, 90.0 for lab walls)
 * @param step_y  Y-axis step size (determined by wall type, e.g. 28.0 for standard, 64.0 for lab walls)
 */
MapPoint to_iso(const GridPoint& grid, float step_x, float step_y, const MapPoint& shift = {0.0f, 0.0f});
MapPoint to_iso(float gx, float gy, float step_x, float step_y, const MapPoint& shift = {0.0f, 0.0f});

} // namespace auto_mapper::core
