"""
Plain project state shared by UI, JSON, and DLL binding layers.
"""
from dataclasses import dataclass, field


DEFAULT_MAP_SIZE_X = 600.0
DEFAULT_MAP_SIZE_Y = 600.0
PROJECT_VERSION_AS1 = "AS1"
PROJECT_VERSION_AS2 = "AS2"
PROJECT_VERSION_AS2R = "AS2R"
DEFAULT_PROJECT_VERSION = PROJECT_VERSION_AS1
SUPPORTED_PROJECT_VERSIONS = {
    PROJECT_VERSION_AS1,
    PROJECT_VERSION_AS2R,
}
AS2_SERIES_PROJECT_VERSIONS = {
    PROJECT_VERSION_AS2,
    PROJECT_VERSION_AS2R,
}
DECORATION_TYPE_INCUBATOR_ARRAY = "incubator_array"
DECORATION_TYPE_DESK_ARRAY = "desk_array"

# TODO: Decoration arrays are now split by type and can generate maps, but
# this whole area still needs a focused cleanup pass. Unify shared array
# fields, preview behavior, UI labels, and C API naming after AS2 automation
# research settles the long-term decoration workflow.


@dataclass
class IncubatorDecoration:
    """
    Incubator array area stored in physical map coordinates.
    """

    start_x: float
    start_y: float
    row_length: float
    column_length: float
    item_spacing_scale: float = 1.0
    row_spacing_scale: float = 1.0
    decoration_type: str = DECORATION_TYPE_INCUBATOR_ARRAY


@dataclass
class DeskDecoration:
    """
    Desk array area stored in physical map coordinates.
    """

    start_x: float
    start_y: float
    row_length: float
    column_length: float
    item_spacing_scale: float = 1.0
    row_spacing_scale: float = 1.0
    decoration_type: str = DECORATION_TYPE_DESK_ARRAY


@dataclass
class ProjectData:
    """
    Editor project data shared by JSON, UI, and DLL binding layers.
    """

    version: str = DEFAULT_PROJECT_VERSION
    map_size_x: float = DEFAULT_MAP_SIZE_X
    map_size_y: float = DEFAULT_MAP_SIZE_Y
    segments: list = field(default_factory=list)
    doors: list = field(default_factory=list)
    decorations: list = field(default_factory=list)
    is_door_open: bool = False


def validate_project_version(version: str) -> str:
    """Return a supported project version or reject an unknown value."""
    if not isinstance(version, str):
        raise ValueError("Project JSON 'version' must be a string.")

    if version not in SUPPORTED_PROJECT_VERSIONS:
        raise ValueError(f"Unsupported project version: {version}")

    return version


def is_as2_series_project_version(version: str) -> bool:
    """Return whether a project version belongs to the AS2 series."""
    return version in AS2_SERIES_PROJECT_VERSIONS


def supports_ceiling_generation(version: str) -> bool:
    """Return whether the selected project format supports ceiling generation."""
    if version == PROJECT_VERSION_AS1:
        return False

    if is_as2_series_project_version(version):
        return True

    raise ValueError(f"Unsupported project version: {version}")


def supports_global_door_state(version: str) -> bool:
    """Return whether the project format supports the global door override."""
    if version == PROJECT_VERSION_AS1:
        return True

    if is_as2_series_project_version(version):
        return False

    raise ValueError(f"Unsupported project version: {version}")
