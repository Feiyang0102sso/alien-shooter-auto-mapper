/**
 * @file res_file.h
 * @brief Sigma Team "RES " container shared by .vid files, objects.res and .map files.
 *
 * Layout (little-endian):
 *   "RES " u32 rootSize(file size - 8) u32 rootType("VID ", "DATA", ...)
 *   records from offset 12, each 2-byte aligned:
 *     u32 tag, u32 size (bytes after these 8), u32 options
 *     options & 0x80000000: u32 packedDiff, u32 subCount follow; else options is subCount
 *     subCount x { u32 subSize, payload[subSize] }
 */

#pragma once
#include <cstdint>
#include <filesystem>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace auto_mapper::game_assets {

using ByteSpan = std::span<const std::uint8_t>;

// One top-level record; subrecord spans point into the owning ResFile buffer.
struct ResRecord {
    std::string tag;
    std::vector<ByteSpan> subrecords;
};

/**
 * Whole-file RES container. Records keep spans into the internal buffer, so the
 * object is move-only (moving a vector keeps its buffer address).
 */
class ResFile {
  public:
    ResFile() = default;
    ResFile(const ResFile &) = delete;
    ResFile &operator=(const ResFile &) = delete;
    ResFile(ResFile &&) = default;
    ResFile &operator=(ResFile &&) = default;

    // Read and parse a file; false (with a log line) when it is missing or malformed.
    bool load(const std::filesystem::path &path);
    // Parse bytes already in memory (used by tests).
    bool parse(std::vector<std::uint8_t> bytes);

    const std::string &root_type() const { return root_type_; }
    const std::vector<ResRecord> &records() const { return records_; }
    // First record with this 4-character tag (e.g. "PAL "), or nullptr.
    const ResRecord *find(std::string_view tag) const;

  private:
    std::vector<std::uint8_t> bytes_;
    std::string root_type_;
    std::vector<ResRecord> records_;
};

} // namespace auto_mapper::game_assets
