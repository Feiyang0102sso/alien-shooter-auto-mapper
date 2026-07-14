/**
 * @file desk_builder.h
 * @brief Build a desk, computer, and chair decoration combo.
 */

#pragma once

#include "auto_mapper/io/map_writer.h"

#include <cstdint>
#include <vector>

namespace auto_mapper::core::indoor_decorations {

struct DeskOffsetRange {
    float min_x;
    float max_x;
    float min_y;
    float max_y;
};

inline constexpr int DESK_BODY_VID = 404;
inline constexpr int DESK_BROKEN_COMPUTER_VID = 411;
inline constexpr int DESK_ENABLED_COMPUTER_VID = 410;
inline constexpr int DESK_CHAIR_VID = 407;
inline constexpr float DESK_COMPUTER_POS_Z = 32.0f;
inline constexpr float DESK_CHAIR_POS_Z = 0.0f;
inline constexpr float DESK_DEFAULT_ROW_SPACING_X = -120.0f;
inline constexpr float DESK_DEFAULT_ROW_SPACING_Y = 80.0f;
inline constexpr float DESK_DEFAULT_COLUMN_SPACING_X = -120.0f;
inline constexpr float DESK_DEFAULT_COLUMN_SPACING_Y = -80.0f;
inline constexpr float DESK_FOOTPRINT_ROW_LENGTH = 60.0f;
inline constexpr float DESK_FOOTPRINT_COLUMN_LENGTH = 90.0f;
inline constexpr float DESK_SIDEWAYS_FOOTPRINT_ROW_LENGTH = 90.0f;
inline constexpr float DESK_SIDEWAYS_FOOTPRINT_COLUMN_LENGTH = 60.0f;
inline constexpr DeskOffsetRange DESK_COMPUTER_OFFSET_RANGE = {
    .min_x = -6.0f,
    .max_x = -3.0f,
    .min_y = 0.0f,
    .max_y = 2.0f,
};
inline constexpr DeskOffsetRange DESK_CHAIR_OFFSET_RANGE = {
    .min_x = 25.0f,
    .max_x = 39.0f,
    .min_y = 28.0f,
    .max_y = 42.0f,
};
inline constexpr DeskOffsetRange DESK_UP_LEFT_COMPUTER_OFFSET_RANGE = {
    .min_x = -13.0f,
    .max_x = -3.0f,
    .min_y = 0.0f,
    .max_y = 4.0f,
};
inline constexpr DeskOffsetRange DESK_UP_LEFT_CHAIR_OFFSET_RANGE = {
    .min_x = -60.0f,
    .max_x = -31.0f,
    .min_y = -38.0f,
    .max_y = -26.0f,
};
inline constexpr DeskOffsetRange DESK_DOWN_LEFT_COMPUTER_OFFSET_RANGE = {
    .min_x = -7.0f,
    .max_x = 2.0f,
    .min_y = -4.0f,
    .max_y = 3.0f,
};
inline constexpr DeskOffsetRange DESK_DOWN_LEFT_CHAIR_OFFSET_RANGE = {
    .min_x = -55.0f,
    .max_x = -8.0f,
    .min_y = 28.0f,
    .max_y = 53.0f,
};
inline constexpr DeskOffsetRange DESK_UP_RIGHT_CHAIR_OFFSET_RANGE = {
    .min_x = 37.0f,
    .max_x = 53.0f,
    .min_y = -39.0f,
    .max_y = -17.0f,
};
inline const std::vector<int> DESK_COMPUTER_VIDS = {
    DESK_BROKEN_COMPUTER_VID,
    DESK_ENABLED_COMPUTER_VID,
};
inline const std::vector<int> DESK_COMPUTER_DIRECTIONS = {
    0,
    128,
};
inline const std::vector<int> DESK_CHAIR_DIRECTIONS = {
    42,
    128,
};
inline const std::vector<int> DESK_BODY_DIRECTIONS = {
    0,
    85,
    170,
};
inline const std::vector<int> DESK_UP_LEFT_COMPUTER_DIRECTIONS = {
    64,
    192,
};
inline const std::vector<int> DESK_UP_LEFT_CHAIR_DIRECTIONS = {
    85,
    128,
    213,
};
inline const std::vector<int> DESK_UP_LEFT_BODY_DIRECTIONS = {
    42,
    128,
    213,
};
inline const std::vector<int> DESK_DOWN_LEFT_COMPUTER_DIRECTIONS = {
    32,
    160,
};
inline const std::vector<int> DESK_DOWN_LEFT_CHAIR_DIRECTIONS = {
    0,
    42,
    85,
    128,
};
inline const std::vector<int> DESK_DOWN_LEFT_BODY_DIRECTIONS = {
    21,
    106,
    192,
};
inline const std::vector<int> DESK_UP_RIGHT_COMPUTER_DIRECTIONS = {
    96,
    224,
};
inline const std::vector<int> DESK_UP_RIGHT_CHAIR_DIRECTIONS = {
    42,
    85,
    128,
    170,
};
inline const std::vector<int> DESK_UP_RIGHT_BODY_DIRECTIONS = {
    64,
    149,
    234,
};

struct DeskTemplate {
    int body_vid = DESK_BODY_VID;
    std::vector<int> body_directions = DESK_BODY_DIRECTIONS;
    std::vector<int> computer_vids = DESK_COMPUTER_VIDS;
    std::vector<int> computer_directions = DESK_COMPUTER_DIRECTIONS;
    std::vector<int> chair_directions = DESK_CHAIR_DIRECTIONS;
    DeskOffsetRange computer_offset_range;
    DeskOffsetRange chair_offset_range;
    float footprint_row_length = DESK_FOOTPRINT_ROW_LENGTH;
    float footprint_column_length = DESK_FOOTPRINT_COLUMN_LENGTH;
};

struct DeskUnit {
    float pos_x = 0.0f;
    float pos_y = 0.0f;
    float pos_z = 0.0f;
};

struct DeskArray {
    float start_x;
    float start_y;
    float row_length = 0.0f;
    float column_length = 0.0f;
    float item_spacing_scale = 1.0f;
    float row_spacing_scale = 1.0f;
    float pos_z = 0.0f;
};

class DeskBuilder {
public:
    /**
     * @brief Generate one desk combo with a random computer and chair.
     */
    std::vector<io::Sprite> build(const DeskUnit& unit) const;

    /**
     * @brief Generate sprites for a desk area.
     */
    std::vector<io::Sprite> build_array(const DeskArray& array) const;
};

const DeskTemplate& get_default_desk_template();
const DeskTemplate& get_up_left_desk_template();
const DeskTemplate& get_down_left_desk_template();
const DeskTemplate& get_up_right_desk_template();

} // namespace auto_mapper::core::indoor_decorations
