/**
 * @file object_table.h
 * @brief nvid -> vid file + color gamma, read from the game's objects.res.
 *
 * objects.res is a RES container (root "DATA") whose "OBJ " record holds one subrecord per object:
 *   i32 nvid, NUL-terminated name, fixed parameter block, NUL-terminated vid path
 * The parameter block differs per engine generation (see object_table.cpp offsets).
 */

#pragma once
#include "game_assets/game_family.h"

#include <filesystem>
#include <map>
#include <string>

namespace auto_mapper::game_assets {

struct ObjectEntry {
    int nvid = 0;
    // Path relative to the game root as stored in objects.res, e.g. "vid\601.vid".
    std::string vid_path;
    // noDir and noAnimCadr[0]: frames are grouped per direction, animation 0 comes first.
    // Several nvids may share one vid file with different counts (AS1 611 uses 607.vid with 2 of its 4).
    int direction_count = 0;
    int frames_per_direction = 0;
    ColorGamma gamma;
};

class ObjectTable {
  public:
    bool load(const std::filesystem::path &objects_res, GameFamily family);
    // Entry for a map sprite nvid, or nullptr when the game does not define it.
    const ObjectEntry *find(int nvid) const;
    std::size_t size() const { return entries_.size(); }

  private:
    std::map<int, ObjectEntry> entries_;
};

} // namespace auto_mapper::game_assets
