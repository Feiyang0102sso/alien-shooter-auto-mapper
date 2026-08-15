#include <gtest/gtest.h>
#include "auto_mapper/core/wall_builder/wall_builder.h"
#include "auto_mapper/core/vid_properties/vid_armies_as2.h"
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
    const std::string temp_output_path = get_test_output_path("AS1/wall/current_wall_builder.map");
    
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

TEST(WallBuilderTest, As2Set2FloorSpritesCarryProfileGamma) {
    WallBuilder builder(600.0f, 600.0f);

    std::vector<Segment> segments = {
        {{0, 0}, {2, 0}, WALL_TYPE_AS2_WALL_SET2_RANDOM},
        {{2, 0}, {2, 2}, WALL_TYPE_AS2_WALL_SET2_RANDOM},
        {{2, 2}, {0, 2}, WALL_TYPE_AS2_WALL_SET2_RANDOM},
        {{0, 2}, {0, 0}, WALL_TYPE_AS2_WALL_SET2_RANDOM},
    };

    std::vector<io::Sprite> sprites = builder.build(segments, true, false);
    bool found_floor = false;

    for (const auto& sprite : sprites) {
        if (sprite.vid == FLOOR_AS2_SET2.vid) {
            found_floor = true;
            EXPECT_EQ(sprite.gamma.r, -80);
            EXPECT_EQ(sprite.gamma.g, -80);
            EXPECT_EQ(sprite.gamma.b, -80);
            EXPECT_EQ(sprite.gamma.a, 0);
        }
    }

    EXPECT_TRUE(found_floor);
}

