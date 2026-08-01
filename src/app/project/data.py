"""
Plain project state shared by UI, JSON, and DLL binding layers.
"""
from dataclasses import dataclass, field


DEFAULT_MAP_SIZE_X = 600.0
DEFAULT_MAP_SIZE_Y = 600.0
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
    Editor project data in the old JSON-compatible shape.
    """

    map_size_x: float = DEFAULT_MAP_SIZE_X
    map_size_y: float = DEFAULT_MAP_SIZE_Y
    segments: list = field(default_factory=list)
    doors: list = field(default_factory=list)
    decorations: list = field(default_factory=list)
    is_door_open: bool = False
