"""Controller for eraser input, preview, and map mutations."""
import math

from PySide6.QtCore import Qt
from PySide6.QtGui import QBrush, QColor, QPen

from app.editor.wall_profiles import get_wall_profile
from app.ui.tools.eraser.config import DEFAULT_ERASER_SIZE, ERASER_PREVIEW_ALPHA, ERASER_PREVIEW_COLOR
from app.ui.tools.eraser.geometry import erase_doors, erase_segments, get_effective_radius


class EraserToolController:
    """Keep all eraser-specific state and behavior in one place."""

    def __init__(self, viewport) -> None:
        self.viewport = viewport
        self.size = DEFAULT_ERASER_SIZE
        self.enabled = False
        self.is_erasing = False
        self.cursor_screen_pos = None

    def set_enabled(self, enabled: bool) -> None:
        """Enable or disable the eraser tool."""
        self.enabled = enabled
        self.is_erasing = False
        if not enabled:
            self.cursor_screen_pos = None

    def set_size(self, size: int) -> None:
        """Set the eraser size in grid units."""
        self.size = size
        self.viewport.update()

    def handle_left_press(self, screen_pos) -> bool:
        """Begin erasing and erase at the pressed position."""
        if not self.enabled:
            return False

        self.is_erasing = True
        self.cursor_screen_pos = screen_pos
        self.erase_at_screen_pos(screen_pos)
        self.viewport.update()
        return True

    def handle_mouse_move(self, screen_pos) -> bool:
        """Track cursor position for the eraser preview."""
        if not self.enabled:
            return False

        self.cursor_screen_pos = screen_pos
        self.viewport.update()
        return True

    def handle_left_release(self) -> bool:
        """Stop an active erase drag."""
        if not self.is_erasing:
            return False

        self.is_erasing = False
        return True

    def clear_hover(self) -> None:
        """Clear eraser hover preview state."""
        self.cursor_screen_pos = None

    def draw_preview(self, painter) -> None:
        """Draw the eraser range preview on the viewport."""
        if not self.enabled:
            return
        if self.cursor_screen_pos is None:
            return

        pixel_radius = self.get_preview_pixel_radius()

        preview_color = QColor(ERASER_PREVIEW_COLOR)
        preview_color.setAlpha(ERASER_PREVIEW_ALPHA)

        pen = QPen(QColor(ERASER_PREVIEW_COLOR))
        pen.setWidth(2)
        pen.setCosmetic(True)
        pen.setStyle(Qt.DashLine)
        painter.setPen(pen)
        painter.setBrush(QBrush(preview_color))
        painter.drawEllipse(self.cursor_screen_pos, pixel_radius, pixel_radius)

    def erase_at_screen_pos(self, screen_pos) -> None:
        """Erase map data at a viewport screen position."""
        grid_point = self.viewport.screen_to_grid(screen_pos)
        if grid_point is None:
            return

        radius = get_effective_radius(self.size)
        active_wall_type = int(self.viewport.active_wall_type)
        self.viewport.segments = erase_segments(
            self.viewport.segments,
            grid_point,
            radius,
            active_wall_type,
        )
        self.viewport.doors = erase_doors(
            self.viewport.doors,
            grid_point,
            radius,
            active_wall_type,
            self.viewport.get_door_grid_points,
        )

    def get_preview_pixel_radius(self) -> float:
        """Convert eraser radius from grid units to screen pixels."""
        radius = get_effective_radius(self.size)
        profile = get_wall_profile(self.viewport.active_wall_type)
        step_x = profile["step_x"]
        step_y = profile["step_y"]
        grid_physical_dist = math.sqrt(step_x * step_x + step_y * step_y)
        return radius * grid_physical_dist * self.viewport.zoom_factor
