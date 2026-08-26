"""
Decoration stamp query helpers backed by the central DLL registry.
"""

from app.binding import dll_registry
from app.i18n.locale import tr
from app.i18n.text_keys import TextKey


# Card text is authored per profile id; the DLL only exports geometry.
DECORATION_STAMP_UI_METADATA = {
    "dorms01": {
        "label_key": TextKey.DECORATION_STAMP_DORMS01_LABEL,
        "description_key": TextKey.DECORATION_STAMP_DORMS01_DESCRIPTION,
    },
}


def get_decoration_stamps() -> list:
    """
    Return all decoration stamps in DLL table order.
    """
    return dll_registry.get_decoration_stamps()


def get_decoration_stamp(profile_id: str) -> dict:
    """
    Return one decoration stamp by profile id.
    """
    return dll_registry.get_decoration_stamp(profile_id)


def get_decoration_stamp_items() -> list:
    """
    Return selectable decoration stamp cards with localized text.
    """
    items = []

    for stamp in get_decoration_stamps():
        metadata = _get_stamp_ui_metadata(stamp["profile_id"])
        item = dict(stamp)
        item["label"] = tr(metadata["label_key"])
        item["description"] = tr(metadata["description_key"])
        items.append(item)

    return items


def get_stamp_frame_size(profile_id: str) -> tuple:
    """
    Return the stamp frame width and height in physical map units.
    """
    corner_offsets = get_decoration_stamp(profile_id)["corner_offsets"]

    x_values = []
    y_values = []
    for corner in corner_offsets:
        x_values.append(corner[0])
        y_values.append(corner[1])

    width = max(x_values) - min(x_values)
    height = max(y_values) - min(y_values)
    return width, height


def _get_stamp_ui_metadata(profile_id: str) -> dict:
    """
    Return card text keys for one profile. Raises when a profile has no text.
    """
    if profile_id not in DECORATION_STAMP_UI_METADATA:
        raise KeyError(f"Decoration stamp has no UI metadata: {profile_id}")

    return DECORATION_STAMP_UI_METADATA[profile_id]
