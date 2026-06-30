"""
Wall profile constants shared by UI drawing and project data.
"""


WALL_TYPE_STANDARD = 0
WALL_TYPE_LAB = 1


WALL_PROFILES = {
    WALL_TYPE_STANDARD: {
        "label": "Standard Wall (N/A)",
        "short_label": "Standard Wall",
        "description": "Classic steel wall set: standard walls, active doors, and dead door variants.",
        "color": "#4f7fbf",
    },
    WALL_TYPE_LAB: {
        "label": "Lab Wall (N/A)",
        "short_label": "Lab Wall",
        "description": "Green laser laboratory set: lab walls, laser doors, and decoration frames.",
        "color": "#2aa879",
    },
}


def apply_wall_profiles_from_dll(dll_profiles: dict) -> None:
    """
    Merge DLL wall profile values into Python UI metadata.
    """
    for wall_type, dll_profile in dll_profiles.items():
        if wall_type not in WALL_PROFILES:
            WALL_PROFILES[wall_type] = _build_unknown_wall_profile(wall_type)

        profile = WALL_PROFILES[wall_type]
        profile["step_x"] = dll_profile["step_x"]
        profile["step_y"] = dll_profile["step_y"]
        profile["grid_divisor"] = dll_profile["grid_divisor"]
        profile["dir_a_vid"] = dll_profile["dir_a_vid"]
        profile["dir_b_vid"] = dll_profile["dir_b_vid"]
        profile["pillar_vid"] = dll_profile["pillar_vid"]
        profile["offset_a_x"] = dll_profile["offset_a_x"]
        profile["offset_a_y"] = dll_profile["offset_a_y"]
        profile["offset_b_x"] = dll_profile["offset_b_x"]
        profile["offset_b_y"] = dll_profile["offset_b_y"]
        profile["offset_p_x"] = dll_profile["offset_p_x"]
        profile["offset_p_y"] = dll_profile["offset_p_y"]
        profile["label"] = f"{profile['short_label']} ({profile['step_x']:.0f}x{profile['step_y']:.0f})"


def get_wall_profile(wall_type: int) -> dict:
    """
    Return a DLL-backed wall profile by type.
    """
    if wall_type not in WALL_PROFILES:
        raise KeyError(f"Wall profile not found: {wall_type}")

    profile = WALL_PROFILES[wall_type]
    _require_dll_values(wall_type, profile)
    return profile


def get_wall_profile_items() -> list:
    """
    Return wall profile choices in stable order.
    """
    items = []

    for wall_type in sorted(WALL_PROFILES.keys()):
        profile = get_wall_profile(wall_type)
        items.append((wall_type, profile["label"]))

    return items


def _build_unknown_wall_profile(wall_type: int) -> dict:
    """
    Build UI metadata for a DLL wall type unknown to Python.
    """
    profile = {
        "label": f"Wall Type {wall_type} (N/A)",
        "short_label": f"Wall Type {wall_type}",
        "description": "Wall set loaded from the C++ DLL.",
        "color": "#c0a05a",
    }
    return profile


def _require_dll_values(wall_type: int, profile: dict) -> None:
    """
    Fail fast when a profile has not been populated from the DLL.
    """
    required_keys = [
        "step_x",
        "step_y",
        "grid_divisor",
        "dir_a_vid",
        "dir_b_vid",
        "pillar_vid",
        "offset_a_x",
        "offset_a_y",
        "offset_b_x",
        "offset_b_y",
        "offset_p_x",
        "offset_p_y",
    ]

    for key in required_keys:
        if key not in profile:
            raise RuntimeError(f"Wall profile {wall_type} was not loaded from DLL. Missing: {key}")


