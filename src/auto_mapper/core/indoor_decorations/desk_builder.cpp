/**
 * @file desk_builder.cpp
 * @brief Implements desk, computer, and chair combo generation.
 */

#include "auto_mapper/core/indoor_decorations/desk_builder.h"
#include "auto_mapper/core/indoor_decorations/array_layout.h"
#include "auto_mapper/core/randomizer.h"

namespace auto_mapper::core::indoor_decorations {

namespace {

uint32_t pick_body_direction(const DeskTemplate& desk_template) {
    if (desk_template.body_directions.empty()) {
        return 0;
    }

    int direction = auto_mapper::core::Random::get(desk_template.body_directions);
    return static_cast<uint32_t>(direction);
}

const DeskTemplate& pick_array_desk_template() {
    int template_index = auto_mapper::core::Random::get(0, 3);
    if (template_index == 0) {
        return get_default_desk_template();
    }

    if (template_index == 1) {
        return get_up_left_desk_template();
    }

    if (template_index == 2) {
        return get_down_left_desk_template();
    }

    return get_up_right_desk_template();
}

void append_desk_sprites(
    std::vector<io::Sprite>& sprites,
    const DeskTemplate& desk_template,
    const DeskUnit& unit,
    uint32_t body_direction
) {
    sprites.push_back(io::Sprite(
        desk_template.body_vid,
        unit.pos_x,
        unit.pos_y,
        unit.pos_z,
        body_direction
    ));

    int computer_vid = DESK_ENABLED_COMPUTER_VID;
    if (!desk_template.computer_vids.empty()) {
        computer_vid = auto_mapper::core::Random::get(desk_template.computer_vids);
    }

    int computer_offset_x = auto_mapper::core::Random::get(
        static_cast<int>(desk_template.computer_offset_range.min_x),
        static_cast<int>(desk_template.computer_offset_range.max_x)
    );
    int computer_offset_y = auto_mapper::core::Random::get(
        static_cast<int>(desk_template.computer_offset_range.min_y),
        static_cast<int>(desk_template.computer_offset_range.max_y)
    );
    uint32_t computer_direction = 0;
    if (!desk_template.computer_directions.empty()) {
        int direction = auto_mapper::core::Random::get(desk_template.computer_directions);
        computer_direction = static_cast<uint32_t>(direction);
    }

    sprites.push_back(io::Sprite(
        computer_vid,
        unit.pos_x + static_cast<float>(computer_offset_x),
        unit.pos_y + static_cast<float>(computer_offset_y),
        DESK_COMPUTER_POS_Z,
        computer_direction
    ));

    int chair_offset_x = auto_mapper::core::Random::get(
        static_cast<int>(desk_template.chair_offset_range.min_x),
        static_cast<int>(desk_template.chair_offset_range.max_x)
    );
    int chair_offset_y = auto_mapper::core::Random::get(
        static_cast<int>(desk_template.chair_offset_range.min_y),
        static_cast<int>(desk_template.chair_offset_range.max_y)
    );
    uint32_t chair_direction = 0;
    if (!desk_template.chair_directions.empty()) {
        int direction = auto_mapper::core::Random::get(desk_template.chair_directions);
        chair_direction = static_cast<uint32_t>(direction);
    }

    sprites.push_back(io::Sprite(
        DESK_CHAIR_VID,
        unit.pos_x + static_cast<float>(chair_offset_x),
        unit.pos_y + static_cast<float>(chair_offset_y),
        DESK_CHAIR_POS_Z,
        chair_direction
    ));
}

} // namespace

const DeskTemplate& get_default_desk_template() {
    static const DeskTemplate desk_template = {
        .body_vid = DESK_BODY_VID,
        .body_directions = DESK_BODY_DIRECTIONS,
        .computer_vids = DESK_COMPUTER_VIDS,
        .computer_directions = DESK_COMPUTER_DIRECTIONS,
        .chair_directions = DESK_CHAIR_DIRECTIONS,
        .computer_offset_range = DESK_COMPUTER_OFFSET_RANGE,
        .chair_offset_range = DESK_CHAIR_OFFSET_RANGE,
    };

    return desk_template;
}

const DeskTemplate& get_up_left_desk_template() {
    static const DeskTemplate desk_template = {
        .body_vid = DESK_BODY_VID,
        .body_directions = DESK_UP_LEFT_BODY_DIRECTIONS,
        .computer_vids = DESK_COMPUTER_VIDS,
        .computer_directions = DESK_UP_LEFT_COMPUTER_DIRECTIONS,
        .chair_directions = DESK_UP_LEFT_CHAIR_DIRECTIONS,
        .computer_offset_range = DESK_UP_LEFT_COMPUTER_OFFSET_RANGE,
        .chair_offset_range = DESK_UP_LEFT_CHAIR_OFFSET_RANGE,
    };

    return desk_template;
}

const DeskTemplate& get_down_left_desk_template() {
    static const DeskTemplate desk_template = {
        .body_vid = DESK_BODY_VID,
        .body_directions = DESK_DOWN_LEFT_BODY_DIRECTIONS,
        .computer_vids = DESK_COMPUTER_VIDS,
        .computer_directions = DESK_DOWN_LEFT_COMPUTER_DIRECTIONS,
        .chair_directions = DESK_DOWN_LEFT_CHAIR_DIRECTIONS,
        .computer_offset_range = DESK_DOWN_LEFT_COMPUTER_OFFSET_RANGE,
        .chair_offset_range = DESK_DOWN_LEFT_CHAIR_OFFSET_RANGE,
        .footprint_row_length = DESK_SIDEWAYS_FOOTPRINT_ROW_LENGTH,
        .footprint_column_length = DESK_SIDEWAYS_FOOTPRINT_COLUMN_LENGTH,
    };

    return desk_template;
}

const DeskTemplate& get_up_right_desk_template() {
    static const DeskTemplate desk_template = {
        .body_vid = DESK_BODY_VID,
        .body_directions = DESK_UP_RIGHT_BODY_DIRECTIONS,
        .computer_vids = DESK_COMPUTER_VIDS,
        .computer_directions = DESK_UP_RIGHT_COMPUTER_DIRECTIONS,
        .chair_directions = DESK_UP_RIGHT_CHAIR_DIRECTIONS,
        .computer_offset_range = DESK_DOWN_LEFT_COMPUTER_OFFSET_RANGE,
        .chair_offset_range = DESK_UP_RIGHT_CHAIR_OFFSET_RANGE,
        .footprint_row_length = DESK_SIDEWAYS_FOOTPRINT_ROW_LENGTH,
        .footprint_column_length = DESK_SIDEWAYS_FOOTPRINT_COLUMN_LENGTH,
    };

    return desk_template;
}

std::vector<io::Sprite> DeskBuilder::build(const DeskUnit& unit) const {
    const DeskTemplate& desk_template = get_default_desk_template();
    std::vector<io::Sprite> sprites;

    uint32_t body_direction = pick_body_direction(desk_template);
    append_desk_sprites(sprites, desk_template, unit, body_direction);

    return sprites;
}

std::vector<io::Sprite> DeskBuilder::build_array(const DeskArray& array) const {
    std::vector<io::Sprite> sprites;

    const DeskTemplate& desk_template = pick_array_desk_template();

    ArrayLayout layout = {
        .start_x = array.start_x,
        .start_y = array.start_y,
        .row_length = array.row_length,
        .column_length = array.column_length,
        .item_axis = {
            .step_x = DESK_DEFAULT_ROW_SPACING_X,
            .step_y = DESK_DEFAULT_ROW_SPACING_Y,
            .spacing_scale = array.item_spacing_scale,
            .footprint_length = desk_template.footprint_row_length,
        },
        .row_axis = {
            .step_x = DESK_DEFAULT_COLUMN_SPACING_X,
            .step_y = DESK_DEFAULT_COLUMN_SPACING_Y,
            .spacing_scale = array.row_spacing_scale,
            .footprint_length = desk_template.footprint_column_length,
        },
    };
    std::vector<ArrayItemAnchor> anchors = calculate_array_item_anchors(layout);
    if (anchors.empty()) {
        return sprites;
    }

    uint32_t array_body_direction = pick_body_direction(desk_template);

    for (const ArrayItemAnchor& anchor : anchors) {
        DeskUnit unit = {
            .pos_x = anchor.pos_x,
            .pos_y = anchor.pos_y,
            .pos_z = array.pos_z,
        };
        append_desk_sprites(sprites, desk_template, unit, array_body_direction);
    }

    return sprites;
}

} // namespace auto_mapper::core::indoor_decorations
