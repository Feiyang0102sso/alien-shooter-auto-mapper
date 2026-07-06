/**
 * @file incubator_builder.h
 * @brief Build one laboratory incubator decoration unit.
 */

#pragma once

#include "auto_mapper/io/map_writer.h"
#include <vector>

namespace auto_mapper::core::indoor_decorations {

inline constexpr int INCUBATOR_BODY_VID = 443;
inline constexpr int INCUBATOR_AIR_WALL_VID = 631;
inline constexpr int INCUBATOR_BIG_COMPUTER_VID = 135;
inline constexpr float INCUBATOR_DEFAULT_ROW_SPACING_X = 150.0f;
inline constexpr float INCUBATOR_DEFAULT_ROW_SPACING_Y = -130.0f;
inline constexpr float INCUBATOR_DEFAULT_COLUMN_SPACING_X = 150.0f;
inline constexpr float INCUBATOR_DEFAULT_COLUMN_SPACING_Y = 130.0f;

inline constexpr uint32_t INCUBATOR_BODY_DIRECTION = 0;
inline constexpr uint32_t INCUBATOR_AIR_WALL_DIRECTION = 128;
inline const std::vector<uint32_t> INCUBATOR_BIG_COMPUTER_DIRECTIONS = {
    18,
    54,
    91,
    128
};

struct IncubatorOptions {
    float pos_z = 0.0f;
    bool with_big_computer = false;
    bool use_fixed_computer_direction = false;
    uint32_t computer_direction = INCUBATOR_BIG_COMPUTER_DIRECTIONS[0];
    float computer_offset_x = 28.0f;
    float computer_offset_y = -7.0f;
};

struct IncubatorUnit {
    float pos_x;
    float pos_y;
    IncubatorOptions options = {};
};

struct IncubatorArray {
    float start_x;
    float start_y;
    float row_length = 0.0f;
    float column_length = 0.0f;
    float item_spacing_scale = 1.0f;
    float row_spacing_scale = 1.0f;
    bool randomize_big_computer = false;
    IncubatorOptions options = {};
};

class IncubatorBuilder {
public:
    /**
     * @brief Generate sprites for one incubator unit.
     */
    std::vector<io::Sprite> build(const IncubatorUnit& unit) const;

    /**
     * @brief Generate sprites for an incubator area.
     */
    std::vector<io::Sprite> build_array(const IncubatorArray& array) const;
};

} // namespace auto_mapper::core::indoor_decorations
