"""
Central registry for DLL-exported metadata used by the UI layer.
"""

from app.i18n.locale import tr
from app.i18n.text_keys import TextKey
from app.logger import logger
from app.ui.colors import (
    WALL_PROFILE_FALLBACK,
    WALL_PROFILE_LAB,
    WALL_PROFILE_STANDARD,
    WALL_PROFILE_STANDARD_DARK,
)


WALL_PROFILES = {}
STANDARD_DOOR_Z_CONFIGS = {}
STANDARD_DOOR_SIZES = []
AUTO_MAPPER_CLIENT = None
WALL_TYPE_STANDARD = 0
WALL_TYPE_LAB = 1
WALL_TYPE_STANDARD_DARK = 2

WALL_PROFILE_UI_METADATA = {
    WALL_TYPE_STANDARD: {
        "short_label_key": TextKey.WALL_STANDARD_SHORT_LABEL,
        "description_key": TextKey.WALL_STANDARD_DESCRIPTION,
        "color": WALL_PROFILE_STANDARD,
        "preview_key": "base",
    },
    WALL_TYPE_LAB: {
        "short_label_key": TextKey.WALL_LAB_SHORT_LABEL,
        "description_key": TextKey.WALL_LAB_DESCRIPTION,
        "color": WALL_PROFILE_LAB,
        "preview_key": "lab",
    },
    WALL_TYPE_STANDARD_DARK: {
        "short_label_key": TextKey.WALL_STANDARD_DARK_SHORT_LABEL,
        "description_key": TextKey.WALL_STANDARD_DARK_DESCRIPTION,
        "color": WALL_PROFILE_STANDARD_DARK,
        "preview_key": "standard_dark",
    },
}

DRAWABLE_PART_LABELS = {
    "wall_body": TextKey.DRAWABLE_WALL_BODY,
    "active_door": TextKey.DRAWABLE_ACTIVE_DOOR,
    "dead_door_closed": TextKey.DRAWABLE_DEAD_DOOR_CLOSED,
    "dead_door_jammed": TextKey.DRAWABLE_DEAD_DOOR_JAMMED,
    "dead_door_open": TextKey.DRAWABLE_DEAD_DOOR_OPEN,
    "lab_laser_closed": TextKey.DRAWABLE_LAB_LASER_CLOSED,
    "lab_laser_open": TextKey.DRAWABLE_LAB_LASER_OPEN,
    "lab_decoration_door": TextKey.DRAWABLE_LAB_DECORATION_DOOR,
}


def register_all_from_dll(auto_mapper_client) -> bool:
    """
    Load and register all DLL-exported UI metadata.
    """
    global AUTO_MAPPER_CLIENT

    wall_profiles = auto_mapper_client.load_wall_profiles()
    if not wall_profiles:
        logger.error("Failed to register wall profiles from DLL.")
        return False

    AUTO_MAPPER_CLIENT = auto_mapper_client
    standard_door_sizes = auto_mapper_client.load_standard_door_sizes()
    standard_door_z_configs = auto_mapper_client.load_standard_door_z_config(standard_door_sizes)

    register_wall_profiles(wall_profiles)
    register_standard_door_sizes(standard_door_sizes)
    register_standard_door_z_configs(standard_door_z_configs)

    logger.info("Registered DLL metadata for UI.")
    return True


def register_wall_profiles(dll_profiles: dict) -> None:
    """
    Replace registered wall profiles with DLL-exported values.
    """
    WALL_PROFILES.clear()

    for wall_type, dll_profile in dll_profiles.items():
        profile = dict(dll_profile)
        metadata = _get_wall_profile_ui_metadata(wall_type)
        profile.update(metadata)
        profile["drawable_parts"] = _build_drawable_part_items(profile["drawable_parts"])
        profile["label"] = f"{profile['short_label']} ({profile['step_x']:.0f}x{profile['step_y']:.0f})"
        WALL_PROFILES[wall_type] = profile


def register_standard_door_z_configs(configs: dict) -> None:
    """
    Replace registered standard door z-offset configs.
    """
    STANDARD_DOOR_Z_CONFIGS.clear()

    for size, config in configs.items():
        STANDARD_DOOR_Z_CONFIGS[size] = config


def register_standard_door_sizes(sizes: list) -> None:
    """
    Replace registered standard door sizes.
    """
    STANDARD_DOOR_SIZES.clear()

    for size in sorted(sizes):
        STANDARD_DOOR_SIZES.append(size)


def get_wall_profile(wall_type: int) -> dict:
    """
    Return a registered wall profile by type.
    """
    if wall_type not in WALL_PROFILES:
        raise KeyError(f"Wall profile not found: {wall_type}")

    profile = WALL_PROFILES[wall_type]
    _require_wall_profile_values(wall_type, profile)
    return profile


def get_wall_profiles() -> list:
    """
    Return all registered wall profiles in stable wall type order.
    """
    profiles = []

    for wall_type in sorted(WALL_PROFILES.keys()):
        profiles.append(get_wall_profile(wall_type))

    return profiles


