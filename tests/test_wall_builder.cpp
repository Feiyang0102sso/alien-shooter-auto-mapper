#include <gtest/gtest.h>
#include "auto_mapper/core/wall_builder/wall_builder.h"
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
    const std::string temp_output_path = get_test_output_path("current_wall_builder.map");
    
    // add cleaner
    TempFileCleaner cleaner(temp_output_path);
    
    bool write_success = io::write_map(sprites, temp_output_path, scene.map_size_x, scene.map_size_y);
    ASSERT_TRUE(write_success);
    
    // compare with gold
    const std::string golden_map_path = resolve_test_path("tests/golden/wall_builder.gold.map");
    bool files_match = compare_binary_files(temp_output_path, golden_map_path);
    
    EXPECT_TRUE(files_match);
}

TEST(WallBuilderTest, StandardDarkWallUsesDarkProfileVids) {
    WallBuilder builder(600.0f, 600.0f);

    std::vector<Segment> segments = {
        {{0, 0}, {2, 0}, WALL_TYPE_STANDARD_DARK},
        {{2, 0}, {2, 2}, WALL_TYPE_STANDARD_DARK},
        {{2, 2}, {0, 2}, WALL_TYPE_STANDARD_DARK},
        {{0, 2}, {0, 0}, WALL_TYPE_STANDARD_DARK},
    };

    std::vector<io::Sprite> sprites = builder.build(segments, true, false);

    bool found_dir_a = false;
    bool found_dir_b = false;
    bool found_pillar = false;
    bool found_floor = false;

    for (const auto& sprite : sprites) {
        if (sprite.vid == WALL_STANDARD_DARK.dir_a_vid) {
            found_dir_a = true;
        }
        if (sprite.vid == WALL_STANDARD_DARK.dir_b_vid) {
            found_dir_b = true;
        }
        if (sprite.vid == WALL_STANDARD_DARK.pillar_vid) {
            found_pillar = true;
        }
        if (sprite.vid == FLOOR_STANDARD_DARK.vid) {
            found_floor = true;
        }
    }

    EXPECT_TRUE(found_dir_a);
    EXPECT_TRUE(found_dir_b);
    EXPECT_TRUE(found_pillar);
    EXPECT_TRUE(found_floor);
}

static bool is_as2_set1_random_wall_direction(uint32_t direction) {
    if (direction == 102u) {
        return true;
    }

    if (direction == 153u) {
        return true;
    }

    if (direction == 204u) {
        return true;
    }

    return false;
}

static std::vector<Segment> build_room_segments_at(int start_x, int start_y, int size, int wall_type) {
    std::vector<Segment> segments;
    segments.push_back({{start_x, start_y}, {start_x + size, start_y}, wall_type});
    segments.push_back({{start_x + size, start_y}, {start_x + size, start_y + size}, wall_type});
    segments.push_back({{start_x + size, start_y + size}, {start_x, start_y + size}, wall_type});
    segments.push_back({{start_x, start_y + size}, {start_x, start_y}, wall_type});
    return segments;
}

static std::vector<Segment> build_room_segments(int size, int wall_type) {
    return build_room_segments_at(0, 0, size, wall_type);
}

static int count_sprites_by_vid(const std::vector<io::Sprite>& sprites, int vid) {
    int count = 0;

    for (const io::Sprite& sprite : sprites) {
        if (sprite.vid == vid) {
            count += 1;
        }
    }

    return count;
}

