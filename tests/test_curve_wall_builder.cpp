#include <gtest/gtest.h>

#include "auto_mapper/core/wall_builder/curve_wall_builder.h"
#include "auto_mapper/core/wall_builder/wall_builder.h"

#include <cmath>
#include <numbers>
#include <set>
#include <stdexcept>
#include <vector>

using auto_mapper::core::build_curve_wall;
using auto_mapper::core::CURVE_WALL_TYPE_ALIEN_NEST;
using auto_mapper::core::CURVE_WALL_TYPE_CLIFF;
using auto_mapper::core::CURVE_WALL_TYPE_ROCK;
using auto_mapper::core::CurveWall;
using auto_mapper::core::erase_curve_wall_circle;
using auto_mapper::core::get_curve_wall_profile;
using auto_mapper::core::MapPoint;
using auto_mapper::core::simplify_stroke;
using auto_mapper::core::smooth_curve_wall;

namespace {

CurveWall open_line(int type, MapPoint first, MapPoint last) {
    CurveWall wall;
    wall.curve_wall_type = type;
    wall.points = {first, last};
    return wall;
}

// An isometric circle: radius `radius` on the ground, so Map y is squashed by 0.7.
CurveWall closed_circle(int type, float radius, int control_points) {
    CurveWall wall;
    wall.curve_wall_type = type;
    wall.closed = true;
    for (int index = 0; index < control_points; ++index) {
        float angle = 2.0f * std::numbers::pi_v<float> * index / control_points;
        wall.points.push_back({1000.0f + radius * std::cos(angle), 1000.0f + radius * 0.7f * std::sin(angle)});
    }
    return wall;
}

// Smallest distance between two directions on the 256-unit circle.
int direction_gap(uint32_t a, uint32_t b) {
    int gap = std::abs(static_cast<int>(a) - static_cast<int>(b)) % 256;
    return std::min(gap, 256 - gap);
}

} // namespace

TEST(CurveWallBuilderTest, ProfilesMatchOfficialMaps) {
    EXPECT_EQ(get_curve_wall_profile(CURVE_WALL_TYPE_ROCK).vid, 2670);
    EXPECT_EQ(get_curve_wall_profile(CURVE_WALL_TYPE_ROCK).army, 2);
    EXPECT_FLOAT_EQ(get_curve_wall_profile(CURVE_WALL_TYPE_ROCK).spacing, 110.0f);
    EXPECT_EQ(get_curve_wall_profile(CURVE_WALL_TYPE_ALIEN_NEST).vid, 2700);
    EXPECT_EQ(get_curve_wall_profile(CURVE_WALL_TYPE_ALIEN_NEST).army, 2);
    EXPECT_FLOAT_EQ(get_curve_wall_profile(CURVE_WALL_TYPE_ALIEN_NEST).spacing, 130.0f);
    EXPECT_FLOAT_EQ(get_curve_wall_profile(CURVE_WALL_TYPE_ROCK).end_reach, 125.0f);
    EXPECT_FLOAT_EQ(get_curve_wall_profile(CURVE_WALL_TYPE_ALIEN_NEST).end_reach, 155.0f);
    EXPECT_EQ(get_curve_wall_profile(CURVE_WALL_TYPE_CLIFF).vid, 3300);
    EXPECT_EQ(get_curve_wall_profile(CURVE_WALL_TYPE_CLIFF).army, 0);
    EXPECT_FLOAT_EQ(get_curve_wall_profile(CURVE_WALL_TYPE_CLIFF).spacing, 185.0f);
    EXPECT_FALSE(get_curve_wall_profile(CURVE_WALL_TYPE_ROCK).one_sided);
    EXPECT_FALSE(get_curve_wall_profile(CURVE_WALL_TYPE_ALIEN_NEST).one_sided);
    EXPECT_TRUE(get_curve_wall_profile(CURVE_WALL_TYPE_CLIFF).one_sided);
    EXPECT_THROW(get_curve_wall_profile(7), std::out_of_range);
}

