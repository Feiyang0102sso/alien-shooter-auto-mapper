"""Grid geometry helpers for the eraser tool."""
import math


def get_effective_radius(size: int) -> float:
    """Return the hit radius in grid units."""
    return size - 0.5


def erase_segments(segments: list, grid_point: tuple, radius: float) -> list:
    """Erase wall segment units near a grid point and return surviving segments."""
    new_segments = []
    mouse_x = grid_point[0]
    mouse_y = grid_point[1]

    for segment in segments:
        surviving_segments = erase_segment(segment, mouse_x, mouse_y, radius)
        new_segments.extend(surviving_segments)

    return new_segments


def erase_segment(segment: tuple, mouse_x: int, mouse_y: int, radius: float) -> list:
    """Erase one wall segment by splitting it into one-grid units."""
    start_point = segment[0]
    end_point = segment[1]
    wall_type = segment[2]

    start_x = start_point[0]
    start_y = start_point[1]
    end_x = end_point[0]
    end_y = end_point[1]

    unit_segments = decompose_to_units(start_x, start_y, end_x, end_y)
    if not unit_segments:
        distance = point_to_segment_distance(mouse_x, mouse_y, start_point, end_point)
        if distance > radius:
            return [segment]
        return []

    kept_units = []
    for unit_start, unit_end in unit_segments:
        mid_x = (unit_start[0] + unit_end[0]) / 2.0
        mid_y = (unit_start[1] + unit_end[1]) / 2.0
        distance = get_point_distance(mouse_x, mouse_y, mid_x, mid_y)
        if distance > radius:
            kept_units.append((unit_start, unit_end))

    return merge_units(kept_units, wall_type)


def erase_doors(doors: list, grid_point: tuple, radius: float, get_door_grid_points) -> list:
    """Erase doors near a grid point and return surviving doors."""
    kept_doors = []
    mouse_x = grid_point[0]
    mouse_y = grid_point[1]

    for door in doors:
        door_start, door_end = get_door_grid_points(door)
        distance = point_to_segment_distance(mouse_x, mouse_y, door_start, door_end)
        if distance > radius:
            kept_doors.append(door)

    return kept_doors


def decompose_to_units(start_x: int, start_y: int, end_x: int, end_y: int) -> list:
    """Split an axis-aligned grid segment into one-grid unit segments."""
    if start_x == end_x and start_y == end_y:
        return []

    if start_x == end_x:
        units = []
        step = 1
        if end_y < start_y:
            step = -1

        y = start_y
        while y != end_y:
            units.append(((start_x, y), (start_x, y + step)))
            y += step
        return units

    if start_y == end_y:
        units = []
        step = 1
        if end_x < start_x:
            step = -1

        x = start_x
        while x != end_x:
            units.append(((x, start_y), (x + step, start_y)))
            x += step
        return units

    return []


def merge_units(kept_units: list, wall_type: int) -> list:
    """Merge adjacent surviving unit segments back into wall segments."""
    if not kept_units:
        return []

    merged_segments = []
    current_start = kept_units[0][0]
    current_end = kept_units[0][1]

    index = 1
    while index < len(kept_units):
        unit_start = kept_units[index][0]
        unit_end = kept_units[index][1]

        if unit_start == current_end:
            current_end = unit_end
        else:
            merged_segments.append((current_start, current_end, wall_type))
            current_start = unit_start
            current_end = unit_end

        index += 1

    merged_segments.append((current_start, current_end, wall_type))
    return merged_segments


def point_to_segment_distance(point_x: float, point_y: float, segment_start: tuple, segment_end: tuple) -> float:
    """Return the shortest Euclidean distance from a point to a segment."""
    start_x = segment_start[0]
    start_y = segment_start[1]
    end_x = segment_end[0]
    end_y = segment_end[1]

    segment_x = end_x - start_x
    segment_y = end_y - start_y
    segment_length_sq = segment_x * segment_x + segment_y * segment_y

    if segment_length_sq == 0:
        return get_point_distance(point_x, point_y, start_x, start_y)

    point_vector_x = point_x - start_x
    point_vector_y = point_y - start_y
    projection = (point_vector_x * segment_x + point_vector_y * segment_y) / segment_length_sq

    if projection < 0.0:
        projection = 0.0
    elif projection > 1.0:
        projection = 1.0

    closest_x = start_x + projection * segment_x
    closest_y = start_y + projection * segment_y
    return get_point_distance(point_x, point_y, closest_x, closest_y)


def get_point_distance(point_x: float, point_y: float, target_x: float, target_y: float) -> float:
    """Return the Euclidean distance between two points."""
    delta_x = point_x - target_x
    delta_y = point_y - target_y
    return math.sqrt(delta_x * delta_x + delta_y * delta_y)
