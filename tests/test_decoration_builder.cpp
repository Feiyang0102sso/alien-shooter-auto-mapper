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

    EXPECT_EQ(AS2_DECORATION_PROFILE_COUNT, 116);
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

TEST(DecorationBuilderTest, RegistersAllStorageProfilesFromPreparedDecorationFiles) {
    const DecorationProfile* profiles[] = {
        &STORAGE01_DECORATION_PROFILE,
        &STORAGE02_DECORATION_PROFILE,
        &STORAGE03_DECORATION_PROFILE,
        &STORAGE04_DECORATION_PROFILE,
        &STORAGE05_DECORATION_PROFILE,
        &STORAGE06_DECORATION_PROFILE,
        &STORAGE07_DECORATION_PROFILE,
        &STORAGE08_DECORATION_PROFILE,
        &STORAGE09_DECORATION_PROFILE,
    };
    const int expected_member_counts[] = {33, 14, 8, 12, 66, 18, 20, 17, 14};
    // Unlike the other series, the storage rooms were authored against
    // several different wall sets.
    const core::WallProfile* expected_wall_profiles[] = {
        &core::WALL_AS2_SET1_RANDOM,
        &core::WALL_AS2_SET1_RANDOM,
        &core::WALL_AS2_SET1_RANDOM,
        &core::WALL_AS2_SET4_RANDOM,
        &core::WALL_AS2_SET5_RANDOM,
        &core::WALL_AS2_SET5_RANDOM,
        &core::WALL_AS2_SET8_RANDOM,
        &core::WALL_AS2_SET6_RANDOM,
        &core::WALL_AS2_SET5_RANDOM,
    };
    // The storage series is registered after the dorm and office profiles.
    const int registry_offset = 17;

    for (int index = 0; index < 9; ++index) {
        EXPECT_EQ(AS2_DECORATION_PROFILES[registry_offset + index], profiles[index]);
        EXPECT_EQ(profiles[index]->members.size(), expected_member_counts[index]);
        EXPECT_EQ(profiles[index]->test_boundary_wall_profile, expected_wall_profiles[index]);
    }
}

