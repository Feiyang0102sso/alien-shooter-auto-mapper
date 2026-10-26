/**
 * @file byte_reader.h
 * @brief Bounds-checked little-endian cursor over a byte span.
 */

#pragma once
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>

namespace auto_mapper::game_assets {

/**
 * Reads little-endian values from a byte span. Reading past the end does not throw:
 * it returns 0 and clears ok(), so a parser checks ok() once after a block of reads.
 */
class ByteReader {
  public:
    explicit ByteReader(std::span<const std::uint8_t> data) : data_(data) {}

    bool ok() const { return ok_; }
    std::size_t position() const { return position_; }
    std::size_t remaining() const {
        if (position_ >= data_.size()) {
            return 0;
        }
        return data_.size() - position_;
    }

    std::uint8_t u8() {
        if (!require(1)) {
            return 0;
        }
        std::uint8_t value = data_[position_];
        position_ += 1;
        return value;
    }

    std::uint16_t u16() {
        if (!require(2)) {
            return 0;
        }
        std::uint16_t value = static_cast<std::uint16_t>(data_[position_] | (data_[position_ + 1] << 8));
        position_ += 2;
        return value;
    }

    std::int16_t i16() { return static_cast<std::int16_t>(u16()); }

    std::uint32_t u32() {
        if (!require(4)) {
            return 0;
        }
        std::uint32_t value = 0;
        for (int i = 3; i >= 0; --i) {
            value = (value << 8) | data_[position_ + i];
        }
        position_ += 4;
        return value;
    }

    std::int32_t i32() { return static_cast<std::int32_t>(u32()); }

    void skip(std::size_t count) {
        if (require(count)) {
            position_ += count;
        }
    }

    // NUL-terminated string; the terminator is consumed.
    std::string c_string() {
        std::string text;
        while (ok_) {
            std::uint8_t c = u8();
            if (c == 0) {
                break;
            }
            text.push_back(static_cast<char>(c));
        }
        return text;
    }

  private:
    bool require(std::size_t count) {
        if (!ok_ || remaining() < count) {
            ok_ = false;
            return false;
        }
        return true;
    }

    std::span<const std::uint8_t> data_;
    std::size_t position_ = 0;
    bool ok_ = true;
};

} // namespace auto_mapper::game_assets
