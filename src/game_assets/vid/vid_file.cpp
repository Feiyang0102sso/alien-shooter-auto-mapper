/**
 * @file vid_file.cpp
 * @brief AS1 / AS2 single-frame VID decoder, reconstructed from the Gen1 / Gen2 engine sources.
 */

#include "game_assets/vid/vid_file.h"
#include "game_assets/res/byte_reader.h"

namespace auto_mapper::game_assets {
namespace {

// HEAD.type bits shared by both engine generations.
constexpr std::uint16_t VID_TYPE_TEXTURE = 0x0001;
constexpr std::uint16_t VID_TYPE_ALPHA = 0x0002;
constexpr std::uint16_t VID_TYPE_ZBUFFER = 0x0004;
constexpr std::uint16_t VID_TYPE_PALETTE = 0x0008;
constexpr std::uint16_t VID_TYPE_NEWVERSION = 0x0010; // AS1: 1024-byte BGRA palette
constexpr std::uint16_t VID_TYPE_HARDWARE = 0x0020;
constexpr std::uint16_t VID_TYPE_LIGHT = 0x0080;
constexpr std::uint16_t VID_TYPE_COMPRESS = 0x0100;
constexpr std::uint16_t VID_TYPE_NEW_ZBUFFER = 0x1000;
// AS2 CADR-only bits.
constexpr std::uint16_t AS2_TYPE_POW2_STRIDE = 0x0010;  // rows are padded to a power of two
constexpr std::uint16_t AS2_TYPE_FRAME_LINK = 0x0200;   // frame header starts with float x, y, z
constexpr std::uint16_t AS2_TYPE_EXTRA_VERTEX = 0x0800; // frame header has additionalDataCount
constexpr std::uint16_t AS2_TYPE_NORMALS = 0x2000;      // +1 byte per pixel
constexpr std::uint16_t AS2_TYPE_MATERIAL = 0x8000;     // +1 byte per pixel

// D3DFORMAT values used by AS2 CADR pixels.
constexpr std::uint32_t D3DFMT_R8G8B8 = 20;
constexpr std::uint32_t D3DFMT_A8R8G8B8 = 21;
constexpr std::uint32_t D3DFMT_X8R8G8B8 = 22;
constexpr std::uint32_t D3DFMT_R5G6B5 = 23;
constexpr std::uint32_t D3DFMT_X1R5G5B5 = 24;
constexpr std::uint32_t D3DFMT_A1R5G5B5 = 25;
constexpr std::uint32_t D3DFMT_A4R4G4B4 = 26;
constexpr std::uint32_t D3DFMT_P8 = 41;

// AS1 DATA frames whose raw size is 2 only hold the index of an earlier frame.
constexpr std::uint32_t AS1_ALIAS_FRAME_SIZE = 2;
// AS2 CADR subrecords this small only hold the index of another frame.
constexpr std::size_t AS2_ALIAS_FRAME_SIZE = 4;
constexpr int MAX_ALIAS_DEPTH = 4;
constexpr int AS1_CONTOUR_POINT_SIZE = 6;
constexpr int AS2_FRAME_LINK_SIZE = 12;
constexpr int AS2_MIN_POW2_STRIDE = 4;

std::uint8_t expand_bits(std::uint32_t value, int bits) {
    std::uint32_t max = (1u << bits) - 1;
    return static_cast<std::uint8_t>(value * 255 / max);
}

std::uint32_t argb(std::uint32_t a, std::uint32_t r, std::uint32_t g, std::uint32_t b) {
    return (a << 24) | (r << 16) | (g << 8) | b;
}

std::uint32_t from_r5g6b5(std::uint32_t v) {
    return argb(255, expand_bits((v >> 11) & 31, 5), expand_bits((v >> 5) & 63, 6), expand_bits(v & 31, 5));
}

void put_pixel(VidImage &image, int x, int y, std::uint32_t color) {
    if (x < 0 || y < 0 || x >= image.width || y >= image.height) {
        return;
    }
    std::size_t offset = (static_cast<std::size_t>(y) * image.width + x) * 4;
    image.rgba[offset + 0] = static_cast<std::uint8_t>(color >> 16); // R
    image.rgba[offset + 1] = static_cast<std::uint8_t>(color >> 8);  // G
    image.rgba[offset + 2] = static_cast<std::uint8_t>(color);       // B
    image.rgba[offset + 3] = static_cast<std::uint8_t>(color >> 24); // A
}

void reset_image(VidImage &image, int width, int height) {
    image.width = width;
    image.height = height;
    image.rgba.assign(static_cast<std::size_t>(width) * height * 4, 0);
}

/**
 * Engine BlendGammaColor: a negative delta multiplies the channel by (256 + delta) / 256,
 * a positive delta adds to it.
 */
std::uint8_t apply_gamma_channel(std::uint8_t value, int delta) {
    if (delta < -255) {
        delta = -255;
    }
    if (delta > 255) {
        delta = 255;
    }
    int result = value;
    if (delta < 0) {
        result = (value * (256 + delta)) >> 8;
    } else {
        result = value + delta;
    }
    if (result > 255) {
        result = 255;
    }
    return static_cast<std::uint8_t>(result);
}

void apply_gamma(VidImage &image, const ColorGamma &gamma) {
    if (gamma.red == 0 && gamma.green == 0 && gamma.blue == 0) {
        return;
    }
    for (std::size_t i = 0; i + 3 < image.rgba.size(); i += 4) {
        if (image.rgba[i + 3] == 0) {
            continue;
        }
        image.rgba[i + 0] = apply_gamma_channel(image.rgba[i + 0], gamma.red);
        image.rgba[i + 1] = apply_gamma_channel(image.rgba[i + 1], gamma.green);
        image.rgba[i + 2] = apply_gamma_channel(image.rgba[i + 2], gamma.blue);
    }
}

int as2_color_bytes(std::uint32_t pixel_format) {
    switch (pixel_format) {
    case D3DFMT_P8:
        return 1;
    case D3DFMT_R5G6B5:
    case D3DFMT_X1R5G5B5:
    case D3DFMT_A1R5G5B5:
    case D3DFMT_A4R4G4B4:
        return 2;
    case D3DFMT_R8G8B8:
        return 3;
    case D3DFMT_A8R8G8B8:
    case D3DFMT_X8R8G8B8:
        return 4;
    default:
        return 0; // DXT and unknown formats are not decoded
    }
}

} // namespace

VidImage clip_below_ground(const VidImage &image, int sink) {
    VidImage clipped = image;
    if (sink <= 0) {
        return clipped;
    }
    for (int x = 0; x < clipped.width; ++x) {
        int ground_row = -1;
        for (int y = clipped.height - 1; y >= 0; --y) {
            std::size_t offset = (static_cast<std::size_t>(y) * clipped.width + x) * 4;
            if (clipped.rgba[offset + 3] != 0) {
                ground_row = y;
                break;
            }
        }
        if (ground_row < 0) {
            continue; // empty column
        }
        for (int y = ground_row - sink + 1; y < clipped.height; ++y) {
            if (y < 0) {
                continue;
            }
            std::size_t offset = (static_cast<std::size_t>(y) * clipped.width + x) * 4;
            clipped.rgba[offset + 0] = 0;
            clipped.rgba[offset + 1] = 0;
            clipped.rgba[offset + 2] = 0;
            clipped.rgba[offset + 3] = 0;
        }
    }
    return clipped;
}

bool VidFile::load(const std::filesystem::path &path) {
    if (!res_.load(path)) {
        return false;
    }
    return parse_head();
}

bool VidFile::parse(std::vector<std::uint8_t> bytes) {
    if (!res_.parse(std::move(bytes))) {
        return false;
    }
    return parse_head();
}

bool VidFile::parse_head() {
    supported_ = false;
    const ResRecord *head = res_.find("HEAD");
    if (head == nullptr || head->subrecords.empty()) {
        return false;
    }
    ByteReader reader(head->subrecords.front());
    type_ = reader.u16();
    reader.u16(); // default frame speed
    frame_count_ = reader.u16();
    box_width_ = reader.i16();
    box_height_ = reader.i16();
    if (!reader.ok()) {
        return false;
    }

    const ResRecord *cadr = res_.find("CADR");
    const ResRecord *data = res_.find("DATA");
    const ResRecord *pal = res_.find("PAL ");
    has_palette_ = (type_ & VID_TYPE_PALETTE) != 0 && pal != nullptr && !pal->subrecords.empty();

    if (cadr != nullptr) {
        family_ = GameFamily::AS2;
        frames_ = cadr;
        pixel_format_ = reader.u32();
        if (!reader.ok() || as2_color_bytes(pixel_format_) == 0) {
            return true; // valid file, unsupported pixel format
        }
        if (pixel_format_ == D3DFMT_P8 && !has_palette_) {
            return true;
        }
        if (has_palette_) {
            ByteReader colors(pal->subrecords.front());
            for (std::uint32_t &color : palette_) {
                color = colors.u32();
            }
        }
        supported_ = true;
        return true;
    }

    // AS1: only uncompressed software frames; SURF / hardware / light vids are skipped.
    if (data == nullptr || res_.find("SURF") != nullptr) {
        return true;
    }
    family_ = GameFamily::AS1;
    frames_ = data;
    std::uint16_t unsupported_bits = VID_TYPE_HARDWARE | VID_TYPE_LIGHT | VID_TYPE_COMPRESS | VID_TYPE_NEW_ZBUFFER;
    if ((type_ & VID_TYPE_TEXTURE) == 0 || (type_ & unsupported_bits) != 0) {
        return true;
    }
    if ((type_ & VID_TYPE_PALETTE) != 0) {
        if (!has_palette_) {
            return true;
        }
        ByteReader colors(pal->subrecords.front());
        for (std::uint32_t &color : palette_) {
            if (type_ & VID_TYPE_NEWVERSION) {
                color = colors.u32(); // stored as B, G, R, A
            } else {
                std::uint32_t r = colors.u8();
                std::uint32_t g = colors.u8();
                std::uint32_t b = colors.u8();
                color = argb(255, r, g, b);
            }
        }
    }
    supported_ = true;
    return true;
}

int VidFile::direction_frame(std::uint32_t direction, int direction_count, int frames_per_direction) const {
    if (direction_count <= 0) {
        direction_count = frame_count_;
        frames_per_direction = 1;
    }
    if (frames_per_direction <= 0) {
        frames_per_direction = 1;
    }
    if (direction_count <= 0) {
        return 0;
    }
    // Engine RealDirection: round to the nearest of noDir sectors, sector 0 centered on "up".
    std::uint32_t rounded = (direction + 128 / direction_count) & 0xFF;
    int sector = static_cast<int>((rounded * direction_count) >> 8);
    int frame = sector * frames_per_direction;
    if (frame >= frame_count_) {
        frame = frame_count_ - 1;
    }
    if (frame < 0) {
        frame = 0;
    }
    return frame;
}

bool VidFile::decode_frame(int frame, const ColorGamma &gamma, VidImage &out) const {
    if (!supported_ || frames_ == nullptr || frame < 0 || frame >= frame_count_) {
        return false;
    }
    bool decoded = false;
    if (family_ == GameFamily::AS1) {
        decoded = decode_as1_frame(frame, 0, out);
    } else {
        decoded = decode_as2_frame(frame, 0, out);
    }
    if (decoded) {
        apply_gamma(out, gamma);
    }
    return decoded;
}

/**
 * AS1 software frame: u32 rawSize, then
 *   i16 contourCount, contourCount x {i16 x, y, z}   shadow outline, skipped
 *   i16 top, i16 rowCount                            top is relative to the vid box
 *   per row: {u8 skip, u8 count, payload} until (0, 0)
 *   payload: PALETTE+ZBUFFER -> count x u16 z then count x u8 index
 *            PALETTE         -> count x u8 index
 *            otherwise       -> count x u16 R5G6B5
 */
bool VidFile::decode_as1_frame(int frame, int alias_depth, VidImage &out) const {
    if (frame >= static_cast<int>(frames_->subrecords.size())) {
        return false;
    }
    ByteReader reader(frames_->subrecords[frame]);
    std::uint32_t raw_size = reader.u32();
    if (raw_size == AS1_ALIAS_FRAME_SIZE) {
        int alias = reader.i16();
        if (alias_depth >= MAX_ALIAS_DEPTH || alias < 0 || alias >= frame_count_ || alias == frame) {
            return false;
        }
        return decode_as1_frame(alias, alias_depth + 1, out);
    }

    int contour_count = reader.i16();
    if (contour_count > 0) {
        reader.skip(static_cast<std::size_t>(contour_count) * AS1_CONTOUR_POINT_SIZE);
    }
    int top = reader.i16();
    int row_count = reader.i16();
    if (!reader.ok() || row_count < 0 || box_width_ <= 0) {
        return false;
    }

    bool palette = (type_ & VID_TYPE_PALETTE) != 0;
    std::uint16_t alpha_bits = VID_TYPE_ALPHA | VID_TYPE_TEXTURE | VID_TYPE_PALETTE;
    bool palette_alpha = (type_ & alpha_bits) == alpha_bits;
    bool z_words = palette && !palette_alpha && (type_ & VID_TYPE_ZBUFFER) != 0;

    reset_image(out, box_width_, row_count);
    out.left = -(box_width_ / 2);
    out.top = -(box_height_ / 2) + top;

    for (int row = 0; row < row_count; ++row) {
        int x = 0;
        while (true) {
            int skip = reader.u8();
            int count = reader.u8();
            if (!reader.ok()) {
                return false;
            }
            if (skip == 0 && count == 0) {
                break; // end of row; (255, 0) is only a long skip
            }
            x += skip;
            if (z_words) {
                reader.skip(static_cast<std::size_t>(count) * 2);
            }
            for (int i = 0; i < count; ++i) {
                std::uint32_t color = 0;
                if (palette) {
                    color = palette_[reader.u8()];
                    if (!palette_alpha) {
                        color |= 0xFF000000u;
                    }
                } else {
                    color = from_r5g6b5(reader.u16());
                }
                put_pixel(out, x + i, row, color);
            }
            x += count;
        }
    }
    return reader.ok();
}

/**
 * AS2 CADR frame:
 *   [f32 x, y, z if 0x200] u16 indexCount [u16 additionalDataCount if 0x800] u16 vertexCount
 *   u16 width, u16 height, i16 offsetX, i16 offsetY
 *   u16 index[indexCount], u32 vertex[vertexCount + additionalDataCount]
 *   pixel stream until u16 0: {u8 skip, u8 count, count pixels}, filled linearly by row stride
 *   pixel = color bytes [+1 if 0x8000] [+1 if 0x2000] [+2 z if 0x4]
 */
bool VidFile::decode_as2_frame(int frame, int alias_depth, VidImage &out) const {
    if (frame >= static_cast<int>(frames_->subrecords.size())) {
        return false;
    }
    ByteSpan bytes = frames_->subrecords[frame];
    ByteReader reader(bytes);
    if (bytes.size() <= AS2_ALIAS_FRAME_SIZE) {
        int alias = reader.i32();
        if (alias_depth >= MAX_ALIAS_DEPTH || alias < 0 || alias >= frame_count_ || alias == frame) {
            return false;
        }
        return decode_as2_frame(alias, alias_depth + 1, out);
    }

    if (type_ & AS2_TYPE_FRAME_LINK) {
        reader.skip(AS2_FRAME_LINK_SIZE);
    }
    int index_count = reader.u16();
    int extra_vertex_count = 0;
    if (type_ & AS2_TYPE_EXTRA_VERTEX) {
        extra_vertex_count = reader.u16();
    }
    int vertex_count = reader.u16();
    int width = reader.u16();
    int height = reader.u16();
    int offset_x = reader.i16();
    int offset_y = reader.i16();
    reader.skip(static_cast<std::size_t>(index_count) * 2 + static_cast<std::size_t>(vertex_count + extra_vertex_count) * 4);
    if (!reader.ok()) {
        return false;
    }

    int stride = width;
    if (type_ & AS2_TYPE_POW2_STRIDE) {
        stride = AS2_MIN_POW2_STRIDE;
        while (stride < width) {
            stride *= 2;
        }
    }
    int extra_bytes = 0;
    if (type_ & AS2_TYPE_MATERIAL) {
        extra_bytes += 1;
    }
    if (type_ & AS2_TYPE_NORMALS) {
        extra_bytes += 1;
    }
    if (type_ & VID_TYPE_ZBUFFER) {
        extra_bytes += 2;
    }
    bool use_alpha = (type_ & VID_TYPE_ALPHA) != 0;

    reset_image(out, width, height);
    out.left = -(box_width_ / 2) + offset_x;
    out.top = -(box_height_ / 2) + offset_y;
    if (stride <= 0) {
        return true; // empty frame
    }

    std::size_t position = 0;
    while (reader.remaining() >= 2) {
        int skip = reader.u8();
        int count = reader.u8();
        if (skip == 0 && count == 0) {
            break;
        }
        position += skip;
        for (int i = 0; i < count; ++i) {
            std::uint32_t color = 0;
            if (pixel_format_ == D3DFMT_P8) {
                color = palette_[reader.u8()];
                if (!use_alpha) {
                    color |= 0xFF000000u;
                }
            } else if (pixel_format_ == D3DFMT_A8R8G8B8 || pixel_format_ == D3DFMT_X8R8G8B8) {
                color = reader.u32();
                if (!use_alpha || pixel_format_ == D3DFMT_X8R8G8B8) {
                    color |= 0xFF000000u;
                }
            } else if (pixel_format_ == D3DFMT_R8G8B8) {
                std::uint32_t b = reader.u8();
                std::uint32_t g = reader.u8();
                std::uint32_t r = reader.u8();
                color = argb(255, r, g, b);
            } else if (pixel_format_ == D3DFMT_R5G6B5) {
                color = from_r5g6b5(reader.u16());
            } else if (pixel_format_ == D3DFMT_A4R4G4B4) {
                std::uint32_t v = reader.u16();
                color = argb(expand_bits((v >> 12) & 15, 4), expand_bits((v >> 8) & 15, 4),
                             expand_bits((v >> 4) & 15, 4), expand_bits(v & 15, 4));
            } else {
                // X1R5G5B5 / A1R5G5B5
                std::uint32_t v = reader.u16();
                std::uint32_t alpha = 255;
                if (pixel_format_ == D3DFMT_A1R5G5B5 && (v & 0x8000) == 0) {
                    alpha = 0;
                }
                color = argb(alpha, expand_bits((v >> 10) & 31, 5), expand_bits((v >> 5) & 31, 5),
                             expand_bits(v & 31, 5));
            }
            reader.skip(extra_bytes);
            int x = static_cast<int>(position % stride);
            int y = static_cast<int>(position / stride);
            put_pixel(out, x, y, color);
            position += 1;
        }
        if (!reader.ok()) {
            return false;
        }
    }
    return true;
}

} // namespace auto_mapper::game_assets
