"""State machine for drawing tools."""
from app.binding import dll_registry
from app.editor.drawable_parts import PART_WALL_BODY
from app.ui.tools.drawing_modes import DrawingMode


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


class DrawingToolController:
    """
    Handle drawing clicks and previews for straight, polyline, and rectangle tools.
    """

    def __init__(self, viewport) -> None:
        self.viewport = viewport
        self.mode = DrawingMode.STRAIGHT_LINE
        self.start_point = None
        self.preview_segments = []

    def set_mode(self, mode: DrawingMode) -> None:
        """Switch drawing mode and clear any unfinished operation."""
        self.mode = mode
        self.cancel()

    def cancel(self) -> bool:
        """Cancel the current drawing operation."""
        had_pending_state = self.start_point is not None or bool(self.preview_segments)
        self.start_point = None
        self.preview_segments = []
        return had_pending_state

    def handle_left_click(self, grid_point):
        """Handle a left click and return newly committed drawable items."""
        result = {
            "segments": [],
            "doors": [],
        }

        if grid_point is None:
            self.cancel()
            return result

        if self.start_point is None:
            self.start_point = grid_point
            self.preview_segments = []
            return result

        if self.mode == DrawingMode.RECTANGLE and self.viewport.active_drawable_part == PART_WALL_BODY:
            segments = self._build_rectangle_segments(self.start_point, grid_point)
            result["segments"] = segments
            self.cancel()
            return result

        end_point = self._get_orthogonal_point(self.start_point, grid_point)
        if end_point == self.start_point:
            self.preview_segments = []
            return result

        if self.viewport.active_drawable_part == PART_WALL_BODY:
            segment = (self.start_point, end_point, self.viewport.active_wall_type)
            result["segments"].append(segment)

            if self.mode == DrawingMode.POLYLINE:
                self.start_point = end_point
                self.preview_segments = []
            else:
                self.cancel()
        else:
            door = self._build_door_from_points(self.start_point, end_point)
            if door is not None:
                result["doors"].append(door)
            self.cancel()

        return result

    def is_waiting_for_end_point(self) -> bool:
        """Return whether a drawing operation has an active start point."""
        return self.start_point is not None

    def get_start_point(self):
        """Return the active drawing start point."""
        return self.start_point

    def update_preview(self, grid_point) -> None:
        """Update preview segments from the current mouse position."""
        if self.start_point is None:
            self.preview_segments = []
            return
        if grid_point is None:
            self.preview_segments = []
            return

        if self.mode == DrawingMode.RECTANGLE and self.viewport.active_drawable_part == PART_WALL_BODY:
            self.preview_segments = self._build_rectangle_segments(self.start_point, grid_point)
            return

        end_point = self._get_orthogonal_point(self.start_point, grid_point)
        if end_point == self.start_point:
            self.preview_segments = []
            return

        segment = (self.start_point, end_point, self.viewport.active_wall_type)
        self.preview_segments = [segment]

    def _build_door_from_points(self, start_point, end_point):
        """Build a door tuple from two logical grid points."""
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
        part_id = self.viewport.active_drawable_part

        if part_id == PART_ACTIVE_DOOR:
            door_state = DOOR_STATE_OPEN
            light_state = LIGHT_STATE_GREEN
        elif part_id == PART_DEAD_DOOR_CLOSED:
            door_state = DOOR_STATE_CLOSED
            light_state = LIGHT_STATE_BROKEN
        elif part_id == PART_DEAD_DOOR_JAMMED:
            door_state = DOOR_STATE_CLOSED
            light_state = LIGHT_STATE_BROKEN
            z_offset = dll_registry.get_standard_door_jam_z_offset(size)
        elif part_id == PART_DEAD_DOOR_OPEN:
            door_state = DOOR_STATE_OPEN
            light_state = LIGHT_STATE_BROKEN
            z_offset = dll_registry.get_standard_door_dead_open_z_offset(size)
        elif part_id == PART_LAB_LASER_CLOSED:
            door_state = DOOR_STATE_CLOSED
            light_state = LIGHT_STATE_RED
            size = 1
        elif part_id == PART_LAB_LASER_OPEN:
            door_state = DOOR_STATE_OPEN
            light_state = LIGHT_STATE_RED
            size = 1
        elif part_id == PART_LAB_DECORATION_DOOR:
            door_state = DOOR_STATE_CLOSED
            light_state = LIGHT_STATE_BROKEN
            size = 1
        else:
            return None

        return pos_x, pos_y, self.viewport.active_wall_type, direction_type, size, door_state, light_state, z_offset

    def _get_door_size(self, raw_size: int) -> int:
        """Clamp a drawn door length to the DLL-supported size range."""
        if raw_size < 1:
            raw_size = 1

        if self.viewport.active_drawable_part in (
            PART_LAB_LASER_CLOSED,
            PART_LAB_LASER_OPEN,
            PART_LAB_DECORATION_DOOR,
        ):
            return 1

        return dll_registry.clamp_standard_door_size(raw_size)

    def get_preview_segments(self) -> list:
        """Return preview segments currently shown on the viewport."""
        return list(self.preview_segments)

    def _build_rectangle_segments(self, start_point, raw_end_point) -> list:
        """Build four wall segments from two diagonal logical corners."""
        start_x = int(start_point[0])
        start_y = int(start_point[1])
        end_x = int(raw_end_point[0])
        end_y = int(raw_end_point[1])

        if start_x == end_x:
            return []
        if start_y == end_y:
            return []

        top_left = (start_x, start_y)
        top_right = (end_x, start_y)
        bottom_right = (end_x, end_y)
        bottom_left = (start_x, end_y)

        wall_type = self.viewport.active_wall_type
        segments = [
            (top_left, top_right, wall_type),
            (top_right, bottom_right, wall_type),
            (bottom_right, bottom_left, wall_type),
            (bottom_left, top_left, wall_type),
        ]

        valid_segments = []
        for segment in segments:
            if self._is_segment_near_bounds(segment):
                valid_segments.append(segment)

        return valid_segments

    def _get_orthogonal_point(self, start_point, raw_end_point):
        """
        Clamp a raw end point to the dominant axis, then step back into bounds.
        """
        start_x = start_point[0]
        start_y = start_point[1]
        end_x = raw_end_point[0]
        end_y = raw_end_point[1]

        delta_x = abs(end_x - start_x)
        delta_y = abs(end_y - start_y)

        if delta_x > delta_y:
            end_y = start_y
        else:
            end_x = start_x

        while end_x != start_x or end_y != start_y:
            test_physical = self.viewport.grid_to_physical(end_x, end_y)
            if self.viewport.is_physical_point_near_bounds(test_physical):
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

    def _is_segment_near_bounds(self, segment: tuple) -> bool:
        """Return whether both segment endpoints are close enough to map bounds."""
        start_point = segment[0]
        end_point = segment[1]
        start_physical = self.viewport.grid_to_physical(start_point[0], start_point[1])
        end_physical = self.viewport.grid_to_physical(end_point[0], end_point[1])

        if not self.viewport.is_physical_point_near_bounds(start_physical):
            return False
        if not self.viewport.is_physical_point_near_bounds(end_physical):
            return False

        return True
