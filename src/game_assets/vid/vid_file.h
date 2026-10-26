/**
 * @file vid_file.h
 * @brief Decode one frame of a .vid sprite into RGBA for the editor preview.
 *
 * Supported layouts (enough for walls, doors, floors, ceilings and decorations):
 *   AS1  HEAD + PAL + DATA, uncompressed software frames (types 0x19 / 0x1B / 0x1D / 0x11).
 *   AS2  HEAD + [PAL] + CADR, linear RLE frames in P8 / 8888 / 888 / 565 / 1555 / 4444.
 * Compressed AS1 hardware vids (HEAD + SURF + DATA) and DXT vids are reported as unsupported.
 */

#pragma once
#include "game_assets/game_family.h"
#include "game_assets/res/res_file.h"

#include <array>
#include <cstdint>
#include <filesystem>
#include <vector>

namespace auto_mapper::game_assets {

/**
 * One decoded frame. `left` / `top` place the image relative to the sprite's
 * Map Coordinate anchor (posX, posY - posZ): the engine centers the vid box on it.
 */
struct VidImage {
    int width = 0;
    int height = 0;
    int left = 0;
    int top = 0;
    std::vector<std::uint8_t> rgba; // width * height * 4, straight alpha
};

/**
 * Hide what a sunk sprite pushes below the ground, e.g. a jammed or open AS1 door panel (posZ < 0).
 * The engine hides those pixels with its z-buffer; the preview cuts every column `sink` pixels above
 * the lowest opaque pixel the frame has there, which is where the sprite meets the ground at posZ = 0.
 */
VidImage clip_below_ground(const VidImage &image, int sink);

class VidFile {
  public:
    bool load(const std::filesystem::path &path);
    // Parse a RES blob already in memory (used by tests).
    bool parse(std::vector<std::uint8_t> bytes);

    bool supported() const { return supported_; }
    int frame_count() const { return frame_count_; }
    int box_width() const { return box_width_; }
    int box_height() const { return box_height_; }

    /**
     * First frame of animation 0 for a sprite direction (0..255, 0 = up, clockwise), as the engine:
     * RealDirection(direction, direction_count) * frames_per_direction.
     * Counts come from objects.res; zero falls back to one frame per direction over the whole file.
     */
    int direction_frame(std::uint32_t direction, int direction_count = 0, int frames_per_direction = 0) const;

    // Decode one frame and apply the object's gamma; false for unsupported or broken data.
    bool decode_frame(int frame, const ColorGamma &gamma, VidImage &out) const;

  private:
    // Read HEAD and the palette after the container is parsed; sets supported_.
    bool parse_head();
    bool decode_as1_frame(int frame, int alias_depth, VidImage &out) const;
    bool decode_as2_frame(int frame, int alias_depth, VidImage &out) const;

    ResFile res_;
    GameFamily family_ = GameFamily::AS1;
    bool supported_ = false;
    std::uint16_t type_ = 0;
    int frame_count_ = 0;
    int box_width_ = 0;
    int box_height_ = 0;
    std::uint32_t pixel_format_ = 0; // AS2 only: D3DFORMAT of the CADR pixels
    bool has_palette_ = false;
    std::array<std::uint32_t, 256> palette_{}; // 0xAARRGGBB
    const ResRecord *frames_ = nullptr;        // AS1 DATA or AS2 CADR
};

} // namespace auto_mapper::game_assets
