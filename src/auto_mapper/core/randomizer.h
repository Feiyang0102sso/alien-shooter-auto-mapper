/**
 * @file randomizer.h
 * @brief Shared random helpers for ranges and lists.
 */

#pragma once

#include <random>
#include <vector>

namespace auto_mapper::core {

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

private:
    static std::mt19937& engine();
};

} // namespace auto_mapper::core
