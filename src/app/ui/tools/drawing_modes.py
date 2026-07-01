"""Drawing mode identifiers used by the map editor."""
from enum import Enum


class DrawingMode(str, Enum):
    """Supported wall drawing modes."""

    STRAIGHT_LINE = "straight_line"
    POLYLINE = "polyline"
    RECTANGLE = "rectangle"


DRAWING_MODE_LABELS = {
    DrawingMode.STRAIGHT_LINE: "Straight Line",
    DrawingMode.POLYLINE: "Polyline",
    DrawingMode.RECTANGLE: "Rectangle",
}
