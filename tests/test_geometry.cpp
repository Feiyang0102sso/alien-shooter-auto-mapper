#include <gtest/gtest.h>
#include "auto_mapper/core/geometry.h"

using namespace auto_mapper::core;

// Standard wall step sizes for test reference
constexpr float TEST_STEP_X = 40.0f;
constexpr float TEST_STEP_Y = 28.0f;

TEST(GeometryTest, ToIsoNoShift) {
    GridPoint g1{0, 0};
    MapPoint p1 = to_iso(g1, TEST_STEP_X, TEST_STEP_Y);
    EXPECT_FLOAT_EQ(p1.x, 0.0f);
    EXPECT_FLOAT_EQ(p1.y, 0.0f);

    GridPoint g2{1, 0};
    MapPoint p2 = to_iso(g2, TEST_STEP_X, TEST_STEP_Y);
    EXPECT_FLOAT_EQ(p2.x, 40.0f);
    EXPECT_FLOAT_EQ(p2.y, 28.0f);

    GridPoint g3{0, 1};
    MapPoint p3 = to_iso(g3, TEST_STEP_X, TEST_STEP_Y);
    EXPECT_FLOAT_EQ(p3.x, -40.0f);
    EXPECT_FLOAT_EQ(p3.y, 28.0f);
}

TEST(GeometryTest, ToIsoWithShift) {
    GridPoint g2{1, 0};
    MapPoint shift{100.0f, 200.0f};
    MapPoint p4 = to_iso(g2, TEST_STEP_X, TEST_STEP_Y, shift);
    EXPECT_FLOAT_EQ(p4.x, 140.0f);
    EXPECT_FLOAT_EQ(p4.y, 228.0f);
}

TEST(GeometryTest, ToIsoLabWallSteps) {
    // Verify lab wall step sizes (90x64) produce correct results
    constexpr float LAB_STEP_X = 90.0f;
    constexpr float LAB_STEP_Y = 64.0f;

    GridPoint g1{1, 0};
    MapPoint p1 = to_iso(g1, LAB_STEP_X, LAB_STEP_Y);
    EXPECT_FLOAT_EQ(p1.x, 90.0f);
    EXPECT_FLOAT_EQ(p1.y, 64.0f);

    GridPoint g2{0, 1};
    MapPoint p2 = to_iso(g2, LAB_STEP_X, LAB_STEP_Y);
    EXPECT_FLOAT_EQ(p2.x, -90.0f);
    EXPECT_FLOAT_EQ(p2.y, 64.0f);

    GridPoint g3{1, 1};
    MapPoint p3 = to_iso(g3, LAB_STEP_X, LAB_STEP_Y);
    EXPECT_FLOAT_EQ(p3.x, 0.0f);
    EXPECT_FLOAT_EQ(p3.y, 128.0f);
}

#include "auto_mapper/core/wall_builder.h"

TEST(WallBuilderTest, FloodFillTopology) {
    // 3x3 square room: (0,0) to (3,0), (3,0) to (3,3), (3,3) to (0,3), (0,3) to (0,0)
    WallBuilder builder(20, 600.0f, 600.0f);
    std::vector<Segment> segments = {
        {{0, 0}, {3, 0}, WALL_TYPE_STANDARD, FLOOR_TYPE_STANDARD},
        {{3, 0}, {3, 3}, WALL_TYPE_STANDARD, FLOOR_TYPE_STANDARD},
        {{3, 3}, {0, 3}, WALL_TYPE_STANDARD, FLOOR_TYPE_STANDARD},
        {{0, 3}, {0, 0}, WALL_TYPE_STANDARD, FLOOR_TYPE_STANDARD}
    };
    
    auto sprites = builder.build(segments);
    
    int floor_count = 0;
    int ceiling_count = 0;
    int wall_count = 0;
    for (const auto& spr : sprites) {
        if (spr.vid == FLOOR_STANDARD.vid || spr.vid == FLOOR_LAB.vid) floor_count++;
        else if (spr.vid == CEILING_STANDARD.vid) ceiling_count++;
        else wall_count++;
    }
    
    // We expect floors inside, ceilings outside, and walls along the boundary.
    EXPECT_GT(floor_count, 0);
    EXPECT_GT(ceiling_count, 0);
    EXPECT_GT(wall_count, 0);
}

