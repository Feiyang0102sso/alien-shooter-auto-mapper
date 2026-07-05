/**
 * @file incubator_builder.cpp
 * @brief Implements laboratory incubator unit sprite generation.
 */

#include "auto_mapper/core/indoor_decorations/incubator_builder.h"
#include "auto_mapper/core/dir_randomizer.h"

namespace auto_mapper::core::indoor_decorations {

std::vector<io::Sprite> IncubatorBuilder::build(const IncubatorUnit& unit) const {
    std::vector<io::Sprite> sprites;

    sprites.push_back(io::Sprite(
        INCUBATOR_BODY_VID,
        unit.pos_x,
        unit.pos_y,
        unit.pos_z,
        INCUBATOR_BODY_DIRECTION
    ));

    sprites.push_back(io::Sprite(
        INCUBATOR_AIR_WALL_VID,
        unit.pos_x,
        unit.pos_y,
        unit.pos_z,
        INCUBATOR_AIR_WALL_DIRECTION
    ));

    if (unit.with_big_computer) {
        uint32_t computer_direction = auto_mapper::core::get_random_direction_from_list(
            INCUBATOR_BIG_COMPUTER_DIRECTIONS,
            INCUBATOR_BIG_COMPUTER_DIRECTIONS[0]
        );

        sprites.push_back(io::Sprite(
            INCUBATOR_BIG_COMPUTER_VID,
            unit.pos_x + unit.computer_offset_x,
            unit.pos_y + unit.computer_offset_y,
            unit.pos_z,
            computer_direction
        ));
    }

    return sprites;
}

std::vector<io::Sprite> IncubatorBuilder::build_array(const IncubatorArray& array) const {
    std::vector<io::Sprite> sprites;

    if (array.count <= 0) {
        return sprites;
    }

    for (int index = 0; index < array.count; ++index) {
        IncubatorUnit unit = {
            .pos_x = array.start_x,
            .pos_y = array.start_y,
            .pos_z = array.pos_z,
            .with_big_computer = array.with_big_computer,
            .computer_offset_x = array.computer_offset_x,
            .computer_offset_y = array.computer_offset_y
        };

        if (array.direction == INCUBATOR_ARRAY_HORIZONTAL) {
            unit.pos_x += static_cast<float>(index) * array.spacing_x;
            unit.pos_y -= static_cast<float>(index) * array.spacing_y;
        }

        if (array.direction == INCUBATOR_ARRAY_VERTICAL) {
            unit.pos_x += static_cast<float>(index) * array.spacing_x;
            unit.pos_y += static_cast<float>(index) * array.spacing_y;
        }

        std::vector<io::Sprite> unit_sprites = build(unit);
        sprites.insert(sprites.end(), unit_sprites.begin(), unit_sprites.end());
    }

    return sprites;
}

} // namespace auto_mapper::core::indoor_decorations
