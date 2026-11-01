#include <gtest/gtest.h>

#include "auto_mapper/core/decoration_builder/decoration_library.h"
#include "auto_mapper/core/map_generator.h"
#include "auto_mapper/io/templates.h"
#include "utils/test_utils.h"

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

using namespace auto_mapper::test;
using auto_mapper::core::MapRequest;
using auto_mapper::io::MapFormat;

namespace {

std::vector<uint8_t> load_generator_test_binary(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return {};
    }

    return std::vector<uint8_t>(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    );
}

uint32_t read_generator_test_uint32(const std::vector<uint8_t>& data, size_t offset) {
    uint32_t value = 0;
    value |= static_cast<uint32_t>(data[offset]);
    value |= static_cast<uint32_t>(data[offset + 1]) << 8;
    value |= static_cast<uint32_t>(data[offset + 2]) << 16;
    value |= static_cast<uint32_t>(data[offset + 3]) << 24;
    return value;
}

// An empty 640 x 480 project in the given format.
MapRequest empty_request(MapFormat format) {
    MapRequest request;
    request.format = format;
    request.map_size_x = 640.0f;
    request.map_size_y = 480.0f;
    return request;
}

} // namespace

TEST(MapGeneratorTest, DefaultAS1CeilingLayerCounts) {
    EXPECT_EQ(auto_mapper::core::AS1_CEILING_LAYER_COUNT_MIN, 1);
    EXPECT_EQ(auto_mapper::core::AS1_CEILING_LAYER_COUNT_MAX, 50);
    EXPECT_EQ(auto_mapper::core::DEFAULT_AS1_CEILING_LAYER_SETTINGS.standard_total_layer_count, 13);
    EXPECT_EQ(auto_mapper::core::DEFAULT_AS1_CEILING_LAYER_SETTINGS.lab_total_layer_count, 6);
}

TEST(MapGeneratorTest, ExplicitFormatControlsEmptyAS2RProjectOutput) {
    std::string output_file = get_test_output_path("empty_as2r_generator_output.map");

    ASSERT_TRUE(auto_mapper::core::generate_map(empty_request(MapFormat::AS2R), output_file));

    std::vector<uint8_t> output = load_generator_test_binary(output_file);
    EXPECT_EQ(output.size(), auto_mapper::io::templates::AS2R_EMPTY_SIZE);
    EXPECT_EQ(read_generator_test_uint32(output, 136), 0x14u);
}

TEST(MapGeneratorTest, ExplicitFormatWritesAS2OELevelResource) {
    std::string output_file = get_test_output_path("empty_as2oe_generator_output.map");

    ASSERT_TRUE(auto_mapper::core::generate_map(empty_request(MapFormat::AS2OE), output_file));

    std::vector<uint8_t> output = load_generator_test_binary(output_file);
    ASSERT_GE(output.size(), 12u);
    EXPECT_EQ(std::string(output.begin() + 8, output.begin() + 12), "LVL ");
    EXPECT_EQ(read_generator_test_uint32(output, 136), 0x14u);
}

TEST(MapGeneratorTest, RejectsInvalidAS1CeilingLayerCount) {
    MapRequest request = empty_request(MapFormat::AS1);
    request.as1_ceiling_layers.standard_total_layer_count = 0;

    EXPECT_FALSE(auto_mapper::core::generate_map(request, get_test_output_path("invalid_ceiling_layers.map")));

    request = empty_request(MapFormat::AS1);
    request.as1_ceiling_layers.lab_total_layer_count = 51;

    EXPECT_FALSE(auto_mapper::core::generate_map(request, get_test_output_path("invalid_ceiling_layers.map")));
}

TEST(MapGeneratorTest, StampsResolveThroughTheDecorationLibrary) {
    auto_mapper::core::decoration_builder::DecorationLibrary library;
    ASSERT_TRUE(library.load_map(resolve_test_path("tests/golden/decoration/01dorm_library.map")));

    MapRequest request = empty_request(MapFormat::AS2R);
    request.decoration_library = &library;
    request.stamps.push_back({"01dorm_library:1", 320.0f, 240.0f});
    request.stamps.push_back({"no_such_stamp", 320.0f, 240.0f});

    std::string output_file = get_test_output_path("stamp_generator_output.map");
    ASSERT_TRUE(auto_mapper::core::generate_map(request, output_file));

    // The SPR record holds one 48-byte record per sprite: 23 from stamp 1, none from the unknown id.
    std::vector<uint8_t> data = load_generator_test_binary(output_file);
    std::vector<uint8_t> tag = {'S', 'P', 'R', ' '};
    auto found = std::search(data.begin(), data.end(), tag.begin(), tag.end());
    ASSERT_NE(found, data.end());
    std::size_t spr = static_cast<std::size_t>(found - data.begin());
    uint32_t spr_info_size = read_generator_test_uint32(data, spr + 20);
    EXPECT_EQ((spr_info_size - 4) / 48, 23u);
}

