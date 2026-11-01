/**
 * @file scatter_fill.h
 * @brief Shared scatter engine of the brush shelves (Loot Box, Gore): spots that grow from a dab center outwards.
 *
 * core/scatter/ holds the brush shelves: this common engine at the top, the Loot Box brush in loot/ and the
 * Gore brush in gore/.
 */

#pragma once

#include <vector>

namespace auto_mapper::core {

/**
 * One Map Coordinate spot of a brush dab.
 */
struct ScatterPoint {
    float x = 0.0f;
    float y = 0.0f;
};

/**
 * Ground spots of one dab: the center plus random points of the circle, taken nearest-to-center first and kept
 * when at least `spacing` from everything kept so far (and from `occupied`). The patch grows from the center outwards.
 * Every kept spot is also appended to `occupied`.
 */
std::vector<ScatterPoint> fill_from_center(float center_x, float center_y, float radius, float spacing,
                                           std::vector<ScatterPoint>& occupied);

/**
 * A uniform random point of the disc; sqrt keeps the points from crowding the center.
 */
ScatterPoint random_point_in_circle(float center_x, float center_y, float radius);

} // namespace auto_mapper::core
