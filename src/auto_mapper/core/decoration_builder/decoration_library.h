/**
 * @file decoration_library.h
 * @brief Decoration Stamps registered from the regions of AS2 Series library maps.
 *
 * A library map is an ordinary map authored in the official AS2 editor. Each
 * region sprite (nvid 99) marks one stamp; every other sprite belongs to the
 * region whose rectangle contains its (posX, posY).
 */

#pragma once

#include "auto_mapper/core/decoration_builder/decoration_stamp.h"

#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace auto_mapper::core::decoration_builder {

class DecorationLibrary {
public:
    /**
     * Load every *.map in `directory`, in file name order.
     * Returns the number of stamps registered from that directory.
     */
    int load_directory(const std::filesystem::path& directory);

    /**
     * Register the regions of one library map. Returns false when the file is unreadable or malformed.
     */
    bool load_map(const std::filesystem::path& map_path);

    /**
     * Every stamp, grouped by series in load order, regions in SPR order.
     */
    const std::vector<DecorationStamp>& stamps() const;

    /**
     * Series ids (library map file stems) in load order.
     */
    const std::vector<std::string>& series() const;

    /**
     * Look up a stamp by id. Returns nullptr when unknown.
     */
    const DecorationStamp* find(std::string_view stamp_id) const;

private:
    std::vector<DecorationStamp> stamps_;
    std::vector<std::string> series_;
};

} // namespace auto_mapper::core::decoration_builder
