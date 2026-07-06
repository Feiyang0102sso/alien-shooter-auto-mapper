"""
Decoration metadata used by the editor UI.
"""

from app.project.data import DECORATION_TYPE_INCUBATOR_ARRAY


INCUBATOR_PLACEHOLDER_WIDTH = 60.0
INCUBATOR_PLACEHOLDER_HEIGHT = 90.0
INCUBATOR_DEFAULT_ITEM_SPACING = 1.0
INCUBATOR_DEFAULT_ROW_SPACING = 1.0


def get_decoration_items() -> list:
    """
    Return selectable decoration cards.
    """
    items = [
        {
            "decoration_type": DECORATION_TYPE_INCUBATOR_ARRAY,
            "label": "Incubator",
            "description": "Draw an incubator array area, then tune row and column spacing.",
        }
    ]
    return items
