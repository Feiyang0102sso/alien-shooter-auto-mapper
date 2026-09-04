/**
 * @file as2_map_writer.cpp
 * @brief write map templates for AS2 and AS2R
 */

#include "auto_mapper/io/as2_map_writer.h"

#include "auto_mapper/common/logger.h"
#include "auto_mapper/io/map_writer_binary.h"
#include "auto_mapper/io/templates.h"

namespace auto_mapper::io::detail {

namespace {

constexpr int32_t AS2_MAP_VERSION = 0x13;
constexpr int32_t AS2R_MAP_VERSION = 0x14;
const std::string AS2_MAP_RESOURCE_KIND = "MAP ";
const std::string AS2OE_LEVEL_RESOURCE_KIND = "LVL ";

// Encode VidGamma into the 8-byte AS2 SpriteRecord layout.
// Binary order: [negB, negG, negR, negA, posB, posG, posR, posA]
// Negative values stored as absolute value in the negative block.
// Positive values stored directly in the positive block.
// See AS2_MAP.bt L184-202 for the reverse (decode) side.
void append_vid_gamma(std::vector<uint8_t>& buffer, const core::VidGamma& gamma) {
    uint8_t neg_b = 0;
    uint8_t neg_g = 0;
    uint8_t neg_r = 0;
    uint8_t neg_a = 0;

    if (gamma.b < 0) neg_b = static_cast<uint8_t>(-gamma.b);
    if (gamma.g < 0) neg_g = static_cast<uint8_t>(-gamma.g);
    if (gamma.r < 0) neg_r = static_cast<uint8_t>(-gamma.r);
    if (gamma.a < 0) neg_a = static_cast<uint8_t>(-gamma.a);

    buffer.push_back(neg_b);
    buffer.push_back(neg_g);
    buffer.push_back(neg_r);
    buffer.push_back(neg_a);

    uint8_t pos_b = 0;
    uint8_t pos_g = 0;
    uint8_t pos_r = 0;
    uint8_t pos_a = 0;

    if (gamma.b > 0) pos_b = static_cast<uint8_t>(gamma.b);
    if (gamma.g > 0) pos_g = static_cast<uint8_t>(gamma.g);
    if (gamma.r > 0) pos_r = static_cast<uint8_t>(gamma.r);
    if (gamma.a > 0) pos_a = static_cast<uint8_t>(gamma.a);

    buffer.push_back(pos_b);
    buffer.push_back(pos_g);
    buffer.push_back(pos_r);
    buffer.push_back(pos_a);
}

void build_as2_spr_section(std::vector<uint8_t>& buffer, const std::vector<Sprite>& sprites) {
    std::vector<uint8_t> records;
    uint32_t object_id = 1;
    for (const auto& spr : sprites) {
        append_uint32(records, object_id++);
        append_int32(records, spr.vid);
        append_float(records, spr.posX);
        append_float(records, spr.posY);
        append_float(records, spr.posZ);
        append_uint32(records, spr.direction);
        append_int32(records, spr.army);

        // AS2 SpriteRecord: VidGamma (8 bytes) + ScaleX/Y/Z (12 bytes)
        append_vid_gamma(records, spr.gamma);
        append_float(records, spr.scale.x);
        append_float(records, spr.scale.y);
        append_float(records, spr.scale.z);
    }

    uint32_t spr_info_size = static_cast<uint32_t>(records.size()) + 4;

    std::vector<uint8_t> header;
    append_uint32(header, OPTIONS);
    append_uint32(header, 0); // packedDiff
    append_uint32(header, 1); // subresourceCount
    append_uint32(header, spr_info_size);

    uint32_t spr_size = static_cast<uint32_t>(header.size() + records.size() + 4);

    append_string(buffer, "SPR ");
    append_uint32(buffer, spr_size);
    buffer.insert(buffer.end(), header.begin(), header.end());
    buffer.insert(buffer.end(), records.begin(), records.end());
    append_uint32(buffer, TERMINATOR);
}

void build_as2_sprd_section(std::vector<uint8_t>& buffer, const std::vector<Sprite>& sprites) {
    std::vector<uint8_t> subs;
    uint32_t object_id = 1;
    for (size_t i = 0; i < sprites.size(); ++i) {
        append_uint32(subs, 17); // objectId + behavior + stackCount + itemCount + name terminator
        append_uint32(subs, object_id++);
        append_uint32(subs, 0); // behavior
        append_uint32(subs, 0); // stackCount
        append_uint32(subs, 0); // itemCount
        subs.push_back(0);      // empty nameText terminator
    }

    append_uint32(subs, 4); // terminator subSize
    append_uint32(subs, TERMINATOR);

    uint32_t sub_count = static_cast<uint32_t>(sprites.size()) + 1;

    std::vector<uint8_t> header;
    append_uint32(header, OPTIONS);
    append_uint32(header, 0); // packedDiff
    append_uint32(header, sub_count);

    uint32_t sprd_size = static_cast<uint32_t>(header.size() + subs.size());

    append_string(buffer, "SPRD");
    append_uint32(buffer, sprd_size);
    buffer.insert(buffer.end(), header.begin(), header.end());
    buffer.insert(buffer.end(), subs.begin(), subs.end());
}

void build_as2_play_section(std::vector<uint8_t>& buffer) {
    std::vector<uint8_t> header;
    append_uint32(header, OPTIONS);
    append_uint32(header, 0);  // packedDiff
    append_uint32(header, 1);  // subresourceCount
    append_uint32(header, 32); // playInfoSize

    std::vector<uint8_t> body;
    for (int i = 0; i < 4; ++i) {
        append_uint32(body, 0); // 4 player ID
    }

    for (int i = 0; i < 16; ++i) {
        body.push_back(0); // reserved
    }

    uint32_t play_size = static_cast<uint32_t>(header.size() + body.size());

    append_string(buffer, "PLAY");
    append_uint32(buffer, play_size);
    buffer.insert(buffer.end(), header.begin(), header.end());
    buffer.insert(buffer.end(), body.begin(), body.end());
}

bool write_as2_family_map(
    const std::vector<Sprite>& sprites,
    const std::string& output_path,
    float map_size_x,
    float map_size_y,
    MapFormat format
) {
    const uint8_t* template_begin = templates::AS2_empty;
    const uint8_t* template_end = templates::AS2_empty + templates::AS2_EMPTY_SIZE;
    size_t expected_template_size = templates::AS2_EMPTY_SIZE;
    size_t grph_end = 56;
    int32_t map_version = AS2_MAP_VERSION;
    std::string resource_kind = AS2_MAP_RESOURCE_KIND;

    if (format == MapFormat::AS2R || format == MapFormat::AS2OE) {
        template_begin = templates::AS2R_empty;
        template_end = templates::AS2R_empty + templates::AS2R_EMPTY_SIZE;
        expected_template_size = templates::AS2R_EMPTY_SIZE;
        grph_end = 92;
        map_version = AS2R_MAP_VERSION;
    }

    if (format == MapFormat::AS2OE) {
        resource_kind = AS2OE_LEVEL_RESOURCE_KIND;
    }

    std::vector<uint8_t> template_data(template_begin, template_end);

    if (template_data.size() != expected_template_size) {
        Logger::error("AS2 template file is corrupted");
        return false;
    }

    std::vector<uint8_t> grph_section(template_data.begin() + 12, template_data.begin() + grph_end);

    std::vector<uint8_t> head_section;
    build_head_section(head_section, map_size_x, map_size_y, map_version);

    std::vector<uint8_t> spr_section;
    build_as2_spr_section(spr_section, sprites);

    std::vector<uint8_t> sprd_section;
    build_as2_sprd_section(sprd_section, sprites);

    std::vector<uint8_t> play_section;
    build_as2_play_section(play_section);

    std::vector<uint8_t> grou_section;
    build_grou_section(grou_section);

    std::vector<uint8_t> payload;
    payload.insert(payload.end(), grph_section.begin(), grph_section.end());
    payload.insert(payload.end(), head_section.begin(), head_section.end());
    payload.insert(payload.end(), spr_section.begin(), spr_section.end());
    payload.insert(payload.end(), sprd_section.begin(), sprd_section.end());

    // AS2 aligns PLAY to a 2-byte boundary after SPRD nameText data.
    if (payload.size() % 2 != 0) {
        payload.push_back(0);
    }

    payload.insert(payload.end(), play_section.begin(), play_section.end());
    payload.insert(payload.end(), grou_section.begin(), grou_section.end());

    return write_payload_to_map_file(payload, output_path, sprites.size(), resource_kind);
}

} // namespace

bool write_as2_map(
    const std::vector<Sprite>& sprites,
    const std::string& output_path,
    float map_size_x,
    float map_size_y
) {
    return write_as2_family_map(sprites, output_path, map_size_x, map_size_y, MapFormat::AS2);
}

bool write_as2r_map(
    const std::vector<Sprite>& sprites,
    const std::string& output_path,
    float map_size_x,
    float map_size_y
) {
    return write_as2_family_map(sprites, output_path, map_size_x, map_size_y, MapFormat::AS2R);
}

bool write_as2oe_map(
    const std::vector<Sprite>& sprites,
    const std::string& output_path,
    float map_size_x,
    float map_size_y
) {
    return write_as2_family_map(sprites, output_path, map_size_x, map_size_y, MapFormat::AS2OE);
}

} // namespace auto_mapper::io::detail
