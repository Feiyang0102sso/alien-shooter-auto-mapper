"""
Drawable part options for each wall set.
"""

from app.editor.wall_profiles import WALL_TYPE_LAB, WALL_TYPE_STANDARD


PART_WALL_BODY = "wall_body"
PART_ACTIVE_DOOR = "active_door"
PART_DEAD_DOOR_CLOSED = "dead_door_closed"
PART_DEAD_DOOR_JAMMED = "dead_door_jammed"
PART_DEAD_DOOR_OPEN = "dead_door_open"
PART_LAB_LASER_CLOSED = "lab_laser_closed"
PART_LAB_LASER_OPEN = "lab_laser_open"
PART_LAB_DECORATION_DOOR = "lab_decoration_door"


DRAWABLE_PARTS_BY_WALL_TYPE = {
    WALL_TYPE_STANDARD: [
        (PART_WALL_BODY, "Wall Body"),
        (PART_ACTIVE_DOOR, "Active Door"),
        (PART_DEAD_DOOR_CLOSED, "Dead Door (Closed)"),
        (PART_DEAD_DOOR_JAMMED, "Dead Door (Jammed)"),
        (PART_DEAD_DOOR_OPEN, "Dead Door (Open)"),
    ],
    WALL_TYPE_LAB: [
        (PART_WALL_BODY, "Wall Body"),
        (PART_LAB_LASER_CLOSED, "Laser Door (Closed)"),
        (PART_LAB_LASER_OPEN, "Laser Door (Open)"),
        (PART_LAB_DECORATION_DOOR, "Decoration Door"),
    ],
}


def get_drawable_parts(wall_type: int) -> list:
    """
    Return drawable parts for a wall set.
    """
    if wall_type in DRAWABLE_PARTS_BY_WALL_TYPE:
        return DRAWABLE_PARTS_BY_WALL_TYPE[wall_type]

    return DRAWABLE_PARTS_BY_WALL_TYPE[WALL_TYPE_STANDARD]
