"""
Interactive isometric viewport.
"""
import math

from PySide6.QtCore import QPointF, Qt, Signal
from PySide6.QtGui import QBrush, QColor, QCursor, QPainter, QPen
from PySide6.QtWidgets import QWidget

from app.binding import dll_registry
from app.editor.drawable_parts import PART_WALL_BODY
from app.editor.wall_profiles import find_wall_type_by_steps, get_default_wall_type, get_wall_profile
from app.project.data import DEFAULT_MAP_SIZE_X, DEFAULT_MAP_SIZE_Y


MIN_ZOOM = 0.002
MAX_ZOOM = 30.0
MIN_GRID_COLUMNS = 1
MIN_GRID_ROWS = 1
DOOR_STATE_CLOSED = 0
DOOR_STATE_OPEN = 1
LIGHT_STATE_GREEN = 0
LIGHT_STATE_RED = 1
LIGHT_STATE_BROKEN = 2
PART_ACTIVE_DOOR = "active_door"
PART_DEAD_DOOR_CLOSED = "dead_door_closed"
PART_DEAD_DOOR_JAMMED = "dead_door_jammed"
PART_DEAD_DOOR_OPEN = "dead_door_open"
PART_LAB_LASER_CLOSED = "lab_laser_closed"
PART_LAB_LASER_OPEN = "lab_laser_open"
PART_LAB_DECORATION_DOOR = "lab_decoration_door"


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

    def set_wall_type(self, wall_type: int) -> None:
        """
        Update the active wall profile and redraw the current grid.
        """
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
        self.cancel_pending_segment()
        self.update()

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
        self._draw_segments(painter)
        self._draw_doors(painter)
        self._draw_preview_segment(painter)
        self._draw_selected_point(painter)
        self._draw_origin_marker(painter)

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
            grid_point = self.screen_to_grid(event.position())
            self._handle_left_click(grid_point)
            self.update()
            event.accept()
            return

        if event.button() == Qt.RightButton:
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

        grid_point = self.screen_to_grid(event.position())
        if grid_point is not None:
            if grid_point != self.last_cursor_grid_point:
                self.last_cursor_grid_point = grid_point
                self.cursor_grid_changed.emit(grid_point[0], grid_point[1])

            if self.pending_start_point is not None:
                self.preview_end_point = self.get_orthogonal_point(self.pending_start_point, grid_point)
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

        super().mouseReleaseEvent(event)

    def leaveEvent(self, event) -> None:
        """
        Clear hover coordinate state when the cursor leaves the canvas.
        """
        self.last_cursor_grid_point = None
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
        painter.fillRect(self.rect(), QColor("#10161a"))

    def _draw_grid(self, painter: QPainter) -> None:
        profile = get_wall_profile(self.active_wall_type)
        step_x = profile["step_x"]
        step_y = profile["step_y"]

        # 1. 计算当前缩放比例下，相邻网格线的屏幕像素间距
        # 相邻网格点之间的物理距离是 math.sqrt(step_x**2 + step_y**2)
        # 屏幕上的距离需要乘上当前的 zoom_factor
        grid_physical_dist = math.sqrt(step_x * step_x + step_y * step_y)
        grid_pixel_dist = self.zoom_factor * grid_physical_dist

        # 2. 根据相邻网格的屏幕像素间距，自适应地调整绘制步长 (LOD)
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
            # 缩放比例极小时，网格线会密集成一片，此时完全不绘制网格线以优化性能，仅画外边界
            draw_step = None

        if draw_step is not None:
            # 3. 计算可视区域内的网格行和列的范围 (Frustum Culling)
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

            # 向外扩展 1 格以防边缘网格在滚动或放大时出现突兀的空缺
            min_visible_x = math.floor(min(visible_x)) - 1
            max_visible_x = math.ceil(max(visible_x)) + 1
            min_visible_y = math.floor(min(visible_y)) - 1
            max_visible_y = math.ceil(max(visible_y)) + 1

            # 网格全局有效范围限制
            min_grid, max_grid = self._get_grid_draw_range(step_x, step_y)

            grid_pen = QPen(self.grid_color)
            grid_pen.setWidth(1)
            grid_pen.setCosmetic(True)
            painter.setPen(grid_pen)

            # 4. 绘制行线（平行于 X 轴，从 start_x 变动到 end_x，y 固定为 row）
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

            # 5. 绘制列线（平行于 Y 轴，从 start_y 变动到 end_y，x 固定为 column）
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

        # 绘制地图可用区域边界矩形（与旧版 mask rectangle 对应）
        bounds = self._get_physical_bounds(step_x, step_y)
        top_left = self._physical_to_screen(QPointF(bounds[0], bounds[1]))
        bottom_right = self._physical_to_screen(QPointF(bounds[2], bounds[3]))

        border_pen = QPen(QColor("#c0c0c0"))
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

        marker_pen = QPen(QColor("#f1f6f3"))
        marker_pen.setWidth(2)
        marker_pen.setCosmetic(True)
        painter.setPen(marker_pen)
        painter.setBrush(self.theme_color)
        painter.drawEllipse(center, 6, 6)

    def _draw_segments(self, painter: QPainter) -> None:
        segment_pen = QPen(QColor("#f1f6f3"))
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
        if self.preview_end_point is None:
            return

        preview_pen = QPen(QColor("#e0b95c"))
        preview_pen.setWidth(3)
        preview_pen.setCosmetic(True)
        preview_pen.setStyle(Qt.DashLine)
        painter.setPen(preview_pen)

        screen_start = self.grid_to_screen(self.pending_start_point[0], self.pending_start_point[1], self.active_wall_type)
        screen_end = self.grid_to_screen(self.preview_end_point[0], self.preview_end_point[1], self.active_wall_type)
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
            colors = self._get_door_colors(door_state, light_state, z_offset)
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
                painter.setBrush(QBrush(QColor("#10161a")))
            else:
                painter.setBrush(QBrush(QColor(dot_color)))

            painter.drawEllipse(QPointF(mid_x, mid_y), radius, radius)

    def _draw_origin_marker(self, painter: QPainter) -> None:
        label_pen = QPen(QColor("#d9e8e2"))
        painter.setPen(label_pen)
        painter.drawText(24, 32, "2.5D Isometric Canvas")
        profile = get_wall_profile(self.active_wall_type)
        size_text = (
            f"Map: {self.map_size_x:.1f} x {self.map_size_y:.1f} | "
            f"Wall: {profile['label']} | Grid: {self.grid_columns} x {self.grid_rows}"
        )
        painter.drawText(24, 54, size_text)
        painter.drawText(24, 76, "Left click twice: draw wall. Right click: cancel. Middle drag: pan. Wheel: zoom.")

    def _handle_left_click(self, grid_point) -> None:
        """
        Start or finish a wall segment from a grid click.
        """
        if grid_point is None:
            self.selected_grid_point = None
            return

        self.selected_grid_point = grid_point
        self.grid_point_selected.emit(grid_point[0], grid_point[1])

        if self.pending_start_point is None:
            self.pending_start_point = grid_point
            self.preview_end_point = None
            self.segment_started.emit(grid_point[0], grid_point[1])
            return

        end_point = self.get_orthogonal_point(self.pending_start_point, grid_point)
        if end_point == self.pending_start_point:
            self.preview_end_point = None
            return

        start_point = self.pending_start_point

        if self.active_drawable_part == PART_WALL_BODY:
            segment = (start_point, end_point, self.active_wall_type)
            self.segments.append(segment)
            count = len(self.segments)
            self.segment_created.emit(start_point[0], start_point[1], end_point[0], end_point[1], count)
        else:
            door = self._build_door_from_points(start_point, end_point)
            if door is not None:
                self.doors.append(door)
                self.door_created.emit(door[0], door[1], len(self.doors))

        self.pending_start_point = None
        self.preview_end_point = None

    def get_orthogonal_point(self, start_point, raw_end_point):
        """
        Clamp a raw end point to the dominant axis, then step back
        towards start until the end point is inside the grid bounds.
        Matches the legacy get_clamped_orthogonal logic.
        """
        start_x = start_point[0]
        start_y = start_point[1]
        end_x = raw_end_point[0]
        end_y = raw_end_point[1]

        # 正交约束：锁定到变化更大的轴
        delta_x = abs(end_x - start_x)
        delta_y = abs(end_y - start_y)

        if delta_x > delta_y:
            end_y = start_y
        else:
            end_x = start_x

        # 边界回退：逐步缩短线段直到终点落入 bounds（含临界一格）内
        while (end_x != start_x or end_y != start_y):
            test_physical = self.grid_to_physical(end_x, end_y)
            if self._is_physical_point_near_bounds(test_physical):
                break

            if end_x > start_x:
                end_x -= 1
            elif end_x < start_x:
                end_x += 1
            elif end_y > start_y:
                end_y -= 1
            elif end_y < start_y:
                end_y += 1

        return end_x, end_y

    def clear_segments(self) -> None:
        """
        Clear all in-memory wall segments and pending drawing state.
        """
        self.segments.clear()
        self.doors.clear()
        self.pending_start_point = None
        self.preview_end_point = None
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

        # 安全系数，留白比例与旧版 0.95 类似
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

        # pan_offset 居中：与 grid_to_screen / screen_to_physical 的映射公式一致
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

        self.pending_start_point = None
        self.preview_end_point = None
        self.drawing_cancelled.emit()
        self.update()

    def _build_door_from_points(self, start_point, end_point):
        """
        Build a door tuple from two grid points.
        """
        start_x = int(start_point[0])
        start_y = int(start_point[1])
        end_x = int(end_point[0])
        end_y = int(end_point[1])

        if start_x == end_x and start_y == end_y:
            return None

        if start_x == end_x:
            direction_type = 0
            raw_size = abs(end_y - start_y)
            pos_x = start_x
            pos_y = min(start_y, end_y)
        else:
            direction_type = 1
            raw_size = abs(end_x - start_x)
            pos_x = min(start_x, end_x)
            pos_y = start_y

        size = self._get_door_size(raw_size)
        door_state = DOOR_STATE_CLOSED
        light_state = LIGHT_STATE_BROKEN
        z_offset = 0.0

        if self.active_drawable_part == PART_ACTIVE_DOOR:
            door_state = DOOR_STATE_OPEN
            light_state = LIGHT_STATE_GREEN
        elif self.active_drawable_part == PART_DEAD_DOOR_CLOSED:
            door_state = DOOR_STATE_CLOSED
            light_state = LIGHT_STATE_BROKEN
        elif self.active_drawable_part == PART_DEAD_DOOR_JAMMED:
            door_state = DOOR_STATE_CLOSED
            light_state = LIGHT_STATE_BROKEN
            z_offset = dll_registry.get_standard_door_jam_z_offset(size)
        elif self.active_drawable_part == PART_DEAD_DOOR_OPEN:
            door_state = DOOR_STATE_OPEN
            light_state = LIGHT_STATE_BROKEN
            z_offset = dll_registry.get_standard_door_dead_open_z_offset(size)
        elif self.active_drawable_part == PART_LAB_LASER_CLOSED:
            door_state = DOOR_STATE_CLOSED
            light_state = LIGHT_STATE_RED
            size = 1
        elif self.active_drawable_part == PART_LAB_LASER_OPEN:
            door_state = DOOR_STATE_OPEN
            light_state = LIGHT_STATE_RED
            size = 1
        elif self.active_drawable_part == PART_LAB_DECORATION_DOOR:
            door_state = DOOR_STATE_CLOSED
            light_state = LIGHT_STATE_BROKEN
            size = 1
        else:
            return None

        return pos_x, pos_y, self.active_wall_type, direction_type, size, door_state, light_state, z_offset

    def _get_door_size(self, raw_size: int) -> int:
        """
        Clamp a drawn door length to the DLL-supported size range.
        """
        if raw_size < 1:
            raw_size = 1

        if self.active_drawable_part in (
            PART_LAB_LASER_CLOSED,
            PART_LAB_LASER_OPEN,
            PART_LAB_DECORATION_DOOR,
        ):
            return 1

        return dll_registry.clamp_standard_door_size(raw_size)

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

    def _get_door_colors(self, door_state: int, light_state: int, z_offset: float) -> tuple:
        """
        Return line color, dot color, and hollow flag for a door marker.
        """
        if light_state == LIGHT_STATE_RED:
            return "#2E8B57", "#32CD32", False
        if light_state == LIGHT_STATE_GREEN:
            return "#32CD32", "#00FF00", False

        if z_offset == 0.0:
            return "#8B0000", "#FF0000", False
        if door_state == DOOR_STATE_CLOSED:
            return "#D2691E", "#FFD700", False

        return "#808080", "#D3D3D3", True

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
