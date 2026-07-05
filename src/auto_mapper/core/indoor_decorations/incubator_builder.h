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
inline constexpr float INCUBATOR_DEFAULT_SPACING_X = 150.0f;
inline constexpr float INCUBATOR_DEFAULT_SPACING_Y = 130.0f;

inline constexpr uint32_t INCUBATOR_BODY_DIRECTION = 0;
inline constexpr uint32_t INCUBATOR_AIR_WALL_DIRECTION = 128;
inline const std::vector<uint32_t> INCUBATOR_BIG_COMPUTER_DIRECTIONS = {
    18,
    54,
    91,
    128
};

struct IncubatorUnit {
    float pos_x;
    float pos_y;
    float pos_z = 0.0f;
    bool with_big_computer = false;
    float computer_offset_x = 28.0f;
    float computer_offset_y = -7.0f;
};

enum IncubatorArrayDirection {
    INCUBATOR_ARRAY_HORIZONTAL = 0,
    INCUBATOR_ARRAY_VERTICAL = 1
};

struct IncubatorArray {
    float start_x;
    float start_y;
    float pos_z = 0.0f;
    int count = 1;
    IncubatorArrayDirection direction = INCUBATOR_ARRAY_HORIZONTAL;
    float spacing_x = INCUBATOR_DEFAULT_SPACING_X;
    float spacing_y = INCUBATOR_DEFAULT_SPACING_Y;
    bool with_big_computer = false;
    float computer_offset_x = 28.0f;
    float computer_offset_y = -7.0f;
};

class IncubatorBuilder {
public:
    /**
     * @brief Generate sprites for one incubator unit.
     */
    std::vector<io::Sprite> build(const IncubatorUnit& unit) const;

    /**
     * @brief Generate sprites for a horizontal row or vertical column.
     */
    std::vector<io::Sprite> build_array(const IncubatorArray& array) const;
};

} // namespace auto_mapper::core::indoor_decorations
