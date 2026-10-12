#include "editor/ui/tools/eraser/eraser_geometry.h"
#include "editor/project/project.h"
#include <QPointF>
#include <algorithm>
#include <cmath>
#include <optional>

namespace editor {
namespace {
double distanceToSegment(const QPointF &point, const QPointF &first, const QPointF &last) {
    QPointF delta = last - first;
    double squaredLength = QPointF::dotProduct(delta, delta);
    if (squaredLength == 0)
        return std::hypot(point.x() - first.x(), point.y() - first.y());
    double t = QPointF::dotProduct(point - first, delta) / squaredLength;
    t = std::clamp(t, 0.0, 1.0);
    QPointF nearest = first + t * delta;
    return std::hypot(point.x() - nearest.x(), point.y() - nearest.y());
}

// -1, 0 or +1 toward `to`.
int stepToward(int from, int to) {
    if (from < to)
        return 1;
    if (from > to)
        return -1;
    return 0;
}
} // namespace

bool eraseAround(std::vector<Segment> &segments, std::vector<DoorInstance> &doors, const QPoint &center, int size,
                 int wallType) {
    double radius = size - 0.5;
    QPointF centerPoint(center);
    std::vector<Segment> kept;
    bool changed = false;

    for (const auto &segment : segments) {
        if (segment.wall_type != wallType) {
            kept.push_back(segment);
            continue;
        }
        // Walk the segment one grid edge at a time and keep the runs outside the radius.
        QPoint current(segment.start.x, segment.start.y);
        QPoint end(segment.end.x, segment.end.y);
        QPoint step(stepToward(current.x(), end.x()), stepToward(current.y(), end.y()));
        std::optional<QPoint> runStart;
        while (current != end) {
            QPoint next = current + step;
            QPointF edgeMiddle = (QPointF(current) + QPointF(next)) / 2;
            double distance = std::hypot(centerPoint.x() - edgeMiddle.x(), centerPoint.y() - edgeMiddle.y());
            if (distance > radius) {
                if (!runStart)
                    runStart = current;
            } else {
                changed = true;
                if (runStart) {
                    kept.push_back({{runStart->x(), runStart->y()}, {current.x(), current.y()}, wallType});
                    runStart.reset();
                }
            }
            current = next;
        }
        if (runStart)
            kept.push_back({{runStart->x(), runStart->y()}, {end.x(), end.y()}, wallType});
    }

    size_t doorCountBefore = doors.size();
    std::erase_if(doors, [&](const DoorInstance &door) {
        if (door.wall_type != wallType)
            return false;
        auto ends = doorGridEndpoints(door);
        return distanceToSegment(centerPoint, ends.first, ends.second) <= radius;
    });

    segments = std::move(kept);
    if (doors.size() != doorCountBefore)
        changed = true;
    return changed;
}

} // namespace editor
