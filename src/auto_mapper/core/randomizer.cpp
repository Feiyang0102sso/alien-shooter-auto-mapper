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
    if (values.empty()) {
        throw std::invalid_argument("Random::get requires a non-empty int list");
    }

    int max_index = static_cast<int>(values.size()) - 1;
    int selected_index = get(0, max_index);

    return values[selected_index];
}

} // namespace auto_mapper::core
