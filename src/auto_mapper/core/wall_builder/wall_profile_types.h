/**
 * @file wall_profile_types.h
 * @brief Shared wall profile data structures.
 */

#pragma once

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

struct WallPartAsset {
    int vid;
    uint32_t direction;
    float offset_x;
    float offset_y;
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
