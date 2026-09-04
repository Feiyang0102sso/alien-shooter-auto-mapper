/**
 * @file map_writer.cpp
 * @brief dispatch map writing by target format
 */

#include "auto_mapper/io/map_writer.h"

#include "auto_mapper/common/logger.h"
#include "auto_mapper/io/as1_map_writer.h"
#include "auto_mapper/io/as2_map_writer.h"

namespace auto_mapper::io {

bool write_map(
    const std::vector<Sprite>& sprites,
    const std::string& output_path,
    float map_size_x,
    float map_size_y
) {
    return detail::write_as1_map(sprites, output_path, map_size_x, map_size_y);
}

bool write_map(
    const std::vector<Sprite>& sprites,
    const std::string& output_path,
    MapFormat format,
    float map_size_x,
    float map_size_y
) {
    if (format == MapFormat::AS1) {
        return detail::write_as1_map(sprites, output_path, map_size_x, map_size_y);
    }

    if (format == MapFormat::AS2) {
        return detail::write_as2_map(sprites, output_path, map_size_x, map_size_y);
    }

    if (format == MapFormat::AS2R) {
        return detail::write_as2r_map(sprites, output_path, map_size_x, map_size_y);
    }

    if (format == MapFormat::AS2OE) {
        return detail::write_as2oe_map(sprites, output_path, map_size_x, map_size_y);
    }

    Logger::error("Unsupported map format");
    return false;
}

} // namespace auto_mapper::io
