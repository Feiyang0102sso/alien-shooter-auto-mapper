#pragma once
#include <algorithm>

namespace auto_mapper::core {

// 物理坐标点 (像素)
struct Point2D {
    float x;
    float y;
};

// 逻辑网格点
struct GridPoint {
    int x;
    int y;
};

// 物理空间的二维包围盒
struct BoundingBox {
    float min_x = 0.0f;
    float max_x = 0.0f;
    float min_y = 0.0f;
    float max_y = 0.0f;
    bool initialized = false;

    // 工具方法：根据传入的点扩展自身的包围盒边界
    void expand(const Point2D& p) {
        if (!initialized) {
            min_x = max_x = p.x;
            min_y = max_y = p.y;
            initialized = true;
        } else {
            min_x = std::min(min_x, p.x);
            max_x = std::max(max_x, p.x);
            min_y = std::min(min_y, p.y);
            max_y = std::max(max_y, p.y);
        }
    }
};

// 核心常量：等轴测投影跨度步长
constexpr float STEP_X = 40.0f;
constexpr float STEP_Y = 28.0f;

/**
 * 将逻辑格点 (x, y) 转换为等轴测物理坐标
 * 公式:
 *   posX = (gx - gy) * STEP_X + shift.x
 *   posY = (gx + gy) * STEP_Y + shift.y
 */
Point2D to_iso(const GridPoint& grid, const Point2D& shift = {0.0f, 0.0f});

/**
 * 计算居中偏移量，并强制进行编辑器引擎网格吸附
 *
 * @param bbox 在 shift=(0,0) 情况下，所有物件物理坐标构成的包围盒
 * @param map_size_x 地图物理尺寸宽度
 * @param map_size_y 地图物理尺寸高度
 * @return 计算出的偏置量 (shift_x, shift_y)
 */
Point2D calculate_shift(const BoundingBox& bbox, float map_size_x, float map_size_y);

} // namespace auto_mapper::core
