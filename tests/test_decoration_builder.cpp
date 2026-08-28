#include <gtest/gtest.h>

#include "auto_mapper/core/decoration_builder/decoration_builder.h"
#include "auto_mapper/core/decoration_builder/decoration_profiles_as2.h"
#include "auto_mapper/core/vid_properties/vid_armies_as2.h"
#include "auto_mapper/core/wall_builder/wall_profiles_as2.h"
#include "auto_mapper/io/map_writer.h"
#include "utils/test_utils.h"

#include <vector>

using namespace auto_mapper;
using namespace auto_mapper::core::decoration_builder;
using namespace auto_mapper::test;

TEST(DecorationBuilderTest, RegistersAllDormProfilesFromPreparedDecorationFiles) {
    const DecorationProfile* profiles[] = {
        &DORMS01_DECORATION_PROFILE,
        &DORMS02_DECORATION_PROFILE,
        &DORMS03_DECORATION_PROFILE,
        &DORMS04_DECORATION_PROFILE,
        &DORMS05_DECORATION_PROFILE,
        &DORMS06_DECORATION_PROFILE,
        &DORMS07_DECORATION_PROFILE,
        &DORMS08_DECORATION_PROFILE,
    };
    const int expected_member_counts[] = {27, 28, 22, 30, 26, 12, 11, 11};

    EXPECT_EQ(AS2_DECORATION_PROFILE_COUNT, 17);
    EXPECT_EQ(
        profiles[0]->test_boundary_wall_profile,
        &core::WALL_AS2_SET6_RANDOM
    );

    for (int index = 1; index < 8; ++index) {
        EXPECT_EQ(AS2_DECORATION_PROFILES[index], profiles[index]);
        EXPECT_EQ(profiles[index]->members.size(), expected_member_counts[index]);
        EXPECT_EQ(profiles[index]->test_boundary_wall_profile, &core::WALL_AS2_SET1_RANDOM);
    }
}

TEST(DecorationBuilderTest, RegistersAllOfficeProfilesFromPreparedDecorationFiles) {
    const DecorationProfile* profiles[] = {
        &OFFICE01_DECORATION_PROFILE,
        &OFFICE02_DECORATION_PROFILE,
        &OFFICE03_DECORATION_PROFILE,
        &OFFICE04_DECORATION_PROFILE,
        &OFFICE05_DECORATION_PROFILE,
        &OFFICE06_DECORATION_PROFILE,
        &OFFICE07_DECORATION_PROFILE,
        &OFFICE08_DECORATION_PROFILE,
        &OFFICE09_DECORATION_PROFILE,
    };
    const int expected_member_counts[] = {77, 20, 33, 9, 53, 19, 67, 78, 21};
    // The office series is registered right after the eight dorm profiles.
    const int registry_offset = 8;

    for (int index = 0; index < 9; ++index) {
        EXPECT_EQ(AS2_DECORATION_PROFILES[registry_offset + index], profiles[index]);
        EXPECT_EQ(profiles[index]->members.size(), expected_member_counts[index]);
        EXPECT_EQ(profiles[index]->test_boundary_wall_profile, &core::WALL_AS2_SET1_RANDOM);
    }
}

TEST(DecorationBuilderTest, Dorms01ProfileKeepsImportedFrameAndMembers) {
    EXPECT_EQ(DORMS01_DECORATION_PROFILE.id, "dorms01");
    ASSERT_EQ(DORMS01_DECORATION_PROFILE.members.size(), 27u);
    EXPECT_EQ(
        DORMS01_DECORATION_PROFILE.test_boundary_wall_profile,
        &core::WALL_AS2_SET6_RANDOM
    );

    const DecorationFrameProfile& frame = DORMS01_DECORATION_PROFILE.frame;
    EXPECT_FLOAT_EQ(frame.corner_offsets[0].x, 0.0f);
    EXPECT_FLOAT_EQ(frame.corner_offsets[0].y, -448.0f);
    EXPECT_FLOAT_EQ(frame.corner_offsets[1].x, -630.0f);
    EXPECT_FLOAT_EQ(frame.corner_offsets[1].y, 0.0f);
    EXPECT_FLOAT_EQ(frame.corner_offsets[2].x, 0.0f);
    EXPECT_FLOAT_EQ(frame.corner_offsets[2].y, 448.0f);
    EXPECT_FLOAT_EQ(frame.corner_offsets[3].x, 630.0f);
    EXPECT_FLOAT_EQ(frame.corner_offsets[3].y, 0.0f);
    EXPECT_FLOAT_EQ(
        frame.default_placeholder_width,
        DEFAULT_DECORATION_PLACEHOLDER_WIDTH
    );
    EXPECT_FLOAT_EQ(
        frame.default_placeholder_height,
        DEFAULT_DECORATION_PLACEHOLDER_HEIGHT
    );
}

