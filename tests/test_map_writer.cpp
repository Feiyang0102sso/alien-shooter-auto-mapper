#include <gtest/gtest.h>
#include "auto_mapper/io/map_writer.h"
#include "auto_mapper/io/templates.h"
#include "utils/test_utils.h"
#include <cstdint>
#include <fstream>
#include <iterator>
#include <string>
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

    std::string output_file = get_test_output_path("AS1/test_cpp_output.map");
    bool success = io::write_map(
        sprites, 
        output_file, 
        600.0f, 600.0f
    );

    EXPECT_TRUE(success);
}

namespace {

std::vector<uint8_t> load_binary_file(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return {};
    }

    return std::vector<uint8_t>(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    );
}

uint32_t read_uint32_at(const uint8_t* data, size_t offset) {
    uint32_t value = 0;
    value |= static_cast<uint32_t>(data[offset]);
    value |= static_cast<uint32_t>(data[offset + 1]) << 8;
    value |= static_cast<uint32_t>(data[offset + 2]) << 16;
    value |= static_cast<uint32_t>(data[offset + 3]) << 24;
    return value;
}

uint32_t read_uint32_at(const std::vector<uint8_t>& data, size_t offset) {
    return read_uint32_at(data.data(), offset);
}

std::string read_magic_at(const std::vector<uint8_t>& data, size_t offset) {
    return std::string(data.begin() + offset, data.begin() + offset + 4);
}

} // namespace

TEST(MapWriterTest, EmptyMapTemplateSizesAreStable) {
    EXPECT_EQ(io::templates::AS1_EMPTY_SIZE, 228u);
    EXPECT_EQ(io::templates::AS2_EMPTY_SIZE, 244u);
    EXPECT_EQ(io::templates::AS2R_EMPTY_SIZE, 280u);
}

TEST(MapWriterTest, AS2EmptyTemplatesKeepExpectedMapVersions) {
    constexpr size_t AS2_VERSION_OFFSET = 100;
    constexpr size_t AS2R_VERSION_OFFSET = 136;

    EXPECT_EQ(read_uint32_at(io::templates::AS2_empty, 4), 236u);
    EXPECT_EQ(read_uint32_at(io::templates::AS2_empty, AS2_VERSION_OFFSET), 0x13u);

    EXPECT_EQ(read_uint32_at(io::templates::AS2R_empty, 4), 272u);
    EXPECT_EQ(read_uint32_at(io::templates::AS2R_empty, AS2R_VERSION_OFFSET), 0x14u);
}

TEST(MapWriterTest, AS2EmptyTemplatesMatchPreDemoSamples) {
    std::string as2_path = resolve_test_path("tests/golden/AS2/empty/AS2empty.map");
    std::string as2r_path = resolve_test_path("tests/golden/AS2/empty/AS2Rempty.map");

    std::vector<uint8_t> as2_sample = load_binary_file(as2_path);
    std::vector<uint8_t> as2r_sample = load_binary_file(as2r_path);

    std::vector<uint8_t> as2_template(
        std::begin(io::templates::AS2_empty),
        std::end(io::templates::AS2_empty)
    );
    std::vector<uint8_t> as2r_template(
        std::begin(io::templates::AS2R_empty),
        std::end(io::templates::AS2R_empty)
    );

    EXPECT_EQ(as2_template, as2_sample);
    EXPECT_EQ(as2r_template, as2r_sample);
}

TEST(MapWriterTest, WritesEmptyAS2MapFromSelectedFormat) {
    std::string output_file = get_test_output_path("empty_as2_writer_output.map");

    std::vector<io::Sprite> sprites;
    bool success = io::write_map(
        sprites,
        output_file,
        io::MapFormat::AS2,
        640.0f,
        480.0f
    );

    ASSERT_TRUE(success);

    std::vector<uint8_t> output = load_binary_file(output_file);

    EXPECT_EQ(output.size(), io::templates::AS2_EMPTY_SIZE);
    EXPECT_EQ(read_uint32_at(output, 4), 236u);
    EXPECT_EQ(read_uint32_at(output, 100), 0x13u);
    EXPECT_EQ(read_magic_at(output, 104), "SPR ");
    EXPECT_EQ(read_magic_at(output, 160), "PLAY");
}

