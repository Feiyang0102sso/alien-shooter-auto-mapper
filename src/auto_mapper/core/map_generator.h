/**
 * @file map_generator.h
 * @brief Whole-map pipeline: walls -> doors -> decorations -> .map file.
 *
 * This is the single entry point the editor calls to generate a map.
 */

#pragma once

#include "auto_mapper/core/door_builder/door_builder.h"
#include "auto_mapper/core/indoor_decorations/desk_builder.h"
#include "auto_mapper/core/indoor_decorations/incubator_builder.h"
#include "auto_mapper/core/wall_builder/wall_builder.h"
#include "auto_mapper/io/map_writer.h"
#include <string>
#include <vector>

namespace auto_mapper::core {

/**
 * One decoration stamp placed by the user: profile id plus its Map Coordinate center.
 * Placement is translation only; the profile keeps every member's authored layout.
 */
struct StampPlacement {
    std::string profile_id;
    float center_x = 0.0f;
    float center_y = 0.0f;
};

/**
 * Everything needed to generate one map. Walls and doors use Grid Coordinates;
 * decorations use Map Coordinates.
 */
struct MapRequest {
    io::MapFormat format = io::MapFormat::AS1;
    float map_size_x = 600.0f;
    float map_size_y = 600.0f;
    std::vector<Segment> segments;
    std::vector<DoorInstance> doors;
    std::vector<indoor_decorations::IncubatorArray> incubator_arrays;
    std::vector<indoor_decorations::DeskArray> desk_arrays;
    std::vector<StampPlacement> stamps;
    AS1CeilingLayerSettings as1_ceiling_layers = DEFAULT_AS1_CEILING_LAYER_SETTINGS;
    bool generate_floor = false;
    bool generate_ceiling = false;
    bool random_direction = false;
};

/**
 * Build every sprite for the request and write the map file.
 * Returns false for invalid AS1 ceiling layer counts or when the file cannot be written.
 */
bool generate_map(const MapRequest& request, const std::string& output_path);

} // namespace auto_mapper::core
