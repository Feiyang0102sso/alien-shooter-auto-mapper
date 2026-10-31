/**
 * @file placeholder_vids.h
 * @brief Which stamp members get an editor placeholder mark.
 */

#pragma once

#include <cstdint>

namespace auto_mapper::core::decoration_builder {

/**
 * False for flat or densely stacked sprites (blood, shadows, terrain, books, ...)
 * whose marks would only clutter the canvas. Editor preview metadata only.
 */
bool draws_placeholder(int32_t vid);

/**
 * True for sprites that lie flat on the ground (blood, shadows, terrain).
 * The sprite preview draws them before every standing sprite. Editor preview metadata only.
 */
bool lies_on_ground(int32_t vid);

/**
 * True for BLOCK air walls, which the game never draws. The editor preview skips them
 * entirely (no sprite, no placeholder); generated maps still keep them.
 */
bool is_invisible_block(int32_t vid);

} // namespace auto_mapper::core::decoration_builder
