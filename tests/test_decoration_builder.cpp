#include <gtest/gtest.h>

#include "auto_mapper/core/decoration_builder/decoration_builder.h"
#include "auto_mapper/core/decoration_builder/decoration_library.h"
#include "auto_mapper/core/decoration_builder/placeholder_vids.h"
#include "utils/test_utils.h"

#include <filesystem>
#include <vector>

using namespace auto_mapper;
using namespace auto_mapper::core::decoration_builder;
using namespace auto_mapper::test;

namespace {

// Frozen copy of assets/decoration_maps/AS2/01dorm.map: 11 regions, 419 member sprites.
const char* LIBRARY_FIXTURE = "tests/golden/decoration/01dorm_library.map";
// Frozen copy of assets/decoration_maps/AS1/08leisure.map: 9 grid cells, 61 member sprites.
// Kept in a subfolder so load_directory() on the AS2 fixture folder does not pick it up.
const char* AS1_LIBRARY_FIXTURE = "tests/golden/decoration/AS1/08leisure.map";

DecorationLibrary load_fixture_library() {
    DecorationLibrary library;
    EXPECT_TRUE(library.load_map(resolve_test_path(LIBRARY_FIXTURE)));
    return library;
}

} // namespace

TEST(DecorationLibraryTest, RegistersEveryRegionInSprOrder) {
    DecorationLibrary library = load_fixture_library();

    const int expected_member_counts[] = {23, 32, 103, 110, 23, 25, 42, 10, 11, 12, 28};
    ASSERT_EQ(library.stamps().size(), 11u);
    ASSERT_EQ(library.series().size(), 1u);
    EXPECT_EQ(library.series()[0], "01dorm_library");

    int total_members = 0;
    for (std::size_t index = 0; index < library.stamps().size(); ++index) {
        const DecorationStamp& stamp = library.stamps()[index];
        EXPECT_EQ(stamp.id, "01dorm_library:" + std::to_string(index + 1));
        EXPECT_EQ(stamp.series_id, "01dorm_library");
        EXPECT_EQ(stamp.members.size(), expected_member_counts[index]) << stamp.id;
        total_members += static_cast<int>(stamp.members.size());
    }
    EXPECT_EQ(total_members, 419);
}

TEST(DecorationLibraryTest, ReadsRegionSizeAndMemberFields) {
    DecorationLibrary library = load_fixture_library();

    // Region 1: center (3411, 3320), 652 x 452; first member is VID 1763 at (3618.09, 3416.06).
    const DecorationStamp* first = library.find("01dorm_library:1");
    ASSERT_NE(first, nullptr);
    EXPECT_FLOAT_EQ(first->width, 652.0f);
    EXPECT_FLOAT_EQ(first->height, 452.0f);
    const DecorationMember& member = first->members.front();
    EXPECT_EQ(member.vid, 1763);
    EXPECT_FLOAT_EQ(member.offset_x, 207.093017578125f);
    EXPECT_FLOAT_EQ(member.offset_y, 96.0625f);
    EXPECT_FLOAT_EQ(member.pos_z, 0.0f);
    EXPECT_EQ(member.direction, 83u);
    EXPECT_EQ(member.army, 0);

    const DecorationStamp* second = library.find("01dorm_library:2");
    ASSERT_NE(second, nullptr);
    EXPECT_FLOAT_EQ(second->width, 490.0f);
    EXPECT_FLOAT_EQ(second->height, 475.0f);

    EXPECT_EQ(library.find("01dorm_library:12"), nullptr);
    EXPECT_EQ(library.find("no_such_stamp"), nullptr);
}

