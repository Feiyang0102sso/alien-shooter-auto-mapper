/**
 * @file door_builder.h
 * @brief Builder class for generating door frame, panel, and indicator light sprites.
 */

#pragma once

#include "auto_mapper/core/door_profile_types.h"
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

struct DoorInstance {
    GridPoint pos;
    int wall_type;      // 0=standard, 1=lab
    int direction_type; // 0=A / direction, 1=B \ direction
    int size;           // 1=small, 2=large
    int door_state;     // 0=closed, 1=open
    int light_state;    // 0=green/open, 1=red/closed, 2=broken/dead
    float z_offset;     // Final panel z offset.
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
