/**
 * @file map_generator.h
 * @brief Whole-map pipeline: walls -> curve walls -> doors -> decorations -> loot boxes -> gore -> loot texts -> .map file.
 *
 * This is the single entry point the editor calls to generate a map.
 */

#pragma once

#include "auto_mapper/core/decoration_builder/decoration_library.h"
#include "auto_mapper/core/door_builder/door_builder.h"
#include "auto_mapper/core/indoor_decorations/desk_builder.h"
#include "auto_mapper/core/indoor_decorations/incubator_builder.h"
#include "auto_mapper/core/scatter/gore/gore_builder.h"
#include "auto_mapper/core/scatter/loot/loot_box_builder.h"
#include "auto_mapper/core/scatter/loot/loot_table.h"
#include "auto_mapper/core/wall_builder/curve_wall_builder.h"
#include "auto_mapper/core/wall_builder/wall_builder.h"
#include "auto_mapper/io/map_writer.h"
#include <string>
#include <vector>

namespace auto_mapper::core {

/**
 * One decoration stamp placed by the user: stamp id plus its region center in Map Coordinates.
 * Placement is translation only; the stamp keeps every member's authored layout.
 */
struct StampPlacement {
    std::string stamp_id;
    float center_x = 0.0f;
    float center_y = 0.0f;
};

/**
 * Everything needed to generate one map. Walls and doors use Grid Coordinates;
 * Curve Walls and decorations use Map Coordinates.
 */
struct MapRequest {
    io::MapFormat format = io::MapFormat::AS1;
    float map_size_x = 600.0f;
    float map_size_y = 600.0f;
    std::vector<Segment> segments;
    std::vector<DoorInstance> doors;
    // AS2 Series only: AS1 has no 360-direction rock / alien nest pieces.
    std::vector<CurveWall> curve_walls;
    std::vector<indoor_decorations::IncubatorArray> incubator_arrays;
    std::vector<indoor_decorations::DeskArray> desk_arrays;
    std::vector<StampPlacement> stamps;
    // Library that resolves `stamps` ids; may be null when there are no stamps.
    const decoration_builder::DecorationLibrary* decoration_library = nullptr;
    // AS2 Series only: brushed loot containers, and the Loot Tables that fill the nameText of every
    // sprite with a listed VID (stamp members included).
    std::vector<LootBox> loot_boxes;
    std::vector<LootTable> loot_tables;
    // AS2 Series only: brushed corpses and blood. Conscription-only corpses need an AS2C / AS2OE format.
    std::vector<GoreItem> gore_items;
    AS1CeilingLayerSettings as1_ceiling_layers = DEFAULT_AS1_CEILING_LAYER_SETTINGS;
    bool generate_floor = false;
    bool generate_ceiling = false;
    bool random_direction = false;
};

/**
 * Build every sprite for the request and write the map file.
 * Returns false for invalid AS1 ceiling layer counts, a Loot Table above 100%, a corpse the format's game lacks,
 * or when the file cannot be written.
 */
bool generate_map(const MapRequest& request, const std::string& output_path);

} // namespace auto_mapper::core
