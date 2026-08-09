/**
 * @file randomizer.h
 * @brief Shared random helpers for ranges and lists.
 */

#pragma once

#include <cstdint>
#include <random>
#include <span>
#include <vector>

namespace auto_mapper::core {

inline constexpr int MAX_DIRECTION_RANDOM_VALUES = 8;

enum class DirectionRandomizationMode {
    Fixed,
    FullRange,
    AllowedValues
};

struct DirectionRandomizationRule {
    DirectionRandomizationMode mode = DirectionRandomizationMode::Fixed;
    int allowed_values[MAX_DIRECTION_RANDOM_VALUES]{};
    int allowed_value_count = 0;
};

inline constexpr DirectionRandomizationRule FULL_RANGE_DIRECTION_RANDOMIZATION = {
    .mode = DirectionRandomizationMode::FullRange
};

class Random {
public:
    /**
     * @brief Return a random integer in the inclusive [min_value, max_value] range.
     */
    static int get(int min_value, int max_value);

    /**
     * @brief Return one random integer from a non-empty list.
     */
    static int get(const std::vector<int>& values);

    /**
     * @brief Return one random integer from a non-empty contiguous list.
     */
    static int get(std::span<const int> values);

private:
    static std::mt19937& engine();
};

class DirectionRandomizer {
public:
    explicit DirectionRandomizer(bool enabled);

    bool is_enabled() const;

    int select(int default_value, int min_value, int max_value) const;
    int select(int default_value, std::span<const int> allowed_values) const;

    uint32_t select_direction(
        uint32_t default_direction,
        const DirectionRandomizationRule& rule
    ) const;

private:
    bool enabled_;
};

} // namespace auto_mapper::core
