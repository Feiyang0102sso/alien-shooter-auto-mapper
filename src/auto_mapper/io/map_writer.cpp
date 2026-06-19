/**
* @file map_writer.cpp
 * @brief write map template for AS1
 */


#include "map_writer.h"
#include "auto_mapper/common/logger.h"

#include <fstream>
#include <cstring>
#include <iterator>

namespace auto_mapper::io {

namespace {

constexpr uint32_t OPTIONS = 0x80000000;
constexpr uint32_t TERMINATOR = 0xFFFFFFFF;
constexpr int32_t MAP_VERSION = 0x0C;

// helper - add to ByteBuffer with Little-Endian
void append_uint32(std::vector<uint8_t>& buffer, uint32_t value) {
    uint8_t bytes[4];
    std::memcpy(bytes, &value, 4);
    buffer.insert(buffer.end(), bytes, bytes + 4);
}

void append_int32(std::vector<uint8_t>& buffer, int32_t value) {
    uint8_t bytes[4];
    std::memcpy(bytes, &value, 4);
    buffer.insert(buffer.end(), bytes, bytes + 4);
}

void append_float(std::vector<uint8_t>& buffer, float value) {
    uint8_t bytes[4];
    std::memcpy(bytes, &value, 4);
    buffer.insert(buffer.end(), bytes, bytes + 4);
}

void append_string(std::vector<uint8_t>& buffer, const std::string& str) {
    buffer.insert(buffer.end(), str.begin(), str.end());
}

// SPR section
void build_spr_section(std::vector<uint8_t>& buffer, const std::vector<Sprite>& sprites) {
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
void build_sprd_section(std::vector<uint8_t>& buffer, const std::vector<Sprite>& sprites) {
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
void build_play_section(std::vector<uint8_t>& buffer) {
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

// GROU Section
void build_grou_section(std::vector<uint8_t>& buffer) {
    std::vector<uint8_t> header;
    append_uint32(header, OPTIONS);
    append_uint32(header, 0); // packedDiff
    append_uint32(header, 1); // subresourceCount
    append_uint32(header, 4); // groupInfoSize
    
    uint32_t grou_size = static_cast<uint32_t>(header.size() + 4); // header + terminator
    
    append_string(buffer, "GROU");
    append_uint32(buffer, grou_size);
    buffer.insert(buffer.end(), header.begin(), header.end());
    append_uint32(buffer, TERMINATOR);
}

} // namespace

bool write_map(const std::vector<Sprite>& sprites, 
               const std::string& output_path, 
               float map_size_x, 
               float map_size_y,
               const std::string& template_path) {
    
    // 1. read empty.map template
    std::ifstream tpl_file(template_path, std::ios::binary);
    if (!tpl_file) {
        Logger::error("Failed to open template file: {}", template_path);
        return false;
    }
    
    std::vector<uint8_t> template_data((std::istreambuf_iterator<char>(tpl_file)),
                                        std::istreambuf_iterator<char>());
                                        
    if (template_data.size() < 56) {
        Logger::error("Template file is too small or corrupted: {}", template_path);
        return false;
    }
    
    // 2. Extract the GRPH (starting at offset 12, 44 bytes in length).
    std::vector<uint8_t> grph_section(template_data.begin() + 12, template_data.begin() + 56);
    
    // 3. construct HEAD
    std::vector<uint8_t> head_header;
    append_uint32(head_header, OPTIONS);
    append_uint32(head_header, 0);
    append_uint32(head_header, 1);
    append_uint32(head_header, 24); // mapInfoSize
    
    std::vector<uint8_t> head_body;
    append_float(head_body, map_size_x);
    append_float(head_body, map_size_y);
    append_float(head_body, 0.0f); // mapCameraX
    append_float(head_body, 0.0f); // mapCameraY
    append_uint32(head_body, 0);   // currentTime
    append_int32(head_body, MAP_VERSION);
    
    uint32_t head_size = static_cast<uint32_t>(head_header.size() + head_body.size());
    std::vector<uint8_t> head_section;
    append_string(head_section, "HEAD");
    append_uint32(head_section, head_size);
    head_section.insert(head_section.end(), head_header.begin(), head_header.end());
    head_section.insert(head_section.end(), head_body.begin(), head_body.end());
    
    // 4. construct other
    std::vector<uint8_t> spr_section;
    build_spr_section(spr_section, sprites);
    
    std::vector<uint8_t> sprd_section;
    build_sprd_section(sprd_section, sprites);
    
    std::vector<uint8_t> play_section;
    build_play_section(play_section);
    
    std::vector<uint8_t> grou_section;
    build_grou_section(grou_section);
    
    // 5. connect all Payload
    std::vector<uint8_t> payload;
    payload.insert(payload.end(), grph_section.begin(), grph_section.end());
    payload.insert(payload.end(), head_section.begin(), head_section.end());
    payload.insert(payload.end(), spr_section.begin(), spr_section.end());
    payload.insert(payload.end(), sprd_section.begin(), sprd_section.end());
    payload.insert(payload.end(), play_section.begin(), play_section.end());
    payload.insert(payload.end(), grou_section.begin(), grou_section.end());
    
    // 6. re-fill FileHeader
    uint32_t final_map_size = static_cast<uint32_t>(4 + payload.size()); // "MAP " + payload
    
    std::vector<uint8_t> final_data;
    append_string(final_data, "RES ");
    append_uint32(final_data, final_map_size);
    append_string(final_data, "MAP ");
    final_data.insert(final_data.end(), payload.begin(), payload.end());
    
    // 7. write into file
    std::ofstream out_file(output_path, std::ios::binary);
    if (!out_file) {
        Logger::error("Failed to open output file: {}", output_path);
        return false;
    }
    
    out_file.write(reinterpret_cast<const char*>(final_data.data()), final_data.size());
    
    Logger::info("Written {} bytes to {}, sprites count: {}", final_data.size(), output_path, sprites.size());
    return true;
}

} // namespace auto_mapper::io
