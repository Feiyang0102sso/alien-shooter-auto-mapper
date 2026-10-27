/**
 * @file decoration_library.cpp
 * @brief Reads library map regions (SPR + SPRD) into Decoration Stamps.
 */

#include "auto_mapper/core/decoration_builder/decoration_library.h"
#include "auto_mapper/common/logger.h"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iterator>
#include <map>
#include <optional>

namespace auto_mapper::core::decoration_builder {
namespace {

// Region object in AS2R / Conscription objects.res (sprite class 23, "Регион").
constexpr int32_t REGION_VID = 99;

// RES container: "RES " + u32 size + root type, then records.
constexpr std::size_t RES_HEADER_SIZE = 12;
constexpr std::size_t RES_RECORD_HEADER_SIZE = 12;
// Record option flag: packedDiff and subrecord count follow the options word.
constexpr uint32_t RES_EXTENDED_HEADER_FLAG = 0x80000000u;

// AS2 SPR sprite record: objectId, vid, posX, posY, posZ, direction, army, VidGamma[8], scale[3].
constexpr std::size_t SPRITE_RECORD_SIZE = 48;
// REGION::Action save block at the end of the region's SPRD subrecord; width/height at +16.
constexpr std::size_t REGION_TAIL_SIZE = 80;
constexpr std::size_t REGION_WIDTH_OFFSET = 16;

using Bytes = std::vector<uint8_t>;

struct ResSubrecord {
    std::size_t offset = 0;
    std::size_t size = 0;
};

struct MapSprite {
    uint32_t object_id = 0;
    DecorationMember member;
    float pos_x = 0.0f;
    float pos_y = 0.0f;
};

struct Region {
    uint32_t object_id = 0;
    float center_x = 0.0f;
    float center_y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;
};

/**
 * UTF-8 text of a path for logs and ids; path::string() throws on characters outside the ANSI code page.
 */
std::string path_text(const std::filesystem::path& path) {
    std::u8string text = path.u8string();
    return std::string(text.begin(), text.end());
}

uint32_t read_u32(const Bytes& bytes, std::size_t offset) {
    uint32_t value = 0;
    std::memcpy(&value, bytes.data() + offset, sizeof(value));
    return value;
}

int32_t read_i32(const Bytes& bytes, std::size_t offset) {
    int32_t value = 0;
    std::memcpy(&value, bytes.data() + offset, sizeof(value));
    return value;
}

float read_f32(const Bytes& bytes, std::size_t offset) {
    float value = 0.0f;
    std::memcpy(&value, bytes.data() + offset, sizeof(value));
    return value;
}

std::optional<Bytes> read_file(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        return std::nullopt;
    }
    Bytes bytes((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return bytes;
}

/**
 * Subrecords of every top-level record, keyed by the 4-character tag.
 * Returns nullopt when the container structure runs past the end of the file.
 */
std::optional<std::map<std::string, std::vector<ResSubrecord>>> read_records(const Bytes& bytes) {
    if (bytes.size() < RES_HEADER_SIZE || std::memcmp(bytes.data(), "RES ", 4) != 0) {
        return std::nullopt;
    }

    std::map<std::string, std::vector<ResSubrecord>> records;
    std::size_t position = RES_HEADER_SIZE;
    while (position + RES_RECORD_HEADER_SIZE <= bytes.size()) {
        std::string tag(reinterpret_cast<const char*>(bytes.data() + position), 4);
        uint32_t record_size = read_u32(bytes, position + 4);
        uint32_t options = read_u32(bytes, position + 8);
        std::size_t record_end = position + 8 + record_size;
        if (record_end > bytes.size()) {
            return std::nullopt;
        }

        std::size_t body = position + RES_RECORD_HEADER_SIZE;
        uint32_t subrecord_count = options;
        if ((options & RES_EXTENDED_HEADER_FLAG) != 0) {
            subrecord_count = read_u32(bytes, body + 4);
            body += 8;
        }

        std::vector<ResSubrecord>& subrecords = records[tag];
        for (uint32_t index = 0; index < subrecord_count; ++index) {
            if (body + 4 > record_end) {
                return std::nullopt;
            }
            ResSubrecord subrecord;
            subrecord.size = read_u32(bytes, body);
            subrecord.offset = body + 4;
            if (subrecord.offset + subrecord.size > record_end) {
                return std::nullopt;
            }
            subrecords.push_back(subrecord);
            body = subrecord.offset + subrecord.size;
        }

        // Records are 2-byte aligned.
        position = record_end + (record_end & 1);
    }
    return records;
}

/**
 * VidGamma bytes are [negB, negG, negR, negA, posB, posG, posR, posA] (see as2_map_writer.cpp).
 */
VidGamma read_gamma(const Bytes& bytes, std::size_t offset) {
    VidGamma gamma;
    gamma.b = bytes[offset + 4] - bytes[offset + 0];
    gamma.g = bytes[offset + 5] - bytes[offset + 1];
    gamma.r = bytes[offset + 6] - bytes[offset + 2];
    gamma.a = bytes[offset + 7] - bytes[offset + 3];
    return gamma;
}

std::vector<MapSprite> read_sprites(const Bytes& bytes, const ResSubrecord& subrecord) {
    std::vector<MapSprite> sprites;
    // The subrecord ends with a 0xFFFFFFFF terminator that is shorter than one record.
    for (std::size_t offset = subrecord.offset; offset + SPRITE_RECORD_SIZE <= subrecord.offset + subrecord.size;
         offset += SPRITE_RECORD_SIZE) {
        MapSprite sprite;
        sprite.object_id = read_u32(bytes, offset);
        sprite.member.vid = read_i32(bytes, offset + 4);
        sprite.pos_x = read_f32(bytes, offset + 8);
        sprite.pos_y = read_f32(bytes, offset + 12);
        sprite.member.pos_z = read_f32(bytes, offset + 16);
        sprite.member.direction = read_u32(bytes, offset + 20);
        sprite.member.army = read_i32(bytes, offset + 24);
        sprite.member.gamma = read_gamma(bytes, offset + 28);
        sprite.member.scale.x = read_f32(bytes, offset + 36);
        sprite.member.scale.y = read_f32(bytes, offset + 40);
        sprite.member.scale.z = read_f32(bytes, offset + 44);
        sprites.push_back(sprite);
    }
    return sprites;
}

/**
 * Region sizes live in the SPRD tail, keyed by object id.
 */
std::map<uint32_t, ResSubrecord> sprd_by_object_id(const Bytes& bytes, const std::vector<ResSubrecord>& subrecords) {
    std::map<uint32_t, ResSubrecord> result;
    for (const ResSubrecord& subrecord : subrecords) {
        if (subrecord.size < 4) {
            continue;
        }
        result[read_u32(bytes, subrecord.offset)] = subrecord;
    }
    return result;
}

bool contains(const Region& region, float x, float y) {
    float half_width = region.width / 2.0f;
    float half_height = region.height / 2.0f;
    bool inside_x = x >= region.center_x - half_width && x <= region.center_x + half_width;
    bool inside_y = y >= region.center_y - half_height && y <= region.center_y + half_height;
    return inside_x && inside_y;
}

} // namespace

int DecorationLibrary::load_directory(const std::filesystem::path& directory) {
    if (!std::filesystem::is_directory(directory)) {
        Logger::warning("Decoration library directory not found: {}", path_text(directory));
        return 0;
    }

    std::vector<std::filesystem::path> map_paths;
    for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file() && entry.path().extension() == ".map") {
            map_paths.push_back(entry.path());
        }
    }
    std::sort(map_paths.begin(), map_paths.end());

