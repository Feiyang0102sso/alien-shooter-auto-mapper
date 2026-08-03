/**
 * @file as1_map_writer.h
 * @brief internal AS1 map writer entry
 */

#pragma once

#include "auto_mapper/io/map_writer.h"

namespace auto_mapper::io::detail {

bool write_as1_map(
    const std::vector<Sprite>& sprites,
    const std::string& output_path,
    float map_size_x,
    float map_size_y
);

} // namespace auto_mapper::io::detail
