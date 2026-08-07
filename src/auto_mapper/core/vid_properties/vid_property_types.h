/**
 * @file vid_property_types.h
 * @brief Per-sprite visual property value types: gamma and scale.
 *
 * These are user-facing values, not the raw binary layout stored in .map
 * files. The IO writer encodes them into the SpriteRecord format described
 * in AS2_MAP.bt (VidGamma as two BGRA blocks, scale as three floats).
 *
 * Properties are NOT globally bound to a VID. The same VID can carry
 * different gamma/scale depending on context (wall set, door state,
 * decoration type, etc.). Lookup tables that pair (context, vid) to
 * these values live in separate profile headers.
 */

#pragma once

#include <cstdint>

namespace auto_mapper::core {

/**
 * Visual gamma for a single sprite.
 *
 * Maps to the 8-byte VidGamma block in SpriteRecord .
 * Each channel range is [-255, 255]:
 *   negative = darken, stored as absolute value in the first BGRA block.
 *   positive = brighten, stored directly in the second BGRA block.
 * Only one side (negative or positive) should be non-zero per channel.
 */
struct VidGamma {
    int r = 0;
    int g = 0;
    int b = 0;
    int a = 0;
};

/**
 * Scale for a single sprite across three axes.
 *
 * Maps to scaleX/scaleY/scaleZ in SpriteRecord.
 * Default 1.0 means "no scaling"
 */
struct VidScale {
    float x = 1.0f;
    float y = 1.0f;
    float z = 1.0f;
};

} // namespace auto_mapper::core
