"""
Interactive isometric viewport.
"""
import math

from PySide6.QtCore import QPointF, Qt, Signal
from PySide6.QtGui import QBrush, QColor, QCursor, QPainter, QPainterPath, QPen, QPolygonF
from PySide6.QtWidgets import QWidget

from app.editor.drawable_parts import PART_WALL_BODY
from app.editor.decorations import INCUBATOR_PLACEHOLDER_HEIGHT, INCUBATOR_PLACEHOLDER_WIDTH
from app.editor.wall_profiles import find_wall_type_by_steps, get_default_wall_type, get_wall_profile
from app.i18n.locale import tr
from app.i18n.text_keys import TextKey
from app.project.data import DECORATION_TYPE_INCUBATOR_ARRAY, DEFAULT_MAP_SIZE_X, DEFAULT_MAP_SIZE_Y, IncubatorDecoration
from app.ui.colors import (
    CANVAS_BACKGROUND,
    CANVAS_BOUNDARY,
    CANVAS_LABEL_TEXT,
    CANVAS_MARKER_OUTLINE,
    CANVAS_PREVIEW_SEGMENT,
    CANVAS_SEGMENT_DEFAULT,
    DOOR_STD_ACTIVE_LINE,
    DOOR_STD_ACTIVE_DOT_OPEN,
    DOOR_STD_ACTIVE_DOT_CLOSED,
    DOOR_STD_DEAD_CLOSED_LINE,
    DOOR_STD_DEAD_CLOSED_DOT,
    DOOR_STD_DEAD_OPEN_LINE,
    DOOR_STD_DEAD_OPEN_DOT,
    DOOR_STD_DEAD_JAMMED_LINE,
    DOOR_STD_DEAD_JAMMED_DOT,
    DOOR_LAB_LASER_WITH_LINE,
    DOOR_LAB_LASER_WITH_DOT,
    DOOR_LAB_LASER_WITHOUT_LINE,
    DOOR_LAB_LASER_WITHOUT_DOT,
    DOOR_LAB_DECO_LINE,
    DOOR_LAB_DECO_DOT,
)
from app.ui.tools.drawing_modes import DrawingMode
from app.ui.tools.drawing_tool import DrawingToolController
from app.ui.tools.eraser import EraserToolController


MIN_ZOOM = 0.002
MAX_ZOOM = 30.0
MIN_GRID_COLUMNS = 1
MIN_GRID_ROWS = 1
DOOR_STATE_CLOSED = 0
LIGHT_STATE_GREEN = 0
LIGHT_STATE_RED = 1
LIGHT_STATE_BROKEN = 2


