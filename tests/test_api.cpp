#include <gtest/gtest.h>

#include "auto_mapper/api.h"
#include "auto_mapper/io/templates.h"
#include "utils/test_utils.h"

#include <cstdint>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

using namespace auto_mapper::test;

namespace {

std::vector<uint8_t> load_api_test_binary(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return {};
    }

    return std::vector<uint8_t>(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    );
}

uint32_t read_api_test_uint32(const std::vector<uint8_t>& data, size_t offset) {
    uint32_t value = 0;
    value |= static_cast<uint32_t>(data[offset]);
    value |= static_cast<uint32_t>(data[offset + 1]) << 8;
    value |= static_cast<uint32_t>(data[offset + 2]) << 16;
    value |= static_cast<uint32_t>(data[offset + 3]) << 24;
    return value;
}

} // namespace

TEST(ApiTest, ReportsVersionNine) {
    EXPECT_EQ(get_auto_mapper_api_version(), 9);
}

TEST(ApiTest, ExposesAS1CeilingLayerConfig) {
    CAS1CeilingLayerConfig config{};

    ASSERT_TRUE(get_as1_ceiling_layer_config(&config));
    EXPECT_EQ(config.min_layer_count, 1);
    EXPECT_EQ(config.max_layer_count, 50);
    EXPECT_EQ(config.default_standard_layer_count, 13);
    EXPECT_EQ(config.default_lab_layer_count, 6);
    EXPECT_FALSE(get_as1_ceiling_layer_config(nullptr));
}

TEST(ApiTest, ExplicitFormatControlsEmptyAS2RProjectOutput) {
    std::string output_file = get_test_output_path("empty_as2r_api_output.map");

    bool success = generate_map_from_segments(
        output_file.c_str(),
        C_MAP_FORMAT_AS2R,
        nullptr,
        0,
        nullptr,
        0,
        nullptr,
        0,
        nullptr,
        0,
        nullptr,
        0,
        640.0f,
        480.0f,
        13,
        6,
        false,
        false,
        false
    );

    ASSERT_TRUE(success);

    std::vector<uint8_t> output = load_api_test_binary(output_file);
    EXPECT_EQ(output.size(), auto_mapper::io::templates::AS2R_EMPTY_SIZE);
    EXPECT_EQ(read_api_test_uint32(output, 136), 0x14u);
}

TEST(ApiTest, ExplicitFormatWritesAS2OELevelResource) {
    std::string output_file = get_test_output_path("empty_as2oe_api_output.map");

    bool success = generate_map_from_segments(
        output_file.c_str(),
        C_MAP_FORMAT_AS2OE,
        nullptr,
        0,
        nullptr,
        0,
        nullptr,
        0,
        nullptr,
        0,
        nullptr,
        0,
        640.0f,
        480.0f,
        13,
        6,
        false,
        false,
        false
    );

    ASSERT_TRUE(success);

    std::vector<uint8_t> output = load_api_test_binary(output_file);
    ASSERT_GE(output.size(), 12u);
    EXPECT_EQ(std::string(output.begin() + 8, output.begin() + 12), "LVL ");
    EXPECT_EQ(read_api_test_uint32(output, 136), 0x14u);
}

TEST(ApiTest, RejectsUnknownMapFormat) {
    std::string output_file = get_test_output_path("invalid_format_api_output.map");

    bool success = generate_map_from_segments(
        output_file.c_str(),
        99,
        nullptr,
        0,
        nullptr,
        0,
        nullptr,
        0,
        nullptr,
        0,
        nullptr,
        0,
        640.0f,
        480.0f,
        13,
        6,
        false,
        false,
        false
    );

    EXPECT_FALSE(success);
}

TEST(ApiTest, RejectsInvalidAS1CeilingLayerCount) {
    bool success = generate_map_from_segments(
        "invalid_ceiling_layers.map",
        C_MAP_FORMAT_AS1,
        nullptr,
        0,
        nullptr,
        0,
        nullptr,
        0,
        nullptr,
        0,
        nullptr,
        0,
        640.0f,
        480.0f,
        0,
        6,
        false,
        false,
        false
    );

    EXPECT_FALSE(success);
}