TEST(MapWriterTest, WritesEmptyAS2RMapFromSelectedFormat) {
    std::string output_file = get_test_output_path("empty_as2r_writer_output.map");

    std::vector<io::Sprite> sprites;
    bool success = io::write_map(
        sprites,
        output_file,
        io::MapFormat::AS2R,
        640.0f,
        480.0f
    );

    ASSERT_TRUE(success);

    std::vector<uint8_t> output = load_binary_file(output_file);

    EXPECT_EQ(output.size(), io::templates::AS2R_EMPTY_SIZE);
    EXPECT_EQ(read_uint32_at(output, 4), 272u);
    EXPECT_EQ(read_uint32_at(output, 136), 0x14u);
    EXPECT_EQ(read_magic_at(output, 140), "SPR ");
    EXPECT_EQ(read_magic_at(output, 196), "PLAY");
}

TEST(MapWriterTest, WritesAS2SpriteRecordsWithVersionTwoLayout) {
    io::Sprite sprite;
    sprite.vid = 601;
    sprite.posX = 300.0f;
    sprite.posY = 238.0f;
    sprite.direction = 64;
    sprite.army = 2;

    std::string output_file = get_test_output_path("single_sprite_as2_writer_output.map");

    bool success = io::write_map(
        {sprite},
        output_file,
        io::MapFormat::AS2,
        640.0f,
        480.0f
    );

    ASSERT_TRUE(success);

    std::vector<uint8_t> output = load_binary_file(output_file);

    constexpr size_t SPR_OFFSET = 104;
    constexpr size_t SPR_INFO_SIZE_OFFSET = SPR_OFFSET + 20;
    constexpr size_t SPR_RECORD_OFFSET = SPR_OFFSET + 24;
    constexpr size_t SPRD_OFFSET = SPR_OFFSET + 8 + 68;
    constexpr size_t PLAY_OFFSET = 230;

    EXPECT_EQ(read_magic_at(output, SPR_OFFSET), "SPR ");
    EXPECT_EQ(read_uint32_at(output, SPR_OFFSET + 4), 68u);
    EXPECT_EQ(read_uint32_at(output, SPR_INFO_SIZE_OFFSET), 52u);
    EXPECT_EQ(read_uint32_at(output, SPR_RECORD_OFFSET), 1u);
    EXPECT_EQ(read_uint32_at(output, SPR_RECORD_OFFSET + 4), 601u);
    EXPECT_EQ(read_uint32_at(output, SPR_RECORD_OFFSET + 24), 2u);

    EXPECT_EQ(read_magic_at(output, SPRD_OFFSET), "SPRD");
    EXPECT_EQ(read_uint32_at(output, SPRD_OFFSET + 4), 41u);
    EXPECT_EQ(read_uint32_at(output, SPRD_OFFSET + 20), 17u);
    EXPECT_EQ(output[PLAY_OFFSET - 1], 0u);
    EXPECT_EQ(read_magic_at(output, PLAY_OFFSET), "PLAY");
}

TEST(MapWriterTest, WritesAS2SpriteGammaIntoSpriteRecord) {
    io::Sprite sprite;
    sprite.vid = 1785;
    sprite.posX = 300.0f;
    sprite.posY = 238.0f;
    sprite.direction = 128;
    sprite.gamma = {
        -140,
        -150,
        -180,
        0
    };

    std::string output_file = get_test_output_path("single_sprite_as2_gamma_output.map");

    bool success = io::write_map(
        {sprite},
        output_file,
        io::MapFormat::AS2,
        640.0f,
        480.0f
    );

    ASSERT_TRUE(success);

    std::vector<uint8_t> output = load_binary_file(output_file);

    constexpr size_t SPR_OFFSET = 104;
    constexpr size_t SPR_RECORD_OFFSET = SPR_OFFSET + 24;
    constexpr size_t SPR_RECORD_GAMMA_OFFSET = SPR_RECORD_OFFSET + 28;

    EXPECT_EQ(output[SPR_RECORD_GAMMA_OFFSET], 180u);
    EXPECT_EQ(output[SPR_RECORD_GAMMA_OFFSET + 1], 150u);
    EXPECT_EQ(output[SPR_RECORD_GAMMA_OFFSET + 2], 140u);
    EXPECT_EQ(output[SPR_RECORD_GAMMA_OFFSET + 3], 0u);
    EXPECT_EQ(output[SPR_RECORD_GAMMA_OFFSET + 4], 0u);
    EXPECT_EQ(output[SPR_RECORD_GAMMA_OFFSET + 5], 0u);
    EXPECT_EQ(output[SPR_RECORD_GAMMA_OFFSET + 6], 0u);
    EXPECT_EQ(output[SPR_RECORD_GAMMA_OFFSET + 7], 0u);
}
