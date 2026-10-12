// Grid-space eraser: removes wall edges and doors of one wall type around a grid point.
#pragma once
#include "editor/project/project.h"
#include <QPoint>
#include <vector>

namespace editor {

// Eraser size range in grid units (the inspector slider uses the same bounds).
inline constexpr int ERASER_MIN_SIZE = 1;
inline constexpr int ERASER_MAX_SIZE = 10;

// Erase every wall edge whose midpoint, and every door whose span, lies within
// (size - 0.5) grid units of `center`. Other wall types are untouched.
// Returns true when anything was removed.
bool eraseAround(std::vector<Segment> &segments, std::vector<DoorInstance> &doors, const QPoint &center, int size,
                 int wallType);

} // namespace editor
