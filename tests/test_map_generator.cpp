#include <gtest/gtest.h>

#include "auto_mapper/core/decoration_builder/decoration_registry.h"
#include "auto_mapper/core/map_generator.h"
#include "auto_mapper/io/templates.h"
#include "utils/test_utils.h"

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

TEST(MapGeneratorTest, DecorationRegistryFindsProfilesById) {
    auto profiles = auto_mapper::core::decoration_builder::as2_decoration_profiles();
    ASSERT_FALSE(profiles.empty());

    for (const auto* profile : profiles) {
        EXPECT_EQ(auto_mapper::core::decoration_builder::find_as2_decoration_profile(profile->id), profile);
    }
    EXPECT_EQ(auto_mapper::core::decoration_builder::find_as2_decoration_profile("no_such_stamp"), nullptr);
}
