/**
 * @file dir_randomizer.h
 * @brief Randomize direction for generated wall, pillar, and floor sprites only.
 */

#pragma once

#include "auto_mapper/io/map_writer.h"
#include <vector>

namespace auto_mapper::core {


void randomize_wall_and_floor_directions(std::vector<io::Sprite>& sprites);
uint32_t get_random_direction();
uint32_t get_random_direction_from_list(
    const std::vector<uint32_t>& allowed_directions,
    uint32_t fallback_direction
);
int get_random_int_from_list(
    const std::vector<int>& allowed_values,
    int fallback_value
);

} // namespace auto_mapper::core
