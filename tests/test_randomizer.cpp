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
