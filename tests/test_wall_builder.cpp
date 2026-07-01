#include <gtest/gtest.h>
#include "auto_mapper/core/wall_builder.h"
#include "auto_mapper/io/map_writer.h"
#include "utils/test_utils.h"
#include <vector>
#include <cmath>
#include <set>
#include <utility>

using namespace auto_mapper;
using namespace auto_mapper::core;
using namespace auto_mapper::test;


/**
 * Tests for AS1 wall builder
 * normal wall + lab wall mixed scene
 * make sure it is aligned and can be manually modified
 */
TEST(WallBuilderTest, WallOnlyGolden) {
    // load json
    const std::string json_path = resolve_test_path("tests/golden/wall_builder.gold.json");
    TestScene scene = load_test_scene(json_path);
    
    // make sure it is load
    ASSERT_GT(scene.segments.size(), 0u);
    
    // only walls
    WallBuilder builder(scene.map_size_x, scene.map_size_y);
    std::vector<io::Sprite> sprites = builder.build(scene.segments, false, false);
    
    // write spirit into temp map
    const std::string temp_output_path = "current_wall_builder.map";
    
    // add cleaner
    TempFileCleaner cleaner(temp_output_path);
    
    bool write_success = io::write_map(sprites, temp_output_path, scene.map_size_x, scene.map_size_y);
    ASSERT_TRUE(write_success);
    
    // compare with gold
    const std::string golden_map_path = resolve_test_path("tests/golden/wall_builder.gold.map");
    bool files_match = compare_binary_files(temp_output_path, golden_map_path);
    
    EXPECT_TRUE(files_match);
}


/**
 * Tests for AS1 Celling Floor builder
 * normal floor + lab floor + celling
 * make sure it is aligned and can be manually modified
 */