TEST(CurveWallBuilderTest, StraightLineSpacesPiecesAndFacesTheTangent) {
    // End pieces sit 125 inside each end: 225 .. 1075 is 850 long -> 8 gaps of 106.25, 9 pieces facing 0.
    auto sprites = build_curve_wall(open_line(CURVE_WALL_TYPE_ROCK, {100, 500}, {1200, 500}), false);
    ASSERT_EQ(sprites.size(), 9u);
    for (size_t index = 0; index < sprites.size(); ++index) {
        EXPECT_EQ(sprites[index].vid, 2670);
        EXPECT_EQ(sprites[index].army, 2);
        EXPECT_FLOAT_EQ(sprites[index].posY, 500.0f);
        EXPECT_FLOAT_EQ(sprites[index].posZ, 0.0f);
        EXPECT_NEAR(sprites[index].posX, 225.0f + 106.25f * index, 1.0f);
        EXPECT_EQ(sprites[index].direction, 0u);
    }
}

TEST(CurveWallBuilderTest, EndReachFollowsTheGroundPlane) {
    // Straight down the screen the ground is squashed by 0.7: the end pieces sit 125 * 0.7 = 87.5 inside.
    auto downward = build_curve_wall(open_line(CURVE_WALL_TYPE_ROCK, {500, 100}, {500, 870}), false);
    ASSERT_GE(downward.size(), 2u);
    EXPECT_NEAR(downward.front().posY, 100.0f + 87.5f, 1.0f);
    EXPECT_NEAR(downward.back().posY, 870.0f - 87.5f, 1.0f);
    // Too short for two pieces: one in the middle.
    auto short_run = build_curve_wall(open_line(CURVE_WALL_TYPE_ROCK, {0, 0}, {280, 0}), false);
    ASSERT_EQ(short_run.size(), 1u);
    EXPECT_NEAR(short_run[0].posX, 140.0f, 1.0f);
    // Loops have no ends to pull in.
    EXPECT_GE(build_curve_wall(closed_circle(CURVE_WALL_TYPE_ROCK, 600, 12), false).size(), 20u);
}

TEST(CurveWallBuilderTest, DirectionFollowsDrawingDirection) {
    // Right-to-left is the same wall seen from behind: half a turn.
    auto leftward = build_curve_wall(open_line(CURVE_WALL_TYPE_ROCK, {1200, 500}, {100, 500}), false);
    EXPECT_EQ(leftward.front().direction, 128u);
    // Straight down the screen is a quarter turn clockwise.
    auto downward = build_curve_wall(open_line(CURVE_WALL_TYPE_ROCK, {500, 100}, {500, 870}), false);
    EXPECT_EQ(downward.front().direction, 64u);
    // Ground diagonal along +x / +y: Map (40, 28) is 45 degrees once y is un-squashed.
    auto diagonal = build_curve_wall(open_line(CURVE_WALL_TYPE_ROCK, {0, 0}, {400, 280}), false);
    EXPECT_EQ(diagonal.front().direction, 32u);
}

TEST(CurveWallBuilderTest, AlienNestAddsItsAngleOffset) {
    // -27 degrees = -19.2 units -> 237. Ends 155 inside: 990 / 130 -> 8 gaps, 9 pieces.
    auto sprites = build_curve_wall(open_line(CURVE_WALL_TYPE_ALIEN_NEST, {100, 500}, {1400, 500}), false);
    ASSERT_EQ(sprites.size(), 9u);
    EXPECT_NEAR(sprites.front().posX, 255.0f, 1.0f);
    EXPECT_NEAR(sprites.back().posX, 1245.0f, 1.0f);
    EXPECT_EQ(sprites.front().vid, 2700);
    EXPECT_EQ(sprites.front().direction, 237u);
}

