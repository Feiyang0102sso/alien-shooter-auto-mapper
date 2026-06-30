"""
Drawable part options for each wall set.
"""

from app.editor.wall_profiles import get_drawable_parts as get_dll_drawable_parts


PART_WALL_BODY = "wall_body"


def get_drawable_parts(wall_type: int) -> list:
    """
    Return DLL-exported drawable parts for a wall set.
    """
    return get_dll_drawable_parts(wall_type)
