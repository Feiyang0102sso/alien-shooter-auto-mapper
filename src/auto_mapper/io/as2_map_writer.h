/**
 * @file as2_map_writer.h
 * @brief internal AS2 and AS2R map writer entries
 */

#pragma once

#include "auto_mapper/io/map_writer.h"

namespace auto_mapper::io::detail {

bool write_as2_map(
    const std::vector<Sprite>& sprites,
    const std::string& output_path,
    float map_size_x,
    float map_size_y
);

bool write_as2r_map(
    const std::vector<Sprite>& sprites,
    const std::string& output_path,
    float map_size_x,
    float map_size_y
);

bool write_as2oe_map(
    const std::vector<Sprite>& sprites,
    const std::string& output_path,
    float map_size_x,
    float map_size_y
);

} // namespace auto_mapper::io::detail