TEST(WallBuilderTest, FloorCeilingManualGoldAlignment) {
    // 3. Generate 3x3 grids for standard floor, lab floor, and ceiling directly
    float map_size_x = 1500.0f;
    float map_size_y = 1500.0f;
    WallBuilder builder(map_size_x, map_size_y);

    std::vector<io::Sprite> generated_sprites;

    // Standard Floor (vid 500) 3x3
    const auto& f_std = core::FLOOR_STANDARD;
    for (int gx = -1; gx <= 1; ++gx) {
        for (int gy = -1; gy <= 1; ++gy) {
            generated_sprites.push_back(builder.place_single_floor_celling(
                gx, gy, f_std.vid, f_std.step_x, f_std.step_y, f_std.pos_z, f_std.grid_divisor
            ));
        }
    }
    // Lab Floor (vid 503) 3x3 (shifted to gx=[4..6] to separate in editor)
    const auto& f_lab = core::FLOOR_LAB;
    for (int gx = 4; gx <= 6; ++gx) {
        for (int gy = -1; gy <= 1; ++gy) {
            generated_sprites.push_back(builder.place_single_floor_celling(
                gx, gy, f_lab.vid, f_lab.step_x, f_lab.step_y, f_lab.pos_z, f_lab.grid_divisor
            ));
        }
    }
    // Ceiling (vid 504) 3x3 (shifted to gy=[4..6] to separate in editor)
    const auto& c_prof = core::CEILING_STANDARD;
    for (int gx = -1; gx <= 1; ++gx) {
        for (int gy = 4; gy <= 6; ++gy) {
            generated_sprites.push_back(builder.place_single_floor_celling(
                gx, gy, c_prof.vid, c_prof.step_x, c_prof.step_y, c_prof.pos_z,
                c_prof.grid_divisor
            ));
        }
    }

    // 1. Resolve coordinate anchors directly from the generated sprites.
    // This implements a robust "Self-Referencing Anchor" strategy, ensuring that
    // grid alignment checks are immune to shift offset changes in the builder.
    io::Sprite ref_500;
    ref_500.vid = -1;
    io::Sprite ref_503;
    ref_503.vid = -1;
    io::Sprite ref_504;
    ref_504.vid = -1;

    for (const auto& spr : generated_sprites) {
        if (spr.vid == 500 && ref_500.vid == -1) ref_500 = spr;
        if (spr.vid == 503 && ref_503.vid == -1) ref_503 = spr;
        if (spr.vid == 504 && ref_504.vid == -1) ref_504 = spr;
    }

    ASSERT_NE(ref_500.vid, -1) << "Standard floor reference not found!";
    ASSERT_NE(ref_503.vid, -1) << "Lab floor reference not found!";
    ASSERT_NE(ref_504.vid, -1) << "Ceiling reference not found!";

    // in case there need to be manually verified
    std::string out_map_path = get_project_root() + "/floor_celling_alignment_test.map";
    io::write_map(generated_sprites, out_map_path, map_size_x, map_size_y);

    // 4. Assert grid alignment properties
    std::set<std::pair<int, int>> floor_std_grid;
    std::set<std::pair<int, int>> floor_lab_grid;
    std::set<std::pair<int, int>> ceiling_grid;

    int floor_std_count = 0;
    int floor_lab_count = 0;
    int ceiling_count = 0;

    for (const auto& spr : generated_sprites) {
        if (spr.vid == 500) { // FLOOR_STANDARD
            floor_std_count++;
            float dx = spr.posX - ref_500.posX;
            float dy = spr.posY - ref_500.posY;
            const auto& f_std = core::FLOOR_STANDARD;
            float gx_diff = ((dx / f_std.step_x) + (dy / f_std.step_y)) / 2.0f;
            float gy_diff = ((dy / f_std.step_y) - (dx / f_std.step_x)) / 2.0f;

            EXPECT_NEAR(gx_diff, std::round(gx_diff), 1e-3f)
                << "Standard Floor at (" << spr.posX << ", " << spr.posY << ") is not aligned to gold grid!";
            EXPECT_NEAR(gy_diff, std::round(gy_diff), 1e-3f)
                << "Standard Floor at (" << spr.posX << ", " << spr.posY << ") is not aligned to gold grid!";

            floor_std_grid.insert({static_cast<int>(std::round(gx_diff)), static_cast<int>(std::round(gy_diff))});
        }
        else if (spr.vid == 503) { // FLOOR_LAB
            floor_lab_count++;
            float dx = spr.posX - ref_503.posX;
            float dy = spr.posY - ref_503.posY;
            const auto& f_lab = core::FLOOR_LAB;
            float gx_diff = ((dx / f_lab.step_x) + (dy / f_lab.step_y)) / 2.0f;
            float gy_diff = ((dy / f_lab.step_y) - (dx / f_lab.step_x)) / 2.0f;

            EXPECT_NEAR(gx_diff, std::round(gx_diff), 1e-3f)
                << "Lab Floor at (" << spr.posX << ", " << spr.posY << ") is not aligned to gold grid!";
            EXPECT_NEAR(gy_diff, std::round(gy_diff), 1e-3f)
                << "Lab Floor at (" << spr.posX << ", " << spr.posY << ") is not aligned to gold grid!";

            floor_lab_grid.insert({static_cast<int>(std::round(gx_diff)), static_cast<int>(std::round(gy_diff))});
        }
        else if (spr.vid == 504) { // CEILING_STANDARD
            ceiling_count++;
            float dx = spr.posX - ref_504.posX;
            float dy = spr.posY - ref_504.posY;
            const auto& c_prof = core::CEILING_STANDARD;
            float gx_diff = ((dx / c_prof.step_x) + (dy / c_prof.step_y)) / 2.0f;
            float gy_diff = ((dy / c_prof.step_y) - (dx / c_prof.step_x)) / 2.0f;

            EXPECT_NEAR(gx_diff, std::round(gx_diff), 1e-3f)
                << "Ceiling at (" << spr.posX << ", " << spr.posY << ") is not aligned to gold grid!";
            EXPECT_NEAR(gy_diff, std::round(gy_diff), 1e-3f)
                << "Ceiling at (" << spr.posX << ", " << spr.posY << ") is not aligned to gold grid!";

            ceiling_grid.insert({static_cast<int>(std::round(gx_diff)), static_cast<int>(std::round(gy_diff))});
        }
    }

    // 5. Ensure no duplicates/overlaps
    EXPECT_EQ(floor_std_grid.size(), static_cast<size_t>(floor_std_count)) << "Found overlapping Standard Floor tiles!";
    EXPECT_EQ(floor_lab_grid.size(), static_cast<size_t>(floor_lab_count)) << "Found overlapping Lab Floor tiles!";
    EXPECT_EQ(ceiling_grid.size(), static_cast<size_t>(ceiling_count)) << "Found overlapping Ceiling tiles!";

    std::cout << "[Test] FloorCeilingManualGoldAlignment summary: Standard Floors=" << floor_std_count
              << ", Lab Floors=" << floor_lab_count << ", Ceilings=" << ceiling_count << std::endl;
}

/**
 * Tests for AS1 floor builder
 * normal floor + lab floor mixed scene
 * make sure it is aligned and can be manually modified
 * no missing
 */
TEST(FloorBuilderTest, FloorGoldenMap) {
    const std::string json_path = resolve_test_path("tests/golden/floor_builder.gold.json");
    TestScene scene = load_test_scene(json_path);

    ASSERT_GT(scene.segments.size(), 0u);

    WallBuilder builder(scene.map_size_x, scene.map_size_y);
    std::vector<io::Sprite> sprites = builder.build(scene.segments, true, false);

    const std::string temp_output_path = "current_floor_builder.map";
    TempFileCleaner cleaner(temp_output_path);

    bool write_success = io::write_map(sprites, temp_output_path, scene.map_size_x, scene.map_size_y);
    ASSERT_TRUE(write_success);

    const std::string golden_map_path = resolve_test_path("tests/golden/floor_builder.gold.map");
    bool files_match = compare_binary_files(temp_output_path, golden_map_path);

    EXPECT_TRUE(files_match);
}
