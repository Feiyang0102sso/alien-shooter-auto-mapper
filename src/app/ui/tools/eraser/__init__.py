"""Eraser tool package."""
from app.ui.tools.eraser.config import DEFAULT_ERASER_SIZE
from app.ui.tools.eraser.controller import EraserToolController
from app.ui.tools.eraser.properties_widget import EraserPropertiesWidget


__all__ = [
    "DEFAULT_ERASER_SIZE",
    "EraserPropertiesWidget",
    "EraserToolController",
]
