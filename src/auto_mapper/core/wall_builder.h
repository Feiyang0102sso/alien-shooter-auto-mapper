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
    float offset_a_x, offset_a_y;
    float offset_b_x, offset_b_y;
    float offset_p_x, offset_p_y;
};

struct FloorProfile {
    int vid;
    float step_x;
    float step_y;
    float pos_z;
    // shift is computed dynamically from map_size in WallBuilder::build()
};

struct CeilingProfile {
    int vid;
    float step_x;
    float step_y;
    float pos_z;
    float shift_offset_x = 0.0f;
    float shift_offset_y = 0.0f;
};

// ── Pre-defined wall profiles ──
constexpr int WALL_TYPE_STANDARD = 0;
constexpr int WALL_TYPE_LAB      = 1;

constexpr WallProfile WALL_STANDARD = {
    601, 602, 604, 40.0f, 28.0f,
    0.0f, 0.0f,       // dir_a offset
    -40.0f, 0.0f,     // dir_b offset
    0.0f, 0.0f        // pillar offset
};

constexpr WallProfile WALL_LAB = {
    651, 650, 652, 90.0f, 64.0f,
    90.0f, 0.0f,      // dir_a offset
    0.0f, 0.0f,       // dir_b offset
    0.0f, 0.0f        // pillar offset
};

// ── Pre-defined floor and ceiling profiles ──
constexpr int FLOOR_TYPE_STANDARD = 0;
constexpr int FLOOR_TYPE_LAB      = 1;

constexpr FloorProfile FLOOR_STANDARD = {500, 40.0f, 28.0f, 0.0f};
constexpr FloorProfile FLOOR_LAB      = {503, 80.0f, 56.0f, 0.0f};

constexpr int CEILING_TYPE_STANDARD = 0;
constexpr CeilingProfile CEILING_STANDARD = {504, 80.0f, 56.0f, 90.0f};

// A straight line segment drawn by the user, each segment carries its own wall type.
struct Segment {
    GridPoint start;
    GridPoint end;
    int wall_type = WALL_TYPE_STANDARD;
    int floor_type = FLOOR_TYPE_STANDARD;
};

class WallBuilder {
public:
    WallBuilder(float map_size_x = 600.0f, float map_size_y = 600.0f);

    // Core pipeline: Group by wall_type -> Rasterize per group ->
    // Intersection check -> Z-Order sort -> Apply shift -> Generate sprites.
    std::vector<io::Sprite> build(const std::vector<Segment>& segments, bool gen_floor = true, bool gen_ceiling = true) const;

private:
    struct RawSprite {
        int gx;
        int gy;
        int wall_type;
        int vid;
        bool operator==(const RawSprite& other) const {
            return gx == other.gx && gy == other.gy && wall_type == other.wall_type && vid == other.vid;
        }
    };

    struct PhysicalGridContext {
        int grid_w;
        int grid_h;
        float min_px;
        float min_py;
        std::vector<bool> physical_grid;
        std::vector<bool> outside_grid;
        std::vector<int> floor_type_grid;
    };

    // Helper methods for each stage
    MapPoint get_phys(int lx, int ly, int w_type) const;
    std::vector<RawSprite> process_wall_sprites(const std::vector<Segment>& segments) const;
    PhysicalGridContext build_physical_grid(const std::vector<Segment>& segments) const;
    std::vector<io::Sprite> place_floors(const std::vector<Segment>& segments, const PhysicalGridContext& grid_ctx) const;
    std::vector<io::Sprite> place_ceilings(const std::vector<Segment>& segments, const PhysicalGridContext& grid_ctx) const;
    std::vector<io::Sprite> convert_to_wall_sprites(const std::vector<RawSprite>& raw_sprites) const;

    // Look up profiles
    static const WallProfile& get_wall_profile(int wall_type);
    static const FloorProfile& get_floor_profile(int floor_type);
    static const CeilingProfile& get_ceiling_profile(int ceiling_type);

    float map_size_x_;
    float map_size_y_;
};

} // namespace auto_mapper::core
