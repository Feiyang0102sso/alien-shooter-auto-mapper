#include "auto_mapper/core/wall_builder.h"
#include <set>
#include <algorithm>
#include <utility>

namespace auto_mapper::core {

WallBuilder::WallBuilder(float map_size_x, float map_size_y)
    : map_size_x_(map_size_x), map_size_y_(map_size_y) {}

std::vector<io::Sprite> WallBuilder::build(const std::vector<Segment>& segments) const {
    using Point = std::pair<int, int>;
    std::set<Point> edges_a; // 走向A (/) 的边，记录点(x,y)，表示从 (x, y-1) 到 (x, y)
    std::set<Point> edges_b; // 走向B (\) 的边，记录点(x,y)，表示从 (x-1, y) 到 (x, y)

    // 1. 光栅化线段 (栅格化为基本单元边)
    for (const auto& seg : segments) {
        int x1 = seg.start.x;
        int y1 = seg.start.y;
        int x2 = seg.end.x;
        int y2 = seg.end.y;

        if (x1 == x2) {
            // 平行于 Y 轴，A走向
            int minY = std::min(y1, y2);
            int maxY = std::max(y1, y2);
            for (int y = minY + 1; y <= maxY; ++y) {
                edges_a.insert({x1, y});
            }
        } else if (y1 == y2) {
            // 平行于 X 轴，B走向
            int minX = std::min(x1, x2);
            int maxX = std::max(x1, x2);
            for (int x = minX + 1; x <= maxX; ++x) {
                edges_b.insert({x, y1});
            }
        }
    }

    // 用于收集带网格坐标的原始精灵
    struct RawSprite {
        int vid;
        int gx;
        int gy;
    };
    std::vector<RawSprite> raw_sprites;

    // 2. 基准墙壁图块分配
    for (const auto& p : edges_a) {
        // TODO: 支持变体墙，暂时固定为 601
        raw_sprites.push_back({601, p.first, p.second});
    }

    for (const auto& p : edges_b) {
        // TODO: 支持变体墙，暂时固定为 602
        raw_sprites.push_back({602, p.first, p.second});
    }

    // 3. 拓扑点提取与立柱放置分析
    std::set<Point> vertices;
    for (const auto& p : edges_a) {
        vertices.insert({p.first, p.second - 1});
        vertices.insert({p.first, p.second});
    }
    for (const auto& p : edges_b) {
        vertices.insert({p.first - 1, p.second});
        vertices.insert({p.first, p.second});
    }

    for (const auto& v : vertices) {
        int x = v.first;
        int y = v.second;
        
        // 探测四个方向的连接度
        bool up    = edges_a.count({x, y}) > 0;
        bool down  = edges_a.count({x, y + 1}) > 0;
        bool left  = edges_b.count({x, y}) > 0;
        bool right = edges_b.count({x + 1, y}) > 0;

        bool conn_a = up || down;
        bool conn_b = left || right;

        int total_conns = (up ? 1 : 0) + (down ? 1 : 0) + (left ? 1 : 0) + (right ? 1 : 0);

        // 如果是孤立端点，或者遇到了拐角/T字/十字交汇口，都需要放立柱
        if (total_conns == 1 || (conn_a && conn_b)) {
            raw_sprites.push_back({604, x, y});
        }
    }

    // 4. Z-Order 深度排序 (自后向前绘制，以防止渲染遮挡)
    std::sort(raw_sprites.begin(), raw_sprites.end(), [](const RawSprite& a, const RawSprite& b) {
        return (a.gx + a.gy) < (b.gx + b.gy);
    });

    // 5. 调用 geometry 计算整体居中偏移量
    BoundingBox bbox;
    for (const auto& rs : raw_sprites) {
        Point2D pos = to_iso({rs.gx, rs.gy}, {0.0f, 0.0f});
        if (rs.vid == 602) {
            // 针对 602(走向B的墙壁) 的特殊视觉修正
            pos.x -= 40.0f;
        }
        bbox.expand(pos);
    }
    Point2D shift = calculate_shift(bbox, map_size_x_, map_size_y_);

    // 6. 生成最终的 io::Sprite 列表
    std::vector<io::Sprite> final_sprites;
    for (const auto& rs : raw_sprites) {
        Point2D pos = to_iso({rs.gx, rs.gy}, shift);
        if (rs.vid == 602) {
            pos.x -= 40.0f;
        }
        
        io::Sprite spr;
        spr.vid = rs.vid;
        spr.posX = pos.x;
        spr.posY = pos.y;
        spr.posZ = 0.0f;
        spr.direction = 32; // 默认方向
        spr.army = 0;       // 中立物体
        
        final_sprites.push_back(spr);
    }

    return final_sprites;
}

} // namespace auto_mapper::core
