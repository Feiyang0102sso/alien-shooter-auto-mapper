"""
JSON import and export helpers for the editor project data.
"""
import json
from pathlib import Path

from app.editor.wall_profiles import get_default_wall_type
from app.project.data import DECORATION_TYPE_INCUBATOR_ARRAY, IncubatorDecoration, ProjectData


def build_project_json_data(project_data: ProjectData) -> dict:
    """
    Build the JSON-compatible project data used by the old Tkinter UI.
    """
    segment_items = []

    for segment in project_data.segments:
        start_point = segment[0]
        end_point = segment[1]
        wall_type = segment[2]

        segment_item = {
            "start": {
                "x": start_point[0],
                "y": start_point[1],
            },
            "end": {
                "x": end_point[0],
                "y": end_point[1],
            },
            "wall_type": wall_type,
            "floor_type": 0,
        }
        segment_items.append(segment_item)

    door_items = []

    for door in project_data.doors:
        door_item = {
            "pos": {
                "x": door[0],
                "y": door[1],
            },
            "wall_type": door[2],
            "direction_type": door[3],
            "size": door[4],
            "door_state": door[5],
            "light_state": door[6],
            "z_offset": door[7],
        }
        door_items.append(door_item)

    decoration_items = []

    for decoration in project_data.decorations:
        if decoration.decoration_type != DECORATION_TYPE_INCUBATOR_ARRAY:
            continue

        decoration_item = {
            "type": decoration.decoration_type,
            "start": {
                "x": decoration.start_x,
                "y": decoration.start_y,
            },
            "row_length": decoration.row_length,
            "column_length": decoration.column_length,
            "item_spacing_scale": decoration.item_spacing_scale,
            "row_spacing_scale": decoration.row_spacing_scale,
        }
        decoration_items.append(decoration_item)

    data = {
        "map_size_x": project_data.map_size_x,
        "map_size_y": project_data.map_size_y,
        "segments": segment_items,
        "doors": door_items,
        "decorations": decoration_items,
        "is_door_open": project_data.is_door_open,
    }
    return data


def save_project_json(file_path: Path, project_data: ProjectData) -> None:
    """
    Save project data to a JSON file.
    """
    data = build_project_json_data(project_data)
    json_text = json.dumps(data, indent=2, ensure_ascii=False)
    file_path.write_text(json_text, encoding="utf-8")


def load_project_json(file_path: Path) -> ProjectData:
    """
    Load project data from a JSON file.
    """
    json_text = file_path.read_text(encoding="utf-8")
    data = json.loads(json_text)
    return parse_project_data(data)


def parse_project_data(data: dict) -> ProjectData:
    """
    Parse old UI compatible JSON data into project state.
    """
    if not isinstance(data, dict):
        raise ValueError("Project JSON root must be an object.")

    map_size_x = float(data.get("map_size_x", 600.0))
    map_size_y = float(data.get("map_size_y", 600.0))
    is_door_open = bool(data.get("is_door_open", False))
    segments = parse_segments(data)
    doors = parse_doors(data)
    decorations = parse_decorations(data)

    project_data = ProjectData(
        map_size_x=map_size_x,
        map_size_y=map_size_y,
        segments=segments,
        doors=doors,
        decorations=decorations,
        is_door_open=is_door_open,
    )
    return project_data


def parse_segments(data: dict) -> list:
    """
    Parse wall segments from old UI compatible JSON data.
    """
    if not isinstance(data, dict):
        raise ValueError("Project JSON root must be an object.")

    segments = []
    json_segments = data.get("segments", [])

    if not isinstance(json_segments, list):
        raise ValueError("Project JSON 'segments' must be a list.")

    for json_segment in json_segments:
        if not isinstance(json_segment, dict):
            raise ValueError("Each segment must be an object.")

        start = json_segment.get("start", {})
        end = json_segment.get("end", {})

        if not isinstance(start, dict):
            raise ValueError("Segment start must be an object.")
        if not isinstance(end, dict):
            raise ValueError("Segment end must be an object.")

        x1 = int(start.get("x", 0))
        y1 = int(start.get("y", 0))
        x2 = int(end.get("x", 0))
        y2 = int(end.get("y", 0))
        wall_type = int(json_segment.get("wall_type", get_default_wall_type()))

        segment = ((x1, y1), (x2, y2), wall_type)
        segments.append(segment)

    return segments


def parse_doors(data: dict) -> list:
    """
    Parse door entries from old UI compatible JSON data.
    """
    if not isinstance(data, dict):
        raise ValueError("Project JSON root must be an object.")

    doors = []
    json_doors = data.get("doors", [])

    if json_doors is None:
        return doors

    if not isinstance(json_doors, list):
        raise ValueError("Project JSON 'doors' must be a list.")

    for json_door in json_doors:
        if not isinstance(json_door, dict):
            raise ValueError("Each door must be an object.")

        position = json_door.get("pos", {})
        if not isinstance(position, dict):
            raise ValueError("Door position must be an object.")

        x = int(position.get("x", 0))
        y = int(position.get("y", 0))
        wall_type = int(json_door.get("wall_type", get_default_wall_type()))
        direction_type = int(json_door.get("direction_type", 0))
        size = int(json_door.get("size", 1))
        door_state = int(json_door.get("door_state", 0))
        light_state = int(json_door.get("light_state", 0))
        z_offset = float(json_door.get("z_offset", 0.0))

        door = (x, y, wall_type, direction_type, size, door_state, light_state, z_offset)
        doors.append(door)

    return doors


def parse_decorations(data: dict) -> list:
    """
    Parse decoration entries from the optional top-level JSON section.
    """
    if not isinstance(data, dict):
        raise ValueError("Project JSON root must be an object.")

    decorations = []
    json_decorations = data.get("decorations", [])

    if json_decorations is None:
        return decorations

    if not isinstance(json_decorations, list):
        raise ValueError("Project JSON 'decorations' must be a list.")

    for json_decoration in json_decorations:
        if not isinstance(json_decoration, dict):
            raise ValueError("Each decoration must be an object.")

        decoration_type = str(json_decoration.get("type", ""))
        if decoration_type != DECORATION_TYPE_INCUBATOR_ARRAY:
            continue

        start = json_decoration.get("start", {})
        if not isinstance(start, dict):
            raise ValueError("Decoration start must be an object.")

        decoration = IncubatorDecoration(
            start_x=float(start.get("x", 0.0)),
            start_y=float(start.get("y", 0.0)),
            row_length=float(json_decoration.get("row_length", 0.0)),
            column_length=float(json_decoration.get("column_length", 0.0)),
            item_spacing_scale=float(json_decoration.get("item_spacing_scale", 1.0)),
            row_spacing_scale=float(json_decoration.get("row_spacing_scale", 1.0)),
        )
        decorations.append(decoration)

    return decorations
