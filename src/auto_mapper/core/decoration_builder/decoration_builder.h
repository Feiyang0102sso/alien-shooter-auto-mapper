/**
 * @file decoration_builder.h
 * @brief Expand a Decoration Stamp at a target Map Coordinate.
 */

#pragma once

#include "auto_mapper/core/decoration_builder/decoration_stamp.h"
#include "auto_mapper/io/map_writer.h"

#include <vector>

namespace auto_mapper::core::decoration_builder {

/**
 * Build every member of one stamp placed with its region center at (center_x, center_y).
 * Placement is translation only; all other sprite fields are copied from the library map.
 */
std::vector<io::Sprite> build_stamp(const DecorationStamp& stamp, float center_x, float center_y);

} // namespace auto_mapper::core::decoration_builder