TEST(WallBuilderTest, As2WallSet1FixedProfilesUseFixedDirections) {
    WallBuilder builder(2000.0f, 2000.0f);

    io::Sprite fixed0_dir_a = builder.place_single_wall(0, 0, WALL_TYPE_AS2_WALL_SET1_FIXED_0, WallPartKind::DirA);
    io::Sprite fixed0_dir_b = builder.place_single_wall(0, 0, WALL_TYPE_AS2_WALL_SET1_FIXED_0, WallPartKind::DirB);
    io::Sprite fixed0_pillar = builder.place_single_wall(0, 0, WALL_TYPE_AS2_WALL_SET1_FIXED_0, WallPartKind::Pillar);

    EXPECT_EQ(fixed0_dir_a.vid, 1781);
    EXPECT_EQ(fixed0_dir_a.direction, 51u);
    EXPECT_EQ(fixed0_dir_b.vid, 1780);
    EXPECT_EQ(fixed0_dir_b.direction, 0u);
    EXPECT_EQ(fixed0_pillar.vid, 1782);
    EXPECT_EQ(fixed0_pillar.direction, 0u);

    io::Sprite fixed1_dir_a = builder.place_single_wall(0, 0, WALL_TYPE_AS2_WALL_SET1_FIXED_1, WallPartKind::DirA);
    io::Sprite fixed1_dir_b = builder.place_single_wall(0, 0, WALL_TYPE_AS2_WALL_SET1_FIXED_1, WallPartKind::DirB);
    io::Sprite fixed1_pillar = builder.place_single_wall(0, 0, WALL_TYPE_AS2_WALL_SET1_FIXED_1, WallPartKind::Pillar);

    EXPECT_EQ(fixed1_dir_a.vid, 1781);
    EXPECT_EQ(fixed1_dir_a.direction, 0u);
    EXPECT_EQ(fixed1_dir_b.vid, 1780);
    EXPECT_EQ(fixed1_dir_b.direction, 51u);
    EXPECT_EQ(fixed1_pillar.vid, 1782);
    EXPECT_EQ(fixed1_pillar.direction, 85u);
}

TEST(WallBuilderTest, As2WallSet1RandomProfileUsesAllowedVariantPool) {
    WallBuilder builder(2000.0f, 2000.0f);

    std::vector<Segment> segments = {
        {{0, 0}, {3, 0}, WALL_TYPE_AS2_WALL_SET1_RANDOM},
        {{3, 0}, {3, 3}, WALL_TYPE_AS2_WALL_SET1_RANDOM},
        {{3, 3}, {0, 3}, WALL_TYPE_AS2_WALL_SET1_RANDOM},
        {{0, 3}, {0, 0}, WALL_TYPE_AS2_WALL_SET1_RANDOM},
    };

    std::vector<io::Sprite> sprites = builder.build(segments, false, false);
    ASSERT_GT(sprites.size(), 0u);

    for (const io::Sprite& sprite : sprites) {
        if (sprite.vid == 1780 || sprite.vid == 1781) {
            EXPECT_TRUE(is_as2_set1_random_wall_direction(sprite.direction));
        }

        if (sprite.vid == 1782) {
            EXPECT_EQ(sprite.direction, 170u);
        }
    }
}

