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
    int dir_a_vid;   // '/' direction wall ID  (e.g. 601 for standard)
    int dir_b_vid;   // '\' direction wall ID  (e.g. 602 for standard)
    int pillar_vid;  // pillar ID              (e.g. 604 for standard)
    float step_x;   // X-axis physical step   (e.g. 40.0 for standard, 90.0 for lab)
    float step_y;   // Y-axis physical step   (e.g. 28.0 for standard, 64.0 for lab)
    float offset_a_x, offset_a_y;
    float offset_b_x, offset_b_y;
    float offset_p_x, offset_p_y;
    int grid_divisor; // division divisor     (1 for standard, 2 for lab)
};

struct FloorProfile {
    int vid;
    float step_x;
    float step_y;
    float pos_z;
    int grid_divisor = 1;
};

struct CeilingProfile {
    int vid;
    float step_x;
    float step_y;
    float pos_z;
    int grid_divisor = 1;
};

// ── Pre-defined wall profiles ──
inline constexpr int WALL_TYPE_STANDARD = 0;
inline constexpr int WALL_TYPE_LAB      = 1;
inline constexpr int WALL_TYPE_STANDARD_DARK = 2;

inline constexpr WallProfile WALL_STANDARD = {
    .dir_a_vid = 601,
    .dir_b_vid = 602,
    .pillar_vid = 604,
    .step_x = 40.0f,
    .step_y = 28.0f,
    .offset_a_x = 0.0f,
    .offset_a_y = 0.0f,
    .offset_b_x = -40.0f,
    .offset_b_y = 0.0f,
    .offset_p_x = 0.0f,
    .offset_p_y = 0.0f,
    .grid_divisor = 1
};

inline constexpr WallProfile WALL_LAB = {
    .dir_a_vid = 651,
    .dir_b_vid = 650,
    .pillar_vid = 652,
    .step_x = 90.0f,
    .step_y = 64.0f,
    .offset_a_x = 90.0f,
    .offset_a_y = 0.0f,
    .offset_b_x = 0.0f,
    .offset_b_y = 0.0f,
    .offset_p_x = 0.0f,
    .offset_p_y = 0.0f,
    .grid_divisor = 2
};

inline constexpr WallProfile WALL_STANDARD_DARK = {
    .dir_a_vid = 620,
    .dir_b_vid = 621,
    .pillar_vid = 622,
    .step_x = 40.0f,
    .step_y = 28.0f,
    .offset_a_x = 0.0f,
    .offset_a_y = 0.0f,
    .offset_b_x = -40.0f,
    .offset_b_y = 0.0f,
    .offset_p_x = 0.0f,
    .offset_p_y = 0.0f,
    .grid_divisor = 1
};

inline constexpr int SUPPORTED_WALL_TYPES[] = {
    WALL_TYPE_STANDARD,
    WALL_TYPE_LAB,
    WALL_TYPE_STANDARD_DARK
};

inline constexpr int SUPPORTED_WALL_TYPE_COUNT =
    static_cast<int>(sizeof(SUPPORTED_WALL_TYPES) / sizeof(SUPPORTED_WALL_TYPES[0]));

// ── Pre-defined floor and ceiling profiles ──
inline constexpr int FLOOR_TYPE_STANDARD = 0;
inline constexpr int FLOOR_TYPE_LAB      = 1;
inline constexpr int FLOOR_TYPE_STANDARD_DARK = 2;

inline constexpr FloorProfile FLOOR_STANDARD = {
    .vid = 500,
    .step_x = 40.0f,
    .step_y = 28.0f,
    .pos_z = 0.0f,
    .grid_divisor = 1
};
inline constexpr FloorProfile FLOOR_LAB = {
    .vid = 503,
    .step_x = 80.0f,
    .step_y = 56.0f,
    .pos_z = 0.0f,
    .grid_divisor = 1
};
inline constexpr FloorProfile FLOOR_STANDARD_DARK = {
    .vid = 502,
    .step_x = 40.0f,
    .step_y = 28.0f,
    .pos_z = 0.0f,
    .grid_divisor = 1
};

inline constexpr int CEILING_TYPE_STANDARD = 0;
inline constexpr CeilingProfile CEILING_STANDARD = {
    .vid = 504,
    .step_x = 80.0f,
    .step_y = 56.0f,
    .pos_z = 90.0f,
    .grid_divisor = 2
};

// A straight line segment drawn by the user, each segment carries its own wall type.
struct Segment {
    GridPoint start;
    GridPoint end;
    int wall_type = WALL_TYPE_STANDARD;
    int floor_type = FLOOR_TYPE_STANDARD;
};

struct DoorExcavation {
    GridPoint pos;
    int direction_type;
    int size;
    int wall_type;
};

class WallBuilder {
public:
    WallBuilder(float map_size_x = 600.0f, float map_size_y = 600.0f);

    // Look up profiles (Moved to public for door_builder.cpp access)
    static const WallProfile& get_wall_profile(int wall_type);
    static MapPoint get_wall_shift(float map_size_x, const WallProfile& profile);

    // Core pipeline: Group by wall_type -> Rasterize per group ->
    // Intersection check -> Z-Order sort -> Apply shift -> Generate sprites.
    std::vector<io::Sprite> build(
        const std::vector<Segment>& segments, 
        bool gen_floor = true, 
        bool gen_ceiling = true,
        const std::vector<DoorExcavation>& excavations = {}
    ) const;

    // Place a single tile/wall sprite at logical grid coordinates
    io::Sprite place_single_floor_celling(int gx, int gy, int vid, float step_x, float step_y, float pos_z, int grid_divisor) const;
    io::Sprite place_single_wall(int gx, int gy, int wall_type, int vid) const;

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
    std::vector<RawSprite> process_wall_sprites(const std::vector<Segment>& segments, const std::vector<DoorExcavation>& excavations) const;
    PhysicalGridContext build_physical_grid(const std::vector<Segment>& segments) const;
    std::vector<io::Sprite> place_floors(const std::vector<Segment>& segments, const PhysicalGridContext& grid_ctx) const;
    std::vector<io::Sprite> place_ceilings(const std::vector<Segment>& segments, const PhysicalGridContext& grid_ctx) const;
    std::vector<io::Sprite> convert_to_wall_sprites(const std::vector<RawSprite>& raw_sprites) const;

    // Look up profiles
    static const FloorProfile& get_floor_profile(int floor_type);
    static const CeilingProfile& get_ceiling_profile(int ceiling_type);

    float map_size_x_;
    float map_size_y_;
};

} // namespace auto_mapper::core
