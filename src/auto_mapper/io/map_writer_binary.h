/**
 * @file map_writer_binary.h
 * @brief shared helpers for little-endian .map binary writing
 */

#pragma once

#include "auto_mapper/common/logger.h"

#include <cstdint>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>

namespace auto_mapper::io::detail {

constexpr uint32_t OPTIONS = 0x80000000;
constexpr uint32_t TERMINATOR = 0xFFFFFFFF;

// helper - add to ByteBuffer with Little-Endian
inline void append_uint32(std::vector<uint8_t>& buffer, uint32_t value) {
    uint8_t bytes[4];
    std::memcpy(bytes, &value, 4);
    buffer.insert(buffer.end(), bytes, bytes + 4);
}

inline void append_int32(std::vector<uint8_t>& buffer, int32_t value) {
    uint8_t bytes[4];
    std::memcpy(bytes, &value, 4);
    buffer.insert(buffer.end(), bytes, bytes + 4);
}

inline void append_float(std::vector<uint8_t>& buffer, float value) {
    uint8_t bytes[4];
    std::memcpy(bytes, &value, 4);
    buffer.insert(buffer.end(), bytes, bytes + 4);
}

inline void append_string(std::vector<uint8_t>& buffer, const std::string& str) {
    buffer.insert(buffer.end(), str.begin(), str.end());
}

inline void build_head_section(
    std::vector<uint8_t>& buffer,
    float map_size_x,
    float map_size_y,
    int32_t map_version
) {
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
    append_int32(head_body, map_version);

    uint32_t head_size = static_cast<uint32_t>(head_header.size() + head_body.size());
    append_string(buffer, "HEAD");
    append_uint32(buffer, head_size);
    buffer.insert(buffer.end(), head_header.begin(), head_header.end());
    buffer.insert(buffer.end(), head_body.begin(), head_body.end());
}

// GROU Section
inline void build_grou_section(std::vector<uint8_t>& buffer) {
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

inline bool write_binary_file(
    const std::vector<uint8_t>& final_data,
    const std::string& output_path,
    size_t sprite_count
) {
    std::ofstream out_file(output_path, std::ios::binary);
    if (!out_file) {
        Logger::error("Failed to open output file: {}", output_path);
        return false;
    }

    out_file.write(reinterpret_cast<const char*>(final_data.data()), final_data.size());

    Logger::info("Written {} bytes to {}, sprites count: {}", final_data.size(), output_path, sprite_count);
    return true;
}

inline bool write_payload_to_map_file(
    const std::vector<uint8_t>& payload,
    const std::string& output_path,
    size_t sprite_count
) {
    uint32_t final_map_size = static_cast<uint32_t>(4 + payload.size()); // "MAP " + payload

    std::vector<uint8_t> final_data;
    append_string(final_data, "RES ");
    append_uint32(final_data, final_map_size);
    append_string(final_data, "MAP ");
    final_data.insert(final_data.end(), payload.begin(), payload.end());

    return write_binary_file(final_data, output_path, sprite_count);
}

} // namespace auto_mapper::io::detail
