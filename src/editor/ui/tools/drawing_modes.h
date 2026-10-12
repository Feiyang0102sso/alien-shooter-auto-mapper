// Drawing tools. The numeric values are also the tool hotkeys 0..4 and the toolbar action order.
#pragma once

namespace editor {

enum class DrawingMode { Select = 0, Polyline = 1, Straight = 2, Rectangle = 3, Eraser = 4 };

inline constexpr int DRAWING_MODE_COUNT = 5;

} // namespace editor