TEST(DecorationBuilderTest, BoundaryIsDisabledByDefaultAndAvailableForManualTests) {
    DecorationPlacement placement = {
        .center_x = 1000.0f,
        .center_y = 1000.0f,
    };

    DecorationBuilder builder;
    std::vector<io::Sprite> default_sprites = builder.build(
        DORMS01_DECORATION_PROFILE,
        placement
    );

    DecorationBuildOptions test_options = {
        .keep_boundary = true,
    };
    std::vector<io::Sprite> test_sprites = builder.build(
        DORMS01_DECORATION_PROFILE,
        placement,
        test_options
    );

    EXPECT_EQ(default_sprites.size(), 27u);
    ASSERT_EQ(test_sprites.size(), 55u);
    EXPECT_EQ(test_sprites[27].vid, 2601);
    EXPECT_EQ(test_sprites[54].vid, 2600);

    for (std::size_t index = 27; index < test_sprites.size(); ++index) {
        EXPECT_TRUE(test_sprites[index].vid == 2600 || test_sprites[index].vid == 2601);
    }
}

TEST(DecorationBuilderTest, BuildTranslatesEveryMemberAndPreservesSpriteValues) {
    DecorationPlacement placement = {
        .center_x = 1000.0f,
        .center_y = 2000.0f,
    };

    DecorationBuilder builder;
    std::vector<io::Sprite> sprites = builder.build(
        DORMS01_DECORATION_PROFILE,
        placement
    );

    ASSERT_EQ(sprites.size(), DORMS01_DECORATION_PROFILE.members.size());

    // Members carry the authored member nudge on top of the placement.
    DecorationPoint shift = resolve_member_shift(DORMS01_DECORATION_PROFILE.member_nudge);

    for (std::size_t index = 0; index < sprites.size(); ++index) {
        const DecorationMemberProfile& member = DORMS01_DECORATION_PROFILE.members[index];
        const io::Sprite& sprite = sprites[index];

        EXPECT_EQ(sprite.vid, member.vid);
        EXPECT_FLOAT_EQ(sprite.posX, placement.center_x + member.offset_x + shift.x);
        EXPECT_FLOAT_EQ(sprite.posY, placement.center_y + member.offset_y + shift.y);
        EXPECT_FLOAT_EQ(sprite.posZ, member.pos_z);
        EXPECT_EQ(sprite.direction, member.direction);
        EXPECT_EQ(sprite.army, core::get_as2_vid_army(member.vid));
        EXPECT_EQ(sprite.gamma.r, member.gamma.r);
        EXPECT_EQ(sprite.gamma.g, member.gamma.g);
        EXPECT_EQ(sprite.gamma.b, member.gamma.b);
        EXPECT_EQ(sprite.gamma.a, member.gamma.a);
        EXPECT_FLOAT_EQ(sprite.scale.x, member.scale.x);
        EXPECT_FLOAT_EQ(sprite.scale.y, member.scale.y);
        EXPECT_FLOAT_EQ(sprite.scale.z, member.scale.z);
    }
}

TEST(DecorationBuilderTest, Dorms01BuildReconstructsImportedSourceCoordinates) {
    DecorationPlacement source_placement = {
        .center_x = 594.8363734654017f,
        .center_y = 760.5689697265625f,
    };

    DecorationBuilder builder;
    std::vector<io::Sprite> sprites = builder.build(
        DORMS01_DECORATION_PROFILE,
        source_placement
    );

    ASSERT_EQ(sprites.size(), 27u);

    // Hard-coded values are the imported source coordinates. The member nudge
    // is the author's deliberate offset from them.
    DecorationPoint shift = resolve_member_shift(DORMS01_DECORATION_PROFILE.member_nudge);

    EXPECT_FLOAT_EQ(sprites[0].posX, 659.0863647460938f + shift.x);
    EXPECT_FLOAT_EQ(sprites[0].posY, 868.5689697265625f + shift.y);
    EXPECT_EQ(sprites[0].vid, 2357);
    EXPECT_EQ(sprites[0].direction, 70u);
    EXPECT_EQ(sprites[0].army, 0);

    EXPECT_EQ(sprites[3].vid, 1288);
    EXPECT_FLOAT_EQ(sprites[3].posZ, 13.0f);

    EXPECT_EQ(sprites[26].vid, 1270);
    EXPECT_FLOAT_EQ(sprites[26].posX, 616.0863647460938f + shift.x);
    EXPECT_FLOAT_EQ(sprites[26].posY, 647.5689697265625f + shift.y);
    EXPECT_EQ(sprites[26].direction, 128u);
    EXPECT_EQ(sprites[26].army, 2);
}

TEST(DecorationBuilderTest, WritesDorms01BoundaryManualTestMap) {
    DecorationPlacement source_placement = {
        .center_x = 594.8363734654017f,
        .center_y = 760.5689697265625f,
    };
    DecorationBuildOptions test_options = {
        .keep_boundary = true,
    };

    DecorationBuilder builder;
    std::vector<io::Sprite> sprites = builder.build(
        DORMS01_DECORATION_PROFILE,
        source_placement,
        test_options
    );

    std::string output_path = get_test_output_path(
        "AS2/decorations/dorms01_boundary_manual_test.map"
    );
    bool success = io::write_map(
        sprites,
        output_path,
        io::MapFormat::AS2R,
        1500.0f,
        1500.0f
    );

    ASSERT_TRUE(success);
}