TEST(DecorationLibraryTest, DecodesSpriteGamma) {
    DecorationLibrary library = load_fixture_library();

    // Region 4 has VID 1232 brightened by bytes [0,0,0,0, 0x19,0x14,0x14,0] (B, G, R, A).
    bool found_bright = false;
    for (const DecorationMember& member : library.find("01dorm_library:4")->members) {
        if (member.vid == 1232) {
            EXPECT_EQ(member.gamma.b, 25);
            EXPECT_EQ(member.gamma.g, 20);
            EXPECT_EQ(member.gamma.r, 20);
            EXPECT_EQ(member.gamma.a, 0);
            found_bright = true;
        }
    }
    EXPECT_TRUE(found_bright);

    // Region 7 has VID 409 darkened by bytes [0x64,0x64,0x64,0, 0,0,0,0].
    bool found_dark = false;
    for (const DecorationMember& member : library.find("01dorm_library:7")->members) {
        if (member.vid == 409 && member.gamma.r != 0) {
            EXPECT_EQ(member.gamma.b, -100);
            EXPECT_EQ(member.gamma.g, -100);
            EXPECT_EQ(member.gamma.r, -100);
            found_dark = true;
        }
    }
    EXPECT_TRUE(found_dark);
}

TEST(DecorationLibraryTest, LoadDirectoryReadsEveryMapFile) {
    std::filesystem::path fixture_directory = std::filesystem::path(resolve_test_path(LIBRARY_FIXTURE)).parent_path();

    DecorationLibrary library;
    EXPECT_EQ(library.load_directory(fixture_directory), 11);
    EXPECT_EQ(library.load_directory(fixture_directory / "missing"), 0);
}

TEST(DecorationLibraryTest, As1GridCellsBecomeStampsNumberedByCell) {
    DecorationLibrary library;
    ASSERT_TRUE(library.load_map(resolve_test_path(AS1_LIBRARY_FIXTURE)));

    const int expected_member_counts[] = {8, 4, 5, 9, 5, 4, 5, 9, 12};
    ASSERT_EQ(library.stamps().size(), 9u);
    ASSERT_EQ(library.series().size(), 1u);
    EXPECT_EQ(library.series()[0], "08leisure");
    for (std::size_t index = 0; index < library.stamps().size(); ++index) {
        const DecorationStamp& stamp = library.stamps()[index];
        EXPECT_EQ(stamp.id, "08leisure:" + std::to_string(index + 1));
        EXPECT_EQ(stamp.members.size(), expected_member_counts[index]) << stamp.id;
    }
}

TEST(DecorationLibraryTest, As1FrameIsTheMemberBoundingBoxPlusMargin) {
    DecorationLibrary library;
    ASSERT_TRUE(library.load_map(resolve_test_path(AS1_LIBRARY_FIXTURE)));

    // Cell 4: anchors span 153 x 157 around (2850.5, 660.5); the frame adds 40 on every side.
    const DecorationStamp* stamp = library.find("08leisure:4");
    ASSERT_NE(stamp, nullptr);
    EXPECT_FLOAT_EQ(stamp->width, 233.0f);
    EXPECT_FLOAT_EQ(stamp->height, 237.0f);
    const DecorationMember& member = stamp->members.front();
    EXPECT_EQ(member.vid, 666);
    EXPECT_FLOAT_EQ(member.offset_x, -38.5f);
    EXPECT_FLOAT_EQ(member.offset_y, -78.5f);
    EXPECT_EQ(member.direction, 36u);
    EXPECT_EQ(member.army, 0);
    // AS1 records carry no gamma or scale.
    EXPECT_EQ(member.gamma.r, 0);
    EXPECT_FLOAT_EQ(member.scale.x, 1.0f);

    // Army is copied from the library map.
    EXPECT_EQ(library.find("08leisure:1")->members.front().army, 2);
}

