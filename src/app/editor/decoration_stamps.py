"""
Decoration stamp query helpers backed by the central DLL registry.
"""

from app.binding import dll_registry
from app.i18n.locale import tr
from app.i18n.text_keys import TextKey


# Card text is authored per profile id; the DLL only exports geometry.
# Stamps sharing a series_id collapse into one shelf card, and the series
# members are picked from the inspector dropdown (same shape as AS2 Wall Set 1).
DECORATION_STAMP_UI_METADATA = {
    "dorms01": {
        "series_id": "dorms",
        "label_key": TextKey.DECORATION_STAMP_DORMS01_LABEL,
        "description_key": TextKey.DECORATION_STAMP_DORMS01_DESCRIPTION,
    },
    "dorms02": {
        "series_id": "dorms",
        "label_key": TextKey.DECORATION_STAMP_DORMS02_LABEL,
        "description_key": TextKey.DECORATION_STAMP_DORMS02_DESCRIPTION,
    },
    "dorms03": {
        "series_id": "dorms",
        "label_key": TextKey.DECORATION_STAMP_DORMS03_LABEL,
        "description_key": TextKey.DECORATION_STAMP_DORMS03_DESCRIPTION,
    },
    "dorms04": {
        "series_id": "dorms",
        "label_key": TextKey.DECORATION_STAMP_DORMS04_LABEL,
        "description_key": TextKey.DECORATION_STAMP_DORMS04_DESCRIPTION,
    },
    "dorms05": {
        "series_id": "dorms",
        "label_key": TextKey.DECORATION_STAMP_DORMS05_LABEL,
        "description_key": TextKey.DECORATION_STAMP_DORMS05_DESCRIPTION,
    },
    "dorms06": {
        "series_id": "dorms",
        "label_key": TextKey.DECORATION_STAMP_DORMS06_LABEL,
        "description_key": TextKey.DECORATION_STAMP_DORMS06_DESCRIPTION,
    },
    "dorms07": {
        "series_id": "dorms",
        "label_key": TextKey.DECORATION_STAMP_DORMS07_LABEL,
        "description_key": TextKey.DECORATION_STAMP_DORMS07_DESCRIPTION,
    },
    "dorms08": {
        "series_id": "dorms",
        "label_key": TextKey.DECORATION_STAMP_DORMS08_LABEL,
        "description_key": TextKey.DECORATION_STAMP_DORMS08_DESCRIPTION,
    },
}

# Shelf card text authored per series.
DECORATION_STAMP_SERIES_UI_METADATA = {
    "dorms": {
        "label_key": TextKey.DECORATION_STAMP_SERIES_DORMS_LABEL,
        "description_key": TextKey.DECORATION_STAMP_SERIES_DORMS_DESCRIPTION,
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
    Return every decoration stamp with localized text, in DLL order.
    """
    items = []

    for stamp in get_decoration_stamps():
        metadata = _get_stamp_ui_metadata(stamp["profile_id"])
        item = dict(stamp)
        item["series_id"] = metadata["series_id"]
        item["label"] = tr(metadata["label_key"])
        item["description"] = tr(metadata["description_key"])
        items.append(item)

    return items


def get_decoration_stamp_series_items() -> list:
    """
    Return one selectable shelf card per stamp series, in DLL order.

    A card carries the series text plus the stamp armed when it is selected.
    """
    series_items = []
    series_index_by_id = {}

    for stamp in get_decoration_stamp_items():
        series_id = stamp["series_id"]

        if series_id in series_index_by_id:
            existing_item = series_items[series_index_by_id[series_id]]
            existing_item["variant_count"] += 1
            continue

        metadata = _get_series_ui_metadata(series_id)
        series_index_by_id[series_id] = len(series_items)
        series_items.append(
            {
                "series_id": series_id,
                "label": tr(metadata["label_key"]),
                "description": tr(metadata["description_key"]),
                "default_profile_id": stamp["profile_id"],
                "variant_count": 1,
            }
        )

    return series_items


def get_decoration_stamp_variants(profile_id: str) -> list:
    """
    Return every stamp sharing a series with the given profile, in DLL order.
    """
    series_id = _get_stamp_ui_metadata(profile_id)["series_id"]

    variants = []
    for stamp in get_decoration_stamp_items():
        if stamp["series_id"] == series_id:
            variants.append(stamp)

    return variants


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


def _get_series_ui_metadata(series_id: str) -> dict:
    """
    Return card text keys for one series. Raises when a series has no text.
    """
    if series_id not in DECORATION_STAMP_SERIES_UI_METADATA:
        raise KeyError(f"Decoration stamp series has no UI metadata: {series_id}")

    return DECORATION_STAMP_SERIES_UI_METADATA[series_id]
