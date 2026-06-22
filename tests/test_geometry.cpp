#include <gtest/gtest.h>
#include "auto_mapper/core/geometry.h"

using namespace auto_mapper::core;

TEST(GeometryTest, ToIsoNoShift) {
    GridPoint g1{0, 0};
    MapPoint p1 = to_iso(g1);
    EXPECT_FLOAT_EQ(p1.x, 0.0f);
    EXPECT_FLOAT_EQ(p1.y, 0.0f);

    GridPoint g2{1, 0};
    MapPoint p2 = to_iso(g2);
    EXPECT_FLOAT_EQ(p2.x, 40.0f);
    EXPECT_FLOAT_EQ(p2.y, 28.0f);

    GridPoint g3{0, 1};
    MapPoint p3 = to_iso(g3);
    EXPECT_FLOAT_EQ(p3.x, -40.0f);
    EXPECT_FLOAT_EQ(p3.y, 28.0f);
}

TEST(GeometryTest, ToIsoWithShift) {
    GridPoint g2{1, 0};
    MapPoint shift{100.0f, 200.0f};
    MapPoint p4 = to_iso(g2, shift);
    EXPECT_FLOAT_EQ(p4.x, 140.0f);
    EXPECT_FLOAT_EQ(p4.y, 228.0f);
}
