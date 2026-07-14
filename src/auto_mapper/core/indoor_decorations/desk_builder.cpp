/**
 * @file desk_builder.cpp
 * @brief Implements desk, computer, and chair combo generation.
 */

#include "auto_mapper/core/indoor_decorations/desk_builder.h"
#include "auto_mapper/core/randomizer.h"

#include <cmath>

namespace auto_mapper::core::indoor_decorations {

namespace {

float get_step_length(float step_x, float step_y, float spacing_scale) {
    return std::sqrt(step_x * step_x + step_y * step_y) * spacing_scale;
}

// Count anchors whose full footprint can fit in the selected area.
int get_slot_count(
    float area_length,
    float step_x,
    float step_y,
    float spacing_scale,
    float footprint_length
) {
    if (area_length < 0.0f) {
        return 0;
    }

    if (spacing_scale <= 0.0f) {
        return 0;
    }

    float step_length = get_step_length(step_x, step_y, spacing_scale);
    if (step_length <= 0.0f) {
        return 0;
    }

    if (area_length <= footprint_length) {
        return 1;
    }

    float anchor_area_length = area_length - footprint_length;
    return static_cast<int>(std::floor(anchor_area_length / step_length)) + 1;
}

float get_center_offset_distance(
    float area_length,
    int slot_count,
    float step_length,
    float footprint_length
) {
    if (slot_count <= 0) {
        return 0.0f;
    }

    float occupied_length = footprint_length + static_cast<float>(slot_count - 1) * step_length;
    float remaining_length = area_length - occupied_length;
    if (remaining_length <= 0.0f) {
        return 0.0f;
    }

    return remaining_length / 2.0f;
}

float get_axis_offset_x(float axis_x, float axis_y, float distance) {
    float axis_length = std::sqrt(axis_x * axis_x + axis_y * axis_y);
    if (axis_length <= 0.0f) {
        return 0.0f;
    }

    return axis_x / axis_length * distance;
}

float get_axis_offset_y(float axis_x, float axis_y, float distance) {
    float axis_length = std::sqrt(axis_x * axis_x + axis_y * axis_y);
    if (axis_length <= 0.0f) {
        return 0.0f;
    }

    return axis_y / axis_length * distance;
}

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

    float item_step_x = DESK_DEFAULT_ROW_SPACING_X * array.item_spacing_scale;
    float item_step_y = DESK_DEFAULT_ROW_SPACING_Y * array.item_spacing_scale;
    float row_step_x = DESK_DEFAULT_COLUMN_SPACING_X * array.row_spacing_scale;
    float row_step_y = DESK_DEFAULT_COLUMN_SPACING_Y * array.row_spacing_scale;
    float item_step_length = get_step_length(
        DESK_DEFAULT_ROW_SPACING_X,
        DESK_DEFAULT_ROW_SPACING_Y,
        array.item_spacing_scale
    );
    float row_step_length = get_step_length(
        DESK_DEFAULT_COLUMN_SPACING_X,
        DESK_DEFAULT_COLUMN_SPACING_Y,
        array.row_spacing_scale
    );

    const DeskTemplate& desk_template = pick_array_desk_template();

    int items_per_row = get_slot_count(
        array.row_length,
        DESK_DEFAULT_ROW_SPACING_X,
        DESK_DEFAULT_ROW_SPACING_Y,
        array.item_spacing_scale,
        desk_template.footprint_row_length
    );

    int row_count = get_slot_count(
        array.column_length,
        DESK_DEFAULT_COLUMN_SPACING_X,
        DESK_DEFAULT_COLUMN_SPACING_Y,
        array.row_spacing_scale,
        desk_template.footprint_column_length
    );

    if (items_per_row <= 0) {
        return sprites;
    }

    if (row_count <= 0) {
        return sprites;
    }

    float item_center_distance = get_center_offset_distance(
        array.row_length,
        items_per_row,
        item_step_length,
        desk_template.footprint_row_length
    );
    float row_center_distance = get_center_offset_distance(
        array.column_length,
        row_count,
        row_step_length,
        desk_template.footprint_column_length
    );
    float item_center_offset_x = get_axis_offset_x(
        DESK_DEFAULT_ROW_SPACING_X,
        DESK_DEFAULT_ROW_SPACING_Y,
        item_center_distance
    );
    float item_center_offset_y = get_axis_offset_y(
        DESK_DEFAULT_ROW_SPACING_X,
        DESK_DEFAULT_ROW_SPACING_Y,
        item_center_distance
    );
    float row_center_offset_x = get_axis_offset_x(
        DESK_DEFAULT_COLUMN_SPACING_X,
        DESK_DEFAULT_COLUMN_SPACING_Y,
        row_center_distance
    );
    float row_center_offset_y = get_axis_offset_y(
        DESK_DEFAULT_COLUMN_SPACING_X,
        DESK_DEFAULT_COLUMN_SPACING_Y,
        row_center_distance
    );

    float footprint_offset_row_x = get_axis_offset_x(
        DESK_DEFAULT_ROW_SPACING_X,
        DESK_DEFAULT_ROW_SPACING_Y,
        desk_template.footprint_row_length / 2.0f
    );
    float footprint_offset_row_y = get_axis_offset_y(
        DESK_DEFAULT_ROW_SPACING_X,
        DESK_DEFAULT_ROW_SPACING_Y,
        desk_template.footprint_row_length / 2.0f
    );
    float footprint_offset_col_x = get_axis_offset_x(
        DESK_DEFAULT_COLUMN_SPACING_X,
        DESK_DEFAULT_COLUMN_SPACING_Y,
        desk_template.footprint_column_length / 2.0f
    );
    float footprint_offset_col_y = get_axis_offset_y(
        DESK_DEFAULT_COLUMN_SPACING_X,
        DESK_DEFAULT_COLUMN_SPACING_Y,
        desk_template.footprint_column_length / 2.0f
    );

    uint32_t array_body_direction = pick_body_direction(desk_template);

    for (int row_index = 0; row_index < row_count; ++row_index) {
        float row_start_x = array.start_x;
        row_start_x += item_center_offset_x;
        row_start_x += row_center_offset_x;
        row_start_x += footprint_offset_row_x;
        row_start_x += footprint_offset_col_x;

        float row_start_y = array.start_y;
        row_start_y += item_center_offset_y;
        row_start_y += row_center_offset_y;
        row_start_y += footprint_offset_row_y;
        row_start_y += footprint_offset_col_y;

        row_start_x += static_cast<float>(row_index) * row_step_x;
        row_start_y += static_cast<float>(row_index) * row_step_y;

        for (int item_index = 0; item_index < items_per_row; ++item_index) {
            DeskUnit unit = {
                .pos_x = row_start_x,
                .pos_y = row_start_y,
                .pos_z = array.pos_z
            };

            unit.pos_x += static_cast<float>(item_index) * item_step_x;
            unit.pos_y += static_cast<float>(item_index) * item_step_y;
            append_desk_sprites(sprites, desk_template, unit, array_body_direction);
        }
    }

    return sprites;
}

} // namespace auto_mapper::core::indoor_decorations
