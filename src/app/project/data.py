"""
Plain project state shared by UI, JSON, and DLL binding layers.
"""
from dataclasses import dataclass, field


DEFAULT_MAP_SIZE_X = 600.0
DEFAULT_MAP_SIZE_Y = 600.0


@dataclass
class ProjectData:
    """
    Editor project data in the old JSON-compatible shape.
    """

    map_size_x: float = DEFAULT_MAP_SIZE_X
    map_size_y: float = DEFAULT_MAP_SIZE_Y
    segments: list = field(default_factory=list)
    doors: list = field(default_factory=list)
