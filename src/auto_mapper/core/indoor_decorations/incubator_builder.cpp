/**
 * @file incubator_builder.cpp
 * @brief Implements laboratory incubator unit sprite generation.
 */

#include "auto_mapper/core/indoor_decorations/incubator_builder.h"
#include "auto_mapper/core/dir_randomizer.h"
#include <cmath>
#include <random>

namespace auto_mapper::core::indoor_decorations {

namespace {

// how many can be placed
int get_slot_count(float area_length, float step_x, float step_y, float spacing_scale) {
    if (area_length < 0.0f) {
        return 0;
    }

    if (spacing_scale <= 0.0f) {
        return 0;
    }

    float step_length = std::sqrt(step_x * step_x + step_y * step_y) * spacing_scale;
    if (step_length <= 0.0f) {
        return 0;
    }

    return static_cast<int>(std::floor(area_length / step_length)) + 1;
}

bool get_random_bool() {
    static thread_local std::mt19937 random_engine(std::random_device{}());
    std::uniform_int_distribution<int> distribution(0, 1);
    int selected_value = distribution(random_engine);

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
            computer_direction = auto_mapper::core::get_random_direction_from_list(
                INCUBATOR_BIG_COMPUTER_DIRECTIONS,
                INCUBATOR_BIG_COMPUTER_DIRECTIONS[0]
            );
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

    float row_step_x = INCUBATOR_DEFAULT_COLUMN_SPACING_X * array.row_spacing_scale;
    float row_step_y = INCUBATOR_DEFAULT_COLUMN_SPACING_Y * array.row_spacing_scale;

    int items_per_row = get_slot_count(
        array.row_length,
        INCUBATOR_DEFAULT_ROW_SPACING_X,
        INCUBATOR_DEFAULT_ROW_SPACING_Y,
        array.item_spacing_scale
    );

    int row_count = get_slot_count(
        array.column_length,
        INCUBATOR_DEFAULT_COLUMN_SPACING_X,
        INCUBATOR_DEFAULT_COLUMN_SPACING_Y,
        array.row_spacing_scale
    );

    if (items_per_row <= 0) {
        return sprites;
    }

    if (row_count <= 0) {
        return sprites;
    }

    IncubatorOptions array_options = array.options;
    if (array.randomize_big_computer) {
        array_options.with_big_computer = get_random_bool();
    }

    if (array_options.with_big_computer) {
        array_options.use_fixed_computer_direction = true;
        array_options.computer_direction = auto_mapper::core::get_random_direction_from_list(
            INCUBATOR_BIG_COMPUTER_DIRECTIONS,
            INCUBATOR_BIG_COMPUTER_DIRECTIONS[0]
        );
    }

    for (int row_index = 0; row_index < row_count; ++row_index) {
        float row_start_x = array.start_x + static_cast<float>(row_index) * row_step_x;
        float row_start_y = array.start_y + static_cast<float>(row_index) * row_step_y;

        for (int item_index = 0; item_index < items_per_row; ++item_index) {
            IncubatorUnit unit = {
                .pos_x = row_start_x,
                .pos_y = row_start_y,
                .options = array_options
            };

            unit.pos_x += static_cast<float>(item_index) * INCUBATOR_DEFAULT_ROW_SPACING_X * array.item_spacing_scale;
            unit.pos_y += static_cast<float>(item_index) * INCUBATOR_DEFAULT_ROW_SPACING_Y * array.item_spacing_scale;

            std::vector<io::Sprite> unit_sprites = build(unit);
            sprites.insert(sprites.end(), unit_sprites.begin(), unit_sprites.end());
        }
    }

    return sprites;
}

} // namespace auto_mapper::core::indoor_decorations