    std::size_t stamp_count_before = stamps_.size();
    for (const std::filesystem::path& map_path : map_paths) {
        load_map(map_path);
    }
    int loaded = static_cast<int>(stamps_.size() - stamp_count_before);
    Logger::info("Decoration library: {} stamps from {}", loaded, path_text(directory));
    return loaded;
}

bool DecorationLibrary::load_map(const std::filesystem::path& map_path) {
    std::optional<Bytes> bytes = read_file(map_path);
    if (!bytes) {
        Logger::warning("Cannot read decoration map: {}", path_text(map_path));
        return false;
    }
    std::optional<std::map<std::string, std::vector<ResSubrecord>>> records = read_records(*bytes);
    if (!records || !records->contains("SPR ") || (*records)["SPR "].empty() || !records->contains("SPRD")) {
        Logger::warning("Malformed decoration map: {}", path_text(map_path));
        return false;
    }

    std::vector<MapSprite> sprites = read_sprites(*bytes, (*records)["SPR "].front());
    std::map<uint32_t, ResSubrecord> sprd = sprd_by_object_id(*bytes, (*records)["SPRD"]);

    // Regions keep their SPR order: that order numbers the stamp ids.
    std::vector<Region> regions;
    for (const MapSprite& sprite : sprites) {
        if (sprite.member.vid != REGION_VID) {
            continue;
        }
        auto found = sprd.find(sprite.object_id);
        if (found == sprd.end() || found->second.size < REGION_TAIL_SIZE + 4) {
            Logger::warning("Region {} has no size data in {}", sprite.object_id, path_text(map_path));
            continue;
        }
        std::size_t tail = found->second.offset + found->second.size - REGION_TAIL_SIZE;
        Region region;
        region.object_id = sprite.object_id;
        region.center_x = sprite.pos_x;
        region.center_y = sprite.pos_y;
        region.width = read_f32(*bytes, tail + REGION_WIDTH_OFFSET);
        region.height = read_f32(*bytes, tail + REGION_WIDTH_OFFSET + 4);
        regions.push_back(region);
    }

    std::string series_id = path_text(map_path.stem());
    std::vector<DecorationStamp> map_stamps(regions.size());
    for (std::size_t index = 0; index < regions.size(); ++index) {
        map_stamps[index].id = series_id + ":" + std::to_string(index + 1);
        map_stamps[index].series_id = series_id;
        map_stamps[index].width = regions[index].width;
        map_stamps[index].height = regions[index].height;
    }

    // Each sprite joins the first region (SPR order) whose rectangle contains its anchor.
    for (const MapSprite& sprite : sprites) {
        if (sprite.member.vid == REGION_VID) {
            continue;
        }
        int owner = -1;
        for (std::size_t index = 0; index < regions.size(); ++index) {
            if (!contains(regions[index], sprite.pos_x, sprite.pos_y)) {
                continue;
            }
            if (owner < 0) {
                owner = static_cast<int>(index);
            } else {
                Logger::warning("Sprite vid {} at ({}, {}) is inside regions {} and {}; kept in {}",
                                sprite.member.vid, sprite.pos_x, sprite.pos_y, owner + 1, index + 1, owner + 1);
            }
        }
        if (owner < 0) {
            Logger::warning("Sprite vid {} at ({}, {}) is outside every region of {}",
                            sprite.member.vid, sprite.pos_x, sprite.pos_y, path_text(map_path));
            continue;
        }

        DecorationMember member = sprite.member;
        member.offset_x = sprite.pos_x - regions[owner].center_x;
        member.offset_y = sprite.pos_y - regions[owner].center_y;
        map_stamps[owner].members.push_back(member);
    }

    int registered = 0;
    for (DecorationStamp& stamp : map_stamps) {
        if (stamp.members.empty()) {
            Logger::warning("Empty decoration region skipped: {}", stamp.id);
            continue;
        }
        stamps_.push_back(std::move(stamp));
        ++registered;
    }
    if (registered > 0) {
        series_.push_back(series_id);
    }
    Logger::info("Decoration map {}: {} regions, {} stamps", path_text(map_path.filename()), regions.size(), registered);
    return true;
}

const std::vector<DecorationStamp>& DecorationLibrary::stamps() const {
    return stamps_;
}

const std::vector<std::string>& DecorationLibrary::series() const {
    return series_;
}

const DecorationStamp* DecorationLibrary::find(std::string_view stamp_id) const {
    for (const DecorationStamp& stamp : stamps_) {
        if (stamp.id == stamp_id) {
            return &stamp;
        }
    }
    return nullptr;
}

} // namespace auto_mapper::core::decoration_builder