TEST(CurveWallBuilderTest, ClosedLoopSpreadsPiecesEvenly) {
    auto sprites = build_curve_wall(closed_circle(CURVE_WALL_TYPE_ROCK, 600, 12), false);
    ASSERT_GE(sprites.size(), 20u);
    // Neighbouring anchors (including last -> first) sit about one step apart.
    std::vector<float> gaps;
    for (size_t index = 0; index < sprites.size(); ++index) {
        const auto &a = sprites[index];
        const auto &b = sprites[(index + 1) % sprites.size()];
        gaps.push_back(std::hypot(b.posX - a.posX, b.posY - a.posY));
    }
    for (float gap : gaps) {
        EXPECT_NEAR(gap, gaps.front(), 25.0f);
        EXPECT_NEAR(gap, 110.0f, 25.0f);
    }
    // Each piece faces along the chord between its neighbours (y un-squashed), within one frame (8 units).
    for (size_t index = 0; index < sprites.size(); ++index) {
        const auto &before = sprites[(index + sprites.size() - 1) % sprites.size()];
        const auto &after = sprites[(index + 1) % sprites.size()];
        float degrees = std::atan2((after.posY - before.posY) / 0.7f, after.posX - before.posX) * 180.0f /
                        std::numbers::pi_v<float>;
        uint32_t expected = static_cast<uint32_t>((std::lround(degrees * 256.0f / 360.0f) % 256 + 256) % 256);
        EXPECT_LE(direction_gap(sprites[index].direction, expected), 8) << "piece " << index;
    }
}

TEST(CurveWallBuilderTest, RandomizationOnlyJittersAroundTheTangent) {
    CurveWall wall = closed_circle(CURVE_WALL_TYPE_ROCK, 600, 12);
    auto exact = build_curve_wall(wall, false);
    EXPECT_EQ(build_curve_wall(wall, false).size(), exact.size());
    auto randomized = build_curve_wall(wall, true);
    ASSERT_EQ(randomized.size(), exact.size());
    for (size_t index = 0; index < exact.size(); ++index) {
        EXPECT_FLOAT_EQ(randomized[index].posX, exact[index].posX);
        EXPECT_FLOAT_EQ(randomized[index].posY, exact[index].posY);
        EXPECT_LE(direction_gap(randomized[index].direction, exact[index].direction),
                  get_curve_wall_profile(CURVE_WALL_TYPE_ROCK).direction_jitter);
    }
}

TEST(CurveWallBuilderTest, SmoothPathPassesThroughControlPoints) {
    CurveWall wall;
    wall.points = {{0, 0}, {300, 100}, {500, 400}, {200, 600}};
    auto path = smooth_curve_wall(wall);
    // 3 gaps x 16 samples + the closing end point.
    ASSERT_EQ(path.size(), 49u);
    for (size_t index = 0; index < wall.points.size(); ++index) {
        EXPECT_FLOAT_EQ(path[index * 16].x, wall.points[index].x);
        EXPECT_FLOAT_EQ(path[index * 16].y, wall.points[index].y);
    }
    wall.closed = true;
    auto loop = smooth_curve_wall(wall);
    ASSERT_EQ(loop.size(), 65u);
    EXPECT_FLOAT_EQ(loop.back().x, wall.points.front().x);
    EXPECT_FLOAT_EQ(loop.back().y, wall.points.front().y);
}

TEST(CurveWallBuilderTest, SimplifyStrokeKeepsCornersAndDropsJitter) {
    std::vector<MapPoint> stroke;
    // A shaky horizontal run, then a sharp turn downwards.
    for (int index = 0; index <= 50; ++index)
        stroke.push_back({index * 20.0f, 300.0f + ((index % 2 == 0) ? 3.0f : -3.0f)});
    for (int index = 1; index <= 30; ++index)
        stroke.push_back({1000.0f, 300.0f + index * 20.0f});
    auto points = simplify_stroke(stroke, 20.0f);
    ASSERT_EQ(points.size(), 3u);
    EXPECT_FLOAT_EQ(points[1].x, 1000.0f);
    EXPECT_NEAR(points[1].y, 300.0f, 3.0f);
    // Tighter than the shake keeps more points.
    EXPECT_GT(simplify_stroke(stroke, 2.0f).size(), 3u);
}

TEST(CurveWallBuilderTest, DegenerateCurvesProduceNothing) {
    CurveWall single;
    single.points = {{10, 10}};
    EXPECT_TRUE(build_curve_wall(single, false).empty());
    EXPECT_TRUE(build_curve_wall(open_line(CURVE_WALL_TYPE_ROCK, {10, 10}, {10, 10}), false).empty());
    EXPECT_TRUE(build_curve_wall(open_line(9, {0, 0}, {500, 0}), false).empty());
}