TEST(DecorationBuilderTest, RegistersAllAdditionalProfilesFromPreparedDecorationFiles) {
    const DecorationProfile* profiles[] = {
        &MACHINES_01_DECORATION_PROFILE,
        &MACHINES_02_DECORATION_PROFILE,
        &MACHINES_03_DECORATION_PROFILE,
        &MACHINES_04_DECORATION_PROFILE,
        &MACHINES_05_DECORATION_PROFILE,
        &MACHINES_06_DECORATION_PROFILE,
        &MACHINES_07_DECORATION_PROFILE,
        &MACHINES_08_DECORATION_PROFILE,
        &MACHINES_09_DECORATION_PROFILE,
        &COMP01_DECORATION_PROFILE,
        &COMP02_DECORATION_PROFILE,
        &COMP03_DECORATION_PROFILE,
        &COMP04_DECORATION_PROFILE,
        &COMP05_DECORATION_PROFILE,
        &COMP06_DECORATION_PROFILE,
        &COMP07_DECORATION_PROFILE,
        &COMP08_DECORATION_PROFILE,
        &COMP09_DECORATION_PROFILE,
        &VEHICLE_WORKSHOP01_DECORATION_PROFILE,
        &VEHICLE_WORKSHOP02_DECORATION_PROFILE,
        &VEHICLE_WORKSHOP03_DECORATION_PROFILE,
        &VEHICLE_WORKSHOP04_DECORATION_PROFILE,
        &VEHICLE_WORKSHOP05_DECORATION_PROFILE,
        &GARAGE01_DECORATION_PROFILE,
        &GARAGE02_DECORATION_PROFILE,
        &GARAGE03_DECORATION_PROFILE,
        &GARAGE04_DECORATION_PROFILE,
        &GARAGE05_DECORATION_PROFILE,
        &GARAGE06_DECORATION_PROFILE,
        &GARAGE07_DECORATION_PROFILE,
        &GARAGE08_DECORATION_PROFILE,
        &GARAGE09_DECORATION_PROFILE,
        &WEAPON_WORKSHOP01_DECORATION_PROFILE,
        &WEAPON_WORKSHOP02_DECORATION_PROFILE,
        &WEAPON_WORKSHOP03_DECORATION_PROFILE,
        &WEAPON_WORKSHOP04_DECORATION_PROFILE,
        &WEAPON_WORKSHOP05_DECORATION_PROFILE,
        &WEAPON_WORKSHOP06_DECORATION_PROFILE,
        &WEAPON_WORKSHOP07_DECORATION_PROFILE,
        &WEAPON_WORKSHOP08_DECORATION_PROFILE,
        &SICK_BAY01_DECORATION_PROFILE,
        &SICK_BAY02_DECORATION_PROFILE,
        &SICK_BAY03_DECORATION_PROFILE,
        &SICK_BAY04_DECORATION_PROFILE,
        &SICK_BAY05_DECORATION_PROFILE,
        &CANTEEN01_DECORATION_PROFILE,
        &CANTEEN02_DECORATION_PROFILE,
        &CANTEEN03_DECORATION_PROFILE,
        &CANTEEN04_DECORATION_PROFILE,
        &LARGE_M01_DECORATION_PROFILE,
        &LARGE_M02_DECORATION_PROFILE,
        &LAB_DESKS01_DECORATION_PROFILE,
        &LAB_DESKS02_DECORATION_PROFILE,
        &LAB_DESKS03_DECORATION_PROFILE,
        &LAB_DESKS04_DECORATION_PROFILE,
        &LAB_DESKS05_DECORATION_PROFILE,
        &LAB_DESKS06_DECORATION_PROFILE,
        &LAB_DESKS07_DECORATION_PROFILE,
        &LAB_DESKS08_DECORATION_PROFILE,
        &LAB_OFFICE01_DECORATION_PROFILE,
        &LAB_OFFICE02_DECORATION_PROFILE,
        &LAB_OFFICE03_DECORATION_PROFILE,
        &LAB_OFFICE04_DECORATION_PROFILE,
        &LAB_COMP01_DECORATION_PROFILE,
        &LAB_COMP02_DECORATION_PROFILE,
        &LAB_COMP03_DECORATION_PROFILE,
        &LAB_COMP04_DECORATION_PROFILE,
        &LAB_BIOEXP01_DECORATION_PROFILE,
        &LAB_BIOEXP02_DECORATION_PROFILE,
        &LAB_BIOEXP03_DECORATION_PROFILE,
        &LAB_BIOEXP04_DECORATION_PROFILE,
        &LAB_BIOEXP05_DECORATION_PROFILE,
        &LAB_BIOEXP06_DECORATION_PROFILE,
        &LAB_BIOEXP07_DECORATION_PROFILE,
        &LAB_BIOEXP08_DECORATION_PROFILE,
        &LAB_SURGERY01_DECORATION_PROFILE,
        &LAB_SURGERY02_DECORATION_PROFILE,
        &LAB_SURGERY03_DECORATION_PROFILE,
        &LAB_SURGERY04_DECORATION_PROFILE,
        &LAB_SURGERY05_DECORATION_PROFILE,
        &LAB_SURGERY06_DECORATION_PROFILE,
        &OTHERS01_DECORATION_PROFILE,
        &OTHERS02_DECORATION_PROFILE,
        &OTHERS03_DECORATION_PROFILE,
        &OTHERS04_DECORATION_PROFILE,
        &OTHERS05_DECORATION_PROFILE,
        &OTHERS06_DECORATION_PROFILE,
        &OTHERS07_DECORATION_PROFILE,
        &OTHERS08_DECORATION_PROFILE,
        &OTHERS09_DECORATION_PROFILE,
    };
    const int expected_member_counts[] = {
        4, 7, 12, 20, 19, 10, 11, 14, 6, 64, 21, 32, 11, 59, 13, 28, 6, 10, 82, 90, 52, 58, 52, 8,
        3, 36, 14, 40, 42, 7, 2, 8, 14, 13, 41, 42, 14, 14, 30, 28, 13, 11, 25, 5, 22, 37, 27, 45,
        19, 3, 8, 25, 14, 5, 10, 13, 34, 25, 36, 21, 28, 23, 30, 15, 13, 17, 19, 14, 8, 10, 15, 9,
        17, 5, 22, 7, 14, 8, 19, 15, 6, 18, 8, 16, 19, 7, 16, 25, 27, 10,
    };
    const int registry_offset = 26;

    EXPECT_EQ(AS2_DECORATION_PROFILE_COUNT, 116);
    EXPECT_EQ(sizeof(profiles) / sizeof(profiles[0]), 90u);

    for (int index = 0; index < 90; ++index) {
        EXPECT_EQ(AS2_DECORATION_PROFILES[registry_offset + index], profiles[index]);
        EXPECT_EQ(profiles[index]->members.size(), expected_member_counts[index]);
    }
}

TEST(DecorationBuilderTest, EveryRegisteredMemberVidHasAnArmyBinding) {
    // get_as2_vid_army throws on an unregistered VID, so a profile added
    // without its Army bindings would only blow up at build time.
    for (int index = 0; index < AS2_DECORATION_PROFILE_COUNT; ++index) {
        const DecorationProfile* profile = AS2_DECORATION_PROFILES[index];
        for (const DecorationMemberProfile& member : profile->members) {
            EXPECT_NO_THROW(core::get_as2_vid_army(member.vid))
                << "profile " << profile->id << " vid " << member.vid;
        }
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
