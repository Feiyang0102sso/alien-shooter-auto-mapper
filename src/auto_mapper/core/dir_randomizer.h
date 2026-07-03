/**
 * @file dir_randomizer.h
 * @brief Randomize direction for generated wall, pillar, and floor sprites only.
 */

#pragma once

#include "auto_mapper/io/map_writer.h"
#include <vector>

namespace auto_mapper::core {


void randomize_wall_and_floor_directions(std::vector<io::Sprite>& sprites);

} // namespace auto_mapper::core
