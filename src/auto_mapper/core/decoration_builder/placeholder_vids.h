/**
 * @file placeholder_vids.h
 * @brief Which stamp members get an editor placeholder mark.
 *
 * AS1 and the AS2 Series number their objects independently, so every query takes the
 * Map Format and reads that game's own list; AS2, AS2R and AS2OE share the AS2 lists.
 */

#pragma once

#include "auto_mapper/io/map_writer.h"

#include <cstdint>

namespace auto_mapper::core::decoration_builder {

/**
 * False for flat or densely stacked sprites (blood, shadows, terrain, shells, books, ...)
 * whose marks would only clutter the canvas. Editor preview metadata only.
 */
bool draws_placeholder(io::MapFormat format, int32_t vid);

/**
 * True for sprites that lie flat on the ground (blood, shadows, terrain, debris).
 * The sprite preview draws them before every standing sprite. Editor preview metadata only.
 */
bool lies_on_ground(io::MapFormat format, int32_t vid);

/**
 * True for BLOCK air walls, which the game never draws. The editor preview skips them
 * entirely (no sprite, no placeholder); generated maps still keep them.
 */
bool is_invisible_block(io::MapFormat format, int32_t vid);

} // namespace auto_mapper::core::decoration_builder
