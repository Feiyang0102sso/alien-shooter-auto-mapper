/**
 * @file door_profile_types.h
 * @brief Shared door profile data structures.
 */

#pragma once

#include "auto_mapper/core/wall_builder/wall_profile_types.h"
#include <cstdint>

namespace auto_mapper::core {

struct DoorZRange {
    float min_z; // Lower z offset.
    float max_z; // Upper z offset. Same as min_z means fixed offset.
};

struct SpriteDirectionMapping {
    uint32_t dir_a; // Direction A / (direction_type == 0)
    uint32_t dir_b; // Direction B \ (direction_type == 1)
};

struct StandardDoorPanelAssets {
    int vid_active_panel; // Active panel VID.
    int vid_dead_panel;   // Dead closed/jammed/open panel VID.
};

struct StandardDoorSizeVariant {
    int span_steps;                 // Wall step multiplier.
    int vid_frame;                  // Frame VID.
    StandardDoorPanelAssets panel;  // Panel VIDs.
    DoorZRange jam_z_range;         // Jammed door z range.
    DoorZRange dead_open_z_offset;  // Dead open fixed z offset.
};

struct StandardDoorProfile {
    StandardDoorSizeVariant small; // 1-step door.
    StandardDoorSizeVariant large; // 2-step door.
    int vid_light_green;           // Active-open signal.
    int vid_light_red;             // Active-closed signal.
    int vid_light_broken;          // Dead signal.
    SpriteDirectionMapping frame_dir_map; // Direction mapping for frame and light.
    SpriteDirectionMapping panel_dir_map; // Direction mapping for door panel.
};

struct LabLaserDoorProfile {
    int span_steps;       // Wall step multiplier.
    int vid_frame;        // Laser door frame VID.
    int vid_laser_closed; // Closed laser pillar VID.
    float laser_offset_x; // Laser pillar x offset.
    float laser_offset_y; // Laser pillar y offset.
    SpriteDirectionMapping frame_dir_map; // Direction mapping for laser door frame.
    SpriteDirectionMapping laser_dir_map; // Direction mapping for closed laser pillar.
};

struct LabDecorationDoorProfile {
    int span_steps; // Wall step multiplier.
    int vid_frame;  // Decoration frame VID.
    SpriteDirectionMapping frame_dir_map; // Direction mapping for decoration frame.
};

struct As2DoorPanelAssets {
    int vid_closed;
    int vid_open;
};

struct As2DoorSizeVariant {
    int span_steps;
    int vid_frame;
    As2DoorPanelAssets panel;
    SpriteDirectionMapping frame_dir_map;
    SpriteDirectionMapping panel_dir_map;
    WallPartAsset compensation_pillar_dir_a;
    WallPartAsset compensation_pillar_dir_b;
};

struct As2DoorProfile {
    As2DoorSizeVariant small;
    As2DoorSizeVariant large;
};

} // namespace auto_mapper::core
