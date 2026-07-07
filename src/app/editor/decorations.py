"""
Decoration metadata used by the editor UI.
"""

from app.project.data import DECORATION_TYPE_INCUBATOR_ARRAY
from app.i18n.locale import tr
from app.i18n.text_keys import TextKey


INCUBATOR_DEFAULT_ITEM_SPACING = 1.0
INCUBATOR_DEFAULT_ROW_SPACING = 1.0


def get_decoration_items() -> list:
    """
    Return selectable decoration cards.
    """
    items = [
        {
            "decoration_type": DECORATION_TYPE_INCUBATOR_ARRAY,
            "label": tr(TextKey.DECORATION_INCUBATOR_LABEL),
            "description": tr(TextKey.DECORATION_INCUBATOR_DESCRIPTION),
            "preview_path": "",
        }
    ]
    return items
