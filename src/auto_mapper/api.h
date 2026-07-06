#pragma once

#ifdef _WIN32
#  ifdef AUTO_MAPPER_EXPORTS
#    define AUTO_MAPPER_API __declspec(dllexport)
#  else
#    define AUTO_MAPPER_API __declspec(dllimport)
#  endif
#else
#  define AUTO_MAPPER_API
#endif

extern "C" {

/**
 * @brief Return C API version expected by the Python binding.
 */
AUTO_MAPPER_API int get_auto_mapper_api_version();

/**
 * @brief Represents a single coordinate segment with its wall type.
 */
struct CSegment {
    int x1;
    int y1;
    int x2;
    int y2;
    int wall_type;  // 0=standard, 1=lab (per-segment)
};

struct CDoor {
    int x;
    int y;
    int wall_type;
    int direction_type;
    int size;
    int door_state;
    int light_state;
    float z_offset;
};

struct CStandardDoorZConfig {
    float jam_min_z;
    float jam_max_z;
    float dead_open_min_z;
    float dead_open_max_z;
};

struct CWallProfile {
    int wall_type;
    int dir_a_vid;
    int dir_b_vid;
    int pillar_vid;
    float step_x;
    float step_y;
    float offset_a_x;
    float offset_a_y;
    float offset_b_x;
    float offset_b_y;
    float offset_p_x;
    float offset_p_y;
    int grid_divisor;
};

struct CDrawablePart {
    const char* part_id;
};

struct CIncubatorArray {
    float start_x;
    float start_y;
    float row_length;
    float column_length;
    float item_spacing_scale;
    float row_spacing_scale;
};

struct CIncubatorArrayProfile {
    float row_axis_x;
    float row_axis_y;
    float column_axis_x;
    float column_axis_y;
    float footprint_width;
    float footprint_height;
};

struct CIncubatorPreviewPoint {
    float x;
    float y;
};

/**
 * @brief Read incubator array layout values used by the C++ builder.
 */
AUTO_MAPPER_API bool get_incubator_array_profile(
    CIncubatorArrayProfile* profile
);

/**
 * @brief Return preview unit count for one incubator array using C++ layout rules.
 */
AUTO_MAPPER_API int get_incubator_array_preview_point_count(
    const CIncubatorArray* array
);

/**
 * @brief Fill preview unit points for one incubator array using C++ layout rules.
 */
AUTO_MAPPER_API int get_incubator_array_preview_points(
    const CIncubatorArray* array,
    CIncubatorPreviewPoint* points,
    int max_points
);

/**
 * @brief Build map from segments and doors, write to file.
 */
AUTO_MAPPER_API bool generate_map_from_segments(
    const char* output_path,
    const CSegment* segments,
    int num_segments,
    const CDoor* doors,
    int num_doors,
    const CIncubatorArray* incubator_arrays,
    int num_incubator_arrays,
    float map_size_x,
    float map_size_y,
    bool gen_floor,
    bool gen_ceiling,
    bool random_direction
);

/**
 * @brief Read z-offset config for a standard door size.
 */
AUTO_MAPPER_API bool get_standard_door_z_config(
    int size,
    CStandardDoorZConfig* config
);

/**
 * @brief Generate one random jammed z-offset for a standard door size.
 */
AUTO_MAPPER_API bool get_standard_door_jam_z_offset(
    int size,
    float* z_offset
);

/**
 * @brief Return number of supported standard door sizes.
 */
AUTO_MAPPER_API int get_standard_door_size_count();

/**
 * @brief Read supported standard door size by index.
 */
AUTO_MAPPER_API bool get_standard_door_size_at(
    int index,
    int* size
);

/**
 * @brief Return number of C++ supported wall profiles.
 */
AUTO_MAPPER_API int get_wall_profile_count();

/**
 * @brief Read wall type by profile index.
 */
AUTO_MAPPER_API bool get_wall_profile_type_at(
    int index,
    int* wall_type
);

/**
 * @brief Read full wall profile by wall type.
 */
AUTO_MAPPER_API bool get_wall_profile(
    int wall_type,
    CWallProfile* profile
);

/**
 * @brief Return number of drawable parts for a wall profile.
 */
AUTO_MAPPER_API int get_wall_drawable_part_count(
    int wall_type
);

/**
 * @brief Read one drawable part by wall profile and part index.
 */
AUTO_MAPPER_API bool get_wall_drawable_part_at(
    int wall_type,
    int index,
    CDrawablePart* part
);

} // extern "C"
