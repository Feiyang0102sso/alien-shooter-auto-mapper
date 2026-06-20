#include "auto_mapper/common/logger.h"
#include "auto_mapper/core/wall_builder.h"
#include "auto_mapper/io/map_writer.h"
#include <iostream>
#include <vector>
#include <string>

using namespace auto_mapper;

int main(int argc, char** argv) {
    Logger::init();
    
    if (argc < 2) {
        std::cerr << "Usage: auto_mapper.exe <output_map_path>\n";
        std::cerr << "Provide segments via stdin in format: x1 y1 x2 y2\n";
        return 1;
    }
    
    std::string output_path = argv[1];
    std::vector<core::Segment> segments;
    
    int x1, y1, x2, y2;
    // 从标准输入读取坐标，直到 EOF
    while (std::cin >> x1 >> y1 >> x2 >> y2) {
        segments.push_back({{x1, y1}, {x2, y2}});
    }
    
    Logger::info("Received {} segments from stdin", segments.size());
    
    if (segments.empty()) {
        std::cout << "WARNING: No segments received.\n";
    }

    core::WallBuilder builder(600.0f, 600.0f);
    std::vector<io::Sprite> sprites = builder.build(segments);
    
    if (io::write_map(sprites, output_path, 600.0f, 600.0f)) {
        Logger::info("Successfully generated map: {}", output_path);
        std::cout << "SUCCESS: " << sprites.size() << " sprites generated into " << output_path << "\n";
        return 0;
    } else {
        Logger::error("Failed to write map to {}", output_path);
        std::cerr << "ERROR: Failed to write map.\n";
        return 1;
    }
}
