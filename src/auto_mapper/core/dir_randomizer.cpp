/**
 * @file dir_randomizer.cpp
 * @brief Randomize direction for generated wall, pillar, and floor sprites only.
 */

#include "auto_mapper/core/dir_randomizer.h"
#include "auto_mapper/core/wall_builder.h"

#include <random>

namespace auto_mapper::core {

namespace {

constexpr int MIN_DIRECTION = 0;
constexpr int MAX_DIRECTION = 255;

bool is_random_direction_sprite(int vid) {
    if (vid == WALL_STANDARD.dir_a_vid) {
        return true;
    }

    if (vid == WALL_STANDARD.dir_b_vid) {
        return true;
    }

    if (vid == WALL_STANDARD.pillar_vid) {
        return true;
    }

    if (vid == WALL_LAB.dir_a_vid) {
        return true;
    }

    if (vid == WALL_LAB.dir_b_vid) {
        return true;
    }

    if (vid == WALL_LAB.pillar_vid) {
        return true;
    }

    if (vid == WALL_STANDARD_DARK.dir_a_vid) {
        return true;
    }

    if (vid == WALL_STANDARD_DARK.dir_b_vid) {
        return true;
    }

    if (vid == WALL_STANDARD_DARK.pillar_vid) {
        return true;
    }

    if (vid == FLOOR_STANDARD.vid) {
        return true;
    }

    if (vid == FLOOR_LAB.vid) {
        return true;
    }

    if (vid == FLOOR_STANDARD_DARK.vid) {
        return true;
    }

    return false;
}

} // namespace

uint32_t get_random_direction() {
    static thread_local std::mt19937 random_engine(std::random_device{}());
    std::uniform_int_distribution<uint32_t> distribution(MIN_DIRECTION, MAX_DIRECTION);

    return distribution(random_engine);
}

void randomize_wall_and_floor_directions(std::vector<io::Sprite>& sprites) {
    for (auto& sprite : sprites) {
        if (is_random_direction_sprite(sprite.vid)) {
            sprite.direction = get_random_direction();
        }
    }
}

uint32_t get_random_direction_from_list(
    const std::vector<uint32_t>& allowed_directions,
    uint32_t fallback_direction
) {
    if (allowed_directions.empty()) {
        return fallback_direction;
    }

    static thread_local std::mt19937 random_engine(std::random_device{}());
    int max_index = static_cast<int>(allowed_directions.size()) - 1;
    std::uniform_int_distribution<int> distribution(0, max_index);
    int selected_index = distribution(random_engine);

    return allowed_directions[selected_index];
}

int get_random_int_from_list(
    const std::vector<int>& allowed_values,
    int fallback_value
) {
    if (allowed_values.empty()) {
        return fallback_value;
    }

    static thread_local std::mt19937 random_engine(std::random_device{}());
    int max_index = static_cast<int>(allowed_values.size()) - 1;
    std::uniform_int_distribution<int> distribution(0, max_index);
    int selected_index = distribution(random_engine);

    return allowed_values[selected_index];
}

} // namespace auto_mapper::core
