/**
 * @file incubator_builder.cpp
 * @brief Implements laboratory incubator unit sprite generation.
 */

#include "auto_mapper/core/indoor_decorations/incubator_builder.h"
#include "auto_mapper/core/indoor_decorations/array_layout.h"
#include "auto_mapper/core/randomizer.h"

namespace auto_mapper::core::indoor_decorations {

namespace {

bool get_random_bool() {
    int selected_value = auto_mapper::core::Random::get(0, 1);

    return selected_value == 1;
}

} // namespace

std::vector<io::Sprite> IncubatorBuilder::build(const IncubatorUnit& unit) const {
    std::vector<io::Sprite> sprites;

    sprites.push_back(io::Sprite(
        INCUBATOR_BODY_VID,
        unit.pos_x,
        unit.pos_y,
        unit.options.pos_z,
        INCUBATOR_BODY_DIRECTION
    ));

    sprites.push_back(io::Sprite(
        INCUBATOR_AIR_WALL_VID,
        unit.pos_x,
        unit.pos_y,
        unit.options.pos_z,
        INCUBATOR_AIR_WALL_DIRECTION
    ));

    if (unit.options.with_big_computer) {
        uint32_t computer_direction = unit.options.computer_direction;

        if (!unit.options.use_fixed_computer_direction) {
            int direction = auto_mapper::core::Random::get(INCUBATOR_BIG_COMPUTER_DIRECTIONS);
            computer_direction = static_cast<uint32_t>(direction);
        }

        sprites.push_back(io::Sprite(
            INCUBATOR_BIG_COMPUTER_VID,
            unit.pos_x + unit.options.computer_offset_x,
            unit.pos_y + unit.options.computer_offset_y,
            unit.options.pos_z,
            computer_direction
        ));
    }

    return sprites;
}

std::vector<io::Sprite> IncubatorBuilder::build_array(const IncubatorArray& array) const {
    std::vector<io::Sprite> sprites;

    ArrayLayout layout = {
        .start_x = array.start_x,
        .start_y = array.start_y,
        .row_length = array.row_length,
        .column_length = array.column_length,
        .item_axis = {
            .step_x = INCUBATOR_DEFAULT_ITEM_STEP_X,
            .step_y = INCUBATOR_DEFAULT_ITEM_STEP_Y,
            .spacing_scale = array.item_spacing_scale,
            .footprint_length = INCUBATOR_FOOTPRINT_ITEM_LENGTH,
        },
        .row_axis = {
            .step_x = INCUBATOR_DEFAULT_ROW_STEP_X,
            .step_y = INCUBATOR_DEFAULT_ROW_STEP_Y,
            .spacing_scale = array.row_spacing_scale,
            .footprint_length = INCUBATOR_FOOTPRINT_ROW_LENGTH,
        },
    };
    std::vector<ArrayItemAnchor> anchors = calculate_array_item_anchors(layout);
    if (anchors.empty()) {
        return sprites;
    }

    IncubatorOptions array_options = array.options;
    if (array.randomize_big_computer) {
        array_options.with_big_computer = get_random_bool();
    }

    if (array_options.with_big_computer) {
        array_options.use_fixed_computer_direction = true;
        int direction = auto_mapper::core::Random::get(INCUBATOR_BIG_COMPUTER_DIRECTIONS);
        array_options.computer_direction = static_cast<uint32_t>(direction);
    }

    for (const ArrayItemAnchor& anchor : anchors) {
        IncubatorUnit unit = {
            .pos_x = anchor.pos_x,
            .pos_y = anchor.pos_y,
            .options = array_options,
        };

        std::vector<io::Sprite> unit_sprites = build(unit);
        sprites.insert(sprites.end(), unit_sprites.begin(), unit_sprites.end());
    }

    return sprites;
}

} // namespace auto_mapper::core::indoor_decorations
