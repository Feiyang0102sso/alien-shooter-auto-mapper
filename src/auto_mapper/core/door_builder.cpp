/**
 * @file door_builder.cpp
 * @brief Implements door generation pipeline.
 */

#include "auto_mapper/core/door_builder.h"
#include "auto_mapper/core/wall_builder.h"
#include "auto_mapper/common/logger.h"
#include <cmath>
#include <unordered_map>

namespace auto_mapper::core {

constexpr int LAB_LASER_FRAME_VID = 653;
constexpr int LAB_DEAD_DOOR_VID = 654;
constexpr int LAB_LASER_PILLAR_VID = 164;
constexpr float LAB_LASER_PILLAR_OFFSET_X = 0.0f;
constexpr float LAB_LASER_PILLAR_OFFSET_Y = -18.0f;

// Copy-paste static helper for isometric alignment, matching wall_builder.cpp exactly
static MapPoint get_door_wall_shift(float map_size_x, const WallProfile& profile) {
    float divisor = static_cast<float>(profile.grid_divisor);
    float grid_step_x = profile.step_x / divisor;
    float grid_step_y = profile.step_y / divisor;

    float remainder_x = grid_step_x / 2.0f;
    float remainder_y = grid_step_y / 2.0f;

    float raw_shift_x = map_size_x / 2.0f;
    float grid_x_shift = std::round((raw_shift_x - remainder_x) / grid_step_x);
    float shift_x = grid_x_shift * grid_step_x + remainder_x;

    float raw_shift_y = remainder_y;
    float grid_y_shift = std::round((raw_shift_y - remainder_y) / grid_step_y);
    float shift_y = grid_y_shift * grid_step_y + remainder_y + profile.step_y;

    return {shift_x, shift_y};
}

static void apply_door_direction_offset(MapPoint& pos, int direction_type, int size, const WallProfile& profile) {
    if (direction_type == 0) {
        pos.x += profile.offset_a_x;
        pos.y += profile.offset_a_y;
        return;
    }

    pos.x += profile.offset_b_x;
    pos.y += profile.offset_b_y;

    if (size == 2) {
        float divisor = static_cast<float>(profile.grid_divisor);
        pos.x += profile.step_x / divisor;
        pos.y += profile.step_y / divisor;
    }
}

static bool is_lab_dead_door(const DoorInstance& door) {
    return door.light_state == LIGHT_STATE_BROKEN;
}

DoorBuilder::DoorBuilder(float map_size_x, float map_size_y)
    : map_size_x_(map_size_x), map_size_y_(map_size_y) {}

const DoorProfile& DoorBuilder::get_door_profile(int wall_type) {
    static const std::unordered_map<int, DoorProfile> profiles = {
        // Standard wall doors:
        // Frame: 606 (small), 608 (large)
        // Panel Small: 605 (open), 617 (closed)
        // Panel Large: 607 (open), 611 (closed)
        // Light: 423 (green), 424 (red), 425 (broken)
        { WALL_TYPE_STANDARD, { 606, 608, 605, 617, 607, 611, 423, 424, 425 } },
    };

    if (profiles.find(wall_type) != profiles.end()) {
        return profiles.at(wall_type);
    }
    return profiles.at(WALL_TYPE_STANDARD);
}

std::vector<io::Sprite> DoorBuilder::build(const std::vector<DoorInstance>& doors) const {
    std::vector<io::Sprite> door_sprites;
    door_sprites.reserve(doors.size() * 3); // frame + panel + light

    for (const auto& door : doors) {
        // Look up wall profile to get physical steps and coordinate shift
        // WallBuilder::get_wall_profile is declared public now
        const WallProfile& w_prof = WallBuilder::get_wall_profile(door.wall_type);
        MapPoint shift = get_door_wall_shift(map_size_x_, w_prof);

        // Grid coordinate to isometric physical coordinate (Frame Center)
        int effective_size = door.size;
        if (door.wall_type == WALL_TYPE_LAB) {
            effective_size = 1;
        }

        MapPoint pt = to_iso(door.pos, w_prof.step_x, w_prof.step_y, shift);
        apply_door_direction_offset(pt, door.direction_type, effective_size, w_prof);

        if (door.wall_type == WALL_TYPE_LAB) {
            if (is_lab_dead_door(door)) {
                uint32_t dead_door_dir = (door.direction_type == 0) ? 64 : 0;
                door_sprites.push_back(io::Sprite(LAB_DEAD_DOOR_VID, pt.x, pt.y, door.z_offset, dead_door_dir));
                continue;
            }

            uint32_t frame_dir = (door.direction_type == 0) ? 64 : 0;
            door_sprites.push_back(io::Sprite(LAB_LASER_FRAME_VID, pt.x, pt.y, 0.0f, frame_dir));

            if (door.door_state == DOOR_STATE_CLOSED) {
                uint32_t pillar_dir = (door.direction_type == 0) ? 128 : 0;
                door_sprites.push_back(io::Sprite(
                    LAB_LASER_PILLAR_VID,
                    pt.x + LAB_LASER_PILLAR_OFFSET_X,
                    pt.y + LAB_LASER_PILLAR_OFFSET_Y,
                    0.0f,
                    pillar_dir
                ));
            }
            continue;
        }

        const DoorProfile& d_prof = get_door_profile(door.wall_type);

        // 1. Frame Sprite
        int frame_vid = (door.size == 1) ? d_prof.id_frame_small : d_prof.id_frame_large;
        uint32_t frame_dir = (door.direction_type == 0) ? 0 : 128;
        if (frame_vid > 0) {
            door_sprites.push_back(io::Sprite(frame_vid, pt.x, pt.y, 0.0f, frame_dir));
        }

        // 2. Panel Sprite
        int panel_vid = 0;
        bool is_active_door = door.light_state != LIGHT_STATE_BROKEN;
        if (door.size == 1 && is_active_door) {
            panel_vid = d_prof.id_panel_small_open;
        } else if (door.size == 1) {
            panel_vid = d_prof.id_panel_small_closed;
        } else if (is_active_door) {
            panel_vid = d_prof.id_panel_large_open;
        } else {
            panel_vid = d_prof.id_panel_large_closed;
        }

        uint32_t panel_dir = (door.direction_type == 0) ? 0 : 64; // Prevent penetration bug on B-dir (64)
        if (panel_vid > 0) {
            door_sprites.push_back(io::Sprite(panel_vid, pt.x, pt.y, door.z_offset, panel_dir));
        }

        // 3. Indicator Light Sprite
        int light_vid = 0;
        if (door.light_state == LIGHT_STATE_GREEN) {
            light_vid = d_prof.id_light_green;
        } else if (door.light_state == LIGHT_STATE_RED) {
            light_vid = d_prof.id_light_red;
        } else if (door.light_state == LIGHT_STATE_BROKEN) {
            light_vid = d_prof.id_light_broken;
        }

        if (light_vid > 0) {
            door_sprites.push_back(io::Sprite(light_vid, pt.x, pt.y, 10.0f, frame_dir));
        }
    }

    return door_sprites;
}

} // namespace auto_mapper::core
