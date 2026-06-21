#include "auto_mapper/core/geometry.h"
#include "auto_mapper/common/logger.h"
#include <iostream>
#include <cmath>

using namespace auto_mapper;
using namespace auto_mapper::core;

bool is_close(float a, float b, float epsilon = 0.001f) {
    return std::abs(a - b) < epsilon;
}

int main() {
    Logger::init();
    Logger::info("Starting geometry tests...");

    int passed = 0;
    int failed = 0;

    auto assert_eq = [&](float actual, float expected, const std::string& name) {
        if (is_close(actual, expected)) {
            Logger::info("  [PASS] {}", name);
            passed++;
        } else {
            Logger::error("  [FAIL] {}: expected {}, got {}", name, expected, actual);
            failed++;
        }
    };

    // Test 1: to_iso WITHOUT shift
    Logger::info("--- Test to_iso (No Shift) ---");
    GridPoint g1{0, 0};
    MapPoint p1 = to_iso(g1);
    assert_eq(p1.x, 0.0f, "to_iso(0,0).x");
    assert_eq(p1.y, 0.0f, "to_iso(0,0).y");

    GridPoint g2{1, 0}; // x+1 -> posX +40, posY +28
    MapPoint p2 = to_iso(g2);
    assert_eq(p2.x, 40.0f, "to_iso(1,0).x");
    assert_eq(p2.y, 28.0f, "to_iso(1,0).y");

    GridPoint g3{0, 1}; // y+1 -> posX -40, posY +28
    MapPoint p3 = to_iso(g3);
    assert_eq(p3.x, -40.0f, "to_iso(0,1).x");
    assert_eq(p3.y, 28.0f, "to_iso(0,1).y");

    // Test 2: to_iso WITH shift
    Logger::info("--- Test to_iso (With Shift) ---");
    MapPoint shift{100.0f, 200.0f};
    MapPoint p4 = to_iso(g2, shift);
    assert_eq(p4.x, 140.0f, "to_iso(1,0, shift).x");
    assert_eq(p4.y, 228.0f, "to_iso(1,0, shift).y");



    Logger::info("====================================");
    if (failed == 0) {
        Logger::info("All {} tests passed successfully!", passed);
    } else {
        Logger::error("{} tests failed out of {}!", failed, passed + failed);
    }

    return failed == 0 ? 0 : 1;
}
