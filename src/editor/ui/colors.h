// Shared UI colors used outside QSS styling (ported one-to-one from the former Python ui/colors.py).
// Widget chrome lives in resources/styles/*.qss; everything painted by code reads from here.
#pragma once
#include <QColor>

namespace editor::colors {

// Canvas surface colors.
inline constexpr const char *CANVAS_BACKGROUND = "#10161a";
inline constexpr const char *CANVAS_BOUNDARY = "#c0c0c0";
inline constexpr const char *CANVAS_MARKER_OUTLINE = "#f1f6f3";
inline constexpr const char *CANVAS_SEGMENT_DEFAULT = "#f1f6f3";
inline constexpr const char *CANVAS_PREVIEW_SEGMENT = "#e0b95c";
inline constexpr const char *CANVAS_LABEL_TEXT = "#d9e8e2";

// Wall profile accent colors.
inline constexpr const char *WALL_PROFILE_STANDARD = "#4f7fbf";
inline constexpr const char *WALL_PROFILE_LAB = "#2aa879";
inline constexpr const char *WALL_PROFILE_STANDARD_DARK = "#4dd0e1";
// AS2
inline constexpr const char *WALL_PROFILE_AS2_SET1_V1 = "#D97706"; // Base Terracotta
inline constexpr const char *WALL_PROFILE_AS2_SET1_V2 = "#F59E0B"; // Light Amber Gold
inline constexpr const char *WALL_PROFILE_AS2_SET1_V3 = "#92400E"; // Dark Red Bronze
inline constexpr const char *WALL_PROFILE_AS2_SET2 = "#FACC15";    // Tactical Yellow
inline constexpr const char *WALL_PROFILE_AS2_SET3 = "#10B981";    // Emerald Green
inline constexpr const char *WALL_PROFILE_AS2_SET4 = "#06B6D4";    // Electric Cyan
inline constexpr const char *WALL_PROFILE_AS2_SET5 = "#3B82F6";    // Sky Blue
inline constexpr const char *WALL_PROFILE_AS2_SET6 = "#8B5CF6";    // Violet Purple
inline constexpr const char *WALL_PROFILE_AS2_SET7 = "#FFB8DB";    // Neon Magenta
inline constexpr const char *WALL_PROFILE_AS2_SET8 = "#CBD5E1";    // Platinum Steel Gray
inline constexpr const char *WALL_PROFILE_AS2_SET9 = "#84CC16";    // Toxic Lime Green
inline constexpr const char *WALL_PROFILE_FALLBACK = "#c0a05a";
// AS2 unified door lines
inline constexpr const char *DOOR_AS2_LINE = "#FF3333"; // Tactical Red Door
inline constexpr const char *DOOR_AS2_DOT = "#FF0000";

// === Standard Wall Set (Base wall is Deep Blue #4f7fbf) ===
// 1. Active Door (Single line color, dot color changes by global 'is_door_open' property)
inline constexpr const char *DOOR_STD_ACTIVE_LINE = "#ff8c00";       // Bright Orange (Contrast with deep blue)
inline constexpr const char *DOOR_STD_ACTIVE_DOT_OPEN = "#00ff00";   // Bright Green
inline constexpr const char *DOOR_STD_ACTIVE_DOT_CLOSED = "#ff0000"; // Bright Red

// 2. Dead Door (Closed)
inline constexpr const char *DOOR_STD_DEAD_CLOSED_LINE = "#d9534f"; // Deep Coral Red
inline constexpr const char *DOOR_STD_DEAD_CLOSED_DOT = "#a94442";  // Dark Red

// 3. Dead Door (Open)
inline constexpr const char *DOOR_STD_DEAD_OPEN_LINE = "#a9c2d0"; // Light Ice Blue
inline constexpr const char *DOOR_STD_DEAD_OPEN_DOT = "#d3d3d3";  // Light Gray

// 4. Dead Door (Jammed)
inline constexpr const char *DOOR_STD_DEAD_JAMMED_LINE = "#f0ad4e"; // Amber Gold
inline constexpr const char *DOOR_STD_DEAD_JAMMED_DOT = "#ffd700";  // Gold Yellow

// === Lab Wall Set (Base wall is Emerald Green #2aa879) ===
// 1. Laser Door with Laser (Closed state, has laser beam)
inline constexpr const char *DOOR_LAB_LASER_WITH_LINE = "#e040fb"; // Vibrant Magenta / Neon Purple
inline constexpr const char *DOOR_LAB_LASER_WITH_DOT = "#ff4081";  // Hot Pink

// 2. Laser Door without Laser (Open state, no laser beam)
inline constexpr const char *DOOR_LAB_LASER_WITHOUT_LINE = "#7986cb"; // Muted Indigo Blue
inline constexpr const char *DOOR_LAB_LASER_WITHOUT_DOT = "#e8eaf6";  // Soft White-Blue

// 3. Lab Decoration Door (Static decorative frame)
inline constexpr const char *DOOR_LAB_DECO_LINE = "#90a4ae"; // Cool Slate Gray
inline constexpr const char *DOOR_LAB_DECO_DOT = "#cfd8dc";  // Light Slate

// Decoration area colors.
inline constexpr const char *DECORATION_AREA_FILL = "#244338";           // Filled area inside a placed decoration frame.
inline constexpr int DECORATION_AREA_FILL_ALPHA = 70;                    // Keep the decoration frame translucent over the grid.
inline constexpr const char *DECORATION_AREA_BORDER = "#45c797";         // Default decoration frame border.
inline constexpr const char *DECORATION_AREA_SELECTED_BORDER = "#f0ad4e"; // Highlight border for the selected decoration.
inline constexpr const char *DECORATION_AREA_PREVIEW_BORDER = "#f0ad4e";  // Dashed border while drawing a new decoration.
inline constexpr const char *DECORATION_STAMP_GHOST_BORDER = "#45c797";   // Dashed ghost frame following the cursor.
inline constexpr const char *DECORATION_STAMP_OUT_OF_BOUNDS = "#e05252";  // Frame border when a stamp sticks out of the map.

// Incubator preview unit colors.
inline constexpr const char *INCUBATOR_PREVIEW_BORDER = "#69f0ae"; // Border for each generated incubator footprint.
inline constexpr const char *INCUBATOR_PREVIEW_FILL = "#145046";   // Fill for each generated incubator footprint.
inline constexpr int INCUBATOR_PREVIEW_FILL_ALPHA = 120;           // Make footprint markers visible without hiding the grid.

// Eraser preview colors.
inline constexpr const char *ERASER_PREVIEW_COLOR = "#ff6b6b";
inline constexpr int ERASER_PREVIEW_ALPHA = 80;

// Build a translucent QColor from a hex constant and an alpha value.
inline QColor withAlpha(const char *hex, int alpha) {
    QColor color(hex);
    color.setAlpha(alpha);
    return color;
}

// Accent color of one wall type; walls, legends and the canvas all read this one mapping.
QColor wallColor(int wallType);

} // namespace editor::colors
