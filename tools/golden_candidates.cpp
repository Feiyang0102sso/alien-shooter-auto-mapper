// Export deterministic test candidates without overwriting tracked golden files.
#include "utils/test_utils.h"
#include "auto_mapper/core/door_builder/door_builder.h"
#include <filesystem>

using namespace auto_mapper;
using namespace auto_mapper::test;

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: auto_mapper_golden_candidates <output-directory>\n";
        return 1;
    }
    const std::filesystem::path output(argv[1]);
    std::filesystem::create_directories(output);
    const std::vector<std::string> names{"wall_builder.gold", "door_builder.gold", "incubator_builder_gold",
                                         "floor_builder.gold"};
    for (const auto &name : names) {
        auto scene = load_test_scene(resolve_test_path("tests/golden/" + name + ".json"));
        core::WallBuilder wall_builder(scene.map_size_x, scene.map_size_y);
        std::vector<core::DoorExcavation> excavations;
        if (name == "door_builder.gold") {
            for (const auto &door : scene.doors) {
                int size = door.size;
                if (door.wall_type == core::WALL_TYPE_LAB)
                    size = 1;
                excavations.push_back({door.pos, door.direction_type, size, door.wall_type});
            }
        }
        bool floor = name == "floor_builder.gold";
        auto sprites = wall_builder.build(scene.segments, floor, false, excavations);
        if (name == "door_builder.gold") {
            core::DoorBuilder builder(scene.map_size_x, scene.map_size_y);
            auto doors = builder.build(scene.doors);
            sprites.insert(sprites.end(), doors.begin(), doors.end());
        }
        if (name == "incubator_builder_gold") {
            core::indoor_decorations::IncubatorBuilder builder;
            for (const auto &array : scene.decorations) {
                auto decorations = builder.build_array(array);
                sprites.insert(sprites.end(), decorations.begin(), decorations.end());
            }
        }
        if (!io::write_map(sprites, (output / (name + ".map")).string(), scene.map_size_x, scene.map_size_y))
            return 1;
    }
    return 0;
}
