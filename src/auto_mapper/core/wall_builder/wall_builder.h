/**
 * @file wall_builder.h
 * @brief Builder class for generating wall and pillar sprites from segment sequences.
 */

#pragma once

#include "auto_mapper/core/geometry.h"
#include "auto_mapper/core/wall_builder/wall_profile_types.h"
#include "auto_mapper/core/wall_builder/ceiling_profiles_as1.h"
#include "auto_mapper/core/wall_builder/wall_profiles_as1.h"
#include "auto_mapper/core/wall_builder/wall_profiles_as2.h"
#include "auto_mapper/io/map_writer.h"
#include <map>
#include <set>
#include <tuple>
#include <vector>

namespace auto_mapper::core {

inline constexpr int SUPPORTED_WALL_TYPES[] = {
    WALL_TYPE_STANDARD,
    WALL_TYPE_LAB,
    WALL_TYPE_STANDARD_DARK,
    WALL_TYPE_AS2_WALL_SET1_FIXED_0,
    WALL_TYPE_AS2_WALL_SET1_FIXED_1,
    WALL_TYPE_AS2_WALL_SET1_RANDOM,
    WALL_TYPE_AS2_WALL_SET2_RANDOM,
    WALL_TYPE_AS2_WALL_SET3_RANDOM,
    WALL_TYPE_AS2_WALL_SET4_RANDOM,
    WALL_TYPE_AS2_WALL_SET5_RANDOM,
    WALL_TYPE_AS2_WALL_SET6_RANDOM,
    WALL_TYPE_AS2_WALL_SET7_RANDOM,
    WALL_TYPE_AS2_WALL_SET8_RANDOM,
    WALL_TYPE_AS2_WALL_SET9_RANDOM
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
    WallBuilder(
        float map_size_x = 600.0f,
        float map_size_y = 600.0f,
        bool randomize_directions = false
    );

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
    io::Sprite place_single_ceiling_curtain(
        int gx,
        int gy,
        int wall_type,
        WallPartKind kind,
        bool use_wide,
        WallOutsideSide outside_side
    ) const;

    // Look up a WallPartAsset from a variant by kind (public for door_builder.cpp access)
    static const WallPartAsset& select_wall_part_asset(const WallVariant& variant, WallPartKind kind);

private:
    using CeilingPoint = std::pair<int, int>;
    using CeilingEdgeKey = std::tuple<int, int, WallPartKind>;

    struct ExteriorCeilingEdge {
        int gx;
        int gy;
        int wall_type;
        WallPartKind kind;
        int opposing_wall_distance;
        WallOutsideSide outside_side;
    };

    struct RawSprite {
        int gx;
        int gy;
        int wall_type;
        WallPartKind kind;
        bool uses_corner_wall_variant = false;
        bool connects_up = false;
        bool connects_down = false;
        bool connects_left = false;
        bool connects_right = false;
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

    struct RareWallPosition {
        int wall_type;
        WallPartKind kind;
        int gx;
        int gy;

        bool operator<(const RareWallPosition& other) const;
    };

    struct StraightWallRun {
        int wall_type;
        WallPartKind kind;
        std::vector<RawSprite> candidates;
        std::size_t next_candidate_index = 0;
    };

    // Helper methods for each stage
    MapPoint get_phys(int lx, int ly, int w_type) const;
    std::vector<RawSprite> process_wall_sprites(const std::vector<Segment>& segments, const std::vector<DoorExcavation>& excavations) const;
    PhysicalGridContext build_physical_grid(const std::vector<Segment>& segments) const;
    std::vector<io::Sprite> place_floors(const std::vector<Segment>& segments, const PhysicalGridContext& grid_ctx) const;
    std::vector<io::Sprite> place_ceilings(const std::vector<Segment>& segments) const;
    std::vector<io::Sprite> place_wall_aligned_ceiling_curtains(const std::vector<Segment>& segments) const;
    // Legacy Wide Ceiling Curtain strategy. It detects narrow recesses, groups
    // their exterior wall parts, and replaces Long curtains with Wide curtains.
    // The current AS2 path intentionally uses Long curtains only, so this
    // method is retained for possible future experiments and is not called.
    std::vector<io::Sprite> place_as2_ceiling_curtains_with_wide(
        std::map<CeilingEdgeKey, ExteriorCeilingEdge> exterior_edges,
        const std::set<CeilingPoint>& vertices,
        const std::set<CeilingPoint>& outside_cells
    ) const;

    // Select rare wall positions after door excavation. The method owns all
    // density, straight-run, fairness, and physical-distance placement rules.
    std::set<RareWallPosition> select_rare_wall_positions(const std::vector<RawSprite>& raw_sprites) const;
    std::vector<io::Sprite> convert_to_wall_sprites(const std::vector<RawSprite>& raw_sprites) const;

    // Look up profiles
    static const FloorProfile& get_floor_profile(int floor_type);
    static const AS1CeilingProfile* get_as1_ceiling_profile(int wall_type);
    static const CeilingCurtainProfile* get_ceiling_curtain_profile(int wall_type);
    const WallVariant& select_wall_variant(const WallProfile& profile) const;
    int select_wall_variant_index(const WallProfile& profile) const;
    static bool has_rare_wall_variant(const WallProfile& profile);
    static const WallPartAsset* select_corner_pillar_asset(const WallProfile& profile, const RawSprite& raw_sprite);
    io::Sprite place_single_wall_with_variant(int gx, int gy, int wall_type, WallPartKind kind, int variant_index) const;
    io::Sprite place_wall_part_asset(int gx, int gy, int wall_type, const WallPartAsset& asset) const;
    std::vector<io::Sprite> place_pillar_slices(const RawSprite& raw_sprite) const;

    float map_size_x_;
    float map_size_y_;
    DirectionRandomizer direction_randomizer_;
};

} // namespace auto_mapper::core
