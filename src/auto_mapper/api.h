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

/**
 * @brief Build map from segments and doors, write to file.
 */
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

} // extern "C"