TEST(CurveWallBuilderTest, EraserCutSplitsAnOpenCurveAtTheCircle) {
    CurveWall line = open_line(CURVE_WALL_TYPE_ROCK, {0, 0}, {1000, 0});
    auto left_and_right = erase_curve_wall_circle(line, {500, 0}, 100);
    ASSERT_EQ(left_and_right.size(), 2u);
    for (const auto &part : left_and_right) {
        EXPECT_FALSE(part.closed);
        EXPECT_EQ(part.curve_wall_type, CURVE_WALL_TYPE_ROCK);
    }
    EXPECT_NEAR(left_and_right[0].points.front().x, 0.0f, 0.01f);
    EXPECT_NEAR(left_and_right[0].points.back().x, 400.0f, 0.01f);
    EXPECT_NEAR(left_and_right[1].points.front().x, 600.0f, 0.01f);
    EXPECT_NEAR(left_and_right[1].points.back().x, 1000.0f, 0.01f);
    // The cut part has no pieces left.
    for (const auto &part : left_and_right) {
        for (const auto &piece : build_curve_wall(part, false))
            EXPECT_GE(std::abs(piece.posX - 500.0f), 100.0f);
    }
}

TEST(CurveWallBuilderTest, EraserCutKeepsUntouchedControlPoints) {
    CurveWall wall;
    wall.points = {{0, 0}, {300, 100}, {600, 0}, {900, 150}, {1200, 0}};
    // Cut near the end: the first four control points survive unchanged.
    auto rest = erase_curve_wall_circle(wall, {1200, 0}, 80);
    ASSERT_EQ(rest.size(), 1u);
    ASSERT_GE(rest[0].points.size(), 5u);
    for (size_t index = 0; index < 4; ++index) {
        EXPECT_FLOAT_EQ(rest[0].points[index].x, wall.points[index].x);
        EXPECT_FLOAT_EQ(rest[0].points[index].y, wall.points[index].y);
    }
    // The new end sits on the circle.
    const auto &end = rest[0].points.back();
    EXPECT_NEAR(std::hypot(end.x - 1200.0f, end.y), 80.0f, 0.5f);
}

TEST(CurveWallBuilderTest, EraserCutOpensALoop) {
    CurveWall loop = closed_circle(CURVE_WALL_TYPE_ROCK, 600, 12);
    auto opened = erase_curve_wall_circle(loop, {1600, 1000}, 60);
    ASSERT_EQ(opened.size(), 1u);
    EXPECT_FALSE(opened[0].closed);
    // Both new ends sit on the eraser circle, and every original control point outside it survives.
    EXPECT_NEAR(std::hypot(opened[0].points.front().x - 1600.0f, opened[0].points.front().y - 1000.0f), 60.0f, 0.5f);
    EXPECT_NEAR(std::hypot(opened[0].points.back().x - 1600.0f, opened[0].points.back().y - 1000.0f), 60.0f, 0.5f);
    EXPECT_EQ(opened[0].points.size(), loop.points.size() - 1 + 2);
}

TEST(CurveWallBuilderTest, EraserMissCoverAndShortRemainders) {
    CurveWall line = open_line(CURVE_WALL_TYPE_ROCK, {0, 0}, {1000, 0});
    auto untouched = erase_curve_wall_circle(line, {500, 300}, 100);
    ASSERT_EQ(untouched.size(), 1u);
    EXPECT_EQ(untouched[0].points.size(), 2u);
    EXPECT_TRUE(erase_curve_wall_circle(line, {500, 0}, 600).empty());
    // 0 .. 30 is shorter than half of the 110 spacing, so it goes with the cut.
    auto right = erase_curve_wall_circle(line, {60, 0}, 30);
    ASSERT_EQ(right.size(), 1u);
    EXPECT_NEAR(right[0].points.front().x, 90.0f, 0.01f);
}

