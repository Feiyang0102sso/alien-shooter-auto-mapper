/**
 * @file object_table.cpp
 * @brief objects.res OBJ record reader.
 */

#include "game_assets/objects/object_table.h"
#include "auto_mapper/common/logger.h"
#include "game_assets/res/byte_reader.h"
#include "game_assets/res/res_file.h"

#include <algorithm>

namespace auto_mapper::game_assets {
namespace {

/**
 * Offsets inside the parameter block that follows the object name (VID::LoadParameters).
 * AS1 has 24 leading dwords, AS2 adds randomSpeed / randomZSpeed / deathDamageMax / deathPush.
 * After noDir come 8 int[17] arrays, then GammaRGBA int[4], ScaleXYZ float[3] and the vid path.
 */
constexpr std::size_t AS1_DIRECTION_COUNT_OFFSET = 112;
constexpr std::size_t AS1_GAMMA_OFFSET = 660;
constexpr std::size_t AS1_VID_PATH_OFFSET = 688;
constexpr std::size_t AS2_DIRECTION_COUNT_OFFSET = 128;
constexpr std::size_t AS2_GAMMA_OFFSET = 676;
constexpr std::size_t AS2_VID_PATH_OFFSET = 704;

} // namespace

bool ObjectTable::load(const std::filesystem::path &objects_res, GameFamily family) {
    entries_.clear();
    ResFile res;
    if (!res.load(objects_res)) {
        return false;
    }
    const ResRecord *objects = res.find("OBJ ");
    if (objects == nullptr) {
        Logger::warning("objects.res has no OBJ record: {}", objects_res.string());
        return false;
    }

    std::size_t direction_count_offset = AS1_DIRECTION_COUNT_OFFSET;
    std::size_t gamma_offset = AS1_GAMMA_OFFSET;
    std::size_t vid_path_offset = AS1_VID_PATH_OFFSET;
    if (family == GameFamily::AS2) {
        direction_count_offset = AS2_DIRECTION_COUNT_OFFSET;
        gamma_offset = AS2_GAMMA_OFFSET;
        vid_path_offset = AS2_VID_PATH_OFFSET;
    }

    for (ByteSpan subrecord : objects->subrecords) {
        ByteReader reader(subrecord);
        ObjectEntry entry;
        entry.nvid = reader.i32();
        reader.c_string(); // display name, not needed for preview
        std::size_t block_start = reader.position();

        // noDir, then noAnimCadr[17]; slot 0 is the default animation a map sprite shows.
        reader.skip(direction_count_offset);
        entry.direction_count = reader.i32();
        entry.frames_per_direction = reader.i32();

        reader.skip(gamma_offset - direction_count_offset - 8);
        entry.gamma.red = reader.i32();
        entry.gamma.green = reader.i32();
        entry.gamma.blue = reader.i32();

        ByteReader path_reader(subrecord.subspan(std::min(subrecord.size(), block_start + vid_path_offset)));
        entry.vid_path = path_reader.c_string();
        if (!reader.ok() || entry.vid_path.empty()) {
            Logger::debug("Skip malformed OBJ entry nvid={}", entry.nvid);
            continue;
        }
        entries_[entry.nvid] = entry;
    }
    Logger::info("Loaded {} objects from {}", entries_.size(), objects_res.string());
    return !entries_.empty();
}

const ObjectEntry *ObjectTable::find(int nvid) const {
    auto found = entries_.find(nvid);
    if (found == entries_.end()) {
        return nullptr;
    }
    return &found->second;
}

} // namespace auto_mapper::game_assets
