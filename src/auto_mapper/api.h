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
 * @brief Represents a single coordinate segment.
 */
struct CSegment {
    int x1;
    int y1;
    int x2;
    int y2;
};

/**
 * @brief Wall type constants for selecting WallProfile.
 *
 * 0 = Standard wall (601/602/604, step 40x28)
 * 1 = Lab long wall (651/650/652, step 90x64)
 */
constexpr int WALL_TYPE_STANDARD = 0;
constexpr int WALL_TYPE_LAB      = 1;

/**
 * @brief Build map from segments and write to file.
 * 
 * @param output_path  Path to the output .map file.
 * @param segments     Array of CSegment structs.
 * @param num_segments Number of segments in the array.
 * @param grid_size    The grid size for building walls.
 * @param map_size_x   Width of the map.
 * @param map_size_y   Height of the map.
 * @param wall_type    Wall type selector (WALL_TYPE_STANDARD or WALL_TYPE_LAB).
 * @return true on success, false on failure.
 */
AUTO_MAPPER_API bool generate_map_from_segments(
    const char* output_path,
    const CSegment* segments,
    int num_segments,
    int grid_size,
    float map_size_x,
    float map_size_y,
    int wall_type
);

} // extern "C"
