"""
Wall profile constants shared by UI drawing and project data.
"""


WALL_TYPE_STANDARD = 0
WALL_TYPE_LAB = 1


WALL_PROFILES = {
    WALL_TYPE_STANDARD: {
        "step_x": 40.0,
        "step_y": 28.0,
        "grid_divisor": 1,
        "label": "Standard Wall (40x28)",
        "short_label": "Standard Wall",
        "description": "Classic steel wall set: standard walls, active doors, and dead door variants.",
        "color": "#4f7fbf",
    },
    WALL_TYPE_LAB: {
        "step_x": 90.0,
        "step_y": 64.0,
        "grid_divisor": 2,
        "label": "Lab Wall (90x64)",
        "short_label": "Lab Wall",
        "description": "Green laser laboratory set: lab walls, laser doors, and decoration frames.",
        "color": "#2aa879",
    },
}


def get_wall_profile(wall_type: int) -> dict:
    """
    Return a wall profile by type, falling back to standard.
    """
    if wall_type in WALL_PROFILES:
        return WALL_PROFILES[wall_type]

    return WALL_PROFILES[WALL_TYPE_STANDARD]


def get_wall_profile_items() -> list:
    """
    Return wall profile choices in stable order.
    """
    items = []

    for wall_type in sorted(WALL_PROFILES.keys()):
        profile = WALL_PROFILES[wall_type]
        items.append((wall_type, profile["label"]))

    return items
