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

struct DoorInstance {
    GridPoint pos;
    int wall_type;      // 0=standard, 1=lab
    int direction_type; // 0=A方向 (纵向), 1=B方向 (横向)
    int size;           // 1=小门, 2=大门
    int door_state;     // 0=关闭, 1=打开
    int light_state;    // 0=绿, 1=红, 2=损坏
    float z_offset;     // 物理Z坐标微调 (主要用于死门开合/卡住)，默认为 0.0f
};

struct DoorProfile {
    int id_frame_small;
    int id_frame_large;
    int id_panel_small_open;
    int id_panel_small_closed;
    int id_panel_large_open;
    int id_panel_large_closed;
    int id_light_green;
    int id_light_red;
    int id_light_broken;
};

class DoorBuilder {
public:
    DoorBuilder(float map_size_x = 600.0f, float map_size_y = 600.0f);

    /**
     * @brief Generate door sprites (frame, panel, indicator light) for all given door instances.
     */
    std::vector<io::Sprite> build(const std::vector<DoorInstance>& doors) const;

private:
    static const DoorProfile& get_door_profile(int wall_type);
    
    float map_size_x_;
    float map_size_y_;
};

} // namespace auto_mapper::core
