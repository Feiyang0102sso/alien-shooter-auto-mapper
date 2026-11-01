/**
 * @file map_writer.h
 * @brief write map template for AS1
 */


#pragma once
#include "auto_mapper/core/vid_properties/vid_property_types.h"

#include <vector>
#include <string>
#include <cstdint>

namespace auto_mapper::io {

enum class MapFormat {
    AS1,
    AS2,
    AS2R,
    // Alien Shooter 2 Conscription: the AS2R file format, but objects.res has extra objects (corpses).
    AS2C,
    AS2OE
};

struct Sprite {
    int32_t vid{};
    float posX{};
    float posY{};
    float posZ = 0.0f;
    uint32_t direction = 32;
    core::VidArmy army = 0;

    // AS2 only: visual gamma and scale. AS1 SpriteRecord has no such fields.
    // Defaults (gamma all-zero, scale 1.0) match the previous hard-coded behavior.
    core::VidGamma gamma{};
    core::VidScale scale{};

    // AS2 only: SPRD nameText, written verbatim. Loot containers keep their whole loot text here
    // (space separated item names, the game picks one); empty for every other sprite.
    std::string name_text;

    Sprite() = default;
    Sprite(int32_t v, float x, float y, float z = 0.0f, uint32_t dir = 32)
        : vid(v), posX(x), posY(y), posZ(z), direction(dir), army(0) {}
};

/**
 * write sprite into .map AS1
 * @param sprites spirit list, ID from 1
 * @param output_path .map path
 * @param map_size_x mapX
 * @param map_size_y mapY
 * @return whether write success
 */
bool write_map(const std::vector<Sprite>& sprites, 
               const std::string& output_path, 
               float map_size_x = 600.0f, 
               float map_size_y = 600.0f);

/**
 * write sprites into selected .map format
 * @param sprites spirit list, ID from 1
 * @param output_path .map path
 * @param format target map format
 * @param map_size_x mapX
 * @param map_size_y mapY
 * @return whether write success
 */
bool write_map(const std::vector<Sprite>& sprites,
               const std::string& output_path,
               MapFormat format,
               float map_size_x = 600.0f,
               float map_size_y = 600.0f);

} // namespace auto_mapper::io
