/**
 * @file randomizer.cpp
 * @brief Shared random helpers for ranges and lists.
 */

#include "auto_mapper/core/randomizer.h"

#include <random>
#include <stdexcept>

namespace auto_mapper::core {

std::mt19937& Random::engine() {
    static thread_local std::mt19937 random_engine(std::random_device{}());

    return random_engine;
}

int Random::get(int min_value, int max_value) {
    std::uniform_int_distribution<int> distribution(min_value, max_value);

    return distribution(engine());
}

int Random::get(const std::vector<int>& values) {
    return get(std::span<const int>(values));
}

int Random::get(std::span<const int> values) {
    if (values.empty()) {
        throw std::invalid_argument("Random::get requires a non-empty int list");
    }

    int max_index = static_cast<int>(values.size()) - 1;
    int selected_index = get(0, max_index);

    return values[selected_index];
}

DirectionRandomizer::DirectionRandomizer(bool enabled)
    : enabled_(enabled) {}

bool DirectionRandomizer::is_enabled() const {
    return enabled_;
}

int DirectionRandomizer::select(
    int default_value,
    int min_value,
    int max_value
) const {
    if (!enabled_) {
        return default_value;
    }

    return Random::get(min_value, max_value);
}

int DirectionRandomizer::select(
    int default_value,
    std::span<const int> allowed_values
) const {
    if (!enabled_) {
        return default_value;
    }

    return Random::get(allowed_values);
}

uint32_t DirectionRandomizer::select_direction(
    uint32_t default_direction,
    const DirectionRandomizationRule& rule
) const {
    if (rule.mode == DirectionRandomizationMode::Fixed) {
        return default_direction;
    }

    if (rule.mode == DirectionRandomizationMode::FullRange) {
        int direction = select(
            static_cast<int>(default_direction),
            0,
            255
        );
        return static_cast<uint32_t>(direction);
    }

    std::span<const int> allowed_values(
        rule.allowed_values,
        static_cast<std::size_t>(rule.allowed_value_count)
    );
    int direction = select(
        static_cast<int>(default_direction),
        allowed_values
    );
    return static_cast<uint32_t>(direction);
}

} // namespace auto_mapper::core
