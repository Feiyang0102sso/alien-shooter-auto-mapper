/**
 * @file vid_library.cpp
 * @brief Game folder sprite lookup and frame cache.
 */

#include "game_assets/vid/vid_library.h"
#include "auto_mapper/common/logger.h"

#include <algorithm>
#include <cctype>

namespace auto_mapper::game_assets {
namespace {

constexpr const char *OBJECTS_FILE_NAME = "objects.res";
constexpr const char *VID_EXTENSION = ".vid"; // compared after the path key is lowercased

// objects.res stores Windows paths such as "vid\601.vid"; normalize for the cache key.
std::string normalized_vid_path(const std::string &vid_path) {
    std::string key = vid_path;
    for (char &c : key) {
        if (c == '\\') {
            c = '/';
        }
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return key;
}

} // namespace

bool VidLibrary::open(const std::filesystem::path &game_root, GameFamily family) {
    root_ = game_root;
    family_ = family;
    files_.clear();
    frames_.clear();
    open_ = objects_.load(game_root / OBJECTS_FILE_NAME, family);
    if (!open_) {
        Logger::warning("Sprite preview disabled, no usable objects.res under {}", game_root.string());
    }
    return open_;
}

const VidImage *VidLibrary::frame(int nvid, std::uint32_t direction) {
    if (!open_) {
        return nullptr;
    }
    const ObjectEntry *object = objects_.find(nvid);
    if (object == nullptr) {
        return nullptr;
    }
    const VidFile *file = vid_file(object->vid_path);
    if (file == nullptr) {
        return nullptr;
    }

    int frame_index = file->direction_frame(direction, object->direction_count, object->frames_per_direction);
    std::pair<int, int> key(nvid, frame_index);
    auto cached = frames_.find(key);
    if (cached == frames_.end()) {
        std::optional<VidImage> image;
        VidImage decoded;
        if (file->decode_frame(frame_index, object->gamma, decoded)) {
            image = std::move(decoded);
        } else {
            Logger::warning("Cannot decode vid frame nvid={} frame={}", nvid, frame_index);
        }
        cached = frames_.emplace(key, std::move(image)).first;
    }
    if (!cached->second) {
        return nullptr;
    }
    return &*cached->second;
}

const VidFile *VidLibrary::vid_file(const std::string &vid_path) {
    std::string key = normalized_vid_path(vid_path);
    auto cached = files_.find(key);
    if (cached != files_.end()) {
        return cached->second.get();
    }

    auto file = std::make_unique<VidFile>();
    std::filesystem::path path = root_ / std::filesystem::path(key);
    if (path.extension() != VID_EXTENSION) {
        // Fonts and .bmp / .tga / .jpg objects are converted by the engine at runtime; not previewed.
        Logger::debug("Object source is not a vid file: {}", path.string());
        file.reset();
    } else if (!file->load(path)) {
        file.reset();
    } else if (!file->supported()) {
        Logger::info("Vid format not previewed: {}", path.string());
        file.reset();
    }
    const VidFile *result = file.get();
    files_.emplace(key, std::move(file));
    return result;
}

} // namespace auto_mapper::game_assets
