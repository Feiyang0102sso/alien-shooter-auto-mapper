"""Drawing mode identifiers used by the map editor."""
from enum import Enum

from app.i18n.locale import tr
from app.i18n.text_keys import TextKey


class DrawingMode(str, Enum):
    """Supported wall drawing modes."""

    STRAIGHT_LINE = "straight_line"
    POLYLINE = "polyline"
    RECTANGLE = "rectangle"
    ERASER = "eraser"


DRAWING_MODE_LABELS = {
    DrawingMode.STRAIGHT_LINE: tr(TextKey.DRAWING_STRAIGHT_LINE),
    DrawingMode.POLYLINE: tr(TextKey.DRAWING_POLYLINE),
    DrawingMode.RECTANGLE: tr(TextKey.DRAWING_RECTANGLE),
    DrawingMode.ERASER: tr(TextKey.DRAWING_ERASER),
}