TEST(CurveWallBuilderTest, CliffFaceFollowsTheDrawingDirection) {
    // +56 degrees = +39.8 units: an eastward run faces 40, the same run drawn westward faces 168.
    auto eastward = build_curve_wall(open_line(CURVE_WALL_TYPE_CLIFF, {0, 500}, {2000, 500}), false);
    auto westward = build_curve_wall(open_line(CURVE_WALL_TYPE_CLIFF, {2000, 500}, {0, 500}), false);
    ASSERT_GE(eastward.size(), 2u);
    ASSERT_EQ(westward.size(), eastward.size());
    EXPECT_EQ(eastward.front().vid, 3300);
    EXPECT_EQ(eastward.front().army, 0);
    EXPECT_EQ(eastward.front().direction, 40u);
    EXPECT_EQ(westward.front().direction, 168u);
    // 2000 long, 200 inside each end: 1600 / 185 -> 9 gaps, 10 pieces.
    EXPECT_EQ(eastward.size(), 10u);
    EXPECT_NEAR(eastward.front().posX, 200.0f, 1.0f);
}

namespace {

// Floor sprites of one VID among everything a WallBuilder returned.
std::vector<auto_mapper::io::Sprite> tiles_of(const std::vector<auto_mapper::io::Sprite> &sprites, int vid) {
    std::vector<auto_mapper::io::Sprite> tiles;
    for (const auto &sprite : sprites) {
        if (sprite.vid == vid)
            tiles.push_back(sprite);
    }
    return tiles;
}

// Inside the isometric ellipse of closed_circle(), grown by `margin` on the ground.
bool inside_circle(const auto_mapper::io::Sprite &tile, float radius, float margin) {
    float dx = (tile.posX - 1000.0f) / (radius + margin);
    float dy = (tile.posY - 1000.0f) / ((radius + margin) * 0.7f);
    return dx * dx + dy * dy <= 1.0f;
}

} // namespace

TEST(CurveWallFloorTest, ClosedLoopsFillTheirInsideWithTheirFloor) {
    struct Case {
        int type;
        int vid;
        float step_x;
        float step_y;
    };
    const Case cases[] = {{CURVE_WALL_TYPE_ROCK, 1121, 80.0f, 56.0f},
                          {CURVE_WALL_TYPE_ALIEN_NEST, 510, 132.0f, 94.0f},
                          {CURVE_WALL_TYPE_CLIFF, 512, 132.0f, 94.0f}};
    for (const Case &test_case : cases) {
        auto_mapper::core::WallBuilder builder(3000.0f, 3000.0f);
        auto sprites = builder.build({}, true, false, {}, {closed_circle(test_case.type, 600, 12)});
        auto tiles = tiles_of(sprites, test_case.vid);
        // Nothing but floor tiles: the loop pieces come from build_curve_wall(), not from here.
        EXPECT_EQ(tiles.size(), sprites.size()) << test_case.vid;
        // The loop encloses pi * 600 * 420 Map units; one tile covers 2 * step_x * step_y.
        float expected = std::numbers::pi_v<float> * 600.0f * 420.0f / (2.0f * test_case.step_x * test_case.step_y);
        EXPECT_GT(tiles.size(), expected * 0.7f) << test_case.vid;
        EXPECT_LT(tiles.size(), expected * 1.6f) << test_case.vid;
        for (const auto &tile : tiles) {
            EXPECT_TRUE(inside_circle(tile, 600.0f, test_case.step_x)) << test_case.vid;
            EXPECT_EQ(tile.army, 0);
            // Direction Randomization is off: every tile keeps the default direction.
            EXPECT_EQ(tile.direction, 0u);
        }
    }
}

TEST(CurveWallFloorTest, OpenCurvesLayNoFloor) {
    auto_mapper::core::WallBuilder builder(3000.0f, 3000.0f);
    EXPECT_TRUE(builder.build({}, true, false, {}, {open_line(CURVE_WALL_TYPE_CLIFF, {200, 500}, {2600, 900})}).empty());
    // Floors off: a loop lays nothing either.
    EXPECT_TRUE(builder.build({}, false, false, {}, {closed_circle(CURVE_WALL_TYPE_ROCK, 600, 12)}).empty());
}

