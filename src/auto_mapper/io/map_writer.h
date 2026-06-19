/**
 * @file map_writer.h
 * @brief write map template for AS1
 */


#pragma once
#include <vector>
#include <string>
#include <cstdint>

namespace auto_mapper::io {

struct Sprite {
    int32_t vid{};
    float posX{};
    float posY{};
    float posZ = 0.0f;
    uint32_t direction = 32;
    int32_t army = 0;
};

/**
 * write sprite into .map AS1
 * @param sprites spirit list, ID from 1
 * @param output_path .map path
 * @param map_size_x mapX
 * @param map_size_y mapY
 * @param template_path empty.map template
 * @return whether write success
 */
bool write_map(const std::vector<Sprite>& sprites, 
               const std::string& output_path, 
               float map_size_x = 600.0f, 
               float map_size_y = 600.0f,
               const std::string& template_path = "src/auto_mapper/io/templates/empty.map");

} // namespace auto_mapper::io