TEST(WallBuilderTest, As2WallAndFloorSpritesUseVidArmyBindings) {
    WallBuilder builder(600.0f, 600.0f);

    std::vector<Segment> segments = {
        {{0, 0}, {2, 0}, WALL_TYPE_AS2_WALL_SET3_RANDOM},
        {{2, 0}, {2, 2}, WALL_TYPE_AS2_WALL_SET3_RANDOM},
        {{2, 2}, {0, 2}, WALL_TYPE_AS2_WALL_SET3_RANDOM},
        {{0, 2}, {0, 0}, WALL_TYPE_AS2_WALL_SET3_RANDOM},
    };

    std::vector<io::Sprite> sprites = builder.build(segments, true, false);
    bool found_army_zero = false;
    bool found_army_two = false;

    for (const io::Sprite& sprite : sprites) {
        VidArmy expected_army = get_as2_wall_set_asset_army(sprite.vid);
        EXPECT_EQ(sprite.army, expected_army) << "Incorrect Army for VID " << sprite.vid;

        if (sprite.army == 0) {
            found_army_zero = true;
        }
        if (sprite.army == 2) {
            found_army_two = true;
        }
    }

    EXPECT_TRUE(found_army_zero);
    EXPECT_TRUE(found_army_two);
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

static bool is_as2_set2_random_wall_direction(uint32_t direction) {
    if (direction == 0u) {
        return true;
    }

    if (direction == 51u) {
        return true;
    }

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

static bool is_as2_set3_dir_a_direction(uint32_t direction) {
    if (direction == 0u) {
        return true;
    }

    if (direction == 51u) {
        return true;
    }

    if (direction == 102u) {
        return true;
    }

    if (direction == 153u) {
        return true;
    }

    return false;
}

static bool is_as2_set3_dir_b_direction(uint32_t direction) {
    if (direction == 0u) {
        return true;
    }

    if (direction == 51u) {
        return true;
    }

    if (direction == 102u) {
        return true;
    }

    if (direction == 153u) {
        return true;
    }

    return false;
}

static bool is_as2_set4_dir_a_direction(uint32_t direction) {
    if (direction == 0u) {
        return true;
    }

    if (direction == 51u) {
        return true;
    }

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

static bool is_as2_set4_dir_b_direction(uint32_t direction) {
    if (direction == 0u) {
        return true;
    }

    if (direction == 51u) {
        return true;
    }

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

static bool is_as2_set5_wall_direction(uint32_t direction) {
    if (direction == 0u) {
        return true;
    }

    if (direction == 51u) {
        return true;
    }

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

static bool is_as2_set6_wall_direction(uint32_t direction) {
    if (direction == 0u) {
        return true;
    }

    if (direction == 51u) {
        return true;
    }

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

static bool is_as2_set7_wall_direction(uint32_t direction) {
    if (direction == 0u) {
        return true;
    }

    if (direction == 51u) {
        return true;
    }

    if (direction == 102u) {
        return true;
    }

    return false;
}

static bool is_as2_set8_wall_direction(uint32_t direction) {
    if (direction == 0u) {
        return true;
    }

    if (direction == 51u) {
        return true;
    }

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

static bool is_as2_set9_wall_direction(uint32_t direction) {
    if (direction == 0u) {
        return true;
    }

    if (direction == 51u) {
        return true;
    }

    if (direction == 102u) {
        return true;
    }

    if (direction == 153u) {
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

static std::vector<Segment> build_concave_room_segments(
    int recess_width,
    int recess_depth,
    int wall_type
) {
    int room_width = 12;
    int room_height = 10;
    int recess_start_x = (room_width - recess_width) / 2;
    int recess_end_x = recess_start_x + recess_width;

    std::vector<Segment> segments;
    segments.push_back({{0, 0}, {recess_start_x, 0}, wall_type});
    segments.push_back({{recess_start_x, 0}, {recess_start_x, recess_depth}, wall_type});
    segments.push_back({{recess_start_x, recess_depth}, {recess_end_x, recess_depth}, wall_type});
    segments.push_back({{recess_end_x, recess_depth}, {recess_end_x, 0}, wall_type});
    segments.push_back({{recess_end_x, 0}, {room_width, 0}, wall_type});
    segments.push_back({{room_width, 0}, {room_width, room_height}, wall_type});
    segments.push_back({{room_width, room_height}, {0, room_height}, wall_type});
    segments.push_back({{0, room_height}, {0, 0}, wall_type});
    return segments;
}

TEST(AS1LabCeilingTest, UsesProfileOffsetsForEveryWallSide) {
    WallBuilder builder(2000.0f, 2000.0f, false);

    struct PlacementCase {
        WallPartKind kind;
        WallOutsideSide outside_side;
        float expected_offset_x;
        float expected_offset_y;
    };

    const PlacementCase cases[] = {
        {WallPartKind::DirA, WallOutsideSide::NegativeGridSide, -72.0f, -6.0f},
        {WallPartKind::DirA, WallOutsideSide::PositiveGridSide, 38.0f, 46.0f},
        {WallPartKind::DirB, WallOutsideSide::NegativeGridSide, 23.0f, -22.0f},
        {WallPartKind::DirB, WallOutsideSide::PositiveGridSide, -37.0f, 42.0f}
    };

    for (const PlacementCase& placement_case : cases) {
        io::Sprite wall = builder.place_single_wall(
            2,
            3,
            WALL_TYPE_LAB,
            placement_case.kind
        );
        io::Sprite ceiling = builder.place_single_ceiling_curtain(
            2,
            3,
            WALL_TYPE_LAB,
            placement_case.kind,
            false,
            placement_case.outside_side
        );

        EXPECT_EQ(ceiling.vid, CEILING_AS1_LAB.vid);
        EXPECT_FLOAT_EQ(ceiling.posX - wall.posX, placement_case.expected_offset_x);
        EXPECT_FLOAT_EQ(ceiling.posY - wall.posY, placement_case.expected_offset_y);
        EXPECT_FLOAT_EQ(ceiling.posZ, CEILING_AS1_LAB.pos_z);
        EXPECT_EQ(ceiling.direction, 0u);
    }
}

TEST(AS1LabCeilingTest, DISABLED_PlacesWallTilesAndFourCornerSupplements) {
    constexpr int room_size = 3;
    WallBuilder builder(2000.0f, 2000.0f, false);
    std::vector<io::Sprite> sprites = builder.build(
        build_room_segments(room_size, WALL_TYPE_LAB),
        false,
        true
    );

    int ceiling_count = 0;
    std::set<std::pair<float, float>> ceiling_positions;
    for (const io::Sprite& sprite : sprites) {
        if (sprite.vid != CEILING_AS1_LAB.vid) {
            continue;
        }

        ceiling_count += 1;
        ceiling_positions.insert({sprite.posX, sprite.posY});
    }

    int expected_exterior_wall_count = room_size * 4;
    int expected_supplement_count =
        CEILING_AS1_LAB.left_upper_corner_dir_a_supplement_count +
        CEILING_AS1_LAB.left_upper_corner_dir_b_supplement_count +
        CEILING_AS1_LAB.left_lower_corner_dir_a_supplement_count +
        CEILING_AS1_LAB.left_lower_corner_dir_b_supplement_count +
        CEILING_AS1_LAB.right_upper_corner_dir_a_supplement_count +
        CEILING_AS1_LAB.right_upper_corner_dir_b_supplement_count +
        CEILING_AS1_LAB.right_lower_corner_dir_a_supplement_count +
        CEILING_AS1_LAB.right_lower_corner_dir_b_supplement_count;
    int expected_ceiling_count =
        expected_exterior_wall_count + expected_supplement_count;
    EXPECT_EQ(ceiling_count, expected_ceiling_count);
    EXPECT_EQ(
        ceiling_positions.size(),
        static_cast<std::size_t>(expected_ceiling_count)
    );

    int connected_pair_count = 0;
    for (auto first_it = ceiling_positions.begin();
         first_it != ceiling_positions.end();
         ++first_it) {
        auto second_it = first_it;
        ++second_it;
        for (; second_it != ceiling_positions.end(); ++second_it) {
            float distance_x = std::abs(second_it->first - first_it->first);
            float distance_y = std::abs(second_it->second - first_it->second);
            bool follows_ceiling_pitch =
                distance_x == 80.0f && distance_y == 56.0f;
            if (follows_ceiling_pitch) {
                connected_pair_count += 1;
            }
        }
    }
}


static TestScene build_project_reference_wall_scene(int wall_type) {
    TestScene scene;
    scene.map_size_x = 7000.0f;
    scene.map_size_y = 6000.0f;

    // Copied from the root project used for manual Ceiling Curtain calibration.
    // The scene includes all four standalone deep-corner quadrants.
    std::vector<Segment>& segments = scene.segments;
    segments.reserve(56);
    segments.push_back({{-8, 11}, {-8, 12}, wall_type});
    segments.push_back({{-8, 18}, {-8, 23}, wall_type});
    segments.push_back({{-8, 23}, {-1, 23}, wall_type});
    segments.push_back({{6, 23}, {16, 23}, wall_type});
    segments.push_back({{16, 23}, {16, 19}, wall_type});
    segments.push_back({{16, 14}, {16, 9}, wall_type});
    segments.push_back({{16, 9}, {8, 9}, wall_type});
    segments.push_back({{-2, 9}, {-8, 9}, wall_type});
    segments.push_back({{-8, 9}, {-8, 12}, wall_type});
    segments.push_back({{-8, 18}, {-5, 18}, wall_type});
    segments.push_back({{-5, 18}, {-5, 12}, wall_type});
    segments.push_back({{-5, 12}, {-8, 12}, wall_type});
    segments.push_back({{-2, 9}, {-2, 12}, wall_type});
    segments.push_back({{-2, 12}, {8, 12}, wall_type});
    segments.push_back({{8, 12}, {8, 9}, wall_type});
    segments.push_back({{16, 14}, {10, 14}, wall_type});
    segments.push_back({{10, 14}, {10, 19}, wall_type});
    segments.push_back({{10, 19}, {16, 19}, wall_type});
    segments.push_back({{-1, 23}, {-1, 18}, wall_type});
    segments.push_back({{-1, 18}, {6, 18}, wall_type});
    segments.push_back({{6, 18}, {6, 22}, wall_type});
    segments.push_back({{6, 22}, {6, 23}, wall_type});
    segments.push_back({{-1, 16}, {-1, 14}, wall_type});
    segments.push_back({{-1, 14}, {5, 14}, wall_type});
    segments.push_back({{5, 14}, {5, 16}, wall_type});
    segments.push_back({{5, 16}, {0, 16}, wall_type});
    segments.push_back({{0, 16}, {-1, 16}, wall_type});
    segments.push_back({{17, 40}, {27, 40}, wall_type});
    segments.push_back({{27, 40}, {27, 50}, wall_type});
    segments.push_back({{27, 50}, {17, 50}, wall_type});
    segments.push_back({{17, 50}, {17, 40}, wall_type});
    segments.push_back({{21, 17}, {21, 10}, wall_type});
    segments.push_back({{21, 10}, {31, 10}, wall_type});
    segments.push_back({{31, 10}, {31, 14}, wall_type});
    segments.push_back({{31, 14}, {26, 14}, wall_type});
    segments.push_back({{26, 14}, {26, 18}, wall_type});
    segments.push_back({{26, 18}, {21, 18}, wall_type});
    segments.push_back({{21, 18}, {21, 17}, wall_type});
    segments.push_back({{37, 13}, {37, 10}, wall_type});
    segments.push_back({{37, 10}, {45, 10}, wall_type});
    segments.push_back({{45, 10}, {45, 19}, wall_type});
    segments.push_back({{45, 19}, {42, 19}, wall_type});
    segments.push_back({{42, 19}, {42, 13}, wall_type});
    segments.push_back({{42, 13}, {37, 13}, wall_type});
    segments.push_back({{24, 23}, {27, 23}, wall_type});
    segments.push_back({{27, 23}, {27, 28}, wall_type});
    segments.push_back({{27, 28}, {35, 28}, wall_type});
    segments.push_back({{35, 28}, {35, 32}, wall_type});
    segments.push_back({{35, 32}, {24, 32}, wall_type});
    segments.push_back({{24, 32}, {24, 23}, wall_type});
    segments.push_back({{45, 23}, {48, 23}, wall_type});
    segments.push_back({{48, 23}, {48, 31}, wall_type});
    segments.push_back({{48, 31}, {40, 31}, wall_type});
    segments.push_back({{40, 31}, {40, 28}, wall_type});
    segments.push_back({{40, 28}, {45, 28}, wall_type});
    segments.push_back({{45, 28}, {45, 23}, wall_type});

    return scene;
}

TEST(AS1LabCeilingTest, BFSExpansionGeneratesContinuousExteriorCeilings) {
    TestScene scene = build_project_reference_wall_scene(WALL_TYPE_LAB);
    WallBuilder builder(scene.map_size_x, scene.map_size_y, false);
    std::vector<io::Sprite> sprites = builder.build(
        scene.segments,
        true,
        true
    );

    int lab_ceiling_count = 0;
    for (const io::Sprite& sprite : sprites) {
        if (sprite.vid == CEILING_AS1_LAB.vid) {
            lab_ceiling_count += 1;
        }
    }

    EXPECT_GT(lab_ceiling_count, 0);

    std::string output_path = get_test_output_path(
        "celling/as1_lab_ceiling_bfs_validation.map"
    );
    io::write_map(
        sprites,
        output_path,
        io::MapFormat::AS1,
        scene.map_size_x,
        scene.map_size_y
    );
}

TEST(AS1LabCeilingTest, NeverPlacesCeilingInsideEnclosedRoom) {
    constexpr int room_size = 6;
    WallBuilder builder(4000.0f, 4000.0f, false);
    std::vector<io::Sprite> sprites = builder.build(
        build_room_segments(room_size, WALL_TYPE_LAB),
        false,
        true
    );

    const WallProfile& profile = WallBuilder::get_wall_profile(WALL_TYPE_LAB);
    MapPoint shift = WallBuilder::get_wall_shift(4000.0f, profile);

    for (const io::Sprite& sprite : sprites) {
        if (sprite.vid != CEILING_AS1_LAB.vid) {
            continue;
        }

        float delta_x = (sprite.posX - shift.x) / profile.step_x;
        float delta_y = (sprite.posY - shift.y) / profile.step_y;
        float gx = (delta_x + delta_y) / 2.0f;
        float gy = (delta_y - delta_x) / 2.0f;

        bool is_strictly_interior = (gx > 0.8f && gx < room_size - 0.8f &&
                                     gy > 0.8f && gy < room_size - 0.8f);
        EXPECT_FALSE(is_strictly_interior)
            << "Ceiling sprite at (" << sprite.posX << ", " << sprite.posY
            << ") mapped to interior grid (" << gx << ", " << gy << ")";
    }
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

static int count_sprites_by_vid_and_direction(
    const std::vector<io::Sprite>& sprites,
    int vid,
    uint32_t direction
) {
    int count = 0;

    for (const io::Sprite& sprite : sprites) {
        if (sprite.vid != vid) {
            continue;
        }

        if (sprite.direction == direction) {
            count += 1;
        }
    }

    return count;
}

static bool has_sprite_at(
    const std::vector<io::Sprite>& sprites,
    int vid,
    float pos_x,
    float pos_y,
    uint32_t direction
) {
    for (const io::Sprite& sprite : sprites) {
        if (sprite.vid != vid) {
            continue;
        }

        if (sprite.direction != direction) {
            continue;
        }

        bool x_matches = std::abs(sprite.posX - pos_x) < 0.01f;
        bool y_matches = std::abs(sprite.posY - pos_y) < 0.01f;
        if (x_matches && y_matches) {
            return true;
        }
    }

    return false;
}

static bool is_rare_wall_sprite(const io::Sprite& sprite, const WallProfile& profile) {
    const WallVariant& rare_variant = profile.variants[profile.rare_variant_index];

    bool matches_dir_a = sprite.vid == rare_variant.dir_a.vid &&
        sprite.direction == rare_variant.dir_a.direction;
    if (matches_dir_a) {
        return true;
    }

    return sprite.vid == rare_variant.dir_b.vid &&
        sprite.direction == rare_variant.dir_b.direction;
}

static std::vector<io::Sprite> get_rare_wall_sprites(
    const std::vector<io::Sprite>& sprites,
    const WallProfile& profile
) {
    std::vector<io::Sprite> rare_sprites;

    for (const io::Sprite& sprite : sprites) {
        if (is_rare_wall_sprite(sprite, profile)) {
            rare_sprites.push_back(sprite);
        }
    }

    return rare_sprites;
}

static void expect_rare_wall_min_distance(
    const std::vector<io::Sprite>& sprites,
    const WallProfile& profile
) {
    std::vector<io::Sprite> rare_sprites = get_rare_wall_sprites(sprites, profile);
    float wall_step_length = std::hypot(profile.step_x, profile.step_y);
    float min_distance = wall_step_length * profile.rare_min_distance_steps;

    for (std::size_t first_index = 0; first_index < rare_sprites.size(); ++first_index) {
        for (std::size_t second_index = first_index + 1;
             second_index < rare_sprites.size();
             ++second_index) {
            float distance_x = rare_sprites[first_index].posX - rare_sprites[second_index].posX;
            float distance_y = rare_sprites[first_index].posY - rare_sprites[second_index].posY;
            float distance = std::hypot(distance_x, distance_y);
            EXPECT_GE(distance + 0.01f, min_distance);
        }
    }
}

static MapPoint get_wall_asset_position(
    float map_size_x,
    int gx,
    int gy,
    const WallProfile& profile,
    const WallPartAsset& asset
) {
    MapPoint shift = WallBuilder::get_wall_shift(map_size_x, profile);
    MapPoint position = to_iso({gx, gy}, profile.step_x, profile.step_y, shift);
    position.x += asset.offset_x;
    position.y += asset.offset_y;
    return position;
}

static void expect_as2_set2_wall_directions_are_allowed(const std::vector<io::Sprite>& sprites) {
    for (const io::Sprite& sprite : sprites) {
        if (sprite.vid != 1700 && sprite.vid != 1701) {
            continue;
        }

        EXPECT_TRUE(is_as2_set2_random_wall_direction(sprite.direction));
    }
}

TEST(As2CeilingCurtainPreviewTest, WritesSet1ThreeCellRecessWithRegressionAssertions) {
    constexpr float map_size_x = 3000.0f;
    constexpr float map_size_y = 3000.0f;
    constexpr int wall_type = WALL_TYPE_AS2_WALL_SET1_FIXED_0;
    constexpr int recess_depth = 4;

    WallBuilder builder(map_size_x, map_size_y, false);
    std::vector<io::Sprite> sprites = builder.build(
        build_concave_room_segments(3, recess_depth, wall_type),
        true,
        true
    );

    int wide_curtain_count = count_sprites_by_vid_and_direction(
        sprites,
        CEILING_CURTAIN_AS2_SET1.vid,
        CEILING_CURTAIN_AS2_SET1.dir_a_wide.direction
    );
    wide_curtain_count += count_sprites_by_vid_and_direction(
        sprites,
        CEILING_CURTAIN_AS2_SET1.vid,
        CEILING_CURTAIN_AS2_SET1.dir_b_wide.direction
    );

    EXPECT_GT(count_sprites_by_vid(sprites, FLOOR_AS2_SET1.vid), 0);
    EXPECT_EQ(wide_curtain_count, 0);
    EXPECT_EQ(count_sprites_by_vid(sprites, CEILING_CURTAIN_AS2_SET1.vid), 55);
    EXPECT_TRUE(CEILING_CURTAIN_AS2_SET1.upper_recess.left_corner.keep_connector_long);
    EXPECT_FALSE(CEILING_CURTAIN_AS2_SET1.upper_recess.right_corner.keep_connector_long);

    io::Sprite left_corner_long = builder.place_single_ceiling_curtain(
        5,
        recess_depth,
        wall_type,
        WallPartKind::DirB,
        false,
        WallOutsideSide::NegativeGridSide
    );
    io::Sprite right_corner_long = builder.place_single_ceiling_curtain(
        7,
        recess_depth,
        wall_type,
        WallPartKind::DirB,
        false,
        WallOutsideSide::NegativeGridSide
    );
    EXPECT_TRUE(has_sprite_at(
        sprites,
        left_corner_long.vid,
        left_corner_long.posX,
        left_corner_long.posY,
        left_corner_long.direction
    ));
    EXPECT_FALSE(has_sprite_at(
        sprites,
        right_corner_long.vid,
        right_corner_long.posX,
        right_corner_long.posY,
        right_corner_long.direction
    ));

    std::string output_path = get_test_output_path(
        "celling/as2_set1_ceiling_concave_3.map"
    );
    ASSERT_TRUE(io::write_map(
        sprites,
        output_path,
        io::MapFormat::AS2R,
        map_size_x,
        map_size_y
    ));
}

TEST(As2CeilingCurtainPreviewTest, WritesManualValidationMapsForSet1ToSet9) {
    constexpr int wall_types[] = {
        WALL_TYPE_AS2_WALL_SET1_FIXED_0,
        WALL_TYPE_AS2_WALL_SET2_RANDOM,
        WALL_TYPE_AS2_WALL_SET3_RANDOM,
        WALL_TYPE_AS2_WALL_SET4_RANDOM,
        WALL_TYPE_AS2_WALL_SET5_RANDOM,
        WALL_TYPE_AS2_WALL_SET6_RANDOM,
        WALL_TYPE_AS2_WALL_SET7_RANDOM,
        WALL_TYPE_AS2_WALL_SET8_RANDOM,
        WALL_TYPE_AS2_WALL_SET9_RANDOM
    };

    // These maps intentionally have no visual assertions. They reproduce the
    // four-quadrant manual Ceiling Curtain calibration scene above.
    for (int set_index = 0; set_index < 9; ++set_index) {
        int set_number = set_index + 1;
        int wall_type = wall_types[set_index];
        TestScene scene = build_project_reference_wall_scene(wall_type);
        ASSERT_FALSE(scene.segments.empty());

        WallBuilder builder(scene.map_size_x, scene.map_size_y, false);
        std::vector<io::Sprite> sprites = builder.build(
            scene.segments,
            true,
            true
        );

        std::string output_path = get_test_output_path(
            "celling/as2_set" + std::to_string(set_number) +
            "_ceiling_manual_validation.map"
        );
        io::write_map(
            sprites,
            output_path,
            io::MapFormat::AS2R,
            scene.map_size_x,
            scene.map_size_y
        );
    }
}

TEST(As2CeilingCurtainGeometryTest, OppositeRoomSidesUseOppositeExteriorOffsets) {
    constexpr int wall_type = WALL_TYPE_AS2_WALL_SET1_FIXED_0;
    WallBuilder builder(3000.0f, 3000.0f, false);
    const WallProfile& wall_profile = WallBuilder::get_wall_profile(wall_type);

    io::Sprite dir_a_wall = builder.place_single_wall(0, 1, wall_type, WallPartKind::DirA);
    io::Sprite dir_a_long_negative = builder.place_single_ceiling_curtain(
        0,
        1,
        wall_type,
        WallPartKind::DirA,
        false,
        WallOutsideSide::NegativeGridSide
    );
    io::Sprite dir_a_long_positive = builder.place_single_ceiling_curtain(
        0,
        1,
        wall_type,
        WallPartKind::DirA,
        false,
        WallOutsideSide::PositiveGridSide
    );
    io::Sprite dir_a_wide_negative = builder.place_single_ceiling_curtain(
        0,
        1,
        wall_type,
        WallPartKind::DirA,
        true,
        WallOutsideSide::NegativeGridSide
    );
    io::Sprite dir_a_wide_positive = builder.place_single_ceiling_curtain(
        0,
        1,
        wall_type,
        WallPartKind::DirA,
        true,
        WallOutsideSide::PositiveGridSide
    );

    float dir_a_long_negative_grid_x = (
        (dir_a_long_negative.posX - dir_a_wall.posX) / wall_profile.step_x
        + (dir_a_long_negative.posY - dir_a_wall.posY) / wall_profile.step_y
    ) / 2.0f;
    float dir_a_long_positive_grid_x = (
        (dir_a_long_positive.posX - dir_a_wall.posX) / wall_profile.step_x
        + (dir_a_long_positive.posY - dir_a_wall.posY) / wall_profile.step_y
    ) / 2.0f;
    float dir_a_wide_negative_grid_x = (
        (dir_a_wide_negative.posX - dir_a_wall.posX) / wall_profile.step_x
        + (dir_a_wide_negative.posY - dir_a_wall.posY) / wall_profile.step_y
    ) / 2.0f;
    float dir_a_wide_positive_grid_x = (
        (dir_a_wide_positive.posX - dir_a_wall.posX) / wall_profile.step_x
        + (dir_a_wide_positive.posY - dir_a_wall.posY) / wall_profile.step_y
    ) / 2.0f;

    EXPECT_LT(dir_a_long_negative_grid_x, 0.0f);
    EXPECT_GT(dir_a_long_positive_grid_x, 0.0f);
    // EXPECT_NEAR(
    //     dir_a_long_positive_grid_x,
    //     -dir_a_long_negative_grid_x
    //         + CEILING_CURTAIN_AS2_SET1.dir_a_long.positive_side_outward_adjustment,
    //     0.001f
    // );
    EXPECT_LT(dir_a_wide_negative_grid_x, 0.0f);
    EXPECT_GT(dir_a_wide_positive_grid_x, 0.0f);
    // EXPECT_NEAR(
    //     -dir_a_wide_negative_grid_x,
    //     dir_a_wide_positive_grid_x
    //         + CEILING_CURTAIN_AS2_SET1.dir_a_wide.negative_side_outward_adjustment,
    //     0.001f
    // );

    io::Sprite dir_b_wall = builder.place_single_wall(1, 0, wall_type, WallPartKind::DirB);
    io::Sprite dir_b_long_negative = builder.place_single_ceiling_curtain(
        1,
        0,
        wall_type,
        WallPartKind::DirB,
        false,
        WallOutsideSide::NegativeGridSide
    );
    io::Sprite dir_b_long_positive = builder.place_single_ceiling_curtain(
        1,
        0,
        wall_type,
        WallPartKind::DirB,
        false,
        WallOutsideSide::PositiveGridSide
    );
    io::Sprite dir_b_wide_negative = builder.place_single_ceiling_curtain(
        1,
        0,
        wall_type,
        WallPartKind::DirB,
        true,
        WallOutsideSide::NegativeGridSide
    );
    io::Sprite dir_b_wide_positive = builder.place_single_ceiling_curtain(
        1,
        0,
        wall_type,
        WallPartKind::DirB,
        true,
        WallOutsideSide::PositiveGridSide
    );

    float dir_b_long_negative_grid_y = (
        (dir_b_long_negative.posY - dir_b_wall.posY) / wall_profile.step_y
        - (dir_b_long_negative.posX - dir_b_wall.posX) / wall_profile.step_x
    ) / 2.0f;
    float dir_b_long_positive_grid_y = (
        (dir_b_long_positive.posY - dir_b_wall.posY) / wall_profile.step_y
        - (dir_b_long_positive.posX - dir_b_wall.posX) / wall_profile.step_x
    ) / 2.0f;
    float dir_b_wide_negative_grid_y = (
        (dir_b_wide_negative.posY - dir_b_wall.posY) / wall_profile.step_y
        - (dir_b_wide_negative.posX - dir_b_wall.posX) / wall_profile.step_x
    ) / 2.0f;
    float dir_b_wide_positive_grid_y = (
        (dir_b_wide_positive.posY - dir_b_wall.posY) / wall_profile.step_y
        - (dir_b_wide_positive.posX - dir_b_wall.posX) / wall_profile.step_x
    ) / 2.0f;

    EXPECT_LT(dir_b_long_negative_grid_y, 0.0f);
    EXPECT_GT(dir_b_long_positive_grid_y, 0.0f);
    EXPECT_LT(dir_b_wide_negative_grid_y, 0.0f);
    EXPECT_GT(dir_b_wide_positive_grid_y, 0.0f);
}

TEST(WallBuilderTest, DisabledAs1DirectionsUseZeroForWallsPillarsAndFloors) {
    const int wall_types[] = {
        WALL_TYPE_STANDARD,
        WALL_TYPE_LAB,
        WALL_TYPE_STANDARD_DARK
    };
    const int floor_vids[] = {
        FLOOR_STANDARD.vid,
        FLOOR_LAB.vid,
        FLOOR_STANDARD_DARK.vid
    };

    for (int index = 0; index < 3; ++index) {
        WallBuilder builder(2000.0f, 2000.0f, false);
        const WallProfile& profile = WallBuilder::get_wall_profile(wall_types[index]);
        std::vector<io::Sprite> sprites = builder.build(
            build_room_segments(8, wall_types[index]),
            true,
            false
        );

        for (const io::Sprite& sprite : sprites) {
            bool is_wall = sprite.vid == profile.dir_a_vid || sprite.vid == profile.dir_b_vid;
            bool is_pillar = sprite.vid == profile.pillar_vid;
            bool is_floor = sprite.vid == floor_vids[index];

            if (is_wall || is_pillar || is_floor) {
                EXPECT_EQ(sprite.direction, 0u);
            }
        }
    }
}

TEST(WallBuilderTest, EnabledAs1DirectionsRandomizeEachEligibleSprite) {
    const int wall_types[] = {
        WALL_TYPE_STANDARD,
        WALL_TYPE_LAB,
        WALL_TYPE_STANDARD_DARK
    };
    const int floor_vids[] = {
        FLOOR_STANDARD.vid,
        FLOOR_LAB.vid,
        FLOOR_STANDARD_DARK.vid
    };

    for (int index = 0; index < 3; ++index) {
        WallBuilder builder(2000.0f, 2000.0f, true);
        const WallProfile& profile = WallBuilder::get_wall_profile(wall_types[index]);
        std::vector<io::Sprite> sprites = builder.build(
            build_room_segments(12, wall_types[index]),
            true,
            false
        );
        std::set<uint32_t> directions;

        for (const io::Sprite& sprite : sprites) {
            bool is_wall = sprite.vid == profile.dir_a_vid || sprite.vid == profile.dir_b_vid;
            bool is_pillar = sprite.vid == profile.pillar_vid;
            bool is_floor = sprite.vid == floor_vids[index];

            if (is_wall || is_pillar || is_floor) {
                directions.insert(sprite.direction);
                EXPECT_LE(sprite.direction, 255u);
            }
        }

        EXPECT_GT(directions.size(), 1u);
    }
}

TEST(WallBuilderTest, DisabledAs2DirectionsUseDefaultsAndSuppressRareWalls) {
    WallBuilder builder(3000.0f, 3000.0f, false);

    std::vector<io::Sprite> set1_sprites = builder.build(
        build_room_segments(12, WALL_TYPE_AS2_WALL_SET1_RANDOM),
        true,
        false
    );
    EXPECT_EQ(count_sprites_by_vid(set1_sprites, 1781), count_sprites_by_vid_and_direction(set1_sprites, 1781, 102u));
    EXPECT_EQ(count_sprites_by_vid(set1_sprites, 1780), count_sprites_by_vid_and_direction(set1_sprites, 1780, 102u));
    EXPECT_EQ(count_sprites_by_vid(set1_sprites, 1782), count_sprites_by_vid_and_direction(set1_sprites, 1782, 170u));
    EXPECT_EQ(count_sprites_by_vid(set1_sprites, 1783), count_sprites_by_vid_and_direction(set1_sprites, 1783, 0u));

    std::vector<io::Sprite> set3_sprites = builder.build(
        build_room_segments(20, WALL_TYPE_AS2_WALL_SET3_RANDOM),
        true,
        false
    );
    EXPECT_EQ(count_sprites_by_vid(set3_sprites, 1100), count_sprites_by_vid_and_direction(set3_sprites, 1100, 0u));
    EXPECT_EQ(count_sprites_by_vid(set3_sprites, 1101), count_sprites_by_vid_and_direction(set3_sprites, 1101, 0u));
    EXPECT_EQ(count_sprites_by_vid(set3_sprites, 1102), count_sprites_by_vid_and_direction(set3_sprites, 1102, 0u));
    EXPECT_EQ(count_sprites_by_vid(set3_sprites, 1121), count_sprites_by_vid_and_direction(set3_sprites, 1121, 0u));

    std::vector<io::Sprite> set7_sprites = builder.build(
        build_room_segments(20, WALL_TYPE_AS2_WALL_SET7_RANDOM),
        true,
        false
    );
    EXPECT_EQ(count_sprites_by_vid(set7_sprites, 2621), count_sprites_by_vid_and_direction(set7_sprites, 2621, 51u));
    EXPECT_EQ(count_sprites_by_vid(set7_sprites, 2620), count_sprites_by_vid_and_direction(set7_sprites, 2620, 51u));
    EXPECT_EQ(count_sprites_by_vid(set7_sprites, 2622), count_sprites_by_vid_and_direction(set7_sprites, 2622, 204u));
    EXPECT_EQ(count_sprites_by_vid(set7_sprites, 2643), count_sprites_by_vid_and_direction(set7_sprites, 2643, 0u));
}

TEST(WallBuilderTest, As2WallSet1FixedProfilesUseFixedDirections) {
    WallBuilder builder(2000.0f, 2000.0f, true);

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

TEST(WallBuilderTest, As2WallSet2BuildsRoomWithDirectionalPillarSlices) {
    WallBuilder builder(2000.0f, 2000.0f, true);

    std::vector<io::Sprite> sprites = builder.build(
        build_room_segments(3, WALL_TYPE_AS2_WALL_SET2_RANDOM),
        false,
        false
    );

    EXPECT_EQ(sprites.size(), 20u);
    EXPECT_EQ(count_sprites_by_vid(sprites, 1701), 6);
    EXPECT_EQ(count_sprites_by_vid(sprites, 1700), 6);
    EXPECT_EQ(count_sprites_by_vid(sprites, 1702), 8);

    EXPECT_EQ(count_sprites_by_vid_and_direction(sprites, 1702, 0u), 2);
    EXPECT_EQ(count_sprites_by_vid_and_direction(sprites, 1702, 64u), 2);
    EXPECT_EQ(count_sprites_by_vid_and_direction(sprites, 1702, 128u), 2);
    EXPECT_EQ(count_sprites_by_vid_and_direction(sprites, 1702, 192u), 2);

    expect_as2_set2_wall_directions_are_allowed(sprites);
}

TEST(WallBuilderTest, As2WallSet2BuildsOpenLShapeWithEndpointAndCornerSlices) {
    WallBuilder builder(2000.0f, 2000.0f, true);

    std::vector<Segment> segments = {
        {{0, 0}, {3, 0}, WALL_TYPE_AS2_WALL_SET2_RANDOM},
        {{3, 0}, {3, 3}, WALL_TYPE_AS2_WALL_SET2_RANDOM},
    };

    std::vector<io::Sprite> sprites = builder.build(segments, false, false);

    EXPECT_EQ(sprites.size(), 10u);
    EXPECT_EQ(count_sprites_by_vid(sprites, 1701), 3);
    EXPECT_EQ(count_sprites_by_vid(sprites, 1700), 3);
    EXPECT_EQ(count_sprites_by_vid(sprites, 1702), 4);

    EXPECT_EQ(count_sprites_by_vid_and_direction(sprites, 1702, 0u), 1);
    EXPECT_EQ(count_sprites_by_vid_and_direction(sprites, 1702, 64u), 1);
    EXPECT_EQ(count_sprites_by_vid_and_direction(sprites, 1702, 128u), 1);
    EXPECT_EQ(count_sprites_by_vid_and_direction(sprites, 1702, 192u), 1);

    expect_as2_set2_wall_directions_are_allowed(sprites);
}

TEST(WallBuilderTest, As2WallSet2BuildsCrossWithFourCenterSlices) {
    WallBuilder builder(2000.0f, 2000.0f, true);

    std::vector<Segment> segments = {
        {{-2, 0}, {2, 0}, WALL_TYPE_AS2_WALL_SET2_RANDOM},
        {{0, -2}, {0, 2}, WALL_TYPE_AS2_WALL_SET2_RANDOM},
    };

    std::vector<io::Sprite> sprites = builder.build(segments, false, false);

    EXPECT_EQ(sprites.size(), 16u);
    EXPECT_EQ(count_sprites_by_vid(sprites, 1701), 4);
    EXPECT_EQ(count_sprites_by_vid(sprites, 1700), 4);
    EXPECT_EQ(count_sprites_by_vid(sprites, 1702), 8);

    EXPECT_EQ(count_sprites_by_vid_and_direction(sprites, 1702, 0u), 2);
    EXPECT_EQ(count_sprites_by_vid_and_direction(sprites, 1702, 64u), 2);
    EXPECT_EQ(count_sprites_by_vid_and_direction(sprites, 1702, 128u), 2);
    EXPECT_EQ(count_sprites_by_vid_and_direction(sprites, 1702, 192u), 2);

    expect_as2_set2_wall_directions_are_allowed(sprites);
}

TEST(WallBuilderTest, As2WallSet2WritesManualPreviewMap) {
    WallBuilder builder(2000.0f, 2000.0f, true);

    std::vector<Segment> segments;
    std::vector<Segment> room_segments = build_room_segments_at(0, 0, 3, WALL_TYPE_AS2_WALL_SET2_RANDOM);
    std::vector<Segment> l_shape_segments = {
        {{6, 0}, {9, 0}, WALL_TYPE_AS2_WALL_SET2_RANDOM},
        {{9, 0}, {9, 3}, WALL_TYPE_AS2_WALL_SET2_RANDOM},
    };
    std::vector<Segment> cross_segments = {
        {{0, 8}, {4, 8}, WALL_TYPE_AS2_WALL_SET2_RANDOM},
        {{2, 6}, {2, 10}, WALL_TYPE_AS2_WALL_SET2_RANDOM},
    };

    segments.insert(segments.end(), room_segments.begin(), room_segments.end());
    segments.insert(segments.end(), l_shape_segments.begin(), l_shape_segments.end());
    segments.insert(segments.end(), cross_segments.begin(), cross_segments.end());

    std::vector<io::Sprite> sprites = builder.build(segments, false, false);
    std::string preview_map_path = get_test_output_path("wall/as2_wall_set2_directional_pillars.map");
    bool write_success = io::write_map(
        sprites,
        preview_map_path,
        io::MapFormat::AS2R,
        2000.0f,
        2000.0f
    );

    ASSERT_TRUE(write_success);
    EXPECT_GT(sprites.size(), 0u);
    EXPECT_EQ(count_sprites_by_vid(sprites, 1702), 20);
}

TEST(WallBuilderTest, As2WallSet3BuildsLargeRoomWithBoundedRareVariants) {
    WallBuilder builder(2000.0f, 2000.0f, true);
    int room_size = 40;

    io::Sprite dir_a_sample = builder.place_single_wall(0, 0, WALL_TYPE_AS2_WALL_SET3_RANDOM, WallPartKind::DirA);
    io::Sprite dir_b_sample = builder.place_single_wall(0, 0, WALL_TYPE_AS2_WALL_SET3_RANDOM, WallPartKind::DirB);
    io::Sprite pillar_sample = builder.place_single_wall(0, 0, WALL_TYPE_AS2_WALL_SET3_RANDOM, WallPartKind::Pillar);

    EXPECT_EQ(WALL_AS2_SET3_RANDOM.dir_a_vid, 1100);
    EXPECT_EQ(WALL_AS2_SET3_RANDOM.dir_b_vid, 1101);
    EXPECT_EQ(dir_a_sample.vid, 1100);
    EXPECT_EQ(dir_b_sample.vid, 1101);
    EXPECT_EQ(pillar_sample.vid, 1102);
    EXPECT_EQ(pillar_sample.direction, 0u);

    std::vector<io::Sprite> sprites = builder.build(
        build_room_segments(room_size, WALL_TYPE_AS2_WALL_SET3_RANDOM),
        false,
        false
    );

    int dir_a_count = count_sprites_by_vid(sprites, 1100);
    int dir_b_count = count_sprites_by_vid(sprites, 1101);
    int dir_a_rare_count = count_sprites_by_vid_and_direction(sprites, 1100, 153u);
    int dir_b_rare_count = count_sprites_by_vid_and_direction(sprites, 1101, 51u);

    EXPECT_EQ(sprites.size(), 164u);
    EXPECT_EQ(dir_a_count, room_size * 2);
    EXPECT_EQ(dir_b_count, room_size * 2);
    EXPECT_EQ(count_sprites_by_vid(sprites, 1102), 4);
    EXPECT_GE(dir_a_rare_count, 1);
    EXPECT_GE(dir_b_rare_count, 1);

    int max_dir_a_rare_count = static_cast<int>(std::floor(
        static_cast<float>(dir_a_count) * WALL_AS2_SET3_RANDOM.rare_target_density
    ));
    int max_dir_b_rare_count = static_cast<int>(std::floor(
        static_cast<float>(dir_b_count) * WALL_AS2_SET3_RANDOM.rare_target_density
    ));
    EXPECT_LE(dir_a_rare_count, max_dir_a_rare_count);
    EXPECT_LE(dir_b_rare_count, max_dir_b_rare_count);
    expect_rare_wall_min_distance(sprites, WALL_AS2_SET3_RANDOM);

    for (const io::Sprite& sprite : sprites) {
        if (sprite.vid == 1100) {
            EXPECT_TRUE(is_as2_set3_dir_a_direction(sprite.direction));
        }

        if (sprite.vid == 1101) {
            EXPECT_TRUE(is_as2_set3_dir_b_direction(sprite.direction));
        }

        if (sprite.vid == 1102) {
            EXPECT_EQ(sprite.direction, 0u);
        }
    }

    std::string preview_map_path = get_test_output_path("wall/as2_wall_set3_rare_variants.map");
    bool write_success = io::write_map(
        sprites,
        preview_map_path,
        io::MapFormat::AS2R,
        2000.0f,
        2000.0f
    );

    ASSERT_TRUE(write_success);
}

TEST(WallBuilderTest, As2WallSet4BuildsLargeRoomWithBoundedRareVariants) {
    WallBuilder builder(2000.0f, 2000.0f, true);
    int room_size = 40;

    io::Sprite dir_a_sample = builder.place_single_wall(0, 0, WALL_TYPE_AS2_WALL_SET4_RANDOM, WallPartKind::DirA);
    io::Sprite dir_b_sample = builder.place_single_wall(0, 0, WALL_TYPE_AS2_WALL_SET4_RANDOM, WallPartKind::DirB);
    io::Sprite pillar_sample = builder.place_single_wall(0, 0, WALL_TYPE_AS2_WALL_SET4_RANDOM, WallPartKind::Pillar);

    EXPECT_EQ(WALL_AS2_SET4_RANDOM.dir_a_vid, 1131);
    EXPECT_EQ(WALL_AS2_SET4_RANDOM.dir_b_vid, 1130);
    EXPECT_EQ(dir_a_sample.vid, 1131);
    EXPECT_EQ(dir_b_sample.vid, 1130);
    EXPECT_EQ(pillar_sample.vid, 1132);
    EXPECT_EQ(pillar_sample.direction, 51u);

    std::vector<io::Sprite> sprites = builder.build(
        build_room_segments(room_size, WALL_TYPE_AS2_WALL_SET4_RANDOM),
        false,
        false
    );

    int dir_a_count = count_sprites_by_vid(sprites, 1131);
    int dir_b_count = count_sprites_by_vid(sprites, 1130);
    int dir_a_rare_count = count_sprites_by_vid_and_direction(sprites, 1131, 153u);
    int dir_b_rare_count = count_sprites_by_vid_and_direction(sprites, 1130, 51u);

    EXPECT_EQ(sprites.size(), 164u);
    EXPECT_EQ(dir_a_count, room_size * 2);
    EXPECT_EQ(dir_b_count, room_size * 2);
    EXPECT_EQ(count_sprites_by_vid(sprites, 1132), 4);
    EXPECT_GE(dir_a_rare_count, 1);
    EXPECT_GE(dir_b_rare_count, 1);

    int max_dir_a_rare_count = static_cast<int>(std::floor(
        static_cast<float>(dir_a_count) * WALL_AS2_SET4_RANDOM.rare_target_density
    ));
    int max_dir_b_rare_count = static_cast<int>(std::floor(
        static_cast<float>(dir_b_count) * WALL_AS2_SET4_RANDOM.rare_target_density
    ));
    EXPECT_LE(dir_a_rare_count, max_dir_a_rare_count);
    EXPECT_LE(dir_b_rare_count, max_dir_b_rare_count);
    expect_rare_wall_min_distance(sprites, WALL_AS2_SET4_RANDOM);

    for (const io::Sprite& sprite : sprites) {
        if (sprite.vid == 1131) {
            EXPECT_TRUE(is_as2_set4_dir_a_direction(sprite.direction));
        }

        if (sprite.vid == 1130) {
            EXPECT_TRUE(is_as2_set4_dir_b_direction(sprite.direction));
        }

        if (sprite.vid == 1132) {
            EXPECT_EQ(sprite.direction, 51u);
        }
    }

    std::string preview_map_path = get_test_output_path("wall/as2_wall_set4_rare_variants.map");
    bool write_success = io::write_map(
        sprites,
        preview_map_path,
        io::MapFormat::AS2R,
        2000.0f,
        2000.0f
    );

    ASSERT_TRUE(write_success);
}

TEST(WallBuilderTest, As2RareWallsRequireDensityBudgetAndStraightRunBuffer) {
    WallBuilder builder(2000.0f, 2000.0f, true);

    std::vector<Segment> below_density_segments = {
        {{0, 0}, {5, 0}, WALL_TYPE_AS2_WALL_SET4_RANDOM}
    };
    std::vector<io::Sprite> below_density_sprites = builder.build(
        below_density_segments,
        false,
        false
    );
    EXPECT_EQ(get_rare_wall_sprites(below_density_sprites, WALL_AS2_SET4_RANDOM).size(), 0u);

    std::vector<Segment> too_short_segments = {
        {{0, 0}, {4, 0}, WALL_TYPE_AS2_WALL_SET5_RANDOM}
    };
    std::vector<io::Sprite> too_short_sprites = builder.build(
        too_short_segments,
        false,
        false
    );
    EXPECT_EQ(get_rare_wall_sprites(too_short_sprites, WALL_AS2_SET5_RANDOM).size(), 0u);

    std::vector<Segment> minimum_run_segments = {
        {{0, 0}, {5, 0}, WALL_TYPE_AS2_WALL_SET5_RANDOM}
    };
    std::vector<io::Sprite> minimum_run_sprites = builder.build(
        minimum_run_segments,
        false,
        false
    );
    std::vector<io::Sprite> minimum_run_rare_sprites = get_rare_wall_sprites(
        minimum_run_sprites,
        WALL_AS2_SET5_RANDOM
    );
    ASSERT_EQ(minimum_run_rare_sprites.size(), 1u);

    const WallVariant& rare_variant =
        WALL_AS2_SET5_RANDOM.variants[WALL_AS2_SET5_RANDOM.rare_variant_index];
    MapPoint expected_position = get_wall_asset_position(
        2000.0f,
        3,
        0,
        WALL_AS2_SET5_RANDOM,
        rare_variant.dir_b
    );
    EXPECT_TRUE(has_sprite_at(
        minimum_run_sprites,
        rare_variant.dir_b.vid,
        expected_position.x,
        expected_position.y,
        rare_variant.dir_b.direction
    ));
}

TEST(WallBuilderTest, As2RareWallsSharePhysicalExclusionAcrossDirections) {
    WallBuilder builder(2000.0f, 2000.0f, true);
    std::vector<Segment> segments = {
        {{0, 0}, {5, 0}, WALL_TYPE_AS2_WALL_SET5_RANDOM},
        {{5, 0}, {5, 5}, WALL_TYPE_AS2_WALL_SET5_RANDOM}
    };

    std::vector<io::Sprite> sprites = builder.build(segments, false, false);
    std::vector<io::Sprite> rare_sprites = get_rare_wall_sprites(
        sprites,
        WALL_AS2_SET5_RANDOM
    );

    // Both directions have one candidate and one budget slot, but their rare
    // anchors are closer than three wall steps, so only one may be selected.
    EXPECT_EQ(rare_sprites.size(), 1u);
}

TEST(WallBuilderTest, As2RareWallsTreatDoorExcavationAsStraightRunBoundary) {
    WallBuilder builder(2000.0f, 2000.0f, true);
    std::vector<Segment> segments = {
        {{0, 0}, {12, 0}, WALL_TYPE_AS2_WALL_SET5_RANDOM}
    };
    std::vector<DoorExcavation> excavations = {
        {{6, 0}, 1, 2, WALL_TYPE_AS2_WALL_SET5_RANDOM}
    };

    std::vector<io::Sprite> sprites = builder.build(
        segments,
        false,
        false,
        excavations
    );
    std::vector<io::Sprite> rare_sprites = get_rare_wall_sprites(
        sprites,
        WALL_AS2_SET5_RANDOM
    );
    ASSERT_EQ(rare_sprites.size(), 2u);

    const WallVariant& rare_variant =
        WALL_AS2_SET5_RANDOM.variants[WALL_AS2_SET5_RANDOM.rare_variant_index];
    MapPoint left_run_position = get_wall_asset_position(
        2000.0f,
        3,
        0,
        WALL_AS2_SET5_RANDOM,
        rare_variant.dir_b
    );
    MapPoint right_run_position = get_wall_asset_position(
        2000.0f,
        10,
        0,
        WALL_AS2_SET5_RANDOM,
        rare_variant.dir_b
    );

    EXPECT_TRUE(has_sprite_at(
        sprites,
        rare_variant.dir_b.vid,
        left_run_position.x,
        left_run_position.y,
        rare_variant.dir_b.direction
    ));
    EXPECT_TRUE(has_sprite_at(
        sprites,
        rare_variant.dir_b.vid,
        right_run_position.x,
        right_run_position.y,
        rare_variant.dir_b.direction
    ));
}

TEST(WallBuilderTest, As2RareWallsGiveEachStraightRunOneOpportunityPerRound) {
    WallBuilder builder(5000.0f, 5000.0f, true);
    std::vector<Segment> segments = {
        {{0, 0}, {8, 0}, WALL_TYPE_AS2_WALL_SET3_RANDOM},
        {{20, 20}, {28, 20}, WALL_TYPE_AS2_WALL_SET3_RANDOM}
    };

    std::vector<io::Sprite> sprites = builder.build(segments, false, false);
    std::vector<io::Sprite> rare_sprites = get_rare_wall_sprites(
        sprites,
        WALL_AS2_SET3_RANDOM
    );
    ASSERT_EQ(rare_sprites.size(), 2u);

    const WallVariant& rare_variant =
        WALL_AS2_SET3_RANDOM.variants[WALL_AS2_SET3_RANDOM.rare_variant_index];
    int first_run_rare_count = 0;
    int second_run_rare_count = 0;

    for (const io::Sprite& rare_sprite : rare_sprites) {
        for (int gx = 3; gx <= 6; ++gx) {
            MapPoint candidate_position = get_wall_asset_position(
                5000.0f,
                gx,
                0,
                WALL_AS2_SET3_RANDOM,
                rare_variant.dir_b
            );
            bool x_matches = std::abs(rare_sprite.posX - candidate_position.x) < 0.01f;
            bool y_matches = std::abs(rare_sprite.posY - candidate_position.y) < 0.01f;
            if (x_matches && y_matches) {
                first_run_rare_count += 1;
            }
        }

        for (int gx = 23; gx <= 26; ++gx) {
            MapPoint candidate_position = get_wall_asset_position(
                5000.0f,
                gx,
                20,
                WALL_AS2_SET3_RANDOM,
                rare_variant.dir_b
            );
            bool x_matches = std::abs(rare_sprite.posX - candidate_position.x) < 0.01f;
            bool y_matches = std::abs(rare_sprite.posY - candidate_position.y) < 0.01f;
            if (x_matches && y_matches) {
                second_run_rare_count += 1;
            }
        }
    }

    EXPECT_EQ(first_run_rare_count, 1);
    EXPECT_EQ(second_run_rare_count, 1);
}

TEST(WallBuilderTest, As2RareWallsDoNotSharePhysicalExclusionAcrossWallSets) {
    WallBuilder builder(2000.0f, 2000.0f, true);
    std::vector<Segment> segments = {
        {{0, 0}, {5, 0}, WALL_TYPE_AS2_WALL_SET5_RANDOM},
        {{0, 0}, {5, 0}, WALL_TYPE_AS2_WALL_SET7_RANDOM}
    };

    std::vector<io::Sprite> sprites = builder.build(segments, false, false);
    EXPECT_EQ(get_rare_wall_sprites(sprites, WALL_AS2_SET5_RANDOM).size(), 1u);
    EXPECT_EQ(get_rare_wall_sprites(sprites, WALL_AS2_SET7_RANDOM).size(), 1u);
}

TEST(WallBuilderTest, As2WallSet5BuildsRoomWithRareDensityAndCornerPillars) {
    WallBuilder builder(2000.0f, 2000.0f, true);
    int room_size = 9;

    io::Sprite dir_a_sample = builder.place_single_wall(0, 0, WALL_TYPE_AS2_WALL_SET5_RANDOM, WallPartKind::DirA);
    io::Sprite dir_b_sample = builder.place_single_wall(0, 0, WALL_TYPE_AS2_WALL_SET5_RANDOM, WallPartKind::DirB);

    EXPECT_EQ(WALL_AS2_SET5_RANDOM.dir_a_vid, 2501);
    EXPECT_EQ(WALL_AS2_SET5_RANDOM.dir_b_vid, 2500);
    EXPECT_EQ(dir_a_sample.vid, 2501);
    EXPECT_EQ(dir_b_sample.vid, 2500);

    std::vector<Segment> room_segments = build_room_segments(room_size, WALL_TYPE_AS2_WALL_SET5_RANDOM);
    std::vector<io::Sprite> room_sprites = builder.build(
        room_segments,
        false,
        false
    );

    int dir_a_count = count_sprites_by_vid(room_sprites, 2501);
    int dir_b_count = count_sprites_by_vid(room_sprites, 2500);
    int dir_a_rare_count = count_sprites_by_vid_and_direction(room_sprites, 2501, 51u);
    int dir_b_rare_count = count_sprites_by_vid_and_direction(room_sprites, 2500, 51u);

    EXPECT_EQ(room_sprites.size(), 40u);
    EXPECT_EQ(dir_a_count, room_size * 2);
    EXPECT_EQ(dir_b_count, room_size * 2);
    int max_dir_a_rare_count = static_cast<int>(std::floor(
        static_cast<float>(dir_a_count) * WALL_AS2_SET5_RANDOM.rare_target_density
    ));
    int max_dir_b_rare_count = static_cast<int>(std::floor(
        static_cast<float>(dir_b_count) * WALL_AS2_SET5_RANDOM.rare_target_density
    ));
    EXPECT_GE(dir_a_rare_count, 1);
    EXPECT_GE(dir_b_rare_count, 1);
    EXPECT_LE(dir_a_rare_count, max_dir_a_rare_count);
    EXPECT_LE(dir_b_rare_count, max_dir_b_rare_count);
    expect_rare_wall_min_distance(room_sprites, WALL_AS2_SET5_RANDOM);
    EXPECT_EQ(count_sprites_by_vid(room_sprites, 2502), 4);
    EXPECT_EQ(count_sprites_by_vid_and_direction(room_sprites, 2502, 0u), 1);
    EXPECT_EQ(count_sprites_by_vid_and_direction(room_sprites, 2502, 51u), 1);
    EXPECT_EQ(count_sprites_by_vid_and_direction(room_sprites, 2502, 128u), 1);
    EXPECT_EQ(count_sprites_by_vid_and_direction(room_sprites, 2502, 192u), 1);

    std::vector<io::Sprite> preview_sprites = room_sprites;

    for (const io::Sprite& sprite : preview_sprites) {
        if (sprite.vid == 2500 || sprite.vid == 2501) {
            EXPECT_TRUE(is_as2_set5_wall_direction(sprite.direction));
        }
    }

    std::string preview_map_path = get_test_output_path("wall/as2_wall_set5_fixed_rare_variants.map");
    bool write_success = io::write_map(
        preview_sprites,
        preview_map_path,
        io::MapFormat::AS2R,
        2000.0f,
        2000.0f
    );

    ASSERT_TRUE(write_success);
}

TEST(WallBuilderTest, As2WallSet6BuildsRoomWithRandomDirectionsAndSingleCornerPillar) {
    WallBuilder builder(2000.0f, 2000.0f, true);
    int room_size = 9;

    io::Sprite dir_a_sample = builder.place_single_wall(0, 0, WALL_TYPE_AS2_WALL_SET6_RANDOM, WallPartKind::DirA);
    io::Sprite dir_b_sample = builder.place_single_wall(0, 0, WALL_TYPE_AS2_WALL_SET6_RANDOM, WallPartKind::DirB);

    EXPECT_EQ(WALL_AS2_SET6_RANDOM.dir_a_vid, 2601);
    EXPECT_EQ(WALL_AS2_SET6_RANDOM.dir_b_vid, 2600);
    EXPECT_EQ(dir_a_sample.vid, 2601);
    EXPECT_EQ(dir_b_sample.vid, 2600);

    std::vector<Segment> room_segments = build_room_segments(room_size, WALL_TYPE_AS2_WALL_SET6_RANDOM);
    std::vector<io::Sprite> room_sprites = builder.build(
        room_segments,
        false,
        false
    );

    int dir_a_count = count_sprites_by_vid(room_sprites, 2601);
    int dir_b_count = count_sprites_by_vid(room_sprites, 2600);

    EXPECT_EQ(room_sprites.size(), 40u);
    EXPECT_EQ(dir_a_count, room_size * 2);
    EXPECT_EQ(dir_b_count, room_size * 2);
    EXPECT_EQ(count_sprites_by_vid(room_sprites, 2602), 4);
    EXPECT_EQ(count_sprites_by_vid_and_direction(room_sprites, 2602, 0u), 1);
    std::vector<io::Sprite> preview_sprites = room_sprites;

    for (const io::Sprite& sprite : preview_sprites) {
        if (sprite.vid == 2600 || sprite.vid == 2601) {
            EXPECT_TRUE(is_as2_set6_wall_direction(sprite.direction));
        }
    }

    std::string preview_map_path = get_test_output_path("wall/as2_wall_set6_random_variants.map");
    bool write_success = io::write_map(
        preview_sprites,
        preview_map_path,
        io::MapFormat::AS2R,
        2000.0f,
        2000.0f
    );

    ASSERT_TRUE(write_success);
}

TEST(WallBuilderTest, As2WallSet7BuildsRoomWithRareDensityAndFixedPillars) {
    WallBuilder builder(2000.0f, 2000.0f, true);
    int room_size = 9;

    io::Sprite dir_a_sample = builder.place_single_wall(0, 0, WALL_TYPE_AS2_WALL_SET7_RANDOM, WallPartKind::DirA);
    io::Sprite dir_b_sample = builder.place_single_wall(0, 0, WALL_TYPE_AS2_WALL_SET7_RANDOM, WallPartKind::DirB);
    io::Sprite pillar_sample = builder.place_single_wall(0, 0, WALL_TYPE_AS2_WALL_SET7_RANDOM, WallPartKind::Pillar);

    EXPECT_EQ(WALL_AS2_SET7_RANDOM.dir_a_vid, 2621);
    EXPECT_EQ(WALL_AS2_SET7_RANDOM.dir_b_vid, 2620);
    EXPECT_EQ(WALL_AS2_SET7_RANDOM.pillar_vid, 2622);
    EXPECT_FLOAT_EQ(WALL_AS2_SET7_RANDOM.offset_p_x, -0.5f);
    EXPECT_FLOAT_EQ(WALL_AS2_SET7_RANDOM.offset_p_y, 64.0f);
    EXPECT_EQ(dir_a_sample.vid, 2621);
    EXPECT_EQ(dir_b_sample.vid, 2620);
    EXPECT_EQ(pillar_sample.vid, 2622);
    EXPECT_EQ(pillar_sample.direction, 204u);

    std::vector<io::Sprite> sprites = builder.build(
        build_room_segments(room_size, WALL_TYPE_AS2_WALL_SET7_RANDOM),
        false,
        false
    );

    int dir_a_count = count_sprites_by_vid(sprites, 2621);
    int dir_b_count = count_sprites_by_vid(sprites, 2620);
    int dir_a_rare_count = count_sprites_by_vid_and_direction(sprites, 2621, 0u);
    int dir_b_rare_count = count_sprites_by_vid_and_direction(sprites, 2620, 0u);

    EXPECT_EQ(sprites.size(), 40u);
    EXPECT_EQ(dir_a_count, room_size * 2);
    EXPECT_EQ(dir_b_count, room_size * 2);
    int max_dir_a_rare_count = static_cast<int>(std::floor(
        static_cast<float>(dir_a_count) * WALL_AS2_SET7_RANDOM.rare_target_density
    ));
    int max_dir_b_rare_count = static_cast<int>(std::floor(
        static_cast<float>(dir_b_count) * WALL_AS2_SET7_RANDOM.rare_target_density
    ));
    EXPECT_GE(dir_a_rare_count, 1);
    EXPECT_GE(dir_b_rare_count, 1);
    EXPECT_LE(dir_a_rare_count, max_dir_a_rare_count);
    EXPECT_LE(dir_b_rare_count, max_dir_b_rare_count);
    expect_rare_wall_min_distance(sprites, WALL_AS2_SET7_RANDOM);
    EXPECT_EQ(count_sprites_by_vid(sprites, 2622), 4);
    EXPECT_EQ(count_sprites_by_vid_and_direction(sprites, 2622, 204u), 4);
    EXPECT_TRUE(has_sprite_at(sprites, 2622, 1012.0f, 144.0f, 204u));
    EXPECT_TRUE(has_sprite_at(sprites, 2622, 202.0f, 720.0f, 204u));
    EXPECT_TRUE(has_sprite_at(sprites, 2622, 1822.0f, 720.0f, 204u));
    EXPECT_TRUE(has_sprite_at(sprites, 2622, 1012.0f, 1296.0f, 204u));

    for (const io::Sprite& sprite : sprites) {
        if (sprite.vid == 2620 || sprite.vid == 2621) {
            EXPECT_TRUE(is_as2_set7_wall_direction(sprite.direction));
        }
    }

    std::string preview_map_path = get_test_output_path("wall/as2_wall_set7_fixed_rare_variants.map");
    bool write_success = io::write_map(
        sprites,
        preview_map_path,
        io::MapFormat::AS2R,
        2000.0f,
        2000.0f
    );

    ASSERT_TRUE(write_success);
}

TEST(WallBuilderTest, As2WallSet8BuildsRoomWithRandomDirectionsAndPerCornerPillars) {
    WallBuilder builder(2000.0f, 2000.0f, true);
    int room_size = 9;

    io::Sprite dir_a_sample = builder.place_single_wall(0, 0, WALL_TYPE_AS2_WALL_SET8_RANDOM, WallPartKind::DirA);
    io::Sprite dir_b_sample = builder.place_single_wall(0, 0, WALL_TYPE_AS2_WALL_SET8_RANDOM, WallPartKind::DirB);

    EXPECT_EQ(WALL_AS2_SET8_RANDOM.dir_a_vid, 2641);
    EXPECT_EQ(WALL_AS2_SET8_RANDOM.dir_b_vid, 2640);
    EXPECT_EQ(WALL_AS2_SET8_RANDOM.pillar_vid, 2642);
    EXPECT_EQ(dir_a_sample.vid, 2641);
    EXPECT_EQ(dir_b_sample.vid, 2640);

    std::vector<io::Sprite> sprites = builder.build(
        build_room_segments(room_size, WALL_TYPE_AS2_WALL_SET8_RANDOM),
        false,
        false
    );

    int dir_a_count = count_sprites_by_vid(sprites, 2641);
    int dir_b_count = count_sprites_by_vid(sprites, 2640);

    EXPECT_EQ(sprites.size(), 40u);
    EXPECT_EQ(dir_a_count, room_size * 2);
    EXPECT_EQ(dir_b_count, room_size * 2);
    EXPECT_EQ(count_sprites_by_vid(sprites, 2642), 4);
    EXPECT_EQ(count_sprites_by_vid_and_direction(sprites, 2642, 0u), 1);
    EXPECT_EQ(count_sprites_by_vid_and_direction(sprites, 2642, 64u), 1);
    EXPECT_EQ(count_sprites_by_vid_and_direction(sprites, 2642, 128u), 1);
    EXPECT_EQ(count_sprites_by_vid_and_direction(sprites, 2642, 192u), 1);
    EXPECT_TRUE(has_sprite_at(sprites, 2642, 1094.5f, 155.0f, 128u));
    EXPECT_TRUE(has_sprite_at(sprites, 2642, 1817.5f, 724.0f, 64u));
    EXPECT_TRUE(has_sprite_at(sprites, 2642, 1018.5f, 1236.0f, 0u));
    EXPECT_TRUE(has_sprite_at(sprites, 2642, 297.5f, 668.0f, 192u));

    for (const io::Sprite& sprite : sprites) {
        if (sprite.vid == 2640 || sprite.vid == 2641) {
            EXPECT_TRUE(is_as2_set8_wall_direction(sprite.direction));
        }
    }

    std::string preview_map_path = get_test_output_path("wall/as2_wall_set8_random_variants.map");
    bool write_success = io::write_map(
        sprites,
        preview_map_path,
        io::MapFormat::AS2R,
        2000.0f,
        2000.0f
    );

    ASSERT_TRUE(write_success);
}

TEST(WallBuilderTest, As2WallSet9BuildsRoomWithCornerWallDirectionsAndNoPillars) {
    WallBuilder builder(2000.0f, 2000.0f, true);
    int room_size = 9;

    io::Sprite dir_a_sample = builder.place_single_wall(0, 0, WALL_TYPE_AS2_WALL_SET9_RANDOM, WallPartKind::DirA);
    io::Sprite dir_b_sample = builder.place_single_wall(0, 0, WALL_TYPE_AS2_WALL_SET9_RANDOM, WallPartKind::DirB);

    EXPECT_EQ(WALL_AS2_SET9_RANDOM.dir_a_vid, 1721);
    EXPECT_EQ(WALL_AS2_SET9_RANDOM.dir_b_vid, 1720);
    EXPECT_EQ(WALL_AS2_SET9_RANDOM.pillar_vid, 0);
    EXPECT_EQ(WALL_AS2_SET9_RANDOM.corner_wall_variant_index, 0);
    EXPECT_EQ(dir_a_sample.vid, 1721);
    EXPECT_EQ(dir_b_sample.vid, 1720);

    std::vector<io::Sprite> sprites = builder.build(
        build_room_segments(room_size, WALL_TYPE_AS2_WALL_SET9_RANDOM),
        false,
        false
    );

    int dir_a_count = count_sprites_by_vid(sprites, 1721);
    int dir_b_count = count_sprites_by_vid(sprites, 1720);

    EXPECT_EQ(sprites.size(), 36u);
    EXPECT_EQ(dir_a_count, room_size * 2);
    EXPECT_EQ(dir_b_count, room_size * 2);

    EXPECT_TRUE(has_sprite_at(sprites, 1721, 1012.5f, 144.0f, 0u));
    EXPECT_TRUE(has_sprite_at(sprites, 1721, 292.5f, 656.0f, 0u));
    EXPECT_TRUE(has_sprite_at(sprites, 1721, 1822.5f, 720.0f, 0u));
    EXPECT_TRUE(has_sprite_at(sprites, 1721, 1102.5f, 1232.0f, 0u));
    EXPECT_TRUE(has_sprite_at(sprites, 1720, 1102.5f, 144.0f, 0u));
    EXPECT_TRUE(has_sprite_at(sprites, 1720, 1822.5f, 656.0f, 0u));
    EXPECT_TRUE(has_sprite_at(sprites, 1720, 292.5f, 720.0f, 0u));
    EXPECT_TRUE(has_sprite_at(sprites, 1720, 1012.5f, 1232.0f, 0u));

    for (const io::Sprite& sprite : sprites) {
        EXPECT_NE(sprite.vid, 0);
        if (sprite.vid == 1720 || sprite.vid == 1721) {
            EXPECT_TRUE(is_as2_set9_wall_direction(sprite.direction));
        }
    }

    std::string preview_map_path = get_test_output_path("wall/as2_wall_set9_corner_wall_variants.map");
    bool write_success = io::write_map(
        sprites,
        preview_map_path,
        io::MapFormat::AS2R,
        2000.0f,
        2000.0f
    );

    ASSERT_TRUE(write_success);
}

TEST(WallBuilderTest, As2WallSet1RandomProfileUsesAllowedVariantPool) {
    WallBuilder builder(2000.0f, 2000.0f, true);

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
    WallBuilder builder(2000.0f, 2000.0f, true);

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
    std::string preview_map_path = get_test_output_path("wall/as2_wall_set1_variants.map");
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
 * Generates ceiling-only AS1 maps from the manually arranged UI fixtures.
 * This test intentionally has no assertions because seams are checked in-game.
 */
TEST(AS1CeilingPreviewTest, WritesManualStandardAndLabMaps) {
    const std::string json_paths[] = {
        "tests/golden/manual_ui_test/AS1_celling_standard.json",
        "tests/golden/manual_ui_test/AS1_celling_lab.json"
    };
    const std::string output_paths[] = {
        "AS1/as1_celling_standard.map",
        "AS1/as1_celling_lab.map"
    };

    for (int scene_index = 0; scene_index < 2; ++scene_index) {
        TestScene scene = load_test_scene(
            resolve_test_path(json_paths[scene_index])
        );
        WallBuilder builder(scene.map_size_x, scene.map_size_y);
        std::vector<io::Sprite> sprites = builder.build(
            scene.segments,
            false,
            true
        );

        io::write_map(
            sprites,
            get_test_output_path(output_paths[scene_index]),
            scene.map_size_x,
            scene.map_size_y
        );
    }
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

    const std::string temp_output_path = get_test_output_path("AS1/floor/current_floor_builder.map");
    TempFileCleaner cleaner(temp_output_path);

    bool write_success = io::write_map(sprites, temp_output_path, scene.map_size_x, scene.map_size_y);
    ASSERT_TRUE(write_success);

    const std::string golden_map_path = resolve_test_path("tests/golden/floor_builder.gold.map");
    bool files_match = compare_binary_files(temp_output_path, golden_map_path);

    EXPECT_TRUE(files_match);
}

// ===========================================================================
// AS2 per-Set 3x3 room floor tests (Set1 ~ Set9)
//
// Each test builds a 3-step grid room using build_room_segments(3, wall_type),
// runs WallBuilder with gen_floor=true, then verifies:
//   1. Expected wall VIDs exist (dir_a / dir_b / pillar)
//   2. Expected floor VID exists and floor_count >= 9 (at least 3x3 tiles)
//   3. Writes a .map preview file to tests/out/floor/ for visual inspection
// Room size (grid) = 3, AS2 grid_divisor=2, so physical room spans
// approx 270x192 which easily fits 9+ floor tiles of step 80x56.
// ===========================================================================

TEST(As2FloorTest, Set1Random3x3RoomUsesVid1783Floor) {
    WallBuilder builder(2000.0f, 2000.0f, true);
    int wall_type = WALL_TYPE_AS2_WALL_SET1_RANDOM;
    int expected_floor_vid = 1783;

    std::vector<Segment> segments = build_room_segments(3, wall_type);
    std::vector<io::Sprite> sprites = builder.build(segments, true, false);

    int wall_dir_a = count_sprites_by_vid(sprites, WALL_AS2_SET1_RANDOM.dir_a_vid);  // 1781
    int wall_dir_b = count_sprites_by_vid(sprites, WALL_AS2_SET1_RANDOM.dir_b_vid);  // 1780
    int wall_pillar = count_sprites_by_vid(sprites, WALL_AS2_SET1_RANDOM.pillar_vid); // 1782
    int floor_count = count_sprites_by_vid(sprites, expected_floor_vid);

    EXPECT_GE(wall_dir_a, 6);
    EXPECT_GE(wall_dir_b, 6);
    EXPECT_GE(wall_pillar, 4);
    EXPECT_GE(floor_count, 9) << "Set1 3x3 room should produce at least 9 floor tiles (VID 1783)";

    std::set<uint32_t> floor_directions;
    for (const io::Sprite& sprite : sprites) {
        if (sprite.vid == expected_floor_vid) {
            floor_directions.insert(sprite.direction);
        }
    }
    EXPECT_GT(floor_directions.size(), 1u);

    std::string out_path = get_test_output_path("floor/as2_floor_set1_3x3.map");
    io::write_map(sprites, out_path, io::MapFormat::AS2R, 2000.0f, 2000.0f);
}

TEST(As2FloorTest, Set2Random3x3RoomUsesVid1724Floor) {
    WallBuilder builder(2000.0f, 2000.0f, true);
    int wall_type = WALL_TYPE_AS2_WALL_SET2_RANDOM;
    int expected_floor_vid = 1724;

    std::vector<Segment> segments = build_room_segments(3, wall_type);
    std::vector<io::Sprite> sprites = builder.build(segments, true, false);

    int wall_dir_a = count_sprites_by_vid(sprites, WALL_AS2_SET2_RANDOM.dir_a_vid);  // 1701
    int wall_dir_b = count_sprites_by_vid(sprites, WALL_AS2_SET2_RANDOM.dir_b_vid);  // 1700
    int wall_pillar = count_sprites_by_vid(sprites, WALL_AS2_SET2_RANDOM.pillar_vid); // 1702
    int floor_count = count_sprites_by_vid(sprites, expected_floor_vid);

    EXPECT_GE(wall_dir_a, 6);
    EXPECT_GE(wall_dir_b, 6);
    EXPECT_GE(wall_pillar, 4);
    EXPECT_GE(floor_count, 9) << "Set2 3x3 room should produce at least 9 floor tiles (VID 1724)";

    for (const io::Sprite& sprite : sprites) {
        if (sprite.vid != expected_floor_vid) {
            continue;
        }

        bool direction_is_allowed = false;
        for (int index = 0; index < AS2_SIX_DIRECTION_RANDOMIZATION.allowed_value_count; ++index) {
            if (sprite.direction == AS2_SIX_DIRECTION_RANDOMIZATION.allowed_values[index]) {
                direction_is_allowed = true;
            }
        }
        EXPECT_TRUE(direction_is_allowed);
    }

    std::string out_path = get_test_output_path("floor/as2_floor_set2_3x3.map");
    io::write_map(sprites, out_path, io::MapFormat::AS2R, 2000.0f, 2000.0f);
}

TEST(As2FloorTest, Set3Random3x3RoomUsesVid1121Floor) {
    WallBuilder builder(2000.0f, 2000.0f, true);
    int wall_type = WALL_TYPE_AS2_WALL_SET3_RANDOM;
    int expected_floor_vid = 1121;

    std::vector<Segment> segments = build_room_segments(3, wall_type);
    std::vector<io::Sprite> sprites = builder.build(segments, true, false);

    int wall_dir_a = count_sprites_by_vid(sprites, WALL_AS2_SET3_RANDOM.dir_a_vid);  // 1100
    int wall_dir_b = count_sprites_by_vid(sprites, WALL_AS2_SET3_RANDOM.dir_b_vid);  // 1101
    int wall_pillar = count_sprites_by_vid(sprites, WALL_AS2_SET3_RANDOM.pillar_vid); // 1102
    int floor_count = count_sprites_by_vid(sprites, expected_floor_vid);

    EXPECT_GE(wall_dir_a, 6);
    EXPECT_GE(wall_dir_b, 6);
    EXPECT_GE(wall_pillar, 4);
    EXPECT_GE(floor_count, 9) << "Set3 3x3 room should produce at least 9 floor tiles (VID 1121)";

    std::string out_path = get_test_output_path("floor/as2_floor_set3_3x3.map");
    io::write_map(sprites, out_path, io::MapFormat::AS2R, 2000.0f, 2000.0f);
}

TEST(As2FloorTest, Set4Random3x3RoomUsesVid1121Floor) {
    WallBuilder builder(2000.0f, 2000.0f, true);
    int wall_type = WALL_TYPE_AS2_WALL_SET4_RANDOM;
    int expected_floor_vid = 1121;

    std::vector<Segment> segments = build_room_segments(3, wall_type);
    std::vector<io::Sprite> sprites = builder.build(segments, true, false);

    int wall_dir_a = count_sprites_by_vid(sprites, WALL_AS2_SET4_RANDOM.dir_a_vid);  // 1131
    int wall_dir_b = count_sprites_by_vid(sprites, WALL_AS2_SET4_RANDOM.dir_b_vid);  // 1130
    int wall_pillar = count_sprites_by_vid(sprites, WALL_AS2_SET4_RANDOM.pillar_vid); // 1132
    int floor_count = count_sprites_by_vid(sprites, expected_floor_vid);

    EXPECT_GE(wall_dir_a, 6);
    EXPECT_GE(wall_dir_b, 6);
    EXPECT_GE(wall_pillar, 4);
    EXPECT_GE(floor_count, 9) << "Set4 3x3 room should produce at least 9 floor tiles (VID 1121)";

    std::string out_path = get_test_output_path("floor/as2_floor_set4_3x3.map");
    io::write_map(sprites, out_path, io::MapFormat::AS2R, 2000.0f, 2000.0f);
}

TEST(As2FloorTest, Set5Random3x3RoomUsesVid2503Floor) {
    WallBuilder builder(2000.0f, 2000.0f, true);
    int wall_type = WALL_TYPE_AS2_WALL_SET5_RANDOM;
    int expected_floor_vid = 2503;

    std::vector<Segment> segments = build_room_segments(3, wall_type);
    std::vector<io::Sprite> sprites = builder.build(segments, true, false);

    int wall_dir_a = count_sprites_by_vid(sprites, WALL_AS2_SET5_RANDOM.dir_a_vid);  // 2501
    int wall_dir_b = count_sprites_by_vid(sprites, WALL_AS2_SET5_RANDOM.dir_b_vid);  // 2500
    int wall_pillar = count_sprites_by_vid(sprites, WALL_AS2_SET5_RANDOM.pillar_vid); // 2502
    int floor_count = count_sprites_by_vid(sprites, expected_floor_vid);

    EXPECT_GE(wall_dir_a, 6);
    EXPECT_GE(wall_dir_b, 6);
    EXPECT_GE(wall_pillar, 4);
    EXPECT_GE(floor_count, 9) << "Set5 3x3 room should produce at least 9 floor tiles (VID 2503)";

    std::string out_path = get_test_output_path("floor/as2_floor_set5_3x3.map");
    io::write_map(sprites, out_path, io::MapFormat::AS2R, 2000.0f, 2000.0f);
}

TEST(As2FloorTest, Set6Random3x3RoomUsesVid2503Floor) {
    WallBuilder builder(2000.0f, 2000.0f, true);
    int wall_type = WALL_TYPE_AS2_WALL_SET6_RANDOM;
    int expected_floor_vid = 2503;

    std::vector<Segment> segments = build_room_segments(3, wall_type);
    std::vector<io::Sprite> sprites = builder.build(segments, true, false);

    int wall_dir_a = count_sprites_by_vid(sprites, WALL_AS2_SET6_RANDOM.dir_a_vid);  // 2601
    int wall_dir_b = count_sprites_by_vid(sprites, WALL_AS2_SET6_RANDOM.dir_b_vid);  // 2600
    int wall_pillar = count_sprites_by_vid(sprites, WALL_AS2_SET6_RANDOM.pillar_vid); // 2602
    int floor_count = count_sprites_by_vid(sprites, expected_floor_vid);

    EXPECT_GE(wall_dir_a, 6);
    EXPECT_GE(wall_dir_b, 6);
    // Set6: skip_unmapped_pillars=true, only pillar_corner_up_left defined -> 1 pillar
    EXPECT_GE(wall_pillar, 1);
    EXPECT_GE(floor_count, 9) << "Set6 3x3 room should produce at least 9 floor tiles (VID 2503)";

    std::string out_path = get_test_output_path("floor/as2_floor_set6_3x3.map");
    io::write_map(sprites, out_path, io::MapFormat::AS2R, 2000.0f, 2000.0f);
}

TEST(As2FloorTest, Set7Random3x3RoomUsesVid2643Floor) {
    WallBuilder builder(2000.0f, 2000.0f, true);
    int wall_type = WALL_TYPE_AS2_WALL_SET7_RANDOM;
    int expected_floor_vid = 2643;

    std::vector<Segment> segments = build_room_segments(3, wall_type);
    std::vector<io::Sprite> sprites = builder.build(segments, true, false);

    int wall_dir_a = count_sprites_by_vid(sprites, WALL_AS2_SET7_RANDOM.dir_a_vid);  // 2621
    int wall_dir_b = count_sprites_by_vid(sprites, WALL_AS2_SET7_RANDOM.dir_b_vid);  // 2620
    int wall_pillar = count_sprites_by_vid(sprites, WALL_AS2_SET7_RANDOM.pillar_vid); // 2622
    int floor_count = count_sprites_by_vid(sprites, expected_floor_vid);

    EXPECT_GE(wall_dir_a, 6);
    EXPECT_GE(wall_dir_b, 6);
    EXPECT_GE(wall_pillar, 4);
    EXPECT_GE(floor_count, 9) << "Set7 3x3 room should produce at least 9 floor tiles (VID 2643)";

    std::string out_path = get_test_output_path("floor/as2_floor_set7_3x3.map");
    io::write_map(sprites, out_path, io::MapFormat::AS2R, 2000.0f, 2000.0f);
}

TEST(As2FloorTest, Set8Random3x3RoomUsesVid2643Floor) {
    WallBuilder builder(2000.0f, 2000.0f, true);
    int wall_type = WALL_TYPE_AS2_WALL_SET8_RANDOM;
    int expected_floor_vid = 2643;

    std::vector<Segment> segments = build_room_segments(3, wall_type);
    std::vector<io::Sprite> sprites = builder.build(segments, true, false);

    int wall_dir_a = count_sprites_by_vid(sprites, WALL_AS2_SET8_RANDOM.dir_a_vid);  // 2641
    int wall_dir_b = count_sprites_by_vid(sprites, WALL_AS2_SET8_RANDOM.dir_b_vid);  // 2640
    int wall_pillar = count_sprites_by_vid(sprites, WALL_AS2_SET8_RANDOM.pillar_vid); // 2642
    int floor_count = count_sprites_by_vid(sprites, expected_floor_vid);

    EXPECT_GE(wall_dir_a, 6);
    EXPECT_GE(wall_dir_b, 6);
    EXPECT_GE(wall_pillar, 4);
    EXPECT_GE(floor_count, 9) << "Set8 3x3 room should produce at least 9 floor tiles (VID 2643)";

    std::string out_path = get_test_output_path("floor/as2_floor_set8_3x3.map");
    io::write_map(sprites, out_path, io::MapFormat::AS2R, 2000.0f, 2000.0f);
}

TEST(As2FloorTest, Set9Random3x3RoomUsesVid1724Floor) {
    WallBuilder builder(2000.0f, 2000.0f, true);
    int wall_type = WALL_TYPE_AS2_WALL_SET9_RANDOM;
    int expected_floor_vid = 1724;

    std::vector<Segment> segments = build_room_segments(3, wall_type);
    std::vector<io::Sprite> sprites = builder.build(segments, true, false);

    int wall_dir_a = count_sprites_by_vid(sprites, WALL_AS2_SET9_RANDOM.dir_a_vid);  // 1721
    int wall_dir_b = count_sprites_by_vid(sprites, WALL_AS2_SET9_RANDOM.dir_b_vid);  // 1720
    // Set9 has pillar_vid=0 (no independent pillar), skip pillar check
    int floor_count = count_sprites_by_vid(sprites, expected_floor_vid);

    EXPECT_GE(wall_dir_a, 6);
    EXPECT_GE(wall_dir_b, 6);
    EXPECT_GE(floor_count, 9) << "Set9 3x3 room should produce at least 9 floor tiles (VID 1724)";

    std::string out_path = get_test_output_path("floor/as2_floor_set9_3x3.map");
    io::write_map(sprites, out_path, io::MapFormat::AS2R, 2000.0f, 2000.0f);
}

TEST(WallBuilderTest, DoorExcavationPreservesCornerPillars) {
    WallBuilder builder(2000.0f, 2000.0f, false);

    // L-shaped wall joining at corner (0, 0)
    std::vector<Segment> segments = {
        {{0, 0}, {5, 0}, WALL_TYPE_STANDARD},
        {{0, 0}, {0, 5}, WALL_TYPE_STANDARD}
    };

    // Place a door right next to corner (0, 0) on the horizontal segment
    std::vector<DoorExcavation> excavations = {
        {{0, 0}, 1, 2, WALL_TYPE_STANDARD}
    };

    std::vector<io::Sprite> sprites_without_doors = builder.build(segments, false, false, {});
    std::vector<io::Sprite> sprites_with_doors = builder.build(segments, false, false, excavations);

    int pillars_without_doors = count_sprites_by_vid(sprites_without_doors, WALL_STANDARD.pillar_vid);
    int pillars_with_doors = count_sprites_by_vid(sprites_with_doors, WALL_STANDARD.pillar_vid);

    EXPECT_EQ(pillars_without_doors, 3);
    EXPECT_EQ(pillars_with_doors, 3) << "Door excavation must preserve corner and endpoint pillars";
}
