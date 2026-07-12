/**
 * @file desk_builder.h
 * @brief Build a desk, computer, and chair decoration combo.
 */

#pragma once

#include "auto_mapper/io/map_writer.h"

#include <cstdint>
#include <vector>

namespace auto_mapper::core::indoor_decorations {

inline constexpr int DESK_BODY_VID = 404;
inline constexpr int DESK_BROKEN_COMPUTER_VID = 411;
inline constexpr int DESK_ENABLED_COMPUTER_VID = 410;
inline constexpr int DESK_CHAIR_VID = 407;
inline constexpr float DESK_COMPUTER_POS_Z = 32.0f;
inline constexpr float DESK_CHAIR_POS_Z = 0.0f;
inline const std::vector<int> DESK_COMPUTER_VIDS = {
    DESK_BROKEN_COMPUTER_VID,
    DESK_ENABLED_COMPUTER_VID,
};
inline const std::vector<int> DESK_COMPUTER_DIRECTIONS = {
    0,
    128,
};
inline const std::vector<int> DESK_BODY_DIRECTIONS = {
    0,
    85,
    170,
};

struct DeskOffsetRange {
    float min_x;
    float max_x;
    float min_y;
    float max_y;
};

struct DeskTemplate {
    int body_vid = DESK_BODY_VID;
    std::vector<int> body_directions = DESK_BODY_DIRECTIONS;
    std::vector<int> computer_vids = DESK_COMPUTER_VIDS;
    std::vector<int> computer_directions = DESK_COMPUTER_DIRECTIONS;
    DeskOffsetRange computer_offset_range;
    DeskOffsetRange chair_offset_range;
};

struct DeskUnit {
    float pos_x = 0.0f;
    float pos_y = 0.0f;
    float pos_z = 0.0f;
    bool use_fixed_body_direction = false;
    uint32_t body_direction = 0;
};

class DeskBuilder {
public:
    /**
     * @brief Generate one desk combo with a random computer and chair.
     */
    std::vector<io::Sprite> build(const DeskUnit& unit) const;

    /**
     * @brief Generate one desk combo. The preset index is reserved for compatibility.
     */
    std::vector<io::Sprite> build_with_preset(const DeskUnit& unit, std::size_t preset_index) const;

    /**
     * @brief Generate a desk batch. Every desk in the batch uses the same random body direction.
     */
    std::vector<io::Sprite> build_batch(const std::vector<DeskUnit>& units) const;
};

const DeskTemplate& get_default_desk_template();

} // namespace auto_mapper::core::indoor_decorations