TEST(CurveWallFloorTest, RandomizedFloorsFollowTheirDirectionRules) {
    auto_mapper::core::WallBuilder builder(3000.0f, 3000.0f, true);
    auto rock = tiles_of(builder.build({}, true, false, {}, {closed_circle(CURVE_WALL_TYPE_ROCK, 600, 12)}), 1121);
    ASSERT_FALSE(rock.empty());
    std::set<uint32_t> rock_directions;
    for (const auto &tile : rock) {
        rock_directions.insert(tile.direction);
        // Same six values as Wall Set 3 / 4.
        EXPECT_TRUE(std::set<uint32_t>({0, 32, 128, 160, 192, 224}).contains(tile.direction)) << tile.direction;
    }
    EXPECT_GT(rock_directions.size(), 1u);
    auto nest = tiles_of(builder.build({}, true, false, {}, {closed_circle(CURVE_WALL_TYPE_ALIEN_NEST, 900, 12)}), 510);
    std::set<uint32_t> nest_directions;
    for (const auto &tile : nest)
        nest_directions.insert(tile.direction);
    // 0 .. 255 at random: far more than six distinct values.
    EXPECT_GT(nest_directions.size(), 6u);
}

TEST(CurveWallFloorTest, RoomInsideARockLoopKeepsItsOwnFloor) {
    using auto_mapper::core::GridPoint;
    using auto_mapper::core::Segment;
    const int wall_type = auto_mapper::core::WALL_TYPE_AS2_WALL_SET1_FIXED_0;
    std::vector<Segment> room;
    GridPoint corners[4] = {{6, 6}, {12, 6}, {12, 11}, {6, 11}};
    for (int index = 0; index < 4; ++index)
        room.push_back({corners[index], corners[(index + 1) % 4], wall_type, 0});

    auto_mapper::core::WallBuilder builder(3000.0f, 3000.0f);
    const auto &profile = auto_mapper::core::WallBuilder::get_wall_profile(wall_type);
    MapPoint shift = auto_mapper::core::WallBuilder::get_wall_shift(3000.0f, profile);
    MapPoint top = auto_mapper::core::to_iso(corners[0], profile.step_x, profile.step_y, shift);
    MapPoint right = auto_mapper::core::to_iso(corners[1], profile.step_x, profile.step_y, shift);
    MapPoint bottom = auto_mapper::core::to_iso(corners[2], profile.step_x, profile.step_y, shift);
    MapPoint left = auto_mapper::core::to_iso(corners[3], profile.step_x, profile.step_y, shift);
    // A loop 300 Map units outside every room corner.
    CurveWall loop;
    loop.curve_wall_type = CURVE_WALL_TYPE_ROCK;
    loop.closed = true;
    loop.points = {{top.x, top.y - 300}, {right.x + 300, right.y}, {bottom.x, bottom.y + 300}, {left.x - 300, left.y}};

    auto sprites = builder.build(room, true, false, {}, {loop});
    auto rock_floor = tiles_of(sprites, 1121);
    auto room_floor = tiles_of(sprites, 1783);
    ASSERT_FALSE(rock_floor.empty());
    ASSERT_FALSE(room_floor.empty());
    // Room floor stays near the room (tiles touching the wall band may poke one tile out, as for any grid
    // room); the rock floor fills the 300-wide ring and never the room middle.
    float middle_x = (left.x + right.x) / 2.0f;
    float middle_y = (top.y + bottom.y) / 2.0f;
    float half_width = (right.x - left.x) / 2.0f;
    float half_height = (bottom.y - top.y) / 2.0f;
    for (const auto &tile : room_floor) {
        float dx = std::abs(tile.posX - middle_x) / (half_width + 160.0f);
        float dy = std::abs(tile.posY - middle_y) / (half_height + 112.0f);
        EXPECT_LE(dx + dy, 1.0f) << tile.posX << ", " << tile.posY;
    }
    for (const auto &tile : rock_floor) {
        float dx = std::abs(tile.posX - middle_x) / half_width;
        float dy = std::abs(tile.posY - middle_y) / half_height;
        EXPECT_GT(dx + dy, 0.6f) << tile.posX << ", " << tile.posY;
    }
}

