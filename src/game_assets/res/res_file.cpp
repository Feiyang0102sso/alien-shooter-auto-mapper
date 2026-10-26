/**
 * @file res_file.cpp
 * @brief RES container reader.
 */

#include "game_assets/res/res_file.h"
#include "auto_mapper/common/logger.h"
#include "game_assets/res/byte_reader.h"

#include <fstream>
#include <iterator>

namespace auto_mapper::game_assets {
namespace {

constexpr std::size_t FILE_HEADER_SIZE = 12;
constexpr std::size_t RECORD_HEADER_SIZE = 8;
constexpr std::uint32_t EXTENDED_RECORD_HEADER = 0x80000000u;

std::string read_tag(ByteReader &reader) {
    std::string tag;
    for (int i = 0; i < 4; ++i) {
        tag.push_back(static_cast<char>(reader.u8()));
    }
    return tag;
}

} // namespace

bool ResFile::load(const std::filesystem::path &path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        Logger::warning("RES file not found: {}", path.string());
        return false;
    }
    std::vector<std::uint8_t> bytes((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    if (!parse(std::move(bytes))) {
        Logger::warning("Malformed RES file: {}", path.string());
        return false;
    }
    return true;
}

bool ResFile::parse(std::vector<std::uint8_t> bytes) {
    bytes_ = std::move(bytes);
    root_type_.clear();
    records_.clear();

    ByteReader header(bytes_);
    std::string signature = read_tag(header);
    header.u32(); // root size, implied by the file length
    root_type_ = read_tag(header);
    if (!header.ok() || (signature != "RES " && signature != "RIFF")) {
        return false;
    }

    std::size_t position = FILE_HEADER_SIZE;
    while (position + RECORD_HEADER_SIZE <= bytes_.size()) {
        ByteReader reader(ByteSpan(bytes_).subspan(position));
        ResRecord record;
        record.tag = read_tag(reader);
        std::uint32_t size = reader.u32();
        std::size_t record_end = position + RECORD_HEADER_SIZE + size;
        if (record_end > bytes_.size()) {
            return false;
        }

        std::uint32_t count = reader.u32();
        if (count & EXTENDED_RECORD_HEADER) {
            reader.u32(); // packed diff, only meaningful for the writer
            count = reader.u32();
        }
        for (std::uint32_t i = 0; i < count; ++i) {
            std::uint32_t sub_size = reader.u32();
            std::size_t sub_begin = position + reader.position();
            if (!reader.ok() || sub_begin + sub_size > record_end) {
                return false;
            }
            record.subrecords.push_back(ByteSpan(bytes_).subspan(sub_begin, sub_size));
            reader.skip(sub_size);
        }
        records_.push_back(std::move(record));

        // Records are 2-byte aligned.
        position = record_end + (record_end & 1);
    }
    return true;
}

const ResRecord *ResFile::find(std::string_view tag) const {
    for (const ResRecord &record : records_) {
        if (record.tag == tag) {
            return &record;
        }
    }
    return nullptr;
}

} // namespace auto_mapper::game_assets