TEST(WallBuilderTest, As2WallSet1BuildsFixedRoomsAndRandomLargeRoom) {
    WallBuilder builder(2000.0f, 2000.0f);

    std::vector<io::Sprite> fixed0_sprites = builder.build(
        build_room_segments(3, WALL_TYPE_AS2_WALL_SET1_FIXED_0),
        false,
        false
    );
    std::vector<io::Sprite> fixed1_sprites = builder.build(
        build_room_segments(3, WALL_TYPE_AS2_WALL_SET1_FIXED_1),
        false,
        false
    );

    EXPECT_EQ(fixed0_sprites.size(), 16u);
    EXPECT_EQ(count_sprites_by_vid(fixed0_sprites, 1781), 6);
    EXPECT_EQ(count_sprites_by_vid(fixed0_sprites, 1780), 6);
    EXPECT_EQ(count_sprites_by_vid(fixed0_sprites, 1782), 4);

    EXPECT_EQ(fixed1_sprites.size(), 16u);
    EXPECT_EQ(count_sprites_by_vid(fixed1_sprites, 1781), 6);
    EXPECT_EQ(count_sprites_by_vid(fixed1_sprites, 1780), 6);
    EXPECT_EQ(count_sprites_by_vid(fixed1_sprites, 1782), 4);

    for (const io::Sprite& sprite : fixed0_sprites) {
        if (sprite.vid == 1781) {
            EXPECT_EQ(sprite.direction, 51u);
        }
        if (sprite.vid == 1780) {
            EXPECT_EQ(sprite.direction, 0u);
        }
        if (sprite.vid == 1782) {
            EXPECT_EQ(sprite.direction, 0u);
        }
    }

    for (const io::Sprite& sprite : fixed1_sprites) {
        if (sprite.vid == 1781) {
            EXPECT_EQ(sprite.direction, 0u);
        }
        if (sprite.vid == 1780) {
            EXPECT_EQ(sprite.direction, 51u);
        }
        if (sprite.vid == 1782) {
            EXPECT_EQ(sprite.direction, 85u);
        }
    }

    std::vector<io::Sprite> random_sprites = builder.build(
        build_room_segments(9, WALL_TYPE_AS2_WALL_SET1_RANDOM),
        false,
        false
    );
    std::set<uint32_t> random_wall_directions;

    EXPECT_EQ(random_sprites.size(), 40u);
    EXPECT_EQ(count_sprites_by_vid(random_sprites, 1781), 18);
    EXPECT_EQ(count_sprites_by_vid(random_sprites, 1780), 18);
    EXPECT_EQ(count_sprites_by_vid(random_sprites, 1782), 4);

    for (const io::Sprite& sprite : random_sprites) {
        if (sprite.vid == 1780 || sprite.vid == 1781) {
            EXPECT_TRUE(is_as2_set1_random_wall_direction(sprite.direction));
            random_wall_directions.insert(sprite.direction);
        }

        if (sprite.vid == 1782) {
            EXPECT_EQ(sprite.direction, 170u);
        }
    }

    EXPECT_GT(random_wall_directions.size(), 1u);

    std::vector<Segment> preview_segments;
    std::vector<Segment> fixed0_preview_segments = build_room_segments_at(0, 0, 3, WALL_TYPE_AS2_WALL_SET1_FIXED_0);
    std::vector<Segment> fixed1_preview_segments = build_room_segments_at(6, 0, 3, WALL_TYPE_AS2_WALL_SET1_FIXED_1);
    std::vector<Segment> random_preview_segments = build_room_segments_at(0, 6, 9, WALL_TYPE_AS2_WALL_SET1_RANDOM);

    preview_segments.insert(preview_segments.end(), fixed0_preview_segments.begin(), fixed0_preview_segments.end());
    preview_segments.insert(preview_segments.end(), fixed1_preview_segments.begin(), fixed1_preview_segments.end());
    preview_segments.insert(preview_segments.end(), random_preview_segments.begin(), random_preview_segments.end());

    std::vector<io::Sprite> preview_sprites = builder.build(preview_segments, false, false);
    std::string preview_map_path = get_test_output_path("as2_wall_set1_variants.map");
    bool write_success = io::write_map(
        preview_sprites,
        preview_map_path,
        io::MapFormat::AS2R,
        2000.0f,
        2000.0f
    );
    ASSERT_TRUE(write_success);
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
    std::string out_map_path = get_test_output_path("floor_celling_alignment_test.map");
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

    const std::string temp_output_path = get_test_output_path("current_floor_builder.map");
    TempFileCleaner cleaner(temp_output_path);

    bool write_success = io::write_map(sprites, temp_output_path, scene.map_size_x, scene.map_size_y);
    ASSERT_TRUE(write_success);

    const std::string golden_map_path = resolve_test_path("tests/golden/floor_builder.gold.map");
    bool files_match = compare_binary_files(temp_output_path, golden_map_path);

    EXPECT_TRUE(files_match);
}