class MapViewport(QWidget):
    """
    Isometric canvas with pan, zoom, and grid-point selection.
    """

    grid_point_selected = Signal(int, int)
    cursor_grid_changed = Signal(int, int)
    view_changed = Signal(float)
    segment_started = Signal(int, int)
    segment_created = Signal(int, int, int, int, int)
    door_created = Signal(int, int, int)
    decoration_created = Signal(int)
    decoration_selected = Signal(object)
    decoration_changed = Signal(object)
    drawing_cancelled = Signal()

    def __init__(self) -> None:
        super().__init__()
        self.setObjectName("mapViewport")
        self.setMinimumSize(720, 540)
        self.setMouseTracking(True)
        self.setCursor(QCursor(Qt.CrossCursor))

        self.active_wall_type = get_default_wall_type()
        default_profile = get_wall_profile(self.active_wall_type)
        self.theme_color = QColor(default_profile["color"])
        self.grid_color = QColor(default_profile["color"])
        self.active_drawable_part = PART_WALL_BODY
        self.map_size_x = DEFAULT_MAP_SIZE_X
        self.map_size_y = DEFAULT_MAP_SIZE_Y
        self.grid_columns = 1
        self.grid_rows = 1
        self.zoom_factor = 1.0
        self.pan_offset = QPointF(0.0, 0.0)
        self.selected_grid_point = None
        self.pending_start_point = None
        self.preview_end_point = None
        self.segments = []
        self.doors = []
        self.decorations = []
        self.is_door_open = False
        self.drawing_tool = DrawingToolController(self)
        self.eraser_tool = EraserToolController(self)
        self.active_decoration_type = None
        self.selected_decoration_index = None
        self.decoration_start_physical = None
        self.decoration_preview_physical = None
        self.decoration_drag_mode = None
        self.decoration_drag_start_physical = None
        self.decoration_drag_original = None
        self.last_cursor_grid_point = None
        self.is_panning = False
        self.pan_start_position = QPointF(0.0, 0.0)
        self.pan_start_offset = QPointF(0.0, 0.0)

        self.set_map_size(DEFAULT_MAP_SIZE_X, DEFAULT_MAP_SIZE_Y, fit_to_view=False)

    def set_theme(self, theme_id: str) -> None:
        """
        Update the grid accent color from the selected theme.
        """
        self.update()

    def set_is_door_open(self, is_open: bool) -> None:
        """
        Set whether standard doors are open globally and redraw.
        """
        self.is_door_open = is_open
        self.update()

    def set_wall_type(self, wall_type: int) -> None:
        """
        Update the active wall profile and redraw the current grid.
        """
        self.clear_active_decoration_tool()
        self.active_wall_type = wall_type
        profile = get_wall_profile(wall_type)
        self.theme_color = QColor(profile["color"])
        self._recalculate_grid_limits()
        self.fit_map_to_view()
        self.update()

    def set_drawable_part(self, part_id: str) -> None:
        """
        Update the current drawable tool.
        """
        self.active_drawable_part = part_id
        self.clear_active_decoration_tool()
        self.cancel_pending_segment()
        self.update()

    def clear_active_decoration_tool(self) -> None:
        """
        Exit decoration drawing and dragging state.
        """
        self.active_decoration_type = None
        self.decoration_start_physical = None
        self.decoration_preview_physical = None
        self.decoration_drag_mode = None
        self.decoration_drag_start_physical = None
        self.decoration_drag_original = None

    def set_active_decoration(self, decoration_type: str) -> None:
        """
        Select a decoration drawing target.
        """
        self.active_decoration_type = decoration_type
        self.active_drawable_part = ""
        self.cancel_pending_segment()
        self.decoration_start_physical = None
        self.decoration_preview_physical = None
        self.update()

    def set_drawing_mode(self, drawing_mode) -> None:
        """
        Update the active drawing tool mode.
        """
        if drawing_mode != DrawingMode.RECTANGLE and self.active_decoration_type is not None:
            self.active_decoration_type = None
            self.active_drawable_part = PART_WALL_BODY
            self.decoration_start_physical = None
            self.decoration_preview_physical = None

        self.drawing_tool.set_mode(drawing_mode)
        self.pending_start_point = None
        self.preview_end_point = None
        self.eraser_tool.set_enabled(drawing_mode == DrawingMode.ERASER)
        self.update()

    def set_eraser_size(self, size: int) -> None:
        """Set eraser size in grid units."""
        self.eraser_tool.set_size(size)

    def set_map_size(self, map_size_x: float, map_size_y: float, fit_to_view: bool = True) -> None:
        """
        Update physical map size and derived logical grid limits.
        """
        self.map_size_x = map_size_x
        self.map_size_y = map_size_y
        self._recalculate_grid_limits()

        if fit_to_view:
            self.fit_map_to_view()

        self.update()

    def paintEvent(self, event) -> None:
        """
        Draw the current isometric grid state.
        """
        painter = QPainter(self)
        painter.setRenderHint(QPainter.Antialiasing, True)

        self._fill_background(painter)
        self._draw_grid(painter)
        self._draw_decorations(painter)
        self._draw_segments(painter)
        self._draw_doors(painter)
        self._draw_decoration_preview(painter)
        self._draw_preview_segment(painter)
        self._draw_selected_point(painter)
        self._draw_origin_marker(painter)
        self.eraser_tool.draw_preview(painter)

    def mousePressEvent(self, event) -> None:
        """
        Select grid points or begin viewport panning.
        """
        if event.button() == Qt.MiddleButton:
            self.is_panning = True
            self.pan_start_position = event.position()
            self.pan_start_offset = QPointF(self.pan_offset)
            self.setCursor(QCursor(Qt.ClosedHandCursor))
            event.accept()
            return

        if event.button() == Qt.LeftButton:
            if self.decoration_drag_mode is not None:
                event.accept()
                return

            if self.eraser_tool.handle_left_press(event.position()):
                event.accept()
                return

            if self.active_decoration_type is not None and self._try_start_decoration_drag(event.position()):
                self.update()
                event.accept()
                return

            if self._handle_decoration_left_click(event.position()):
                self.update()
                event.accept()
                return

            grid_point = self.screen_to_grid(event.position())
            self._handle_left_click(grid_point)
            self.update()
            event.accept()
            return

        if event.button() == Qt.RightButton:
            if self._cancel_pending_decoration():
                self.update()
                event.accept()
                return

            self.cancel_pending_segment()
            self.update()
            event.accept()
            return

        super().mousePressEvent(event)

    def mouseMoveEvent(self, event) -> None:
        """
        Update pan offset or report hover grid coordinates.
        """
        if self.is_panning:
            current_position = event.position()
            delta = current_position - self.pan_start_position
            self.pan_offset = self.pan_start_offset + delta
            self.update()
            event.accept()
            return

        if self.decoration_drag_mode is not None:
            self._update_decoration_drag(event.position())
            self.update()
            event.accept()
            return

        self.eraser_tool.handle_mouse_move(event.position())
        self._update_decoration_cursor(event.position())

        grid_point = self.screen_to_grid(event.position())
        if grid_point is not None:
            if grid_point != self.last_cursor_grid_point:
                self.last_cursor_grid_point = grid_point
                self.cursor_grid_changed.emit(grid_point[0], grid_point[1])

            if self.pending_start_point is not None:
                self.drawing_tool.update_preview(grid_point)
                self.update()

            if self.decoration_start_physical is not None:
                physical_point = self.screen_to_physical(event.position())
                self.decoration_preview_physical = self._clamp_physical_point(physical_point)
                self.update()

        super().mouseMoveEvent(event)

    def mouseReleaseEvent(self, event) -> None:
        """
        End viewport panning.
        """
        if event.button() == Qt.MiddleButton and self.is_panning:
            self.is_panning = False
            self.setCursor(QCursor(Qt.CrossCursor))
            event.accept()
            return

        if event.button() == Qt.LeftButton and self.eraser_tool.handle_left_release():
            event.accept()
            return

        if event.button() == Qt.LeftButton and self.decoration_drag_mode is not None:
            self.decoration_drag_mode = None
            self.decoration_drag_start_physical = None
            self.decoration_drag_original = None
            self.setCursor(QCursor(Qt.CrossCursor))
            event.accept()
            return

        super().mouseReleaseEvent(event)

    def leaveEvent(self, event) -> None:
        """
        Clear hover coordinate state when the cursor leaves the canvas.
        """
        self.last_cursor_grid_point = None
        self.eraser_tool.clear_hover()
        if self.decoration_drag_mode is None:
            self.setCursor(QCursor(Qt.CrossCursor))
        super().leaveEvent(event)

    def wheelEvent(self, event) -> None:
        """
        Zoom around the cursor position.
        """
        wheel_delta = event.angleDelta().y()
        if wheel_delta == 0:
            event.accept()
            return

        old_zoom = self.zoom_factor
        if wheel_delta > 0:
            new_zoom = old_zoom * 1.15
        else:
            new_zoom = old_zoom / 1.15

        if new_zoom < MIN_ZOOM:
            new_zoom = MIN_ZOOM
        if new_zoom > MAX_ZOOM:
            new_zoom = MAX_ZOOM

        cursor_position = event.position()
        physical_before_zoom = self.screen_to_physical(cursor_position)
        self.zoom_factor = new_zoom
        physical_after_zoom = self.screen_to_physical(cursor_position)

        physical_delta = physical_after_zoom - physical_before_zoom
        self.pan_offset = self.pan_offset + physical_delta * self.zoom_factor

        self.view_changed.emit(self.zoom_factor)
        self.update()
        event.accept()

    def _fill_background(self, painter: QPainter) -> None:
        painter.fillRect(self.rect(), QColor(CANVAS_BACKGROUND))

    def _draw_grid(self, painter: QPainter) -> None:
        profile = get_wall_profile(self.active_wall_type)
        step_x = profile["step_x"]
        step_y = profile["step_y"]

        # 1. 璁＄畻褰撳墠缂╂斁姣斾緥涓嬶紝鐩搁偦缃戞牸绾跨殑灞忓箷鍍忕礌闂磋窛
        # 鐩搁偦缃戞牸鐐逛箣闂寸殑鐗╃悊璺濈鏄?math.sqrt(step_x**2 + step_y**2)
        # 灞忓箷涓婄殑璺濈闇€瑕佷箻涓婂綋鍓嶇殑 zoom_factor
        grid_physical_dist = math.sqrt(step_x * step_x + step_y * step_y)
        grid_pixel_dist = self.zoom_factor * grid_physical_dist

        # 2. 鏍规嵁鐩搁偦缃戞牸鐨勫睆骞曞儚绱犻棿璺濓紝鑷€傚簲鍦拌皟鏁寸粯鍒舵闀?(LOD)
        if grid_pixel_dist >= 12.0:
            draw_step = 1
        elif grid_pixel_dist >= 6.0:
            draw_step = 2
        elif grid_pixel_dist >= 3.0:
            draw_step = 5
        elif grid_pixel_dist >= 1.5:
            draw_step = 10
        elif grid_pixel_dist >= 0.5:
            draw_step = 50
        else:
            # 缂╂斁姣斾緥鏋佸皬鏃讹紝缃戞牸绾夸細瀵嗛泦鎴愪竴鐗囷紝姝ゆ椂瀹屽叏涓嶇粯鍒剁綉鏍肩嚎浠ヤ紭鍖栨€ц兘锛屼粎鐢诲杈圭晫
            draw_step = None

        if draw_step is not None:
            # 3. 璁＄畻鍙鍖哄煙鍐呯殑缃戞牸琛屽拰鍒楃殑鑼冨洿 (Frustum Culling)
            w = self.width()
            h = self.height()
            screen_corners = [
                QPointF(0.0, 0.0),
                QPointF(w, 0.0),
                QPointF(0.0, h),
                QPointF(w, h),
            ]

            visible_x = []
            visible_y = []
            for corner in screen_corners:
                physical_pt = self.screen_to_physical(corner)
                grid_x, grid_y = self.physical_to_grid(physical_pt, self.active_wall_type)
                visible_x.append(grid_x)
                visible_y.append(grid_y)

            # Expand one grid outward to avoid edge gaps while panning or zooming.
            min_visible_x = math.floor(min(visible_x)) - 1
            max_visible_x = math.ceil(max(visible_x)) + 1
            min_visible_y = math.floor(min(visible_y)) - 1
            max_visible_y = math.ceil(max(visible_y)) + 1

            # 缃戞牸鍏ㄥ眬鏈夋晥鑼冨洿闄愬埗
            min_grid, max_grid = self._get_grid_draw_range(step_x, step_y)

            grid_pen = QPen(self.grid_color)
            grid_pen.setWidth(1)
            grid_pen.setCosmetic(True)
            painter.setPen(grid_pen)

            # Draw row lines with fixed y and changing x.
            start_row = math.ceil(max(min_grid, min_visible_y) / draw_step) * draw_step
            end_row = min(max_grid, max_visible_y)
            start_x = max(min_grid, min_visible_x)
            end_x = min(max_grid, max_visible_x)

            if start_x <= end_x:
                row = start_row
                while row <= end_row:
                    start = self.grid_to_screen(start_x, row, self.active_wall_type)
                    end = self.grid_to_screen(end_x, row, self.active_wall_type)
                    painter.drawLine(start, end)
                    row += draw_step

            # Draw column lines with fixed x and changing y.
            start_col = math.ceil(max(min_grid, min_visible_x) / draw_step) * draw_step
            end_col = min(max_grid, max_visible_x)
            start_y = max(min_grid, min_visible_y)
            end_y = min(max_grid, max_visible_y)

            if start_y <= end_y:
                column = start_col
                while column <= end_col:
                    start = self.grid_to_screen(column, start_y, self.active_wall_type)
                    end = self.grid_to_screen(column, end_y, self.active_wall_type)
                    painter.drawLine(start, end)
                    column += draw_step

        # Draw the usable physical map boundary.
        bounds = self._get_physical_bounds(step_x, step_y)
        top_left = self._physical_to_screen(QPointF(bounds[0], bounds[1]))
        bottom_right = self._physical_to_screen(QPointF(bounds[2], bounds[3]))

        border_pen = QPen(QColor(CANVAS_BOUNDARY))
        border_pen.setWidth(2)
        border_pen.setCosmetic(True)
        painter.setPen(border_pen)
        painter.setBrush(Qt.NoBrush)
        painter.drawRect(
            top_left.x(), top_left.y(),
            bottom_right.x() - top_left.x(),
            bottom_right.y() - top_left.y(),
        )

    def _draw_selected_point(self, painter: QPainter) -> None:
        if self.selected_grid_point is None:
            return

        grid_x = self.selected_grid_point[0]
        grid_y = self.selected_grid_point[1]
        center = self.grid_to_screen(grid_x, grid_y, self.active_wall_type)

        marker_pen = QPen(QColor(CANVAS_MARKER_OUTLINE))
        marker_pen.setWidth(2)
        marker_pen.setCosmetic(True)
        painter.setPen(marker_pen)
        painter.setBrush(self.theme_color)
        painter.drawEllipse(center, 6, 6)

    def _draw_segments(self, painter: QPainter) -> None:
        segment_pen = QPen(QColor(CANVAS_SEGMENT_DEFAULT))
        segment_pen.setWidth(4)
        segment_pen.setCosmetic(True)
        painter.setPen(segment_pen)

        for segment in self.segments:
            start_point = segment[0]
            end_point = segment[1]
            wall_type = segment[2]
            profile = get_wall_profile(wall_type)

            segment_pen = QPen(QColor(profile["color"]))
            segment_pen.setWidth(4)
            segment_pen.setCosmetic(True)
            painter.setPen(segment_pen)

            screen_start = self.grid_to_screen(start_point[0], start_point[1], wall_type)
            screen_end = self.grid_to_screen(end_point[0], end_point[1], wall_type)
            painter.drawLine(screen_start, screen_end)

    def _draw_preview_segment(self, painter: QPainter) -> None:
        if self.pending_start_point is None:
            return
        preview_segments = self.drawing_tool.get_preview_segments()
        if not preview_segments:
            return

        preview_pen = QPen(QColor(CANVAS_PREVIEW_SEGMENT))
        preview_pen.setWidth(3)
        preview_pen.setCosmetic(True)
        preview_pen.setStyle(Qt.DashLine)
        painter.setPen(preview_pen)

        for segment in preview_segments:
            start_point = segment[0]
            end_point = segment[1]
            wall_type = segment[2]
            screen_start = self.grid_to_screen(start_point[0], start_point[1], wall_type)
            screen_end = self.grid_to_screen(end_point[0], end_point[1], wall_type)
            painter.drawLine(screen_start, screen_end)

    def _draw_doors(self, painter: QPainter) -> None:
        """
        Draw door markers as a line with one status dot in the middle.
        """
        for door in self.doors:
            door_points = self._get_door_grid_points(door)
            start_point = door_points[0]
            end_point = door_points[1]
            wall_type = int(door[2])
            door_state = int(door[5])
            light_state = int(door[6])
            z_offset = float(door[7])

            screen_start = self.grid_to_screen(start_point[0], start_point[1], wall_type)
            screen_end = self.grid_to_screen(end_point[0], end_point[1], wall_type)
            colors = self._get_door_colors(wall_type, door_state, light_state, z_offset)
            line_color = colors[0]
            dot_color = colors[1]
            hollow = colors[2]

            door_pen = QPen(QColor(line_color))
            door_pen.setWidth(5)
            door_pen.setCosmetic(True)
            painter.setPen(door_pen)
            painter.drawLine(screen_start, screen_end)

            mid_x = (screen_start.x() + screen_end.x()) / 2.0
            mid_y = (screen_start.y() + screen_end.y()) / 2.0
            radius = 5
            dot_pen = QPen(QColor(line_color))
            dot_pen.setWidth(2)
            dot_pen.setCosmetic(True)
            painter.setPen(dot_pen)

            if hollow:
                painter.setBrush(QBrush(QColor(CANVAS_BACKGROUND)))
            else:
                painter.setBrush(QBrush(QColor(dot_color)))

            painter.drawEllipse(QPointF(mid_x, mid_y), radius, radius)

    def _draw_decorations(self, painter: QPainter) -> None:
        """
        Draw incubator decoration areas and placeholder units.
        """
        index = 0
        for decoration in self.decorations:
            corners = self._get_decoration_corners(decoration)
            self._draw_decoration_rect(painter, corners, index == self.selected_decoration_index)
            self._draw_incubator_placeholders(painter, decoration)
            index += 1

    def _draw_decoration_rect(self, painter: QPainter, rect: tuple, selected: bool) -> None:
        """
        Draw one decoration parallelogram in physical map coordinates.
        """
        fill_color = QColor("#244338")
        fill_color.setAlpha(70)
        border_color = QColor("#45c797")
        if selected:
            border_color = QColor("#f0ad4e")

        painter.setBrush(QBrush(fill_color))
        pen = QPen(border_color)
        pen.setWidth(2)
        pen.setCosmetic(True)
        painter.setPen(pen)
        painter.drawPolygon(self._physical_polygon_to_screen(rect))

    def _draw_incubator_placeholders(self, painter: QPainter, decoration: IncubatorDecoration) -> None:
        """
        Fill the decoration area with incubator placeholder rectangles.
        """
        item_step = self._get_incubator_item_spacing() * decoration.item_spacing_scale
        row_step = self._get_incubator_row_spacing() * decoration.row_spacing_scale

        if item_step <= 0.0:
            return
        if row_step <= 0.0:
            return

        row_count = int(decoration.column_length / row_step) + 1
        item_count = int(decoration.row_length / item_step) + 1
        axes = self._get_decoration_axes()
        row_axis = axes[0]
        column_axis = axes[1]

        row_index = 0
        while row_index < row_count:
            item_index = 0
            while item_index < item_count:
                row_offset = item_index * item_step
                column_offset = row_index * row_step
                x = decoration.start_x + row_axis.x() * row_offset + column_axis.x() * column_offset
                y = decoration.start_y + row_axis.y() * row_offset + column_axis.y() * column_offset
                self._draw_incubator_placeholder(painter, QPointF(x, y))
                item_index += 1

            row_index += 1

    def _draw_incubator_placeholder(self, painter: QPainter, origin: QPointF) -> None:
        """
        Draw one incubator placeholder along the wall-set axes.
        """
        corners = self._build_decoration_corners(
            origin,
            INCUBATOR_PLACEHOLDER_WIDTH,
            INCUBATOR_PLACEHOLDER_HEIGHT,
        )

        pen = QPen(QColor("#69f0ae"))
        pen.setWidth(1)
        pen.setCosmetic(True)
        painter.setPen(pen)
        painter.setBrush(QBrush(QColor(20, 80, 70, 120)))
        painter.drawPolygon(self._physical_polygon_to_screen(corners))

    def _draw_decoration_preview(self, painter: QPainter) -> None:
        """
        Draw a pending decoration rectangle preview.
        """
        if self.decoration_start_physical is None:
            return
        if self.decoration_preview_physical is None:
            return

        preview = self._build_decoration_from_points(
            self.decoration_start_physical,
            self.decoration_preview_physical,
        )
        corners = self._get_decoration_corners(preview)
        pen = QPen(QColor("#f0ad4e"))
        pen.setWidth(2)
        pen.setCosmetic(True)
        pen.setStyle(Qt.DashLine)
        painter.setPen(pen)
        painter.setBrush(Qt.NoBrush)
        painter.drawPolygon(self._physical_polygon_to_screen(corners))

    def _draw_origin_marker(self, painter: QPainter) -> None:
        label_pen = QPen(QColor(CANVAS_LABEL_TEXT))
        painter.setPen(label_pen)
        painter.drawText(24, 32, tr(TextKey.CANVAS_TITLE))
        profile = get_wall_profile(self.active_wall_type)
        size_text = tr(
            TextKey.CANVAS_INFO,
            map_size_x=self.map_size_x,
            map_size_y=self.map_size_y,
            wall_label=profile["label"],
            grid_columns=self.grid_columns,
            grid_rows=self.grid_rows,
        )
        painter.drawText(24, 54, size_text)
        painter.drawText(24, 76, tr(TextKey.CANVAS_HELP))

    def _handle_left_click(self, grid_point) -> None:
        """
        Start or finish a wall segment from a grid click.
        """
        if grid_point is None:
            self.selected_grid_point = None
            return

        self.selected_grid_point = grid_point
        self.grid_point_selected.emit(grid_point[0], grid_point[1])

        was_waiting_for_end_point = self.drawing_tool.is_waiting_for_end_point()
        result = self.drawing_tool.handle_left_click(grid_point)
        self.pending_start_point = self.drawing_tool.get_start_point()
        self.preview_end_point = None

        if not was_waiting_for_end_point and self.pending_start_point is not None:
            self.segment_started.emit(grid_point[0], grid_point[1])
            return

        for segment in result["segments"]:
            self.segments.append(segment)
            count = len(self.segments)
            start_point = segment[0]
            end_point = segment[1]
            self.segment_created.emit(start_point[0], start_point[1], end_point[0], end_point[1], count)

        for door in result["doors"]:
            self.doors.append(door)
            self.door_created.emit(door[0], door[1], len(self.doors))

        self.pending_start_point = self.drawing_tool.get_start_point()

    def _handle_decoration_left_click(self, screen_point: QPointF) -> bool:
        """
        Start or finish an incubator decoration rectangle.
        """
        if self.active_decoration_type != DECORATION_TYPE_INCUBATOR_ARRAY:
            return False
        if self.drawing_tool.mode != DrawingMode.RECTANGLE:
            return False

        physical_point = self.screen_to_physical(screen_point)
        physical_point = self._clamp_physical_point(physical_point)

        if self.decoration_start_physical is None:
            self.decoration_start_physical = physical_point
            self.decoration_preview_physical = physical_point
            return True

        decoration = self._build_decoration_from_points(self.decoration_start_physical, physical_point)
        self.decoration_start_physical = None
        self.decoration_preview_physical = None

        if decoration.row_length < INCUBATOR_PLACEHOLDER_WIDTH:
            decoration.row_length = INCUBATOR_PLACEHOLDER_WIDTH
        if decoration.column_length < INCUBATOR_PLACEHOLDER_HEIGHT:
            decoration.column_length = INCUBATOR_PLACEHOLDER_HEIGHT

        self.decorations.append(decoration)
        self.selected_decoration_index = len(self.decorations) - 1
        self.decoration_created.emit(len(self.decorations))
        self.decoration_selected.emit(decoration)
        return True

    def _try_start_decoration_drag(self, screen_point: QPointF) -> bool:
        """
        Select a decoration and start edge/body dragging when hit.
        """
        hit = self._hit_test_decoration(screen_point)
        if hit is None:
            self.selected_decoration_index = None
            return False

        self.selected_decoration_index = hit[0]
        self.decoration_drag_mode = hit[1]
        self.decoration_drag_start_physical = self.screen_to_physical(screen_point)
        self.decoration_drag_original = self._copy_decoration(self.decorations[hit[0]])
        self.decoration_selected.emit(self.decorations[hit[0]])
        return True

    def _update_decoration_drag(self, screen_point: QPointF) -> None:
        """
        Update the selected decoration while dragging an edge or the body.
        """
        if self.selected_decoration_index is None:
            return
        if self.decoration_drag_start_physical is None:
            return
        if self.decoration_drag_original is None:
            return

        current_physical = self.screen_to_physical(screen_point)
        delta_x = current_physical.x() - self.decoration_drag_start_physical.x()
        delta_y = current_physical.y() - self.decoration_drag_start_physical.y()

        original = self.decoration_drag_original
        axes = self._get_decoration_axes()
        row_axis = axes[0]
        column_axis = axes[1]
        axis_delta = self._project_delta_to_decoration_axes(QPointF(delta_x, delta_y))
        delta_row = axis_delta[0]
        delta_column = axis_delta[1]
        min_width = INCUBATOR_PLACEHOLDER_WIDTH
        min_height = INCUBATOR_PLACEHOLDER_HEIGHT

        start_x = original.start_x
        start_y = original.start_y
        row_length = original.row_length
        column_length = original.column_length

        if self.decoration_drag_mode == "move":
            start_x += delta_x
            start_y += delta_y
        elif self.decoration_drag_mode == "left":
            next_row_length = row_length - delta_row
            if next_row_length >= min_width:
                start_x += row_axis.x() * delta_row
                start_y += row_axis.y() * delta_row
                row_length = next_row_length
        elif self.decoration_drag_mode == "right":
            row_length += delta_row
            if row_length < min_width:
                row_length = min_width
        elif self.decoration_drag_mode == "top":
            next_column_length = column_length - delta_column
            if next_column_length >= min_height:
                start_x += column_axis.x() * delta_column
                start_y += column_axis.y() * delta_column
                column_length = next_column_length
        elif self.decoration_drag_mode == "bottom":
            column_length += delta_column
            if column_length < min_height:
                column_length = min_height

        decoration = self.decorations[self.selected_decoration_index]
        decoration.start_x = start_x
        decoration.start_y = start_y
        decoration.row_length = row_length
        decoration.column_length = column_length
        self.decoration_changed.emit(decoration)

    def _update_decoration_cursor(self, screen_point: QPointF) -> None:
        """
        Show resize/move cursors when hovering a decoration.
        """
        hit = self._hit_test_decoration(screen_point)
        if hit is None:
            self.setCursor(QCursor(Qt.CrossCursor))
            return

        mode = hit[1]
        if mode == "left" or mode == "right":
            self.setCursor(QCursor(Qt.SizeHorCursor))
            return
        if mode == "top" or mode == "bottom":
            self.setCursor(QCursor(Qt.SizeVerCursor))
            return

        self.setCursor(QCursor(Qt.SizeAllCursor))

    def _hit_test_decoration(self, screen_point: QPointF):
        """
        Return the topmost hit decoration index and drag mode.
        """
        physical_point = self.screen_to_physical(screen_point)
        margin = 8.0 / self.zoom_factor

        index = len(self.decorations) - 1
        while index >= 0:
            decoration = self.decorations[index]
            corners = self._get_decoration_corners(decoration)

            if self._point_to_segment_distance(physical_point, corners[0], corners[3]) <= margin:
                return index, "left"
            if self._point_to_segment_distance(physical_point, corners[1], corners[2]) <= margin:
                return index, "right"
            if self._point_to_segment_distance(physical_point, corners[0], corners[1]) <= margin:
                return index, "top"
            if self._point_to_segment_distance(physical_point, corners[3], corners[2]) <= margin:
                return index, "bottom"
            if self._physical_polygon_contains(corners, physical_point):
                return index, "move"

            index -= 1

        return None

    def clear_segments(self) -> None:
        """
        Clear all in-memory wall segments and pending drawing state.
        """
        self.segments.clear()
        self.doors.clear()
        self.decorations.clear()
        self.pending_start_point = None
        self.preview_end_point = None
        self.decoration_start_physical = None
        self.decoration_preview_physical = None
        self.selected_decoration_index = None
        self.drawing_tool.cancel()
        self.selected_grid_point = None
        self.update()

    def set_doors(self, doors: list) -> None:
        """
        Replace the current door instances.
        """
        self.doors = []

        for door in doors:
            normalized_door = (
                int(door[0]),
                int(door[1]),
                int(door[2]),
                int(door[3]),
                int(door[4]),
                int(door[5]),
                int(door[6]),
                float(door[7]),
            )
            self.doors.append(normalized_door)

        self.pending_start_point = None
        self.preview_end_point = None
        self.drawing_tool.cancel()
        self.selected_grid_point = None
        self._recalculate_grid_limits()
        self.update()

    def get_doors(self) -> list:
        """
        Return a copy of current door instances.
        """
        doors = []

        for door in self.doors:
            copied_door = (
                door[0],
                door[1],
                door[2],
                door[3],
                door[4],
                door[5],
                door[6],
                door[7],
            )
            doors.append(copied_door)

        return doors

    def set_decorations(self, decorations: list) -> None:
        """
        Replace current decoration instances.
        """
        self.decorations = []

        for decoration in decorations:
            copied_decoration = self._copy_decoration(decoration)
            self.decorations.append(copied_decoration)

        self.selected_decoration_index = None
        self.decoration_start_physical = None
        self.decoration_preview_physical = None
        self.decoration_drag_mode = None
        self.update()

    def get_decorations(self) -> list:
        """
        Return a copy of current decorations.
        """
        decorations = []

        for decoration in self.decorations:
            decorations.append(self._copy_decoration(decoration))

        return decorations

    def update_selected_decoration_spacing(self, item_spacing_scale: float, row_spacing_scale: float) -> None:
        """
        Update spacing on the selected decoration.
        """
        if self.selected_decoration_index is None:
            return
        if self.selected_decoration_index >= len(self.decorations):
            return

        decoration = self.decorations[self.selected_decoration_index]
        decoration.item_spacing_scale = item_spacing_scale
        decoration.row_spacing_scale = row_spacing_scale
        self.decoration_changed.emit(decoration)
        self.update()

    def set_segments(self, segments: list) -> None:
        """
        Replace the current wall segments.
        """
        self.segments = []

        for segment in segments:
            start_point = segment[0]
            end_point = segment[1]
            wall_type = segment[2]
            normalized_segment = (start_point, end_point, wall_type)
            self.segments.append(normalized_segment)

        self.pending_start_point = None
        self.preview_end_point = None
        self.drawing_tool.cancel()
        self.selected_grid_point = None
        self._recalculate_grid_limits()
        self.update()

    def get_segments(self) -> list:
        """
        Return a copy of current wall segments.
        """
        segments = []

        for segment in self.segments:
            copied_segment = (segment[0], segment[1], segment[2])
            segments.append(copied_segment)

        return segments

    def fit_map_to_view(self) -> None:
        """
        Fit current map bounds into the visible widget area.
        Uses a 0.90 safety factor on full widget size to keep zoom
        and pan_offset calculations consistent (matching legacy auto_zoom).
        """
        widget_width = self.width()
        widget_height = self.height()

        # 瀹夊叏绯绘暟锛岀暀鐧芥瘮渚嬩笌鏃х増 0.95 绫讳技
        VIEW_FIT_MARGIN = 0.90

        world_width = self.map_size_x
        world_height = self.map_size_y

        if world_width <= 0:
            world_width = self.active_step_x
        if world_height <= 0:
            world_height = self.active_step_y

        zoom_x = (widget_width * VIEW_FIT_MARGIN) / world_width
        zoom_y = (widget_height * VIEW_FIT_MARGIN) / world_height
        new_zoom = min(zoom_x, zoom_y)

        if new_zoom < MIN_ZOOM:
            new_zoom = MIN_ZOOM
        if new_zoom > MAX_ZOOM:
            new_zoom = MAX_ZOOM

        self.zoom_factor = new_zoom

        # Keep pan offset aligned with grid_to_screen and screen_to_physical.
        self.pan_offset = QPointF(
            (widget_width - world_width * self.zoom_factor) / 2,
            (widget_height - world_height * self.zoom_factor) / 2,
        )
        self.view_changed.emit(self.zoom_factor)

    def _recalculate_grid_limits(self) -> None:
        """
        Derive logical grid limits from map size and imported segment bounds.
        """
        map_columns = math.ceil(self.map_size_x / self.active_step_x)
        map_rows = math.ceil(self.map_size_y / self.active_step_y)

        max_segment_x = 0
        max_segment_y = 0

        for segment in self.segments:
            start_point = segment[0]
            end_point = segment[1]

            max_segment_x = max(max_segment_x, int(start_point[0]), int(end_point[0]))
            max_segment_y = max(max_segment_y, int(start_point[1]), int(end_point[1]))

        for door in self.doors:
            door_points = self._get_door_grid_points(door)
            start_point = door_points[0]
            end_point = door_points[1]
            max_segment_x = max(max_segment_x, int(start_point[0]), int(end_point[0]))
            max_segment_y = max(max_segment_y, int(start_point[1]), int(end_point[1]))

        self.grid_columns = max(MIN_GRID_COLUMNS, map_columns, max_segment_x)
        self.grid_rows = max(MIN_GRID_ROWS, map_rows, max_segment_y)

    def cancel_pending_segment(self) -> None:
        """
        Cancel the active first-click segment start.
        """
        if self.pending_start_point is None:
            return

        self.drawing_tool.cancel()
        self.pending_start_point = None
        self.preview_end_point = None
        self.drawing_cancelled.emit()
        self.update()

    def _get_door_grid_points(self, door: tuple) -> tuple:
        """
        Return start and end grid points for a door tuple.
        """
        pos_x = int(door[0])
        pos_y = int(door[1])
        direction_type = int(door[3])
        size = int(door[4])

        start_point = (pos_x, pos_y)
        if direction_type == 0:
            end_point = (pos_x, pos_y + size)
        else:
            end_point = (pos_x + size, pos_y)

        return start_point, end_point

    def get_door_grid_points(self, door: tuple) -> tuple:
        """
        Return start and end grid points for callers outside the viewport.
        """
        return self._get_door_grid_points(door)

    def _get_door_colors(self, wall_type: int, door_state: int, light_state: int, z_offset: float) -> tuple:
        """
        Return line color, dot color, and hollow flag for a door marker.
        """
        WALL_TYPE_STANDARD = 0
        WALL_TYPE_LAB = 1

        # --- 1. Lab Wall Set (Base wall is Emerald Green) ---
        if wall_type == WALL_TYPE_LAB:
            # Lab Decoration Door
            if light_state == LIGHT_STATE_BROKEN:
                return DOOR_LAB_DECO_LINE, DOOR_LAB_DECO_DOT, False
            # Laser Door with Laser (Closed)
            if door_state == DOOR_STATE_CLOSED:
                return DOOR_LAB_LASER_WITH_LINE, DOOR_LAB_LASER_WITH_DOT, False
            # Laser Door without Laser (Open)
            return DOOR_LAB_LASER_WITHOUT_LINE, DOOR_LAB_LASER_WITHOUT_DOT, True

        # --- 2. Standard Wall Set (Base wall is Deep Blue) ---
        is_active_door = (light_state != LIGHT_STATE_BROKEN)
        if is_active_door:
            # Active Door (State controlled by global is_door_open)
            if self.is_door_open:
                return DOOR_STD_ACTIVE_LINE, DOOR_STD_ACTIVE_DOT_OPEN, True
            else:
                return DOOR_STD_ACTIVE_LINE, DOOR_STD_ACTIVE_DOT_CLOSED, False

        # Dead Doors (3 variants)
        if z_offset == 0.0:
            # Dead Door (Closed)
            return DOOR_STD_DEAD_CLOSED_LINE, DOOR_STD_DEAD_CLOSED_DOT, False
        if door_state == DOOR_STATE_CLOSED:
            # Dead Door (Jammed)
            return DOOR_STD_DEAD_JAMMED_LINE, DOOR_STD_DEAD_JAMMED_DOT, False
        # Dead Door (Open)
        return DOOR_STD_DEAD_OPEN_LINE, DOOR_STD_DEAD_OPEN_DOT, True

    @property
    def active_step_x(self) -> float:
        """
        Return current wall profile x step.
        """
        profile = get_wall_profile(self.active_wall_type)
        return profile["step_x"]

    @property
    def active_step_y(self) -> float:
        """
        Return current wall profile y step.
        """
        profile = get_wall_profile(self.active_wall_type)
        return profile["step_y"]

    def grid_to_screen(self, grid_x: int, grid_y: int, wall_type: int = None) -> QPointF:
        """
        Convert logical grid coordinates into widget coordinates.
        """
        physical_point = self.grid_to_physical(grid_x, grid_y, wall_type)
        screen_x = physical_point.x() * self.zoom_factor + self.pan_offset.x()
        screen_y = physical_point.y() * self.zoom_factor + self.pan_offset.y()
        return QPointF(screen_x, screen_y)

    def grid_to_physical(self, grid_x: int, grid_y: int, wall_type: int = None) -> QPointF:
        """
        Convert logical grid coordinates into physical map coordinates.
        """
        if wall_type is None:
            wall_type = self.active_wall_type

        profile = get_wall_profile(wall_type)
        step_x = profile["step_x"]
        step_y = profile["step_y"]
        grid_shift = self._get_grid_shift(step_x, step_y)

        physical_x = (grid_x - grid_y) * step_x + grid_shift.x()
        physical_y = (grid_x + grid_y) * step_y + grid_shift.y()
        return QPointF(physical_x, physical_y)

    def screen_to_physical(self, screen_point: QPointF) -> QPointF:
        """
        Convert widget coordinates into physical map coordinates.
        """
        physical_x = (screen_point.x() - self.pan_offset.x()) / self.zoom_factor
        physical_y = (screen_point.y() - self.pan_offset.y()) / self.zoom_factor
        return QPointF(physical_x, physical_y)

    def physical_to_grid(self, physical_point: QPointF, wall_type: int = None) -> tuple:
        """
        Convert physical map coordinates into logical grid coordinates.
        """
        if wall_type is None:
            wall_type = self.active_wall_type

        profile = get_wall_profile(wall_type)
        step_x = profile["step_x"]
        step_y = profile["step_y"]
        grid_shift = self._get_grid_shift(step_x, step_y)

        value_a = (physical_point.x() - grid_shift.x()) / step_x
        value_b = (physical_point.y() - grid_shift.y()) / step_y
        grid_x = (value_a + value_b) / 2.0
        grid_y = (value_b - value_a) / 2.0
        return grid_x, grid_y

    def screen_to_grid(self, screen_point: QPointF):
        """
        Convert widget coordinates into the nearest in-bounds grid point.
        Exactly matches legacy snap_to_grid:
        """
        physical_point = self.screen_to_physical(screen_point)

        clamped_point = self._clamp_physical_point(physical_point)
        grid_float = self.physical_to_grid(clamped_point)

        base_grid_x = int(round(grid_float[0]))
        base_grid_y = int(round(grid_float[1]))

        best_point = (base_grid_x, base_grid_y)
        best_distance = float('inf')

        offset_x = -4
        while offset_x <= 4:
            offset_y = -4
            while offset_y <= 4:
                test_grid_x = base_grid_x + offset_x
                test_grid_y = base_grid_y + offset_y
                test_physical = self.grid_to_physical(test_grid_x, test_grid_y)

                if self._is_physical_point_near_bounds(test_physical):
                    delta_x = test_physical.x() - physical_point.x()
                    delta_y = test_physical.y() - physical_point.y()
                    distance = delta_x * delta_x + delta_y * delta_y

                    if distance < best_distance:
                        best_distance = distance
                        best_point = (test_grid_x, test_grid_y)

                offset_y += 1
            offset_x += 1

        return best_point

    def _get_grid_shift(self, step_x: float, step_y: float) -> QPointF:
        """
        Match the old UI grid shift formula for wall profile alignment.
        """
        wall_type = find_wall_type_by_steps(step_x)
        profile = get_wall_profile(wall_type)
        divisor = profile["grid_divisor"]

        grid_step_x = step_x / divisor
        grid_step_y = step_y / divisor
        remainder_x = grid_step_x / 2.0
        remainder_y = grid_step_y / 2.0

        raw_shift_x = self.map_size_x / 2.0
        grid_shift_x = round((raw_shift_x - remainder_x) / grid_step_x) * grid_step_x + remainder_x

        raw_shift_y = remainder_y
        grid_shift_y = round((raw_shift_y - remainder_y) / grid_step_y) * grid_step_y + remainder_y + step_y
        return QPointF(grid_shift_x, grid_shift_y)

    def _clamp_physical_point(self, physical_point: QPointF) -> QPointF:
        """
        Clamp a physical point into the map bounds.
        """
        bounds = self._get_physical_bounds(self.active_step_x, self.active_step_y)
        min_x = bounds[0]
        min_y = bounds[1]
        max_x = bounds[2]
        max_y = bounds[3]

        clamped_x = physical_point.x()
        clamped_y = physical_point.y()

        if clamped_x < min_x:
            clamped_x = min_x
        if clamped_y < min_y:
            clamped_y = min_y
        if clamped_x > max_x:
            clamped_x = max_x
        if clamped_y > max_y:
            clamped_y = max_y

        return QPointF(clamped_x, clamped_y)

    def _is_physical_point_in_bounds(self, physical_point: QPointF) -> bool:
        """
        Return whether a physical point is strictly inside the map bounds.
        """
        bounds = self._get_physical_bounds(self.active_step_x, self.active_step_y)
        min_x = bounds[0]
        min_y = bounds[1]
        max_x = bounds[2]
        max_y = bounds[3]

        if physical_point.x() < min_x:
            return False
        if physical_point.y() < min_y:
            return False
        if physical_point.x() > max_x:
            return False
        if physical_point.y() > max_y:
            return False

        return True

    def _is_physical_point_near_bounds(self, physical_point: QPointF) -> bool:
        """
        Return whether a physical point is within one grid step of the map bounds.
        """
        bounds = self._get_physical_bounds(self.active_step_x, self.active_step_y)
        margin_x = self.active_step_x
        margin_y = self.active_step_y

        if physical_point.x() < bounds[0] - margin_x:
            return False
        if physical_point.y() < bounds[1] - margin_y:
            return False
        if physical_point.x() > bounds[2] + margin_x:
            return False
        if physical_point.y() > bounds[3] + margin_y:
            return False

        return True

    def is_physical_point_near_bounds(self, physical_point: QPointF) -> bool:
        """
        Public wrapper used by drawing tool controllers.
        """
        return self._is_physical_point_near_bounds(physical_point)

    def _get_physical_bounds(self, step_x: float, step_y: float) -> tuple:
        """
        Match the old UI physical grid bounds for a wall profile.
        """
        half_step_x = step_x / 2.0
        half_step_y = step_y / 2.0

        min_x = half_step_x
        max_x_count = int((self.map_size_x - half_step_x) / step_x)
        max_x = max_x_count * step_x + half_step_x

        min_y = half_step_y
        max_y_count = int((self.map_size_y - half_step_y) / step_y)
        max_y = max_y_count * step_y + half_step_y

        if max_x < min_x:
            max_x = min_x
        if max_y < min_y:
            max_y = min_y

        return min_x, min_y, max_x, max_y

    def _get_grid_draw_range(self, step_x: float, step_y: float) -> tuple:
        """
        Return the logical grid range that covers the valid physical bounds.
        Uses _get_physical_bounds instead of [0, map_size] so the drawn grid
        exactly matches the clickable/drawable area.
        """
        bounds = self._get_physical_bounds(step_x, step_y)
        corners = [
            QPointF(bounds[0], bounds[1]),
            QPointF(bounds[2], bounds[1]),
            QPointF(bounds[0], bounds[3]),
            QPointF(bounds[2], bounds[3]),
        ]

        logical_values = []
        profile_wall_type = find_wall_type_by_steps(step_x, step_y)

        for corner in corners:
            grid_point = self.physical_to_grid(corner, profile_wall_type)
            logical_values.append(grid_point[0])
            logical_values.append(grid_point[1])

        min_grid = math.floor(min(logical_values)) - 1
        max_grid = math.ceil(max(logical_values)) + 1
        return min_grid, max_grid

    def _physical_to_screen(self, physical_point: QPointF) -> QPointF:
        """
        Convert physical coordinates directly to screen coordinates.
        """
        screen_x = physical_point.x() * self.zoom_factor + self.pan_offset.x()
        screen_y = physical_point.y() * self.zoom_factor + self.pan_offset.y()
        return QPointF(screen_x, screen_y)

    def _get_decoration_corners(self, decoration: IncubatorDecoration) -> list:
        """
        Return decoration corners along the active wall-set axes.
        """
        origin = QPointF(decoration.start_x, decoration.start_y)
        return self._build_decoration_corners(origin, decoration.row_length, decoration.column_length)

    def _build_decoration_corners(self, origin: QPointF, row_length: float, column_length: float) -> list:
        """
        Build a physical parallelogram from row and column lengths.
        """
        axes = self._get_decoration_axes()
        row_axis = axes[0]
        column_axis = axes[1]

        row_vector = QPointF(row_axis.x() * row_length, row_axis.y() * row_length)
        column_vector = QPointF(column_axis.x() * column_length, column_axis.y() * column_length)

        first = QPointF(origin)
        second = first + row_vector
        third = second + column_vector
        fourth = first + column_vector
        return [first, second, third, fourth]

    def _build_decoration_from_points(self, start_point: QPointF, end_point: QPointF) -> IncubatorDecoration:
        """
        Build a positive-length decoration by projecting onto wall-set axes.
        """
        axes = self._get_decoration_axes()
        row_axis = axes[0]
        column_axis = axes[1]
        delta_x = end_point.x() - start_point.x()
        delta_y = end_point.y() - start_point.y()
        axis_delta = self._project_delta_to_decoration_axes(QPointF(delta_x, delta_y))
        row_length = axis_delta[0]
        column_length = axis_delta[1]

        origin = QPointF(start_point)
        if row_length < 0.0:
            origin = origin + QPointF(row_axis.x() * row_length, row_axis.y() * row_length)
            row_length = -row_length
        if column_length < 0.0:
            origin = origin + QPointF(column_axis.x() * column_length, column_axis.y() * column_length)
            column_length = -column_length

        return IncubatorDecoration(
            start_x=origin.x(),
            start_y=origin.y(),
            row_length=row_length,
            column_length=column_length,
        )

    def _get_decoration_axes(self) -> tuple:
        """
        Return normalized row and column axes matching the active wall-set grid.
        """
        row_axis = QPointF(self.active_step_x, -self.active_step_y)
        column_axis = QPointF(self.active_step_x, self.active_step_y)
        return self._normalize_vector(row_axis), self._normalize_vector(column_axis)

    def _normalize_vector(self, vector: QPointF) -> QPointF:
        """
        Return a unit vector. Falls back to horizontal on invalid length.
        """
        length = math.sqrt(vector.x() * vector.x() + vector.y() * vector.y())
        if length <= 0.0:
            return QPointF(1.0, 0.0)

        return QPointF(vector.x() / length, vector.y() / length)

    def _project_delta_to_decoration_axes(self, delta: QPointF) -> tuple:
        """
        Solve a physical delta into row and column axis distances.
        """
        axes = self._get_decoration_axes()
        row_axis = axes[0]
        column_axis = axes[1]
        determinant = row_axis.x() * column_axis.y() - row_axis.y() * column_axis.x()

        if abs(determinant) <= 0.000001:
            return 0.0, 0.0

        row_distance = (delta.x() * column_axis.y() - delta.y() * column_axis.x()) / determinant
        column_distance = (row_axis.x() * delta.y() - row_axis.y() * delta.x()) / determinant
        return row_distance, column_distance

    def _physical_polygon_to_screen(self, points: list) -> QPolygonF:
        """
        Convert a physical polygon to screen coordinates.
        """
        polygon = QPolygonF()
        for point in points:
            polygon.append(self._physical_to_screen(point))

        return polygon

    def _physical_polygon_contains(self, corners: list, point: QPointF) -> bool:
        """
        Return whether a physical point is inside a decoration polygon.
        """
        path = QPainterPath()
        path.moveTo(corners[0])

        index = 1
        while index < len(corners):
            path.lineTo(corners[index])
            index += 1

        path.closeSubpath()
        return path.contains(point)

    def _point_to_segment_distance(self, point: QPointF, start: QPointF, end: QPointF) -> float:
        """
        Return physical distance from a point to a line segment.
        """
        segment_x = end.x() - start.x()
        segment_y = end.y() - start.y()
        length_squared = segment_x * segment_x + segment_y * segment_y

        if length_squared <= 0.0:
            delta_x = point.x() - start.x()
            delta_y = point.y() - start.y()
            return math.sqrt(delta_x * delta_x + delta_y * delta_y)

        point_x = point.x() - start.x()
        point_y = point.y() - start.y()
        projection = (point_x * segment_x + point_y * segment_y) / length_squared
        if projection < 0.0:
            projection = 0.0
        if projection > 1.0:
            projection = 1.0

        closest_x = start.x() + segment_x * projection
        closest_y = start.y() + segment_y * projection
        delta_x = point.x() - closest_x
        delta_y = point.y() - closest_y
        return math.sqrt(delta_x * delta_x + delta_y * delta_y)

    def _get_incubator_item_spacing(self) -> float:
        """
        Return default physical item spacing used by the C++ incubator builder.
        """
        return math.sqrt(150.0 * 150.0 + 130.0 * 130.0)

    def _get_incubator_row_spacing(self) -> float:
        """
        Return default physical row spacing used by the C++ incubator builder.
        """
        return math.sqrt(150.0 * 150.0 + 130.0 * 130.0)

    def _cancel_pending_decoration(self) -> bool:
        """
        Cancel an unfinished decoration rectangle.
        """
        if self.decoration_start_physical is None and self.decoration_preview_physical is None:
            return False

        self.decoration_start_physical = None
        self.decoration_preview_physical = None
        self.drawing_cancelled.emit()
        return True

    def _copy_decoration(self, decoration: IncubatorDecoration) -> IncubatorDecoration:
        """
        Return a copied incubator decoration.
        """
        return IncubatorDecoration(
            start_x=decoration.start_x,
            start_y=decoration.start_y,
            row_length=decoration.row_length,
            column_length=decoration.column_length,
            item_spacing_scale=decoration.item_spacing_scale,
            row_spacing_scale=decoration.row_spacing_scale,
        )