TEST(MapGeneratorTest, As1StampsWriteAs1SpriteRecords) {
    auto_mapper::core::decoration_builder::DecorationLibrary library;
    ASSERT_TRUE(library.load_map(resolve_test_path("tests/golden/decoration/AS1/08leisure.map")));

    MapRequest request = empty_request(MapFormat::AS1);
    request.decoration_library = &library;
    request.stamps.push_back({"08leisure:9", 320.0f, 240.0f});

    std::string output_file = get_test_output_path("as1_stamp_generator_output.map");
    ASSERT_TRUE(auto_mapper::core::generate_map(request, output_file));

    // AS1 SPR records are 28 bytes: the 12 members of cell 9.
    std::vector<uint8_t> data = load_generator_test_binary(output_file);
    std::vector<uint8_t> tag = {'S', 'P', 'R', ' '};
    auto found = std::search(data.begin(), data.end(), tag.begin(), tag.end());
    ASSERT_NE(found, data.end());
    std::size_t spr = static_cast<std::size_t>(found - data.begin());
    uint32_t spr_info_size = read_generator_test_uint32(data, spr + 20);
    EXPECT_EQ((spr_info_size - 4) / 28, 12u);
}

TEST(MapGeneratorTest, LootBoxesGetLootTextAndSkipAs1) {
    const std::string text = "EMPTY Ammo_shotgun Obj_money3";
    auto_mapper::core::LootTable wood;
    wood.vids = {auto_mapper::core::LOOT_VID_WOOD_BOX};
    wood.lines.push_back({text, 100.0});

    MapRequest request = empty_request(MapFormat::AS2R);
    request.loot_boxes.push_back({auto_mapper::core::LOOT_VID_WOOD_BOX, 200.0f, 200.0f, 0.0f, 0});
    request.loot_boxes.push_back({auto_mapper::core::LOOT_VID_BROKEN_EGG, 260.0f, 200.0f, 0.0f, 0});
    request.loot_tables.push_back(wood);
    std::string output_file = get_test_output_path("loot_generator_output.map");
    ASSERT_TRUE(auto_mapper::core::generate_map(request, output_file));

    // Two SPR records, and the wood box's whole loot text sits in the SPRD block.
    std::vector<uint8_t> data = load_generator_test_binary(output_file);
    std::vector<uint8_t> tag = {'S', 'P', 'R', ' '};
    auto found = std::search(data.begin(), data.end(), tag.begin(), tag.end());
    ASSERT_NE(found, data.end());
    std::size_t spr = static_cast<std::size_t>(found - data.begin());
    EXPECT_EQ((read_generator_test_uint32(data, spr + 20) - 4) / 48, 2u);
    EXPECT_NE(std::search(data.begin(), data.end(), text.begin(), text.end()), data.end());

    // A table above 100% is refused.
    request.loot_tables[0].lines.push_back({"Obj_money1", 1.0});
    EXPECT_FALSE(auto_mapper::core::generate_map(request, output_file));

    // AS1 has no loot box brush: the boxes are skipped and the map stays empty.
    request = empty_request(MapFormat::AS1);
    request.loot_boxes.push_back({auto_mapper::core::LOOT_VID_WOOD_BOX, 200.0f, 200.0f, 0.0f, 0});
    output_file = get_test_output_path("loot_as1_generator_output.map");
    ASSERT_TRUE(auto_mapper::core::generate_map(request, output_file));
    EXPECT_EQ(load_generator_test_binary(output_file).size(), auto_mapper::io::templates::AS1_EMPTY_SIZE);
}

TEST(MapGeneratorTest, CurveWallsWriteAs2PiecesAndSkipAs1) {
    auto_mapper::core::CurveWall wall;
    wall.curve_wall_type = auto_mapper::core::CURVE_WALL_TYPE_ROCK;
    wall.points = {{100.0f, 200.0f}, {1200.0f, 200.0f}};

    MapRequest request = empty_request(MapFormat::AS2R);
    request.curve_walls.push_back(wall);
    std::string output_file = get_test_output_path("curve_wall_generator_output.map");
    ASSERT_TRUE(auto_mapper::core::generate_map(request, output_file));

    // 1100 long, end pieces 125 inside -> 9 pieces of 2670, one 48-byte SPR record each.
    std::vector<uint8_t> data = load_generator_test_binary(output_file);
    std::vector<uint8_t> tag = {'S', 'P', 'R', ' '};
    auto found = std::search(data.begin(), data.end(), tag.begin(), tag.end());
    ASSERT_NE(found, data.end());
    std::size_t spr = static_cast<std::size_t>(found - data.begin());
    uint32_t spr_info_size = read_generator_test_uint32(data, spr + 20);
    EXPECT_EQ((spr_info_size - 4) / 48, 9u);

    // AS1 has no 2670 / 2700: the curve wall is skipped and the map stays empty.
    request = empty_request(MapFormat::AS1);
    request.curve_walls.push_back(wall);
    output_file = get_test_output_path("curve_wall_as1_generator_output.map");
    ASSERT_TRUE(auto_mapper::core::generate_map(request, output_file));
    EXPECT_EQ(load_generator_test_binary(output_file).size(), auto_mapper::io::templates::AS1_EMPTY_SIZE);
}
