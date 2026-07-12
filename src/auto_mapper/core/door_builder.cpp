/**
 * @file door_builder.cpp
 * @brief Implements door generation pipeline.
 */

#include "auto_mapper/core/door_builder.h"
#include "auto_mapper/core/randomizer.h"
#include "auto_mapper/core/wall_builder.h"

namespace auto_mapper::core {


static uint32_t get_sprite_dir(const SpriteDirectionMapping& mapping, int direction_type) {
    return (direction_type == 0) ? mapping.dir_a : mapping.dir_b;
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

static bool is_standard_family_wall(int wall_type) {
    if (wall_type == WALL_TYPE_STANDARD) {
        return true;
    }

    if (wall_type == WALL_TYPE_STANDARD_DARK) {
        return true;
    }

    return false;
}

static const StandardDoorProfile& get_standard_door_profile(int wall_type) {
    if (wall_type == WALL_TYPE_STANDARD_DARK) {
        return DOOR_STANDARD_DARK;
    }

    return DOOR_STANDARD;
}

const StandardDoorSizeVariant& get_standard_door_variant(int size) {
    if (size == DOOR_STANDARD.small.span_steps) {
        return DOOR_STANDARD.small;
    }

    return DOOR_STANDARD.large;
}

float get_random_standard_jam_z_offset(int size) {
    const StandardDoorSizeVariant& variant = get_standard_door_variant(size);
    int min_z = static_cast<int>(variant.jam_z_range.min_z);
    int max_z = static_cast<int>(variant.jam_z_range.max_z);
    int z_offset = Random::get(min_z, max_z);

    return static_cast<float>(z_offset);
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

static int get_standard_light_id(const StandardDoorProfile& profile, const DoorInstance& door) {
    if (door.light_state == LIGHT_STATE_GREEN) {
        return profile.vid_light_green;
    }

    if (door.light_state == LIGHT_STATE_RED) {
        return profile.vid_light_red;
    }

    if (door.light_state == LIGHT_STATE_BROKEN) {
        return profile.vid_light_broken;
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
        MapPoint shift = WallBuilder::get_wall_shift(map_size_x_, w_prof);

        int effective_size = door.size;
        if (door.wall_type == WALL_TYPE_LAB) {
            effective_size = DOOR_LAB_LASER.span_steps;
        }

        MapPoint pt = to_iso(door.pos, w_prof.step_x, w_prof.step_y, shift);
        apply_door_direction_offset(pt, door.direction_type, effective_size, w_prof);

        if (door.wall_type == WALL_TYPE_LAB) {
            if (is_lab_dead_door(door)) {
                uint32_t dead_door_dir = get_sprite_dir(DOOR_LAB_DECORATION.frame_dir_map, door.direction_type);
                door_sprites.push_back(io::Sprite(
                    DOOR_LAB_DECORATION.vid_frame,
                    pt.x,
                    pt.y,
                    door.z_offset,
                    dead_door_dir
                ));
                continue;
            }

            uint32_t frame_dir = get_sprite_dir(DOOR_LAB_LASER.frame_dir_map, door.direction_type);
            door_sprites.push_back(io::Sprite(DOOR_LAB_LASER.vid_frame, pt.x, pt.y, 0.0f, frame_dir));

            if (door.door_state == DOOR_STATE_CLOSED) {
                uint32_t pillar_dir = get_sprite_dir(DOOR_LAB_LASER.laser_dir_map, door.direction_type);
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

        if (!is_standard_family_wall(door.wall_type)) {
            continue;
        }

        const StandardDoorProfile& door_profile = get_standard_door_profile(door.wall_type);
        const StandardDoorSizeVariant& variant =
            (door.size == door_profile.small.span_steps) ? door_profile.small : door_profile.large;

        int frame_vid = variant.vid_frame;
        uint32_t frame_dir = get_sprite_dir(door_profile.frame_dir_map, door.direction_type);
        if (frame_vid > 0) {
            door_sprites.push_back(io::Sprite(frame_vid, pt.x, pt.y, 0.0f, frame_dir));
        }

        int panel_vid = get_standard_panel_id(variant, door);
        uint32_t panel_dir = get_sprite_dir(door_profile.panel_dir_map, door.direction_type);
        float panel_z_offset = get_standard_panel_z_offset(variant, door);
        if (panel_vid > 0) {
            door_sprites.push_back(io::Sprite(panel_vid, pt.x, pt.y, panel_z_offset, panel_dir));
        }

        int light_vid = get_standard_light_id(door_profile, door);
        if (light_vid > 0) {
            door_sprites.push_back(io::Sprite(light_vid, pt.x, pt.y, 10.0f, frame_dir));
        }
    }

    return door_sprites;
}

} // namespace auto_mapper::core
