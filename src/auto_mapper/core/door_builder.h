/**
 * @file door_builder.h
 * @brief Builder class for generating door frame, panel, and indicator light sprites.
 */

#pragma once

#include "auto_mapper/core/geometry.h"
#include "auto_mapper/io/map_writer.h"
#include <vector>

namespace auto_mapper::core {

enum LightState {
    LIGHT_STATE_GREEN = 0,
    LIGHT_STATE_RED = 1,
    LIGHT_STATE_BROKEN = 2
};

enum DoorState {
    DOOR_STATE_CLOSED = 0,
    DOOR_STATE_OPEN = 1
};

struct DoorZRange {
    float min_z; // Lower z offset.
    float max_z; // Upper z offset. Same as min_z means fixed offset.
};

struct DoorInstance {
    GridPoint pos;
    int wall_type;      // 0=standard, 1=lab
    int direction_type; // 0=A / direction, 1=B \ direction
    int size;           // 1=small, 2=large
    int door_state;     // 0=closed, 1=open
    int light_state;    // 0=green/open, 1=red/closed, 2=broken/dead
    float z_offset;     // Final panel z offset.
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
};

struct LabLaserDoorProfile {
    int span_steps;       // Wall step multiplier.
    int vid_frame;        // Laser door frame VID.
    int vid_laser_closed; // Closed laser pillar VID.
    float laser_offset_x; // Laser pillar x offset.
    float laser_offset_y; // Laser pillar y offset.
};

struct LabDecorationDoorProfile {
    int span_steps; // Wall step multiplier.
    int vid_frame;  // Decoration frame VID.
};

inline constexpr StandardDoorProfile DOOR_STANDARD = {
    .small = {
        .span_steps = 1,
        .vid_frame = 606,
        .panel = {
            .vid_active_panel = 605,
            .vid_dead_panel = 617
        },
        .jam_z_range = {.min_z = -51.0f, .max_z = -24.0f},
        .dead_open_z_offset = {.min_z = -63.0f, .max_z = -63.0f}
    },
    .large = {
        .span_steps = 2,
        .vid_frame = 608,
        .panel = {
            .vid_active_panel = 607,
            .vid_dead_panel = 611
        },
        .jam_z_range = {.min_z = -50.0f, .max_z = -28.0f},
        .dead_open_z_offset = {.min_z = -68.0f, .max_z = -68.0f}
    },
    .vid_light_green = 423,
    .vid_light_red = 424,
    .vid_light_broken = 425
};

inline constexpr LabLaserDoorProfile DOOR_LAB_LASER = {
    .span_steps = 1,
    .vid_frame = 653,
    .vid_laser_closed = 164,
    .laser_offset_x = 0.0f,
    .laser_offset_y = -18.0f
};

inline constexpr LabDecorationDoorProfile DOOR_LAB_DECORATION = {
    .span_steps = 1,
    .vid_frame = 654
};

const StandardDoorSizeVariant& get_standard_door_variant(int size);
float get_random_standard_jam_z_offset(int size);

class DoorBuilder {
public:
    DoorBuilder(float map_size_x = 600.0f, float map_size_y = 600.0f);

    /**
     * @brief Generate door sprites (frame, panel, indicator light) for all given door instances.
     */
    std::vector<io::Sprite> build(const std::vector<DoorInstance>& doors) const;

private:
    float map_size_x_;
    float map_size_y_;
};

} // namespace auto_mapper::core
