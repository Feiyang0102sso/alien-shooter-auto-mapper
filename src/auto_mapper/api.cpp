#include "auto_mapper/api.h"
#include "auto_mapper/core/wall_builder/wall_builder.h"
#include "core/door_builder/door_builder.h"
#include "core/door_builder/door_profiles_as1.h"
#include "auto_mapper/core/randomizer.h"
#include "auto_mapper/core/indoor_decorations/desk_builder.h"
#include "auto_mapper/core/indoor_decorations/incubator_builder.h"
#include "auto_mapper/io/map_writer.h"
#include <vector>
#include <string>

extern "C" {

static constexpr int AUTO_MAPPER_API_VERSION = 5;
static constexpr int MIN_SPRITE_DIRECTION = 0;
static constexpr int MAX_SPRITE_DIRECTION = 255;

AUTO_MAPPER_API int get_auto_mapper_api_version() {
    return AUTO_MAPPER_API_VERSION;
}

static constexpr CDrawablePart STANDARD_DRAWABLE_PARTS[] = {
    {"wall_body"},
    {"active_door"},
    {"dead_door_closed"},
    {"dead_door_jammed"},
    {"dead_door_open"}
};

static constexpr CDrawablePart LAB_DRAWABLE_PARTS[] = {
    {"wall_body"},
    {"lab_laser_closed"},
    {"lab_laser_open"},
    {"lab_decoration_door"}
};

static constexpr CDrawablePart AS2_DRAWABLE_PARTS[] = {
    {"wall_body"},
    {"as2_door_closed"},
    {"as2_door_open"}
};

static constexpr int STANDARD_DOOR_SIZES[] = {
    auto_mapper::core::DOOR_STANDARD.small.span_steps,
    auto_mapper::core::DOOR_STANDARD.large.span_steps
};

// ---------------------------------------------------------------------------
// Floor direction randomization rules
//
// VIDs with 0-255 full range: use Random::get(min, max)
// VIDs with a discrete direction set: use Random::get(vector<int>)
//
// AS1 floors (500, 502, 503): always 0-255
// AS2 Set1 VID 1783:          0-255
// AS2 Set5 VID 2503:          0-255
// AS2 Set6 VID 2503:          0-255 (same VID as Set5)
// AS2 Set7 VID 2643:          0-255
// AS2 Set8 VID 2643:          0-255 (same VID as Set7)
// AS2 Set2 VID 1724:          {0, 32, 128, 160, 192, 224}
// AS2 Set9 VID 1724:          {0, 32, 128, 160, 192, 224} (same VID as Set2)
// AS2 Set3 VID 1121:          {0, 32, 128, 160, 192, 224}
// AS2 Set4 VID 1121:          {0, 32, 128, 160, 192, 224} (same VID as Set3)
// ---------------------------------------------------------------------------

// Shared 6-direction pool for VIDs 1724 and 1121 (Set2, Set9, Set3, Set4).
static const std::vector<int> FLOOR_DIR_POOL_SIX = {
    0, 32, 128, 160, 192, 224
};

static bool is_as1_floor_vid(int vid) {
    if (vid == auto_mapper::core::FLOOR_STANDARD.vid) {
        return true;
    }
    if (vid == auto_mapper::core::FLOOR_LAB.vid) {
        return true;
    }
    if (vid == auto_mapper::core::FLOOR_STANDARD_DARK.vid) {
        return true;
    }
    return false;
}

static bool is_full_range_floor_vid(int vid) {
    // 1783 Set1
    if (vid == auto_mapper::core::FLOOR_AS2_SET1.vid) {
        return true;
    }
    // 2503 Set5 / Set6
    if (vid == auto_mapper::core::FLOOR_AS2_SET5.vid) {
        return true;
    }
    // 2643 Set7 / Set8
    if (vid == auto_mapper::core::FLOOR_AS2_SET7.vid) {
        return true;
    }
    return false;
}

static bool is_six_pool_floor_vid(int vid) {
    // 1724 Set2 / Set9
    if (vid == auto_mapper::core::FLOOR_AS2_SET2.vid) {
        return true;
    }
    // 1121 Set3 / Set4
    if (vid == auto_mapper::core::FLOOR_AS2_SET3.vid) {
        return true;
    }
    return false;
}

static bool is_as2_wall_type(int wall_type) {
    if (wall_type >= auto_mapper::core::WALL_TYPE_AS2_WALL_SET1_FIXED_0 &&
        wall_type <= auto_mapper::core::WALL_TYPE_AS2_WALL_SET9_RANDOM) {
        return true;
    }

    return false;
}

static auto_mapper::io::MapFormat select_output_map_format(
    const std::vector<auto_mapper::core::Segment>& segments,
    const std::vector<auto_mapper::core::DoorInstance>& doors
) {
    for (const auto& segment : segments) {
        if (is_as2_wall_type(segment.wall_type)) {
            return auto_mapper::io::MapFormat::AS2R;
        }
    }

    for (const auto& door : doors) {
        if (is_as2_wall_type(door.wall_type)) {
            return auto_mapper::io::MapFormat::AS2R;
        }
    }

    return auto_mapper::io::MapFormat::AS1;
}

static void randomize_floor_directions(
    std::vector<auto_mapper::io::Sprite>& sprites
) {
    using auto_mapper::core::Random;

    for (auto_mapper::io::Sprite& sprite : sprites) {
        int vid = sprite.vid;

        // AS1 + full-range AS2 floors: 0-255
        if (is_as1_floor_vid(vid) || is_full_range_floor_vid(vid)) {
            int direction = Random::get(
                MIN_SPRITE_DIRECTION,
                MAX_SPRITE_DIRECTION
            );
            sprite.direction = static_cast<uint32_t>(direction);
            continue;
        }

        // Six-option pool floors: pick one from the list via Random::get(vector)
        if (is_six_pool_floor_vid(vid)) {
            int direction = Random::get(FLOOR_DIR_POOL_SIX);
            sprite.direction = static_cast<uint32_t>(direction);
            continue;
        }
    }
}

AUTO_MAPPER_API bool get_incubator_array_profile(
    CIncubatorArrayProfile* profile
) {
    if (profile == nullptr) {
        return false;
    }

    profile->row_axis_x = auto_mapper::core::indoor_decorations::INCUBATOR_DEFAULT_ITEM_STEP_X;
    profile->row_axis_y = auto_mapper::core::indoor_decorations::INCUBATOR_DEFAULT_ITEM_STEP_Y;
    profile->column_axis_x = auto_mapper::core::indoor_decorations::INCUBATOR_DEFAULT_ROW_STEP_X;
    profile->column_axis_y = auto_mapper::core::indoor_decorations::INCUBATOR_DEFAULT_ROW_STEP_Y;
    profile->footprint_width = auto_mapper::core::indoor_decorations::INCUBATOR_FOOTPRINT_ITEM_LENGTH;
    profile->footprint_height = auto_mapper::core::indoor_decorations::INCUBATOR_FOOTPRINT_ROW_LENGTH;
    return true;
}

AUTO_MAPPER_API bool get_desk_array_profile(
    CDeskArrayProfile* profile
) {
    if (profile == nullptr) {
        return false;
    }

    profile->row_axis_x = auto_mapper::core::indoor_decorations::DESK_DEFAULT_ROW_SPACING_X;
    profile->row_axis_y = auto_mapper::core::indoor_decorations::DESK_DEFAULT_ROW_SPACING_Y;
    profile->column_axis_x = auto_mapper::core::indoor_decorations::DESK_DEFAULT_COLUMN_SPACING_X;
    profile->column_axis_y = auto_mapper::core::indoor_decorations::DESK_DEFAULT_COLUMN_SPACING_Y;
    profile->footprint_width = auto_mapper::core::indoor_decorations::DESK_FOOTPRINT_ROW_LENGTH;
    profile->footprint_height = auto_mapper::core::indoor_decorations::DESK_FOOTPRINT_COLUMN_LENGTH;
    return true;
}

static auto_mapper::core::indoor_decorations::IncubatorArray convert_incubator_array(
    const CIncubatorArray& array
) {
    return {
        .start_x = array.start_x,
        .start_y = array.start_y,
        .row_length = array.row_length,
        .column_length = array.column_length,
        .item_spacing_scale = array.item_spacing_scale,
        .row_spacing_scale = array.row_spacing_scale
    };
}

static std::vector<CIncubatorPreviewPoint> build_incubator_preview_points(
    const CIncubatorArray& array
) {
    auto_mapper::core::indoor_decorations::IncubatorBuilder builder;
    auto_mapper::core::indoor_decorations::IncubatorArray cpp_array = convert_incubator_array(array);
    std::vector<auto_mapper::io::Sprite> sprites = builder.build_array(cpp_array);
    std::vector<CIncubatorPreviewPoint> points;

    for (const auto_mapper::io::Sprite& sprite : sprites) {
        if (sprite.vid != auto_mapper::core::indoor_decorations::INCUBATOR_BODY_VID) {
            continue;
        }

        points.push_back({
            .x = sprite.posX,
            .y = sprite.posY
        });
    }

    return points;
}

static auto_mapper::core::indoor_decorations::DeskArray convert_desk_array(
    const CDeskArray& array
) {
    return {
        .start_x = array.start_x,
        .start_y = array.start_y,
        .row_length = array.row_length,
        .column_length = array.column_length,
        .item_spacing_scale = array.item_spacing_scale,
        .row_spacing_scale = array.row_spacing_scale,
        .pos_z = 0.0f
    };
}

AUTO_MAPPER_API int get_incubator_array_preview_point_count(
    const CIncubatorArray* array
) {
    if (array == nullptr) {
        return 0;
    }

    std::vector<CIncubatorPreviewPoint> points = build_incubator_preview_points(*array);
    return static_cast<int>(points.size());
}

AUTO_MAPPER_API int get_incubator_array_preview_points(
    const CIncubatorArray* array,
    CIncubatorPreviewPoint* points,
    int max_points
) {
    if (array == nullptr) {
        return 0;
    }

    if (points == nullptr) {
        return 0;
    }

    if (max_points <= 0) {
        return 0;
    }

    std::vector<CIncubatorPreviewPoint> preview_points = build_incubator_preview_points(*array);
    int copied_count = 0;

    while (copied_count < max_points && copied_count < static_cast<int>(preview_points.size())) {
        points[copied_count] = preview_points[copied_count];
        copied_count += 1;
    }

    return copied_count;
}

static bool is_supported_wall_type(int wall_type) {
    for (int i = 0; i < auto_mapper::core::SUPPORTED_WALL_TYPE_COUNT; ++i) {
        if (auto_mapper::core::SUPPORTED_WALL_TYPES[i] == wall_type) {
            return true;
        }
    }

    return false;
}

static const CDrawablePart* get_drawable_parts(int wall_type, int& count) {
    if (wall_type == auto_mapper::core::WALL_TYPE_STANDARD ||
        wall_type == auto_mapper::core::WALL_TYPE_STANDARD_DARK) {
        count = static_cast<int>(sizeof(STANDARD_DRAWABLE_PARTS) / sizeof(STANDARD_DRAWABLE_PARTS[0]));
        return STANDARD_DRAWABLE_PARTS;
    }

    if (wall_type == auto_mapper::core::WALL_TYPE_LAB) {
        count = static_cast<int>(sizeof(LAB_DRAWABLE_PARTS) / sizeof(LAB_DRAWABLE_PARTS[0]));
        return LAB_DRAWABLE_PARTS;
    }

    // All AS2 wall types share the same drawable parts: wall body + door closed/open.
    if (is_supported_wall_type(wall_type) &&
        wall_type != auto_mapper::core::WALL_TYPE_STANDARD &&
        wall_type != auto_mapper::core::WALL_TYPE_STANDARD_DARK &&
        wall_type != auto_mapper::core::WALL_TYPE_LAB) {
        count = static_cast<int>(sizeof(AS2_DRAWABLE_PARTS) / sizeof(AS2_DRAWABLE_PARTS[0]));
        return AS2_DRAWABLE_PARTS;
    }

    count = 0;
    return nullptr;
}

AUTO_MAPPER_API bool get_standard_door_z_config(
    int size,
    CStandardDoorZConfig* config
) {
    if (config == nullptr) {
        return false;
    }

    bool known_size = false;
    int count = static_cast<int>(sizeof(STANDARD_DOOR_SIZES) / sizeof(STANDARD_DOOR_SIZES[0]));
    for (int i = 0; i < count; ++i) {
        if (STANDARD_DOOR_SIZES[i] == size) {
            known_size = true;
        }
    }

    if (!known_size) {
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

    bool known_size = false;
    int count = static_cast<int>(sizeof(STANDARD_DOOR_SIZES) / sizeof(STANDARD_DOOR_SIZES[0]));
    for (int i = 0; i < count; ++i) {
        if (STANDARD_DOOR_SIZES[i] == size) {
            known_size = true;
        }
    }

    if (!known_size) {
        return false;
    }

    *z_offset = auto_mapper::core::get_random_standard_jam_z_offset(size);
    return true;
}

AUTO_MAPPER_API int get_standard_door_size_count() {
    return static_cast<int>(sizeof(STANDARD_DOOR_SIZES) / sizeof(STANDARD_DOOR_SIZES[0]));
}

AUTO_MAPPER_API bool get_standard_door_size_at(
    int index,
    int* size
) {
    if (size == nullptr) {
        return false;
    }

    int count = get_standard_door_size_count();
    if (index < 0 || index >= count) {
        return false;
    }

    *size = STANDARD_DOOR_SIZES[index];
    return true;
}

AUTO_MAPPER_API int get_wall_profile_count() {
    return auto_mapper::core::SUPPORTED_WALL_TYPE_COUNT;
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

    *wall_type = auto_mapper::core::SUPPORTED_WALL_TYPES[index];
    return true;
}

AUTO_MAPPER_API bool get_wall_profile(
    int wall_type,
    CWallProfile* profile
) {
    if (profile == nullptr) {
        return false;
    }

    if (!is_supported_wall_type(wall_type)) {
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

AUTO_MAPPER_API int get_wall_drawable_part_count(
    int wall_type
) {
    int count = 0;
    get_drawable_parts(wall_type, count);
    return count;
}

AUTO_MAPPER_API bool get_wall_drawable_part_at(
    int wall_type,
    int index,
    CDrawablePart* part
) {
    if (part == nullptr) {
        return false;
    }

    int count = 0;
    const CDrawablePart* parts = get_drawable_parts(wall_type, count);
    if (parts == nullptr) {
        return false;
    }

    if (index < 0 || index >= count) {
        return false;
    }

    part->part_id = parts[index].part_id;
    return true;
}

AUTO_MAPPER_API bool generate_map_from_segments(
    const char* output_path,
    const CSegment* segments,
    int num_segments,
    const CDoor* doors,
    int num_doors,
    const CIncubatorArray* incubator_arrays,
    int num_incubator_arrays,
    const CDeskArray* desk_arrays,
    int num_desk_arrays,
    float map_size_x,
    float map_size_y,
    bool gen_floor,
    bool gen_ceiling,
    bool random_direction
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
        int door_wall_type = doors[i].wall_type;

        // Auto-sync Set1 door wall_type to the segment wall_type on the same grid line
        if (door_wall_type == 3 || door_wall_type == 4 || door_wall_type == 5) {
            for (int s = 0; s < num_segments; ++s) {
                int seg_wall_type = segments[s].wall_type;
                if (seg_wall_type == 3 || seg_wall_type == 4 || seg_wall_type == 5) {
                    int dx = doors[i].x;
                    int dy = doors[i].y;
                    int dir = doors[i].direction_type;
                    int min_x = std::min(segments[s].x1, segments[s].x2);
                    int max_x = std::max(segments[s].x1, segments[s].x2);
                    int min_y = std::min(segments[s].y1, segments[s].y2);
                    int max_y = std::max(segments[s].y1, segments[s].y2);

                    if (dir == 0 && segments[s].x1 == segments[s].x2 && dx == segments[s].x1) {
                        if (dy >= min_y - 1 && dy <= max_y + 1) {
                            door_wall_type = seg_wall_type;
                            break;
                        }
                    } else if (dir == 1 && segments[s].y1 == segments[s].y2 && dy == segments[s].y1) {
                        if (dx >= min_x - 1 && dx <= max_x + 1) {
                            door_wall_type = seg_wall_type;
                            break;
                        }
                    }
                }
            }
        }

        int door_size = doors[i].size;
        // AS2 Set2 ~ Set9 only have 2-step large doors
        if (door_wall_type >= 6 && door_wall_type <= 13) {
            door_size = 2;
        }

        auto_mapper::core::DoorInstance di = {
            {doors[i].x, doors[i].y},
            door_wall_type,
            doors[i].direction_type,
            door_size,
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

    if (random_direction) {
        randomize_floor_directions(sprites);
    }

    // 2. Build doors
    auto_mapper::core::DoorBuilder door_builder(map_size_x, map_size_y);
    std::vector<auto_mapper::io::Sprite> door_sprites = door_builder.build(cpp_doors);

    // 3. Build indoor decorations
    auto_mapper::core::indoor_decorations::IncubatorBuilder incubator_builder;
    auto_mapper::core::indoor_decorations::DeskBuilder desk_builder;
    std::vector<auto_mapper::io::Sprite> decoration_sprites;

    for (int i = 0; i < num_incubator_arrays; ++i) {
        auto_mapper::core::indoor_decorations::IncubatorArray array = convert_incubator_array(incubator_arrays[i]);

        std::vector<auto_mapper::io::Sprite> array_sprites = incubator_builder.build_array(array);
        decoration_sprites.insert(decoration_sprites.end(), array_sprites.begin(), array_sprites.end());
    }

    for (int i = 0; i < num_desk_arrays; ++i) {
        auto_mapper::core::indoor_decorations::DeskArray array = convert_desk_array(desk_arrays[i]);

        std::vector<auto_mapper::io::Sprite> array_sprites = desk_builder.build_array(array);
        decoration_sprites.insert(decoration_sprites.end(), array_sprites.begin(), array_sprites.end());
    }

    // 4. Merge sprites
    sprites.insert(sprites.end(), door_sprites.begin(), door_sprites.end());
    sprites.insert(sprites.end(), decoration_sprites.begin(), decoration_sprites.end());

    auto_mapper::io::MapFormat output_format = select_output_map_format(cpp_segments, cpp_doors);
    if (auto_mapper::io::write_map(sprites, out_path, output_format, map_size_x, map_size_y)) {
        return true;
    } else {
        return false;
    }
}

} // extern "C"
