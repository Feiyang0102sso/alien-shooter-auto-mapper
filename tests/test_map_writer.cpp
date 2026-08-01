#include <gtest/gtest.h>
#include "auto_mapper/io/map_writer.h"
#include "utils/test_utils.h"
#include <vector>

using namespace auto_mapper;
using namespace auto_mapper::test;

TEST(MapWriterTest, WriteMapSuccess) {
    std::vector<io::Sprite> sprites;
    for (int i = 0; i < 5; ++i) {
        io::Sprite spr;
        spr.vid = 601;

        spr.posX = 300.0f - (i * 40.0f);
        spr.posY = 238.0f + (i * 28.0f);
        
        sprites.push_back(spr);
    }

    std::string output_file = get_test_output_path("test_cpp_output.map");
    bool success = io::write_map(
        sprites, 
        output_file, 
        600.0f, 600.0f
    );

    EXPECT_TRUE(success);
}
