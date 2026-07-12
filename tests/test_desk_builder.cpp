#include <gtest/gtest.h>

#include "auto_mapper/core/indoor_decorations/desk_builder.h"
#include "auto_mapper/io/map_writer.h"
#include "utils/test_utils.h"

#include <string>
#include <vector>

using namespace auto_mapper;
using namespace auto_mapper::core::indoor_decorations;
using namespace auto_mapper::test;

namespace {

DeskUnit get_fixed_test_unit() {
    return {
        .pos_x = 470.0f,
        .pos_y = 430.0f,
        .pos_z = 0.0f,
        .use_fixed_body_direction = true,
        .body_direction = 170,
    };
}

bool is_computer_vid(int vid) {
    if (vid == DESK_BROKEN_COMPUTER_VID) {
        return true;
    }

    if (vid == DESK_ENABLED_COMPUTER_VID) {
        return true;
    }

    return false;
}

bool is_valid_computer_direction(uint32_t direction) {
    if (direction == 0u) {
        return true;
    }

    if (direction == 128u) {
        return true;
    }

    return false;
}

bool is_valid_desk_direction(uint32_t direction) {
    if (direction == 0u) {
        return true;
    }

    if (direction == 85u) {
        return true;
    }

    if (direction == 170u) {
        return true;
    }

    return false;
}

} // namespace

TEST(DeskBuilderTest, DefaultTemplateUsesThreeDeskDirections) {
    const DeskTemplate& desk_template = get_default_desk_template();

    EXPECT_EQ(desk_template.body_vid, DESK_BODY_VID);
    ASSERT_EQ(desk_template.body_directions.size(), 3u);
    EXPECT_EQ(desk_template.body_directions[0], 0u);
    EXPECT_EQ(desk_template.body_directions[1], 85u);
    EXPECT_EQ(desk_template.body_directions[2], 170u);

    ASSERT_EQ(desk_template.computer_vids.size(), 2u);
    EXPECT_EQ(desk_template.computer_vids[0], DESK_BROKEN_COMPUTER_VID);
    EXPECT_EQ(desk_template.computer_vids[1], DESK_ENABLED_COMPUTER_VID);

    ASSERT_EQ(desk_template.computer_directions.size(), 2u);
    EXPECT_EQ(desk_template.computer_directions[0], 0u);
    EXPECT_EQ(desk_template.computer_directions[1], 128u);
}

TEST(DeskBuilderTest, BuildsDeskComputerAndChairCombo) {
    DeskBuilder builder;
    DeskUnit unit = get_fixed_test_unit();

    for (int iteration = 0; iteration < 30; ++iteration) {
        std::vector<io::Sprite> sprites = builder.build(unit);

        ASSERT_EQ(sprites.size(), 3u);

        const io::Sprite& desk = sprites[0];
        EXPECT_EQ(desk.vid, DESK_BODY_VID);
        EXPECT_EQ(desk.posX, unit.pos_x);
        EXPECT_EQ(desk.posY, unit.pos_y);
        EXPECT_EQ(desk.posZ, unit.pos_z);
        EXPECT_EQ(desk.direction, 170u);

        const io::Sprite& computer = sprites[1];
        EXPECT_TRUE(is_computer_vid(computer.vid));
        EXPECT_GE(computer.posX, unit.pos_x - 13.0f);
        EXPECT_LE(computer.posX, unit.pos_x - 3.0f);
        EXPECT_GE(computer.posY, unit.pos_y);
        EXPECT_LE(computer.posY, unit.pos_y + 4.0f);
        EXPECT_EQ(computer.posZ, DESK_COMPUTER_POS_Z);
        EXPECT_TRUE(is_valid_computer_direction(computer.direction));

        const io::Sprite& chair = sprites[2];
        EXPECT_EQ(chair.vid, DESK_CHAIR_VID);
        EXPECT_GE(chair.posX, unit.pos_x + 25.0f);
        EXPECT_LE(chair.posX, unit.pos_x + 39.0f);
        EXPECT_GE(chair.posY, unit.pos_y + 28.0f);
        EXPECT_LE(chair.posY, unit.pos_y + 42.0f);
        EXPECT_EQ(chair.posZ, DESK_CHAIR_POS_Z);
        EXPECT_LE(chair.direction, 255u);
    }
}

TEST(DeskBuilderTest, UsesFixedDeskDirectionForBatchControl) {
    DeskBuilder builder;
    DeskUnit unit = {
        .pos_x = 390.0f,
        .pos_y = 410.0f,
        .pos_z = 0.0f,
        .use_fixed_body_direction = true,
        .body_direction = 85,
    };

    std::vector<io::Sprite> sprites = builder.build(unit);

    ASSERT_EQ(sprites.size(), 3u);
    EXPECT_EQ(sprites[0].direction, 85u);
}

TEST(DeskBuilderTest, RandomDeskDirectionUsesAllowedDirections) {
    DeskBuilder builder;

    for (int iteration = 0; iteration < 30; ++iteration) {
        DeskUnit unit = {
            .pos_x = 390.0f,
            .pos_y = 410.0f,
        };

        std::vector<io::Sprite> sprites = builder.build(unit);

        ASSERT_EQ(sprites.size(), 3u);
        EXPECT_TRUE(is_valid_desk_direction(sprites[0].direction));
    }
}

TEST(DeskBuilderTest, BatchUsesOneRandomDeskDirection) {
    DeskBuilder builder;
    std::vector<DeskUnit> units = {
        {610.0f, 250.0f},
        {510.0f, 330.0f},
        {390.0f, 410.0f},
    };

    std::vector<io::Sprite> sprites = builder.build_batch(units);

    ASSERT_EQ(sprites.size(), 9u);
    uint32_t batch_direction = sprites[0].direction;
    EXPECT_TRUE(is_valid_desk_direction(batch_direction));

    for (std::size_t index = 0; index < sprites.size(); index += 3) {
        EXPECT_EQ(sprites[index].vid, DESK_BODY_VID);
        EXPECT_EQ(sprites[index].direction, batch_direction);
    }
}

TEST(DeskBuilderTest, WritesRandomManualPreviewMap) {
    DeskBuilder builder;
    std::vector<DeskUnit> units = {
        {610.0f, 250.0f},
        {510.0f, 330.0f},
        {390.0f, 410.0f},
        {530.0f, 170.0f},
        {390.0f, 270.0f},
        {270.0f, 350.0f},
        {390.0f, 110.0f},
        {270.0f, 190.0f},
        {150.0f, 270.0f},
    };

    std::vector<io::Sprite> sprites = builder.build_batch(units);

    std::string output_path = get_project_root() + "/desk_builder_manual_test.map";
    ASSERT_TRUE(io::write_map(sprites, output_path, 800.0f, 800.0f));
}
