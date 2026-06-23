/**
 * @file wall_builder.h
 * @brief Builder class for generating wall and pillar sprites from segment sequences.
 *
 * WallProfile: Decouples logical grid from physical wall dimensions.
 * Different wall types (standard 40x28, lab 90x64, etc.) are expressed
 * as self-contained profiles rather than global constants.
 */

#pragma once

#include "auto_mapper/core/geometry.h"
#include "auto_mapper/io/map_writer.h"
#include <vector>

namespace auto_mapper::core {

/// Wall asset configuration — each wall type carries its own IDs and step sizes.
struct WallProfile {
    int id_dir_a;   // '/' direction wall ID  (e.g. 601 for standard)
    int id_dir_b;   // '\' direction wall ID  (e.g. 602 for standard)
    int id_pillar;  // pillar ID              (e.g. 604 for standard)
    float step_x;   // X-axis physical step   (e.g. 40.0 for standard, 90.0 for lab)
    float step_y;   // Y-axis physical step   (e.g. 28.0 for standard, 64.0 for lab)
    float offset_x; // anchor correction for '\' direction walls (= -step_x)
};

// ── Pre-defined wall profiles ──
constexpr int WALL_TYPE_STANDARD = 0;
constexpr int WALL_TYPE_LAB      = 1;

constexpr WallProfile WALL_STANDARD = {601, 602, 604, 40.0f, 28.0f, -40.0f};
constexpr WallProfile WALL_LAB      = {651, 650, 652, 90.0f, 64.0f, -90.0f};

// A straight line segment drawn by the user, each segment carries its own wall type.
struct Segment {
    GridPoint start;
    GridPoint end;
    int wall_type = WALL_TYPE_STANDARD;
};

class WallBuilder {
public:
    WallBuilder(int grid_size = 20, float map_size_x = 600.0f, float map_size_y = 600.0f);

    // Core pipeline: Group by wall_type -> Rasterize per group ->
    // Intersection check -> Z-Order sort -> Apply shift -> Generate sprites.
    std::vector<io::Sprite> build(const std::vector<Segment>& segments) const;

private:
    // Look up WallProfile by wall_type id. Falls back to WALL_STANDARD.
    static const WallProfile& get_profile(int wall_type);

    int grid_size_;
    float map_size_x_;
    float map_size_y_;
};

} // namespace auto_mapper::core
