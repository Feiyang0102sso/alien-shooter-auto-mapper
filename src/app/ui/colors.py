"""Shared UI colors used outside QSS styling."""

# Canvas surface colors.
CANVAS_BACKGROUND = "#10161a"
CANVAS_BOUNDARY = "#c0c0c0"
CANVAS_MARKER_OUTLINE = "#f1f6f3"
CANVAS_SEGMENT_DEFAULT = "#f1f6f3"
CANVAS_PREVIEW_SEGMENT = "#e0b95c"
CANVAS_LABEL_TEXT = "#d9e8e2"

# Wall profile accent colors.
WALL_PROFILE_STANDARD = "#4f7fbf"
WALL_PROFILE_LAB = "#2aa879"
WALL_PROFILE_STANDARD_DARK = "#7d8794"
WALL_PROFILE_FALLBACK = "#c0a05a"

# === Standard Wall Set (Base wall is Deep Blue #4f7fbf) ===
# 1. Active Door (Single line color, dot color changes by global 'is_door_open' property)
DOOR_STD_ACTIVE_LINE = "#ff8c00"        # Bright Orange (Contrast with deep blue)
DOOR_STD_ACTIVE_DOT_OPEN = "#00ff00"    # Bright Green
DOOR_STD_ACTIVE_DOT_CLOSED = "#ff0000"  # Bright Red

# 2. Dead Door (Closed)
DOOR_STD_DEAD_CLOSED_LINE = "#d9534f"   # Deep Coral Red
DOOR_STD_DEAD_CLOSED_DOT = "#a94442"    # Dark Red

# 3. Dead Door (Open)
DOOR_STD_DEAD_OPEN_LINE = "#a9c2d0"     # Light Ice Blue
DOOR_STD_DEAD_OPEN_DOT = "#d3d3d3"      # Light Gray

# 4. Dead Door (Jammed)
DOOR_STD_DEAD_JAMMED_LINE = "#f0ad4e"   # Amber Gold
DOOR_STD_DEAD_JAMMED_DOT = "#ffd700"    # Gold Yellow

# === Lab Wall Set (Base wall is Emerald Green #2aa879) ===
# 1. Laser Door with Laser (Closed state, has laser beam)
DOOR_LAB_LASER_WITH_LINE = "#e040fb"    # Vibrant Magenta / Neon Purple (Contrast with green)
DOOR_LAB_LASER_WITH_DOT = "#ff4081"     # Hot Pink

# 2. Laser Door without Laser (Open state, no laser beam)
DOOR_LAB_LASER_WITHOUT_LINE = "#7986cb" # Muted Indigo Blue
DOOR_LAB_LASER_WITHOUT_DOT = "#e8eaf6"  # Soft White-Blue

# 3. Lab Decoration Door (Static decorative frame)
DOOR_LAB_DECO_LINE = "#90a4ae"          # Cool Slate Gray
DOOR_LAB_DECO_DOT = "#cfd8dc"           # Light Slate


# Eraser preview colors.
ERASER_PREVIEW_COLOR = "#ff6b6b"
ERASER_PREVIEW_ALPHA = 80
