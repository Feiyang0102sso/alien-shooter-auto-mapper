/**
 * @file wall_builder.h
 * @brief Builder class for generating wall and pillar sprites from segment sequences.
 */

#pragma once

#include "auto_mapper/core/geometry.h"
#include "auto_mapper/io/map_writer.h"
#include <vector>

namespace auto_mapper::core {

// A straight line segment drawn by the user and snapped to grid by frontend.
struct Segment {
    GridPoint start;
    GridPoint end;
};

class WallBuilder {
public:
    WallBuilder(float map_size_x = 600.0f, float map_size_y = 600.0f);

    // Core pipeline: Segments -> Rasterize ->
    // Intersection check -> Z-Order sort -> Apply shift -> Generate sprites.
    std::vector<io::Sprite> build(const std::vector<Segment>& segments) const;

private:
    float map_size_x_;
    float map_size_y_;
};

} // namespace auto_mapper::core
