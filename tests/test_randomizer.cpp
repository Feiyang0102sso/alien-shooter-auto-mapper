#include <gtest/gtest.h>

#include "auto_mapper/core/randomizer.h"

#include <vector>

using namespace auto_mapper::core;

TEST(RandomizerTest, GetsRandomIntInInclusiveRange) {
    for (int iteration = 0; iteration < 50; ++iteration) {
        int value = Random::get(3, 7);

        EXPECT_GE(value, 3);
        EXPECT_LE(value, 7);
    }
}

TEST(RandomizerTest, GetsRandomIntFromList) {
    std::vector<int> values = {
        10,
        20,
        30,
        40
    };

    int selected_value = Random::get(values);
    bool value_is_allowed = false;

    for (int value : values) {
        if (selected_value == value) {
            value_is_allowed = true;
        }
    }

    EXPECT_TRUE(value_is_allowed);
}

TEST(RandomizerTest, DisabledDirectionRandomizationUsesDefault) {
    DirectionRandomizationRule rule = FULL_RANGE_DIRECTION_RANDOMIZATION;
    DirectionRandomizer randomizer(false);

    uint32_t direction = randomizer.select_direction(51u, rule);

    EXPECT_EQ(direction, 51u);
}

TEST(RandomizerTest, FullRangeDirectionStaysBetweenZeroAnd255) {
    DirectionRandomizer randomizer(true);

    for (int iteration = 0; iteration < 50; ++iteration) {
        uint32_t direction = randomizer.select_direction(
            0u,
            FULL_RANGE_DIRECTION_RANDOMIZATION
        );

        EXPECT_LE(direction, 255u);
    }
}

TEST(RandomizerTest, AllowedDirectionUsesDeclaredValues) {
    DirectionRandomizationRule rule = {
        .mode = DirectionRandomizationMode::AllowedValues,
        .allowed_values = {0, 32, 128, 160, 192, 224},
        .allowed_value_count = 6
    };
    DirectionRandomizer randomizer(true);

    for (int iteration = 0; iteration < 50; ++iteration) {
        uint32_t direction = randomizer.select_direction(0u, rule);
        bool direction_is_allowed = false;

        for (int index = 0; index < rule.allowed_value_count; ++index) {
            if (direction == rule.allowed_values[index]) {
                direction_is_allowed = true;
            }
        }

        EXPECT_TRUE(direction_is_allowed);
    }
}
