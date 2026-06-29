/**
 * @file door_builder.cpp
 * @brief Implements door generation pipeline.
 */

#include "auto_mapper/core/door_builder.h"
#include "auto_mapper/core/wall_builder.h"
#include <cmath>
#include <random>

namespace auto_mapper::core {

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

const StandardDoorSizeVariant& get_standard_door_variant(int size) {
    if (size == DOOR_STANDARD.small.span_steps) {
        return DOOR_STANDARD.small;
    }

    return DOOR_STANDARD.large;
}

float get_random_standard_jam_z_offset(int size) {
    const StandardDoorSizeVariant& variant = get_standard_door_variant(size);
    static thread_local std::mt19937 random_engine(std::random_device{}());
    std::uniform_real_distribution<float> distribution(
        variant.jam_z_range.min_z,
        variant.jam_z_range.max_z
    );

    return distribution(random_engine);
}

static int get_standard_panel_id(const StandardDoorSizeVariant& variant, const DoorInstance& door) {
    bool is_active_door = door.light_state != LIGHT_STATE_BROKEN;
    if (is_active_door) {
        return variant.panel.vid_active_panel;
    }

    return variant.panel.vid_dead_panel;
}

static float get_fixed_z_offset(const DoorZRange& range) {
    return range.min_z;
}

static float get_standard_panel_z_offset(const StandardDoorSizeVariant& variant, const DoorInstance& door) {
    bool is_dead_open = door.light_state == LIGHT_STATE_BROKEN && door.door_state == DOOR_STATE_OPEN;
    if (is_dead_open) {
        return get_fixed_z_offset(variant.dead_open_z_offset);
    }

    return door.z_offset;
}

static int get_standard_light_id(const DoorInstance& door) {
    if (door.light_state == LIGHT_STATE_GREEN) {
        return DOOR_STANDARD.vid_light_green;
    }

    if (door.light_state == LIGHT_STATE_RED) {
        return DOOR_STANDARD.vid_light_red;
    }

    if (door.light_state == LIGHT_STATE_BROKEN) {
        return DOOR_STANDARD.vid_light_broken;
    }

    return 0;
}

DoorBuilder::DoorBuilder(float map_size_x, float map_size_y)
    : map_size_x_(map_size_x), map_size_y_(map_size_y) {}

std::vector<io::Sprite> DoorBuilder::build(const std::vector<DoorInstance>& doors) const {
    std::vector<io::Sprite> door_sprites;
    door_sprites.reserve(doors.size() * 3); // frame + panel + light

    for (const auto& door : doors) {
        // Use the wall profile for placement alignment.
        const WallProfile& w_prof = WallBuilder::get_wall_profile(door.wall_type);
        MapPoint shift = get_door_wall_shift(map_size_x_, w_prof);

        int effective_size = door.size;
        if (door.wall_type == WALL_TYPE_LAB) {
            effective_size = DOOR_LAB_LASER.span_steps;
        }

        MapPoint pt = to_iso(door.pos, w_prof.step_x, w_prof.step_y, shift);
        apply_door_direction_offset(pt, door.direction_type, effective_size, w_prof);

        if (door.wall_type == WALL_TYPE_LAB) {
            if (is_lab_dead_door(door)) {
                uint32_t dead_door_dir = (door.direction_type == 0) ? 64 : 0;
                door_sprites.push_back(io::Sprite(
                    DOOR_LAB_DECORATION.vid_frame,
                    pt.x,
                    pt.y,
                    door.z_offset,
                    dead_door_dir
                ));
                continue;
            }

            uint32_t frame_dir = (door.direction_type == 0) ? 64 : 0;
            door_sprites.push_back(io::Sprite(DOOR_LAB_LASER.vid_frame, pt.x, pt.y, 0.0f, frame_dir));

            if (door.door_state == DOOR_STATE_CLOSED) {
                uint32_t pillar_dir = (door.direction_type == 0) ? 128 : 0;
                door_sprites.push_back(io::Sprite(
                    DOOR_LAB_LASER.vid_laser_closed,
                    pt.x + DOOR_LAB_LASER.laser_offset_x,
                    pt.y + DOOR_LAB_LASER.laser_offset_y,
                    0.0f,
                    pillar_dir
                ));
            }
            continue;
        }

        const StandardDoorSizeVariant& variant = get_standard_door_variant(door.size);

        int frame_vid = variant.vid_frame;
        uint32_t frame_dir = (door.direction_type == 0) ? 0 : 128;
        if (frame_vid > 0) {
            door_sprites.push_back(io::Sprite(frame_vid, pt.x, pt.y, 0.0f, frame_dir));
        }

        int panel_vid = get_standard_panel_id(variant, door);
        uint32_t panel_dir = (door.direction_type == 0) ? 0 : 64;
        float panel_z_offset = get_standard_panel_z_offset(variant, door);
        if (panel_vid > 0) {
            door_sprites.push_back(io::Sprite(panel_vid, pt.x, pt.y, panel_z_offset, panel_dir));
        }

        int light_vid = get_standard_light_id(door);
        if (light_vid > 0) {
            door_sprites.push_back(io::Sprite(light_vid, pt.x, pt.y, 10.0f, frame_dir));
        }
    }

    return door_sprites;
}

} // namespace auto_mapper::core
