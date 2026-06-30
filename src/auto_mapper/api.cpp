#include "auto_mapper/api.h"
#include "auto_mapper/core/wall_builder.h"
#include "auto_mapper/core/door_builder.h"
#include "auto_mapper/io/map_writer.h"
#include <vector>
#include <string>

extern "C" {

static constexpr int SUPPORTED_WALL_TYPES[] = {
    auto_mapper::core::WALL_TYPE_STANDARD,
    auto_mapper::core::WALL_TYPE_LAB
};

AUTO_MAPPER_API bool get_standard_door_z_config(
    int size,
    CStandardDoorZConfig* config
) {
    if (config == nullptr) {
        return false;
    }

    const auto_mapper::core::StandardDoorSizeVariant& variant = auto_mapper::core::get_standard_door_variant(size);

    config->jam_min_z = variant.jam_z_range.min_z;
    config->jam_max_z = variant.jam_z_range.max_z;
    config->dead_open_min_z = variant.dead_open_z_offset.min_z;
    config->dead_open_max_z = variant.dead_open_z_offset.max_z;
    return true;
}

AUTO_MAPPER_API bool get_standard_door_jam_z_offset(
    int size,
    float* z_offset
) {
    if (z_offset == nullptr) {
        return false;
    }

    *z_offset = auto_mapper::core::get_random_standard_jam_z_offset(size);
    return true;
}

AUTO_MAPPER_API int get_wall_profile_count() {
    return static_cast<int>(sizeof(SUPPORTED_WALL_TYPES) / sizeof(SUPPORTED_WALL_TYPES[0]));
}

AUTO_MAPPER_API bool get_wall_profile_type_at(
    int index,
    int* wall_type
) {
    if (wall_type == nullptr) {
        return false;
    }

    int count = get_wall_profile_count();
    if (index < 0 || index >= count) {
        return false;
    }

    *wall_type = SUPPORTED_WALL_TYPES[index];
    return true;
}

AUTO_MAPPER_API bool get_wall_profile(
    int wall_type,
    CWallProfile* profile
) {
    if (profile == nullptr) {
        return false;
    }

    bool known_wall_type = false;
    int count = get_wall_profile_count();
    for (int i = 0; i < count; ++i) {
        if (SUPPORTED_WALL_TYPES[i] == wall_type) {
            known_wall_type = true;
        }
    }

    if (!known_wall_type) {
        return false;
    }

    const auto_mapper::core::WallProfile& cpp_profile = auto_mapper::core::WallBuilder::get_wall_profile(wall_type);

    profile->wall_type = wall_type;
    profile->dir_a_vid = cpp_profile.dir_a_vid;
    profile->dir_b_vid = cpp_profile.dir_b_vid;
    profile->pillar_vid = cpp_profile.pillar_vid;
    profile->step_x = cpp_profile.step_x;
    profile->step_y = cpp_profile.step_y;
    profile->offset_a_x = cpp_profile.offset_a_x;
    profile->offset_a_y = cpp_profile.offset_a_y;
    profile->offset_b_x = cpp_profile.offset_b_x;
    profile->offset_b_y = cpp_profile.offset_b_y;
    profile->offset_p_x = cpp_profile.offset_p_x;
    profile->offset_p_y = cpp_profile.offset_p_y;
    profile->grid_divisor = cpp_profile.grid_divisor;
    return true;
}

AUTO_MAPPER_API bool generate_map_from_segments(
    const char* output_path,
    const CSegment* segments,
    int num_segments,
    const CDoor* doors,
    int num_doors,
    float map_size_x,
    float map_size_y,
    bool gen_floor,
    bool gen_ceiling
) {
    std::string out_path(output_path);
    std::vector<auto_mapper::core::Segment> cpp_segments;
    cpp_segments.reserve(num_segments);

    for (int i = 0; i < num_segments; ++i) {
        cpp_segments.push_back({
            {segments[i].x1, segments[i].y1},
            {segments[i].x2, segments[i].y2},
            segments[i].wall_type
        });
    }

    std::vector<auto_mapper::core::DoorInstance> cpp_doors;
    cpp_doors.reserve(num_doors);
    std::vector<auto_mapper::core::DoorExcavation> excavations;
    excavations.reserve(num_doors);

    for (int i = 0; i < num_doors; ++i) {
        auto_mapper::core::DoorInstance di = {
            {doors[i].x, doors[i].y},
            doors[i].wall_type,
            doors[i].direction_type,
            doors[i].size,
            doors[i].door_state,
            doors[i].light_state,
            doors[i].z_offset
        };
        cpp_doors.push_back(di);

        int excavation_size = di.size;
        if (di.wall_type == auto_mapper::core::WALL_TYPE_LAB) {
            excavation_size = 1;
        }

        // Generate excavation area for this door
        excavations.push_back({
            di.pos,
            di.direction_type,
            excavation_size,
            di.wall_type
        });
    }

    // API logging is intentionally disabled for UI-driven DLL calls.

    // 1. Build walls with excavations
    auto_mapper::core::WallBuilder wall_builder(map_size_x, map_size_y);
    std::vector<auto_mapper::io::Sprite> sprites = wall_builder.build(cpp_segments, gen_floor, gen_ceiling, excavations);

    // 2. Build doors
    auto_mapper::core::DoorBuilder door_builder(map_size_x, map_size_y);
    std::vector<auto_mapper::io::Sprite> door_sprites = door_builder.build(cpp_doors);

    // 3. Merge sprites
    sprites.insert(sprites.end(), door_sprites.begin(), door_sprites.end());

    if (auto_mapper::io::write_map(sprites, out_path, map_size_x, map_size_y)) {
        return true;
    } else {
        return false;
    }
}

} // extern "C"
