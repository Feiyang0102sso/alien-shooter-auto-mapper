/**
 * @file desk_builder.cpp
 * @brief Implements desk, computer, and chair combo generation.
 */

#include "auto_mapper/core/indoor_decorations/desk_builder.h"
#include "auto_mapper/core/dir_randomizer.h"

#include <random>

namespace auto_mapper::core::indoor_decorations {

namespace {

float get_random_float(float min_value, float max_value) {
    static thread_local std::mt19937 random_engine(std::random_device{}());
    std::uniform_real_distribution<float> distribution(min_value, max_value);

    return distribution(random_engine);
}

uint32_t pick_body_direction(const DeskTemplate& desk_template) {
    return auto_mapper::core::get_random_direction_from_list(
        desk_template.body_directions,
        0
    );
}

} // namespace

const DeskTemplate& get_default_desk_template() {
    static const DeskTemplate desk_template = {
        .body_vid = DESK_BODY_VID,
        .body_directions = DESK_BODY_DIRECTIONS,
        .computer_vids = DESK_COMPUTER_VIDS,
        .computer_directions = DESK_COMPUTER_DIRECTIONS,
        .computer_offset_range = {
            .min_x = -13.0f,
            .max_x = -3.0f,
            .min_y = 0.0f,
            .max_y = 4.0f,
        },
        .chair_offset_range = {
            .min_x = 25.0f,
            .max_x = 39.0f,
            .min_y = 28.0f,
            .max_y = 42.0f,
        },
    };

    return desk_template;
}

std::vector<io::Sprite> DeskBuilder::build(const DeskUnit& unit) const {
    return build_with_preset(unit, 0);
}

std::vector<io::Sprite> DeskBuilder::build_with_preset(
    const DeskUnit& unit,
    std::size_t preset_index
) const {
    (void)preset_index;

    const DeskTemplate& desk_template = get_default_desk_template();
    std::vector<io::Sprite> sprites;

    uint32_t body_direction = pick_body_direction(desk_template);
    if (unit.use_fixed_body_direction) {
        body_direction = unit.body_direction;
    }

    sprites.push_back(io::Sprite(
        desk_template.body_vid,
        unit.pos_x,
        unit.pos_y,
        unit.pos_z,
        body_direction
    ));

    int computer_vid = auto_mapper::core::get_random_int_from_list(
        desk_template.computer_vids,
        DESK_ENABLED_COMPUTER_VID
    );
    float computer_offset_x = get_random_float(
        desk_template.computer_offset_range.min_x,
        desk_template.computer_offset_range.max_x
    );
    float computer_offset_y = get_random_float(
        desk_template.computer_offset_range.min_y,
        desk_template.computer_offset_range.max_y
    );
    uint32_t computer_direction = auto_mapper::core::get_random_direction_from_list(
        desk_template.computer_directions,
        0
    );

    sprites.push_back(io::Sprite(
        computer_vid,
        unit.pos_x + computer_offset_x,
        unit.pos_y + computer_offset_y,
        DESK_COMPUTER_POS_Z,
        computer_direction
    ));

    float chair_offset_x = get_random_float(
        desk_template.chair_offset_range.min_x,
        desk_template.chair_offset_range.max_x
    );
    float chair_offset_y = get_random_float(
        desk_template.chair_offset_range.min_y,
        desk_template.chair_offset_range.max_y
    );

    sprites.push_back(io::Sprite(
        DESK_CHAIR_VID,
        unit.pos_x + chair_offset_x,
        unit.pos_y + chair_offset_y,
        DESK_CHAIR_POS_Z,
        auto_mapper::core::get_random_direction()
    ));

    return sprites;
}

std::vector<io::Sprite> DeskBuilder::build_batch(const std::vector<DeskUnit>& units) const {
    const DeskTemplate& desk_template = get_default_desk_template();
    uint32_t batch_body_direction = pick_body_direction(desk_template);
    std::vector<io::Sprite> sprites;

    for (DeskUnit unit : units) {
        unit.use_fixed_body_direction = true;
        unit.body_direction = batch_body_direction;

        std::vector<io::Sprite> desk_sprites = build(unit);
        sprites.insert(sprites.end(), desk_sprites.begin(), desk_sprites.end());
    }

    return sprites;
}

} // namespace auto_mapper::core::indoor_decorations
