// Drawing tools. The numeric values are also the tool hotkeys 0..5.
// Freehand only draws Curve Walls; the grid tools (Polyline, Straight, Rectangle) hide while it is shown.
#pragma once

namespace editor {

enum class DrawingMode { Select = 0, Polyline = 1, Straight = 2, Rectangle = 3, Eraser = 4, Freehand = 5 };

inline constexpr int DRAWING_MODE_COUNT = 6;

} // namespace editor
