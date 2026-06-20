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

// bounding box for map points
struct BoundingBox {
    float min_x = 0.0f;
    float max_x = 0.0f;
    float min_y = 0.0f;
    float max_y = 0.0f;
    bool initialized = false;

    // Expand its bounding box boundary based on the input point.
    void expand(const MapPoint& p) {
        if (!initialized) {
            min_x = max_x = p.x;
            min_y = max_y = p.y;
            initialized = true;
        } else {
            min_x = std::min(min_x, p.x);
            max_x = std::max(max_x, p.x);
            min_y = std::min(min_y, p.y);
            max_y = std::max(max_y, p.y);
        }
    }
};

// Isometric projection span step for maps
constexpr float STEP_X = 40.0f;
constexpr float STEP_Y = 28.0f;

/**
 * Convert grid points (x, y) to isometric ME coordinates.
 * formular:
 *   posX = (gx - gy) * STEP_X + shift.x
 *   posY = (gx + gy) * STEP_Y + shift.y
 */
MapPoint to_iso(const GridPoint& grid, const MapPoint& shift = {0.0f, 0.0f});

/**
 * Calculate and move the entire building to center of the map
 * let them also snap to grid
 */
MapPoint calculate_shift(const BoundingBox& bbox, float map_size_x, float map_size_y);

} // namespace auto_mapper::core
