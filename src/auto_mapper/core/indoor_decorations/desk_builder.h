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

struct DeskTemplate {
    int body_vid = DESK_BODY_VID;
    std::vector<int> body_directions = DESK_BODY_DIRECTIONS;
    std::vector<int> computer_vids = DESK_COMPUTER_VIDS;
    std::vector<int> computer_directions = DESK_COMPUTER_DIRECTIONS;
    std::vector<int> chair_directions = DESK_CHAIR_DIRECTIONS;
    DeskOffsetRange computer_offset_range;
    DeskOffsetRange chair_offset_range;
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

} // namespace auto_mapper::core::indoor_decorations
