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

/**
 * @brief Build map from segments and write to file.
 *        Each segment carries its own wall_type for mixed wall support.
 */
AUTO_MAPPER_API bool generate_map_from_segments(
    const char* output_path,
    const CSegment* segments,
    int num_segments,
    int grid_size,
    float map_size_x,
    float map_size_y,
    bool gen_floor,
    bool gen_ceiling
);

} // extern "C"
