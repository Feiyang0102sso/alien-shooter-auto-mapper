/**
 * @file vid_library.h
 * @brief One game folder's sprites: objects.res lookup plus cached, decoded vid frames.
 */

#pragma once
#include "game_assets/game_family.h"
#include "game_assets/objects/object_table.h"
#include "game_assets/vid/vid_file.h"

#include <cstdint>
#include <filesystem>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <utility>

namespace auto_mapper::game_assets {

/**
 * Game root layout: <root>/objects.res and <root>/Vid/*.vid.
 * Every vid file and decoded frame is loaded once and kept for the library lifetime,
 * so returned pointers stay valid until the library is reopened or destroyed.
 */
class VidLibrary {
  public:
    // Load objects.res from the game root; false (preview off) when it is missing.
    bool open(const std::filesystem::path &game_root, GameFamily family);
    bool is_open() const { return open_; }
    GameFamily family() const { return family_; }

    // Decoded frame of one map sprite, or nullptr for unknown nvids and unsupported vids.
    const VidImage *frame(int nvid, std::uint32_t direction);

  private:
    // Parsed vid file for an objects.res path; nullptr is cached for missing / unsupported files.
    const VidFile *vid_file(const std::string &vid_path);

    std::filesystem::path root_;
    GameFamily family_ = GameFamily::AS1;
    bool open_ = false;
    ObjectTable objects_;
    std::map<std::string, std::unique_ptr<VidFile>> files_;
    // (nvid, frame) -> decoded image; std::nullopt remembers a failed decode.
    std::map<std::pair<int, int>, std::optional<VidImage>> frames_;
};

} // namespace auto_mapper::game_assets