TEST(DecorationLibraryTest, As1EmptyCellsKeepTheirNumbersAndOffGridSpritesAreDropped) {
    std::vector<io::Sprite> sprites(3);
    // Cell 1 (row 0, column 0) and cell 8 (row 1, column 1); cells 2..7 stay empty.
    sprites[0].vid = 401;
    sprites[0].posX = 750.0f;
    sprites[0].posY = 660.0f;
    sprites[1].vid = 402;
    sprites[1].posX = 1450.0f;
    sprites[1].posY = 1180.0f;
    // Left of the grid origin: ignored with a warning.
    sprites[2].vid = 403;
    sprites[2].posX = 100.0f;
    sprites[2].posY = 100.0f;
    std::string map_path = get_test_output_path("as1_grid_gaps.map");
    ASSERT_TRUE(io::write_map(sprites, map_path, io::MapFormat::AS1, 5000.0f, 5000.0f));

    DecorationLibrary library;
    ASSERT_TRUE(library.load_map(map_path));
    ASSERT_EQ(library.stamps().size(), 2u);
    EXPECT_EQ(library.stamps()[0].id, "as1_grid_gaps:1");
    EXPECT_EQ(library.stamps()[1].id, "as1_grid_gaps:8");
    // A single member sits on the frame center; the frame is just the margin.
    EXPECT_FLOAT_EQ(library.stamps()[1].members.front().offset_x, 0.0f);
    EXPECT_FLOAT_EQ(library.stamps()[1].width, 80.0f);
}

TEST(DecorationBuilderTest, BuildTranslatesEveryMemberAndPreservesSpriteValues) {
    DecorationStamp stamp;
    stamp.id = "test:1";
    DecorationMember member;
    member.vid = 1288;
    member.offset_x = -40.5f;
    member.offset_y = 263.0f;
    member.pos_z = 13.0f;
    member.direction = 64;
    member.army = 2;
    member.gamma.r = -30;
    member.scale.x = 0.5f;
    stamp.members.push_back(member);

    std::vector<io::Sprite> sprites = build_stamp(stamp, 1000.0f, 2000.0f);

    ASSERT_EQ(sprites.size(), 1u);
    EXPECT_EQ(sprites[0].vid, 1288);
    EXPECT_FLOAT_EQ(sprites[0].posX, 959.5f);
    EXPECT_FLOAT_EQ(sprites[0].posY, 2263.0f);
    EXPECT_FLOAT_EQ(sprites[0].posZ, 13.0f);
    EXPECT_EQ(sprites[0].direction, 64u);
    EXPECT_EQ(sprites[0].army, 2);
    EXPECT_EQ(sprites[0].gamma.r, -30);
    EXPECT_FLOAT_EQ(sprites[0].scale.x, 0.5f);
}

TEST(DecorationBuilderTest, PlacingAtTheRegionCenterRestoresLibraryCoordinates) {
    DecorationLibrary library = load_fixture_library();

    std::vector<io::Sprite> sprites = build_stamp(*library.find("01dorm_library:1"), 3411.0f, 3320.0f);

    ASSERT_EQ(sprites.size(), 23u);
    EXPECT_FLOAT_EQ(sprites[0].posX, 3618.093017578125f);
    EXPECT_FLOAT_EQ(sprites[0].posY, 3416.0625f);
}

TEST(DecorationBuilderTest, FlatAndClutterVidsHaveNoPlaceholder) {
    const io::MapFormat as2 = io::MapFormat::AS2R;
    EXPECT_FALSE(draws_placeholder(as2, 120));  // blood
    EXPECT_FALSE(draws_placeholder(as2, 135));  // shadow
    EXPECT_FALSE(draws_placeholder(as2, 1783)); // office floor tile
    EXPECT_FALSE(draws_placeholder(as2, 1765)); // books
    EXPECT_TRUE(draws_placeholder(as2, 413));   // cabinet
    EXPECT_TRUE(draws_placeholder(as2, 2300));  // bedroom interior
}

TEST(DecorationBuilderTest, BlockAirWallsAreInvisible) {
    const io::MapFormat as2 = io::MapFormat::AS2R;
    EXPECT_TRUE(is_invisible_block(as2, 630));  // Z dummy, big
    EXPECT_TRUE(is_invisible_block(as2, 2648)); // closed electric door dummy
    EXPECT_FALSE(draws_placeholder(as2, 634));  // player blocker
    EXPECT_FALSE(is_invisible_block(as2, 413)); // cabinet
}

