/**
 * @file wall_builder.h
 * @brief Builder class for generating wall and pillar sprites from segment sequences.
 */

#pragma once

#include "auto_mapper/core/geometry.h"
#include "auto_mapper/core/wall_builder/wall_profile_types.h"
#include "auto_mapper/core/wall_builder/wall_profiles_as1.h"
#include "auto_mapper/core/wall_builder/wall_profiles_as2.h"
#include "auto_mapper/io/map_writer.h"
#include <vector>

namespace auto_mapper::core {

inline constexpr int SUPPORTED_WALL_TYPES[] = {
    WALL_TYPE_STANDARD,
    WALL_TYPE_LAB,
    WALL_TYPE_STANDARD_DARK,
    WALL_TYPE_AS2_WALL_SET1_FIXED_0,
    WALL_TYPE_AS2_WALL_SET1_FIXED_1,
    WALL_TYPE_AS2_WALL_SET1_RANDOM
};

inline constexpr int SUPPORTED_WALL_TYPE_COUNT =
    static_cast<int>(sizeof(SUPPORTED_WALL_TYPES) / sizeof(SUPPORTED_WALL_TYPES[0]));

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
    io::Sprite place_single_wall(int gx, int gy, int wall_type, WallPartKind kind) const;

private:
    struct RawSprite {
        int gx;
        int gy;
        int wall_type;
        WallPartKind kind;
        bool operator==(const RawSprite& other) const {
            return gx == other.gx && gy == other.gy && wall_type == other.wall_type && kind == other.kind;
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
    static const WallVariant& select_wall_variant(const WallProfile& profile);
    static int select_wall_variant_index(const WallProfile& profile);
    static const WallPartAsset& select_wall_part_asset(const WallVariant& variant, WallPartKind kind);
    io::Sprite place_single_wall_with_variant(int gx, int gy, int wall_type, WallPartKind kind, int variant_index) const;

    float map_size_x_;
    float map_size_y_;
};

} // namespace auto_mapper::core
