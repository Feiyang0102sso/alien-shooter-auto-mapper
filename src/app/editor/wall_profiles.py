"""
Wall profile query helpers backed by the central DLL registry.
"""

from app.binding import dll_registry


def get_wall_profile(wall_type: int) -> dict:
    """
    Return a DLL-backed wall profile by type.
    """
    return dll_registry.get_wall_profile(wall_type)


def get_wall_profiles() -> list:
    """
    Return all wall profiles in stable wall type order.
    """
    return dll_registry.get_wall_profiles()


def get_wall_profile_items() -> list:
    """
    Return wall profile choices in stable order.
    """
    items = []

    for profile in get_wall_profiles():
        items.append((profile["wall_type"], profile["label"]))

    return items


def get_default_wall_type() -> int:
    """
    Return the first DLL-exported wall type.
    """
    return dll_registry.get_default_wall_type()


def get_drawable_parts(wall_type: int) -> list:
    """
    Return DLL-exported drawable parts for a wall profile.
    """
    return dll_registry.get_drawable_parts(wall_type)


def find_wall_type_by_steps(step_x: float, step_y: float = None) -> int:
    """
    Find a wall type by physical step dimensions.
    """
    return dll_registry.find_wall_type_by_steps(step_x, step_y)
