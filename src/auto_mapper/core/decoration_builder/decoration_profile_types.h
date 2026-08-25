/**
 * @file decoration_profile_types.h
 * @brief Shared profile types for authored decoration stamps.
 */

#pragma once

#include "auto_mapper/core/wall_builder/wall_profile_types.h"
#include "auto_mapper/core/vid_properties/vid_property_types.h"

#include <array>
#include <cstdint>
#include <string_view>
#include <vector>

namespace auto_mapper::core::decoration_builder {

inline constexpr float DEFAULT_DECORATION_PLACEHOLDER_WIDTH = 40.0f;
inline constexpr float DEFAULT_DECORATION_PLACEHOLDER_HEIGHT = 28.0f;

struct DecorationPoint {
    float x = 0.0f;
    float y = 0.0f;
};

/**
 * One sprite stored relative to the decoration frame center.
 */
struct DecorationMemberProfile {
    int32_t vid = 0;
    float offset_x = 0.0f;
    float offset_y = 0.0f;
    float pos_z = 0.0f;
    uint32_t direction = 0;
    VidGamma gamma{};
    VidScale scale{};
};

/**
 * Coarse editor preview frame. Corners are stored relative to the frame center.
 */
struct DecorationFrameProfile {
    // Corner order: top, left, bottom, right.
    std::array<DecorationPoint, 4> corner_offsets{};
    float default_placeholder_width = DEFAULT_DECORATION_PLACEHOLDER_WIDTH;
    float default_placeholder_height = DEFAULT_DECORATION_PLACEHOLDER_HEIGHT;
};

/**
 * One authored decoration stamp.
 */
struct DecorationProfile {
    std::string_view id;
    DecorationFrameProfile frame;
    std::vector<DecorationMemberProfile> members;

    // Optional test-only wall style. Regular builds never emit this boundary.
    const WallProfile* test_boundary_wall_profile = nullptr;
};

/**
 * Runtime placement of one complete decoration stamp.
 */
struct DecorationPlacement {
    float center_x = 0.0f;
    float center_y = 0.0f;
};

struct DecorationBuildOptions {
    bool keep_boundary = false;
};

} // namespace auto_mapper::core::decoration_builder
