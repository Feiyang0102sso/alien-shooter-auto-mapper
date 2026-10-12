/**
 * @file map_generator.cpp
 * @brief Whole-map pipeline (formerly the C API function generate_map_from_segments(), logic unchanged).
 */

#include "auto_mapper/core/map_generator.h"
#include "auto_mapper/common/logger.h"
#include "auto_mapper/core/decoration_builder/decoration_builder.h"
#include "auto_mapper/core/decoration_builder/decoration_registry.h"
#include <algorithm>

namespace auto_mapper::core {
namespace {

bool is_valid_as1_ceiling_layer_count(int layer_count) {
    return layer_count >= AS1_CEILING_LAYER_COUNT_MIN && layer_count <= AS1_CEILING_LAYER_COUNT_MAX;
}

bool is_as2_set1_wall_type(int wall_type) {
    return wall_type == 3 || wall_type == 4 || wall_type == 5;
}

/**
 * Auto-sync an AS2 Set1 door to the Set1 variant of the wall it sits on.
 */
int resolve_door_wall_type(const DoorInstance& door, const std::vector<Segment>& segments) {
    if (!is_as2_set1_wall_type(door.wall_type)) {
        return door.wall_type;
    }

    for (const Segment& segment : segments) {
        if (!is_as2_set1_wall_type(segment.wall_type)) {
            continue;
        }

        int min_x = std::min(segment.start.x, segment.end.x);
        int max_x = std::max(segment.start.x, segment.end.x);
        int min_y = std::min(segment.start.y, segment.end.y);
        int max_y = std::max(segment.start.y, segment.end.y);

        if (door.direction_type == 0 && segment.start.x == segment.end.x && door.pos.x == segment.start.x) {
            if (door.pos.y >= min_y - 1 && door.pos.y <= max_y + 1) {
                return segment.wall_type;
            }
        } else if (door.direction_type == 1 && segment.start.y == segment.end.y && door.pos.y == segment.start.y) {
            if (door.pos.x >= min_x - 1 && door.pos.x <= max_x + 1) {
                return segment.wall_type;
            }
        }
    }

    return door.wall_type;
}

} // namespace

bool generate_map(const MapRequest& request, const std::string& output_path) {
    if (!is_valid_as1_ceiling_layer_count(request.as1_ceiling_layers.standard_total_layer_count)) {
        Logger::error("Invalid AS1 standard ceiling layer count: {}", request.as1_ceiling_layers.standard_total_layer_count);
        return false;
    }

    if (!is_valid_as1_ceiling_layer_count(request.as1_ceiling_layers.lab_total_layer_count)) {
        Logger::error("Invalid AS1 lab ceiling layer count: {}", request.as1_ceiling_layers.lab_total_layer_count);
        return false;
    }

    std::vector<DoorInstance> doors;
    doors.reserve(request.doors.size());
    std::vector<DoorExcavation> excavations;
    excavations.reserve(request.doors.size());

    for (const DoorInstance& requested_door : request.doors) {
        DoorInstance door = requested_door;
        door.wall_type = resolve_door_wall_type(requested_door, request.segments);

        // AS2 Set2 ~ Set9 only have 2-step large doors
        if (door.wall_type >= 6 && door.wall_type <= 13) {
            door.size = 2;
        }
        doors.push_back(door);

        // Lab laser doors always open a single wall slot.
        int excavation_size = door.size;
        if (door.wall_type == WALL_TYPE_LAB) {
            excavation_size = 1;
        }

        // Generate excavation area for this door
        excavations.push_back({door.pos, door.direction_type, excavation_size, door.wall_type});
    }

    // 1. Build walls with excavations
    WallBuilder wall_builder(request.map_size_x, request.map_size_y, request.random_direction, request.as1_ceiling_layers);
    std::vector<io::Sprite> sprites =
        wall_builder.build(request.segments, request.generate_floor, request.generate_ceiling, excavations);

    // 2. Build doors
    DoorBuilder door_builder(request.map_size_x, request.map_size_y);
    std::vector<io::Sprite> door_sprites = door_builder.build(doors);

    // 3. Build indoor decorations
    indoor_decorations::IncubatorBuilder incubator_builder;
    indoor_decorations::DeskBuilder desk_builder;
    std::vector<io::Sprite> decoration_sprites;

    for (const indoor_decorations::IncubatorArray& array : request.incubator_arrays) {
        std::vector<io::Sprite> array_sprites = incubator_builder.build_array(array);
        decoration_sprites.insert(decoration_sprites.end(), array_sprites.begin(), array_sprites.end());
    }

    for (const indoor_decorations::DeskArray& array : request.desk_arrays) {
        std::vector<io::Sprite> array_sprites = desk_builder.build_array(array);
        decoration_sprites.insert(decoration_sprites.end(), array_sprites.begin(), array_sprites.end());
    }

    // 3b. Build authored decoration stamps
    decoration_builder::DecorationBuilder stamp_builder;

    for (const StampPlacement& stamp : request.stamps) {
        const decoration_builder::DecorationProfile* profile =
            decoration_builder::find_as2_decoration_profile(stamp.profile_id);

        if (profile == nullptr) {
            Logger::error("Unknown decoration stamp profile: {}", stamp.profile_id);
            continue;
        }

        decoration_builder::DecorationPlacement placement;
        placement.center_x = stamp.center_x;
        placement.center_y = stamp.center_y;

        std::vector<io::Sprite> stamp_sprites = stamp_builder.build(*profile, placement);
        decoration_sprites.insert(decoration_sprites.end(), stamp_sprites.begin(), stamp_sprites.end());
    }

    // 4. Merge sprites
    sprites.insert(sprites.end(), door_sprites.begin(), door_sprites.end());
    sprites.insert(sprites.end(), decoration_sprites.begin(), decoration_sprites.end());

    return io::write_map(sprites, output_path, request.format, request.map_size_x, request.map_size_y);
}

} // namespace auto_mapper::core
