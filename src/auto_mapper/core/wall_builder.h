#pragma once

#include "auto_mapper/core/geometry.h"
#include "auto_mapper/io/map_writer.h"
#include <vector>

namespace auto_mapper::core {

// 一条用户拖拽并被前端纠偏后的直线线段
struct Segment {
    GridPoint start;
    GridPoint end;
};

class WallBuilder {
public:
    WallBuilder(float map_size_x = 600.0f, float map_size_y = 600.0f);

    // 核心管线：线段 -> 光栅化 -> 交点计算 -> Z-Order 排序 -> 偏置应用 -> 精灵生成
    std::vector<io::Sprite> build(const std::vector<Segment>& segments) const;

private:
    float map_size_x_;
    float map_size_y_;
};

} // namespace auto_mapper::core
