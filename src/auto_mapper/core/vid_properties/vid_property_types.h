/**
 * @file vid_property_types.h
 * @brief Per-sprite property value types: army, gamma, and scale.
 *
 * These are user-facing values, not the raw binary layout stored in .map
 * files. The IO writer stores Army directly and encodes VidGamma as two BGRA
 * blocks plus scale as three floats, matching AS2_MAP.bt.
 *
 * Army is globally bound to a VID. Gamma and scale are contextual: the same
 * VID can use different visual values for a wall set, door state, decoration
 * type, or another placement context.
 */

#pragma once

#include <cstdint>

namespace auto_mapper::core {

/**
 * Army allegiance stored on a sprite record.
 *
 * The value is defined by the sprite VID and is shared across map formats.
 */
using VidArmy = int32_t;

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
