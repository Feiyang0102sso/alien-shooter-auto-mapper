#include "auto_mapper/api.h"
#include "auto_mapper/common/logger.h"
#include "auto_mapper/common/config.h"
#include "auto_mapper/core/wall_builder.h"
#include "auto_mapper/io/map_writer.h"
#include <vector>
#include <string>

extern "C" {

AUTO_MAPPER_API bool generate_map_from_segments(
    const char* output_path,
    const CSegment* segments,
    int num_segments,
    int grid_size,
    float map_size_x,
    float map_size_y
) {
    // initial log
    static bool logger_initialized = false;
    if (!logger_initialized) {
        auto_mapper::Config::init_env();
        logger_initialized = true;
    }

    std::string out_path(output_path);
    std::vector<auto_mapper::core::Segment> cpp_segments;
    cpp_segments.reserve(num_segments);

    for (int i = 0; i < num_segments; ++i) {
        cpp_segments.push_back({
            {segments[i].x1, segments[i].y1},
            {segments[i].x2, segments[i].y2}
        });
    }

    auto_mapper::Logger::info("Received {} segments from API", cpp_segments.size());

    auto_mapper::core::WallBuilder builder(grid_size, map_size_x, map_size_y);
    std::vector<auto_mapper::io::Sprite> sprites = builder.build(cpp_segments);

    if (auto_mapper::io::write_map(sprites, out_path, map_size_x, map_size_y)) {
        auto_mapper::Logger::info("Successfully generated map: {}", out_path);
        return true;
    } else {
        auto_mapper::Logger::error("Failed to write map to {}", out_path);
        return false;
    }
}

} // extern "C"
