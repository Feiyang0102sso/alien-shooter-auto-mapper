/**
 * @file wall_profile_types.h
 * @brief Shared wall profile data structures.
 */

#pragma once

#include "auto_mapper/core/randomizer.h"
#include "auto_mapper/core/vid_properties/vid_property_types.h"

#include <cstdint>

namespace auto_mapper::core {

inline constexpr int MAX_WALL_VARIANTS = 8;
inline constexpr int MAX_WALL_VARIANT_POOLS = 4;
inline constexpr int MAX_WALL_VARIANTS_PER_POOL = 8;

enum class WallPartKind {
    DirA,
    DirB,
    Pillar
};

// The exterior cell lies on the negative or positive logical-grid side of a wall.
// DirA uses the grid X axis; DirB uses the grid Y axis.
enum class WallOutsideSide {
    NegativeGridSide,
    PositiveGridSide
};

enum class WallPillarMode {
    Complete,
    DirectionalSlices
};

struct WallPartAsset {
    int vid;
    uint32_t direction;
    float offset_x;
    float offset_y;

    // Visual styling applied when this wall-part is emitted as a sprite.
    // Used by both wall builder (rare variant paint) and door builder
    // (flank compensation parts like Set1 pillars or Set5 air walls).
    VidGamma gamma{};
    VidScale scale{};
};

struct WallVariant {
    WallPartAsset dir_a;
    WallPartAsset dir_b;
    WallPartAsset pillar;
};

struct WallVariantPool {
    int variant_indices[MAX_WALL_VARIANTS_PER_POOL];
    int variant_count;
    bool randomize;
};

struct WallProfile {
    int dir_a_vid;   // '/' direction wall ID.
    int dir_b_vid;   // '\' direction wall ID.
    int pillar_vid;  // Pillar wall ID.
    float step_x;    // X-axis physical step.
    float step_y;    // Y-axis physical step.
    float offset_a_x;
    float offset_a_y;
    float offset_b_x;
    float offset_b_y;
    float offset_p_x;
    float offset_p_y;
    int grid_divisor;
    WallVariant variants[MAX_WALL_VARIANTS];
    int variant_count = 0;
    WallVariantPool variant_pools[MAX_WALL_VARIANT_POOLS];
    int variant_pool_count = 0;
    int active_variant_pool = 0;
    WallPillarMode pillar_mode = WallPillarMode::Complete;
    WallPartAsset pillar_slice_up = {0, 0, 0.0f, 0.0f};
    WallPartAsset pillar_slice_down = {0, 0, 0.0f, 0.0f};
    WallPartAsset pillar_slice_left = {0, 0, 0.0f, 0.0f};
    WallPartAsset pillar_slice_right = {0, 0, 0.0f, 0.0f};
    bool randomize_wall_parts_independently = false;
    int rare_variant_index = -1;
    float rare_target_density = 0.0f;
    int rare_straight_buffer = 0;
    float rare_min_distance_steps = 0.0f;
    int corner_wall_variant_index = -1;
    bool use_corner_pillar_assets = false;
    bool skip_unmapped_pillars = false;
    WallPartAsset pillar_corner_down_right = {0, 0, 0.0f, 0.0f};
    WallPartAsset pillar_corner_down_left = {0, 0, 0.0f, 0.0f};
    WallPartAsset pillar_corner_up_left = {0, 0, 0.0f, 0.0f};
    WallPartAsset pillar_corner_up_right = {0, 0, 0.0f, 0.0f};
    // Door flank: when > 0, WallBuilder excavates this many extra wall slots
    // on each side of a door opening (DirA/DirB segments only, not pillars).
    // DoorBuilder then re-places those slots with a fixed variant so the
    // wall direction is locked (e.g. direction 102 for Set7/Set9).
    int door_flank_clear = 0;
    // Variant index to use when re-placing flank walls. -1 = no flank walls.
    int door_flank_variant_index = -1;
    // AS1 wall parts use the full 0-255 range. AS2 wall directions are
    // selected through variant pools, while their pillars stay fixed.
    DirectionRandomizationRule part_direction_randomization{};
};

struct FloorProfile {
    int vid;
    float step_x;
    float step_y;
    float pos_z;
    int grid_divisor = 1;
    // Per-floor gamma tint. Most floors use all-zero (no tint).
    // Set2 uses (-80, -80, -80) to darken VID 1724 for its unique palette.
    VidGamma gamma{};
    uint32_t direction = 0;
    DirectionRandomizationRule direction_randomization{};
};

struct CeilingWallOffset {
    float x = 0.0f;
    float y = 0.0f;
};

struct AS1RecessCornerSupplementProfile {
    // Positive adjustment moves the added square farther beyond the corner.
    // The value is measured in AS1 ceiling-tile pitches.
    bool keep_connector_ceiling = false;
    bool keep_side_ceiling = true;
    int supplement_count = 0;
    float away_from_corner_adjustment = 0.0f;
};

struct AS1HorizontalRecessSupplementProfile {
    AS1RecessCornerSupplementProfile left_corner{};
    AS1RecessCornerSupplementProfile right_corner{};
};

struct AS1VerticalRecessSupplementProfile {
    AS1RecessCornerSupplementProfile upper_corner{};
    AS1RecessCornerSupplementProfile lower_corner{};
};

struct AS1StandaloneDeepCornerSupplementProfile {
    // Standalone right-angle recesses can continue both touching wall runs.
    int dir_a_supplement_count = 0;
    float dir_a_away_from_corner_adjustment = 0.0f;
    int dir_b_supplement_count = 0;
    float dir_b_away_from_corner_adjustment = 0.0f;
};

struct AS1StandaloneDeepCornersSupplementProfile {
    AS1StandaloneDeepCornerSupplementProfile upper_left{};
    AS1StandaloneDeepCornerSupplementProfile upper_right{};
    AS1StandaloneDeepCornerSupplementProfile lower_left{};
    AS1StandaloneDeepCornerSupplementProfile lower_right{};
};

// AS1 ceiling tiles are plain square assets. The profile stores wall alignment,
// straight-run pitch correction, convex corners, and concave recess fills.
struct AS1CeilingProfile {
    int vid;
    float pos_z;
    float step_x;
    float step_y;
    // Total coverage depth. Layer 1 is the calibrated wall-aligned layer.
    int total_layer_count = 1;
    // From which layer (1-based index) to start snapping to the global isometric grid.
    int grid_snapping_start_layer = 2;
    // Physical coordinate bounds tolerance relative to map boundaries.
    float min_bounds_margin = -20.0f;
    float max_bounds_margin = 30.0f;
    CeilingWallOffset dir_a_negative_outside_offset{};
    CeilingWallOffset dir_a_positive_outside_offset{};
    CeilingWallOffset dir_b_negative_outside_offset{};
    CeilingWallOffset dir_b_positive_outside_offset{};
    CeilingWallOffset dir_a_step_adjustment{};
    CeilingWallOffset dir_b_step_adjustment{};
    int left_upper_corner_dir_a_supplement_count = 0;
    int left_upper_corner_dir_b_supplement_count = 0;
    int left_lower_corner_dir_a_supplement_count = 0;
    int left_lower_corner_dir_b_supplement_count = 0;
    int right_upper_corner_dir_a_supplement_count = 0;
    int right_upper_corner_dir_b_supplement_count = 0;
    int right_lower_corner_dir_a_supplement_count = 0;
    int right_lower_corner_dir_b_supplement_count = 0;
    AS1HorizontalRecessSupplementProfile upper_recess{};
    AS1HorizontalRecessSupplementProfile lower_recess{};
    AS1VerticalRecessSupplementProfile left_recess{};
    AS1VerticalRecessSupplementProfile right_recess{};
    AS1StandaloneDeepCornersSupplementProfile standalone_deep_corners{};
};

struct CeilingCurtainPartProfile {
    uint32_t direction;
    float offset_x;
    float offset_y;
    // Extra outward distance in logical wall-grid widths for asymmetric sprites.
    float negative_side_outward_adjustment = 0.0f;
    float positive_side_outward_adjustment = 0.0f;
    // AS1 Lab ceilings were calibrated independently on all four wall sides.
    // When enabled, these offsets are applied directly from the emitted wall.
    bool use_side_specific_offsets = false;
    CeilingWallOffset negative_side_offset{};
    CeilingWallOffset positive_side_offset{};
};

struct RecessCornerCurtainProfile {
    bool keep_connector_long = true;
    // Moves the touching side-wall Long away from the recess corner along its wall.
    float side_long_away_from_corner_adjustment = 0.0f;
};

struct HorizontalRecessCurtainProfile {
    RecessCornerCurtainProfile left_corner;
    RecessCornerCurtainProfile right_corner;
};

struct VerticalRecessCurtainProfile {
    RecessCornerCurtainProfile upper_corner;
    RecessCornerCurtainProfile lower_corner;
};

struct StandaloneDeepCornerCurtainProfile {
    bool keep_dir_a_long = true;
    float dir_a_long_away_from_corner_adjustment = 0.0f;
    bool keep_dir_b_long = true;
    float dir_b_long_away_from_corner_adjustment = 0.0f;
};

struct StandaloneDeepCornersCurtainProfile {
    // Names describe the visible direction in which the L-shaped recess opens.
    StandaloneDeepCornerCurtainProfile upper_left;
    StandaloneDeepCornerCurtainProfile upper_right;
    StandaloneDeepCornerCurtainProfile lower_left;
    StandaloneDeepCornerCurtainProfile lower_right;
};

struct CeilingCurtainProfile {
    int vid;
    float pos_z;
    int maximum_wide_opposing_wall_distance;
    // Extra along-wall coverage placed at the two lower exterior corners.
    // Values use logical wall-grid widths and may differ by wall set.
    float dir_a_lower_corner_supplement;
    float dir_b_lower_corner_supplement;
    // Each recess corner controls its connector-wall Long and side-wall Long separately.
    HorizontalRecessCurtainProfile upper_recess;
    HorizontalRecessCurtainProfile lower_recess;
    VerticalRecessCurtainProfile left_recess;
    VerticalRecessCurtainProfile right_recess;
    // Independent L-shaped deep corners that are not paired into a complete recess.
    StandaloneDeepCornersCurtainProfile standalone_deep_corners;
    // Number of Long sprites added along each lower exterior corner edge.
    // Zero disables the supplement; values above one continue at equal spacing.
    int left_lower_corner_dir_a_supplement_count;
    int left_lower_corner_dir_b_supplement_count;
    int right_lower_corner_dir_a_supplement_count;
    int right_lower_corner_dir_b_supplement_count;
    CeilingCurtainPartProfile dir_a_long;
    CeilingCurtainPartProfile dir_b_long;
    CeilingCurtainPartProfile dir_a_wide;
    CeilingCurtainPartProfile dir_b_wide;
};

inline constexpr WallPartAsset make_wall_part_asset(
    int vid,
    uint32_t direction,
    float offset_x,
    float offset_y
) {
    return {
        .vid = vid,
        .direction = direction,
        .offset_x = offset_x,
        .offset_y = offset_y
    };
}

inline constexpr WallVariant make_wall_variant(
    int dir_a_vid,
    uint32_t dir_a_direction,
    float offset_a_x,
    float offset_a_y,
    int dir_b_vid,
    uint32_t dir_b_direction,
    float offset_b_x,
    float offset_b_y,
    int pillar_vid,
    uint32_t pillar_direction,
    float offset_p_x,
    float offset_p_y
) {
    return {
        .dir_a = make_wall_part_asset(dir_a_vid, dir_a_direction, offset_a_x, offset_a_y),
        .dir_b = make_wall_part_asset(dir_b_vid, dir_b_direction, offset_b_x, offset_b_y),
        .pillar = make_wall_part_asset(pillar_vid, pillar_direction, offset_p_x, offset_p_y)
    };
}

inline constexpr WallVariantPool make_fixed_wall_variant_pool(int variant_index) {
    return {
        .variant_indices = {variant_index},
        .variant_count = 1,
        .randomize = false
    };
}

inline constexpr WallVariantPool make_random_wall_variant_pool(
    int first_variant_index,
    int second_variant_index,
    int third_variant_index
) {
    return {
        .variant_indices = {first_variant_index, second_variant_index, third_variant_index},
        .variant_count = 3,
        .randomize = true
    };
}

} // namespace auto_mapper::core
