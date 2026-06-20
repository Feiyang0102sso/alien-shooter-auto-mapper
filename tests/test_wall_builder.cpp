#include "auto_mapper/core/wall_builder.h"
#include "auto_mapper/common/logger.h"
#include <iostream>
#include <vector>

using namespace auto_mapper;
using namespace auto_mapper::core;

int main() {
    Logger::init();
    Logger::info("Starting WallBuilder test...");

    WallBuilder builder(600.0f, 600.0f);

    // 构造一个简单的十字形交叉
    // 横向线段 B走向 (0,5) -> (5,5)
    // 纵向线段 A走向 (2,0) -> (2,10)
    std::vector<Segment> segments = {
        {{0, 5}, {5, 5}},
        {{2, 0}, {2, 10}}
    };

    Logger::info("Input segments: (0,5)->(5,5), (2,0)->(2,10)");

    std::vector<io::Sprite> sprites = builder.build(segments);

    Logger::info("WallBuilder produced {} sprites.", sprites.size());

    int num_pillars = 0;
    int num_wall_a = 0;
    int num_wall_b = 0;

    for (const auto& spr : sprites) {
        if (spr.vid == 604) num_pillars++;
        else if (spr.vid == 601) num_wall_a++;
        else if (spr.vid == 602) num_wall_b++;
    }

    Logger::info("Counts - Pillars: {}, Wall A: {}, Wall B: {}", num_pillars, num_wall_a, num_wall_b);

    // 理论计算：
    // 横向边 B走向，从 x=1 到 x=5，共 5 条边。
    // 纵向边 A走向，从 y=1 到 y=10，共 10 条边。
    // 所以 Wall A = 10, Wall B = 5.
    // 顶点：
    // 横向顶点: {0,5}, {1,5}, {2,5}, {3,5}, {4,5}, {5,5}
    // 纵向顶点: {2,0}, {2,1}, {2,2}, ..., {2,10}
    // 孤立端点：{0,5}, {5,5}, {2,0}, {2,10} -> 4个端点
    // 十字交汇：{2,5} -> 1个交汇
    // 总计立柱应为 5 根。

    bool success = true;

    if (num_wall_a != 10) {
        Logger::error("Expected 10 Wall A, got {}", num_wall_a);
        success = false;
    }
    if (num_wall_b != 5) {
        Logger::error("Expected 5 Wall B, got {}", num_wall_b);
        success = false;
    }
    if (num_pillars != 5) {
        Logger::error("Expected 5 Pillars, got {}", num_pillars);
        success = false;
    }

    if (success) {
        Logger::info("WallBuilder logic is PERFECT!");
        // 我们甚至可以直接调用 map_writer 把这个极其复杂的十字型导出为 map 文件！
        if (io::write_map(sprites, "test_wall_builder_output.map")) {
            Logger::info("Successfully dumped output to test_wall_builder_output.map");
        } else {
            Logger::error("Failed to write map output!");
            success = false;
        }
    }

    return success ? 0 : 1;
}
