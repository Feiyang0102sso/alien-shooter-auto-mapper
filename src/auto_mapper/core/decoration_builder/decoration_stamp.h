/**
 * @file decoration_stamp.h
 * @brief Decoration Stamp data read from a library map region.
 */

#pragma once

#include "auto_mapper/core/vid_properties/vid_property_types.h"

#include <cstdint>
#include <string>
#include <vector>

namespace auto_mapper::core::decoration_builder {

/**
 * One sprite of a stamp. Offsets are Map Coordinates relative to the stamp frame center.
 * Every other field is copied verbatim from the library map sprite record; AS1 records
 * carry no gamma or scale, so those keep their defaults.
 */
struct DecorationMember {
    int32_t vid = 0;
    float offset_x = 0.0f;
    float offset_y = 0.0f;
    float pos_z = 0.0f;
    uint32_t direction = 0;
    VidArmy army = 0;
    VidGamma gamma{};
    VidScale scale{};
};

/**
 * One stamp of a library map: an axis-aligned Map Coordinate frame and its members.
 * AS2 Series frames are the authored regions; AS1 frames are the member bounding box plus a margin.
 */
struct DecorationStamp {
    // "<map file stem>:<number>", e.g. "01dorm:3". AS2 Series: region number from 1 in SPR order;
    // AS1: grid cell number (row * 6 + column + 1).
    std::string id;
    // Library map file stem, shared by every stamp of that map.
    std::string series_id;
    float width = 0.0f;
    float height = 0.0f;
    std::vector<DecorationMember> members;
};

} // namespace auto_mapper::core::decoration_builder
