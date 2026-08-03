/**
 * @file as1_map_writer.cpp
 * @brief write map template for AS1
 */

#include "auto_mapper/io/as1_map_writer.h"

#include "auto_mapper/common/logger.h"
#include "auto_mapper/io/map_writer_binary.h"
#include "auto_mapper/io/templates.h"

#include <iterator>

namespace auto_mapper::io::detail {

namespace {

constexpr int32_t AS1_MAP_VERSION = 0x0C;

// SPR section
void build_as1_spr_section(std::vector<uint8_t>& buffer, const std::vector<Sprite>& sprites) {
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
    }

    uint32_t spr_info_size = static_cast<uint32_t>(records.size()) + 4; // records + terminator

    std::vector<uint8_t> header;
    append_uint32(header, OPTIONS);
    append_uint32(header, 0); // packedDiff
    append_uint32(header, 1); // subresourceCount
    append_uint32(header, spr_info_size);

    uint32_t spr_size = static_cast<uint32_t>(header.size() + records.size() + 4); // header + records + terminator

    append_string(buffer, "SPR ");
    append_uint32(buffer, spr_size);
    buffer.insert(buffer.end(), header.begin(), header.end());
    buffer.insert(buffer.end(), records.begin(), records.end());
    append_uint32(buffer, TERMINATOR);
}

// SPRD section
void build_as1_sprd_section(std::vector<uint8_t>& buffer, const std::vector<Sprite>& sprites) {
    std::vector<uint8_t> subs;
    uint32_t object_id = 1;
    for (size_t i = 0; i < sprites.size(); ++i) {
        append_uint32(subs, 12); // subSize
        append_uint32(subs, object_id++); // objectId
        append_uint32(subs, 0); // cmdCount
        append_uint32(subs, 0); // itemCount
    }
    // terminator sub
    append_uint32(subs, 4); // subSize
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

// PLAY section
void build_as1_play_section(std::vector<uint8_t>& buffer) {
    std::vector<uint8_t> header;
    append_uint32(header, OPTIONS);
    append_uint32(header, 0); // packedDiff
    append_uint32(header, 1); // subresourceCount
    append_uint32(header, 16); // playInfoSize

    std::vector<uint8_t> body;
    for (int i = 0; i < 4; ++i) {
        append_uint32(body, 0); // 4 player ID
    }

    uint32_t play_size = static_cast<uint32_t>(header.size() + body.size());

    append_string(buffer, "PLAY");
    append_uint32(buffer, play_size);
    buffer.insert(buffer.end(), header.begin(), header.end());
    buffer.insert(buffer.end(), body.begin(), body.end());
}

} // namespace

bool write_as1_map(
    const std::vector<Sprite>& sprites,
    const std::string& output_path,
    float map_size_x,
    float map_size_y
) {
    std::vector<uint8_t> template_data(std::begin(templates::AS1_empty), std::end(templates::AS1_empty));

    if (template_data.size() != templates::AS1_EMPTY_SIZE) {
        Logger::error("AS1 template file is corrupted");
        return false;
    }

    // 1. Extract the GRPH (starting at offset 12, 44 bytes in length).
    std::vector<uint8_t> grph_section(template_data.begin() + 12, template_data.begin() + 56);

    // 2. construct HEAD
    std::vector<uint8_t> head_section;
    build_head_section(head_section, map_size_x, map_size_y, AS1_MAP_VERSION);

    // 3. construct other
    std::vector<uint8_t> spr_section;
    build_as1_spr_section(spr_section, sprites);

    std::vector<uint8_t> sprd_section;
    build_as1_sprd_section(sprd_section, sprites);

    std::vector<uint8_t> play_section;
    build_as1_play_section(play_section);

    std::vector<uint8_t> grou_section;
    build_grou_section(grou_section);

    // 4. connect all Payload
    std::vector<uint8_t> payload;
    payload.insert(payload.end(), grph_section.begin(), grph_section.end());
    payload.insert(payload.end(), head_section.begin(), head_section.end());
    payload.insert(payload.end(), spr_section.begin(), spr_section.end());
    payload.insert(payload.end(), sprd_section.begin(), sprd_section.end());
    payload.insert(payload.end(), play_section.begin(), play_section.end());
    payload.insert(payload.end(), grou_section.begin(), grou_section.end());

    return write_payload_to_map_file(payload, output_path, sprites.size());
}

} // namespace auto_mapper::io::detail
