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

// Isometric projection span step for maps
constexpr float STEP_X = 40.0f;
constexpr float STEP_Y = 28.0f;

/**
 * Project grid to map point
 * Formula:
 *   map_point.x = (grid.x - grid.y) * STEP_X + shift.x
 *   map_point.y = (grid.x + grid.y) * STEP_Y + shift.y
 */
MapPoint to_iso(const GridPoint& grid, const MapPoint& shift = {0.0f, 0.0f});

} // namespace auto_mapper::core
