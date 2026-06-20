#include "auto_mapper/io/map_writer.h"
#include "auto_mapper/common/logger.h"
#include <iostream>

using namespace auto_mapper;

int main() {
    Logger::init();
    Logger::info("Starting map writer test...");

    // 5 wall / (601)
    std::vector<io::Sprite> sprites;
    for (int i = 0; i < 5; ++i) {
        io::Sprite spr;
        spr.vid = 601;

        spr.posX = 300.0f - (i * 40.0f);
        spr.posY = 238.0f + (i * 28.0f);
        
        sprites.push_back(spr);
    }

    std::string output_file = "test_cpp_output.map";
    const bool success = io::write_map(
        sprites, 
        output_file, 
        600.0f, 600.0f, 
        "src/auto_mapper/io/templates/empty.map"
    );

    if (success) {
        Logger::info("Successfully generated {}!", output_file);
        Logger::info("You can test it by running: python _pre-demo/as1_map_unpack.py {}", output_file);
    } else {
        Logger::error("Failed to generate map.");
    }

    return 0;
}