def get_default_wall_type() -> int:
    """
    Return the first registered DLL wall type.
    """
    if not WALL_PROFILES:
        raise RuntimeError("Wall profiles were not registered from DLL.")

    wall_types = sorted(WALL_PROFILES.keys())
    return wall_types[0]


def get_drawable_parts(wall_type: int) -> list:
    """
    Return registered drawable parts for a wall profile.
    """
    profile = get_wall_profile(wall_type)
    return list(profile["drawable_parts"])


def find_wall_type_by_steps(step_x: float, step_y: float = None) -> int:
    """
    Find a wall type by physical step dimensions.
    """
    for profile in get_wall_profiles():
        x_matches = abs(profile["step_x"] - step_x) < 0.01
        y_matches = True

        if step_y is not None:
            y_matches = abs(profile["step_y"] - step_y) < 0.01

        if x_matches and y_matches:
            return profile["wall_type"]

    return get_default_wall_type()


def get_standard_door_z_config(size: int):
    """
    Return a registered standard door z-offset config by size.
    """
    if size not in STANDARD_DOOR_Z_CONFIGS:
        raise KeyError(f"Standard door z config not found: {size}")

    return STANDARD_DOOR_Z_CONFIGS[size]


def get_standard_door_jam_z_offset(size: int) -> float:
    """
    Return one DLL-generated jammed standard door z-offset.
    """
    if AUTO_MAPPER_CLIENT is None:
        raise RuntimeError("Auto mapper DLL client was not registered.")

    clamped_size = clamp_standard_door_size(size)
    return AUTO_MAPPER_CLIENT.get_standard_door_jam_z_offset(clamped_size)


def get_standard_door_dead_open_z_offset(size: int) -> float:
    """
    Return the fixed dead-open z-offset for a standard door size.
    """
    clamped_size = clamp_standard_door_size(size)
    config = get_standard_door_z_config(clamped_size)
    return (config.dead_open_min_z + config.dead_open_max_z) / 2.0


def get_standard_door_z_configs() -> dict:
    """
    Return registered standard door z-offset configs.
    """
    configs = {}

    for size, config in STANDARD_DOOR_Z_CONFIGS.items():
        configs[size] = config

    return configs


def get_standard_door_sizes() -> list:
    """
    Return registered standard door sizes.
    """
    return list(STANDARD_DOOR_SIZES)


def get_max_standard_door_size() -> int:
    """
    Return the largest registered standard door size.
    """
    if not STANDARD_DOOR_SIZES:
        raise RuntimeError("Standard door sizes were not registered from DLL.")

    return max(STANDARD_DOOR_SIZES)


def clamp_standard_door_size(raw_size: int) -> int:
    """
    Clamp a requested standard door size to the DLL-supported range.
    """
    if not STANDARD_DOOR_SIZES:
        raise RuntimeError("Standard door sizes were not registered from DLL.")

    min_size = min(STANDARD_DOOR_SIZES)
    max_size = max(STANDARD_DOOR_SIZES)

    if raw_size < min_size:
        return min_size
    if raw_size > max_size:
        return max_size

    if raw_size in STANDARD_DOOR_SIZES:
        return raw_size

    fallback_size = min_size
    for size in STANDARD_DOOR_SIZES:
        if size <= raw_size:
            fallback_size = size

    return fallback_size


def _require_wall_profile_values(wall_type: int, profile: dict) -> None:
    """
    Fail fast when a profile has not been populated from the DLL.
    """
    required_keys = [
        "wall_type",
        "short_label",
        "description",
        "color",
        "preview_key",
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
        "drawable_parts",
    ]

    for key in required_keys:
        if key not in profile:
            raise RuntimeError(f"Wall profile {wall_type} was not registered from DLL. Missing: {key}")


def _get_wall_profile_ui_metadata(wall_type: int) -> dict:
    """
    Return UI-owned metadata for a wall profile.
    """
    if wall_type in WALL_PROFILE_UI_METADATA:
        metadata = dict(WALL_PROFILE_UI_METADATA[wall_type])
        metadata["short_label"] = tr(metadata.pop("short_label_key"))
        metadata["description"] = tr(metadata.pop("description_key"))
        return metadata

    metadata = {
        "short_label": tr(TextKey.WALL_TYPE_FALLBACK_SHORT_LABEL, wall_type=wall_type),
        "description": tr(TextKey.WALL_TYPE_FALLBACK_DESCRIPTION),
        "color": WALL_PROFILE_FALLBACK,
        "preview_key": "",
    }
    return metadata


def _build_drawable_part_items(part_ids: list) -> list:
    """
    Convert DLL-exported part ids into UI-owned display labels.
    """
    items = []

    for part_id in part_ids:
        label_key = DRAWABLE_PART_LABELS.get(part_id)
        if label_key is None:
            label = part_id
        else:
            label = tr(label_key)

        items.append((part_id, label))

    return items