TEST(DecorationBuilderTest, OnlyFlatVidsLieOnTheGround) {
    const io::MapFormat as2 = io::MapFormat::AS2R;
    EXPECT_TRUE(lies_on_ground(as2, 120));   // blood
    EXPECT_TRUE(lies_on_ground(as2, 135));   // shadow
    EXPECT_TRUE(lies_on_ground(as2, 1783));  // office floor tile
    EXPECT_FALSE(lies_on_ground(as2, 1765)); // books stand on furniture
    EXPECT_FALSE(lies_on_ground(as2, 413));  // cabinet
}

TEST(DecorationBuilderTest, As2SeriesFormatsShareOneList) {
    EXPECT_TRUE(is_invisible_block(io::MapFormat::AS2, 630));
    EXPECT_TRUE(is_invisible_block(io::MapFormat::AS2R, 630));
    EXPECT_TRUE(is_invisible_block(io::MapFormat::AS2OE, 630));
}

TEST(DecorationBuilderTest, As1LitterHasNoPlaceholder) {
    const io::MapFormat as1 = io::MapFormat::AS1;
    EXPECT_FALSE(draws_placeholder(as1, 916)); // dried red blood
    EXPECT_FALSE(draws_placeholder(as1, 152)); // lying gibs
    EXPECT_FALSE(draws_placeholder(as1, 103)); // shell
    EXPECT_FALSE(draws_placeholder(as1, 764)); // monster armor fragments
    EXPECT_FALSE(draws_placeholder(as1, 783)); // rocks
    EXPECT_FALSE(draws_placeholder(as1, 428)); // small boxes on shelves
    EXPECT_FALSE(draws_placeholder(as1, 408)); // scattered paper
}

TEST(DecorationBuilderTest, As1CorpsesAndFrozenMonstersKeepPlaceholder) {
    const io::MapFormat as1 = io::MapFormat::AS1;
    EXPECT_TRUE(draws_placeholder(as1, 430)); // soldier corpse
    EXPECT_TRUE(draws_placeholder(as1, 431)); // scientist corpse
    EXPECT_TRUE(draws_placeholder(as1, 62));  // monster 2 corpse
    EXPECT_TRUE(draws_placeholder(as1, 882)); // flyer corpse
    EXPECT_TRUE(draws_placeholder(as1, 59));  // frozen monster
    EXPECT_TRUE(draws_placeholder(as1, 404)); // desk
}

TEST(DecorationBuilderTest, As1AirWallsAndGround) {
    const io::MapFormat as1 = io::MapFormat::AS1;
    EXPECT_TRUE(is_invisible_block(as1, 631));  // Z dummy
    EXPECT_TRUE(is_invisible_block(as1, 365));  // Z dummy
    EXPECT_FALSE(is_invisible_block(as1, 908)); // scripted monster generator has the flag but is no air wall
    EXPECT_TRUE(lies_on_ground(as1, 916));      // blood
    EXPECT_TRUE(lies_on_ground(as1, 103));      // shell
    EXPECT_FALSE(lies_on_ground(as1, 428));     // small boxes stand on shelves
    EXPECT_FALSE(lies_on_ground(as1, 430));     // corpses are drawn as standing sprites
}

TEST(DecorationBuilderTest, As1AndAs2ListsAreSeparate) {
    // 504 is AS1 ground but not an AS2 TERRAIN entry; 630 is an AS2 air wall but an ordinary AS1 nvid.
    EXPECT_TRUE(lies_on_ground(io::MapFormat::AS1, 504));
    EXPECT_FALSE(lies_on_ground(io::MapFormat::AS2R, 504));
    EXPECT_TRUE(is_invisible_block(io::MapFormat::AS2R, 630));
    EXPECT_FALSE(is_invisible_block(io::MapFormat::AS1, 630));
}
