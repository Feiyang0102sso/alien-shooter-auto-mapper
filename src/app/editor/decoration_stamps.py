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
    "office01": {
        "series_id": "office",
        "label_key": TextKey.DECORATION_STAMP_OFFICE01_LABEL,
        "description_key": TextKey.DECORATION_STAMP_OFFICE01_DESCRIPTION,
    },
    "office02": {
        "series_id": "office",
        "label_key": TextKey.DECORATION_STAMP_OFFICE02_LABEL,
        "description_key": TextKey.DECORATION_STAMP_OFFICE02_DESCRIPTION,
    },
    "office03": {
        "series_id": "office",
        "label_key": TextKey.DECORATION_STAMP_OFFICE03_LABEL,
        "description_key": TextKey.DECORATION_STAMP_OFFICE03_DESCRIPTION,
    },
    "office04": {
        "series_id": "office",
        "label_key": TextKey.DECORATION_STAMP_OFFICE04_LABEL,
        "description_key": TextKey.DECORATION_STAMP_OFFICE04_DESCRIPTION,
    },
    "office05": {
        "series_id": "office",
        "label_key": TextKey.DECORATION_STAMP_OFFICE05_LABEL,
        "description_key": TextKey.DECORATION_STAMP_OFFICE05_DESCRIPTION,
    },
    "office06": {
        "series_id": "office",
        "label_key": TextKey.DECORATION_STAMP_OFFICE06_LABEL,
        "description_key": TextKey.DECORATION_STAMP_OFFICE06_DESCRIPTION,
    },
    "office07": {
        "series_id": "office",
        "label_key": TextKey.DECORATION_STAMP_OFFICE07_LABEL,
        "description_key": TextKey.DECORATION_STAMP_OFFICE07_DESCRIPTION,
    },
    "office08": {
        "series_id": "office",
        "label_key": TextKey.DECORATION_STAMP_OFFICE08_LABEL,
        "description_key": TextKey.DECORATION_STAMP_OFFICE08_DESCRIPTION,
    },
    "office09": {
        "series_id": "office",
        "label_key": TextKey.DECORATION_STAMP_OFFICE09_LABEL,
        "description_key": TextKey.DECORATION_STAMP_OFFICE09_DESCRIPTION,
    },
    "storage01": {
        "series_id": "storage",
        "label_key": TextKey.DECORATION_STAMP_STORAGE01_LABEL,
        "description_key": TextKey.DECORATION_STAMP_STORAGE01_DESCRIPTION,
    },
    "storage02": {
        "series_id": "storage",
        "label_key": TextKey.DECORATION_STAMP_STORAGE02_LABEL,
        "description_key": TextKey.DECORATION_STAMP_STORAGE02_DESCRIPTION,
    },
    "storage03": {
        "series_id": "storage",
        "label_key": TextKey.DECORATION_STAMP_STORAGE03_LABEL,
        "description_key": TextKey.DECORATION_STAMP_STORAGE03_DESCRIPTION,
    },
    "storage04": {
        "series_id": "storage",
        "label_key": TextKey.DECORATION_STAMP_STORAGE04_LABEL,
        "description_key": TextKey.DECORATION_STAMP_STORAGE04_DESCRIPTION,
    },
    "storage05": {
        "series_id": "storage",
        "label_key": TextKey.DECORATION_STAMP_STORAGE05_LABEL,
        "description_key": TextKey.DECORATION_STAMP_STORAGE05_DESCRIPTION,
    },
    "storage06": {
        "series_id": "storage",
        "label_key": TextKey.DECORATION_STAMP_STORAGE06_LABEL,
        "description_key": TextKey.DECORATION_STAMP_STORAGE06_DESCRIPTION,
    },
    "storage07": {
        "series_id": "storage",
        "label_key": TextKey.DECORATION_STAMP_STORAGE07_LABEL,
        "description_key": TextKey.DECORATION_STAMP_STORAGE07_DESCRIPTION,
    },
    "storage08": {
        "series_id": "storage",
        "label_key": TextKey.DECORATION_STAMP_STORAGE08_LABEL,
        "description_key": TextKey.DECORATION_STAMP_STORAGE08_DESCRIPTION,
    },
    "storage09": {
        "series_id": "storage",
        "label_key": TextKey.DECORATION_STAMP_STORAGE09_LABEL,
        "description_key": TextKey.DECORATION_STAMP_STORAGE09_DESCRIPTION,
    },
}

# Shelf card text authored per series.
DECORATION_STAMP_SERIES_UI_METADATA = {
    "dorms": {
        "label_key": TextKey.DECORATION_STAMP_SERIES_DORMS_LABEL,
        "description_key": TextKey.DECORATION_STAMP_SERIES_DORMS_DESCRIPTION,
    },
    "office": {
        "label_key": TextKey.DECORATION_STAMP_SERIES_OFFICE_LABEL,
        "description_key": TextKey.DECORATION_STAMP_SERIES_OFFICE_DESCRIPTION,
    },
    "storage": {
        "label_key": TextKey.DECORATION_STAMP_SERIES_STORAGE_LABEL,
        "description_key": TextKey.DECORATION_STAMP_SERIES_STORAGE_DESCRIPTION,
    },
}


# The newer collections are strictly numbered variants, so they share one
# localized label and description template per series instead of duplicating
# two text keys for every profile.
ADDITIONAL_DECORATION_STAMP_SERIES = (
    {
        "series_id": "machines",
        "profile_ids": (
            "machines_01", "machines_02", "machines_03", "machines_04", "machines_05",
            "machines_06", "machines_07", "machines_08", "machines_09",
        ),
        "series_label_key": TextKey.DECORATION_STAMP_SERIES_MACHINES_LABEL,
        "series_description_key": TextKey.DECORATION_STAMP_SERIES_MACHINES_DESCRIPTION,
        "profile_label_key": TextKey.DECORATION_STAMP_MACHINES_LABEL,
        "profile_description_key": TextKey.DECORATION_STAMP_MACHINES_DESCRIPTION,
    },
    {
        "series_id": "computers",
        "profile_ids": (
            "comp01", "comp02", "comp03", "comp04", "comp05",
            "comp06", "comp07", "comp08", "comp09",
        ),
        "series_label_key": TextKey.DECORATION_STAMP_SERIES_COMPUTERS_LABEL,
        "series_description_key": TextKey.DECORATION_STAMP_SERIES_COMPUTERS_DESCRIPTION,
        "profile_label_key": TextKey.DECORATION_STAMP_COMPUTERS_LABEL,
        "profile_description_key": TextKey.DECORATION_STAMP_COMPUTERS_DESCRIPTION,
    },
    {
        "series_id": "vehicle_workshop",
        "profile_ids": (
            "vehicle_workshop01", "vehicle_workshop02", "vehicle_workshop03",
            "vehicle_workshop04", "vehicle_workshop05",
        ),
        "series_label_key": TextKey.DECORATION_STAMP_SERIES_VEHICLE_WORKSHOP_LABEL,
        "series_description_key": TextKey.DECORATION_STAMP_SERIES_VEHICLE_WORKSHOP_DESCRIPTION,
        "profile_label_key": TextKey.DECORATION_STAMP_VEHICLE_WORKSHOP_LABEL,
        "profile_description_key": TextKey.DECORATION_STAMP_VEHICLE_WORKSHOP_DESCRIPTION,
    },
    {
        "series_id": "garage",
        "profile_ids": (
            "garage01", "garage02", "garage03", "garage04", "garage05",
            "garage06", "garage07", "garage08", "garage09",
        ),
        "series_label_key": TextKey.DECORATION_STAMP_SERIES_GARAGE_LABEL,
        "series_description_key": TextKey.DECORATION_STAMP_SERIES_GARAGE_DESCRIPTION,
        "profile_label_key": TextKey.DECORATION_STAMP_GARAGE_LABEL,
        "profile_description_key": TextKey.DECORATION_STAMP_GARAGE_DESCRIPTION,
    },
    {
        "series_id": "weapon_workshop",
        "profile_ids": (
            "weapon_workshop01", "weapon_workshop02", "weapon_workshop03",
            "weapon_workshop04", "weapon_workshop05", "weapon_workshop06",
            "weapon_workshop07", "weapon_workshop08",
        ),
        "series_label_key": TextKey.DECORATION_STAMP_SERIES_WEAPON_WORKSHOP_LABEL,
        "series_description_key": TextKey.DECORATION_STAMP_SERIES_WEAPON_WORKSHOP_DESCRIPTION,
        "profile_label_key": TextKey.DECORATION_STAMP_WEAPON_WORKSHOP_LABEL,
        "profile_description_key": TextKey.DECORATION_STAMP_WEAPON_WORKSHOP_DESCRIPTION,
    },
    {
        "series_id": "sick_bay",
        "profile_ids": (
            "sick_bay01", "sick_bay02", "sick_bay03", "sick_bay04", "sick_bay05",
        ),
        "series_label_key": TextKey.DECORATION_STAMP_SERIES_SICK_BAY_LABEL,
        "series_description_key": TextKey.DECORATION_STAMP_SERIES_SICK_BAY_DESCRIPTION,
        "profile_label_key": TextKey.DECORATION_STAMP_SICK_BAY_LABEL,
        "profile_description_key": TextKey.DECORATION_STAMP_SICK_BAY_DESCRIPTION,
    },
    {
        "series_id": "canteen",
        "profile_ids": ("canteen01", "canteen02", "canteen03", "canteen04"),
        "series_label_key": TextKey.DECORATION_STAMP_SERIES_CANTEEN_LABEL,
        "series_description_key": TextKey.DECORATION_STAMP_SERIES_CANTEEN_DESCRIPTION,
        "profile_label_key": TextKey.DECORATION_STAMP_CANTEEN_LABEL,
        "profile_description_key": TextKey.DECORATION_STAMP_CANTEEN_DESCRIPTION,
    },
    {
        "series_id": "large_machine",
        "profile_ids": ("large_m01", "large_m02"),
        "series_label_key": TextKey.DECORATION_STAMP_SERIES_LARGE_MACHINE_LABEL,
        "series_description_key": TextKey.DECORATION_STAMP_SERIES_LARGE_MACHINE_DESCRIPTION,
        "profile_label_key": TextKey.DECORATION_STAMP_LARGE_MACHINE_LABEL,
        "profile_description_key": TextKey.DECORATION_STAMP_LARGE_MACHINE_DESCRIPTION,
    },
    {
        "series_id": "lab_desks",
        "profile_ids": (
            "lab_desks01", "lab_desks02", "lab_desks03", "lab_desks04",
            "lab_desks05", "lab_desks06", "lab_desks07", "lab_desks08",
        ),
        "series_label_key": TextKey.DECORATION_STAMP_SERIES_LAB_DESKS_LABEL,
        "series_description_key": TextKey.DECORATION_STAMP_SERIES_LAB_DESKS_DESCRIPTION,
        "profile_label_key": TextKey.DECORATION_STAMP_LAB_DESKS_LABEL,
        "profile_description_key": TextKey.DECORATION_STAMP_LAB_DESKS_DESCRIPTION,
    },
    {
        "series_id": "lab_office",
        "profile_ids": ("lab_office01", "lab_office02", "lab_office03", "lab_office04"),
        "series_label_key": TextKey.DECORATION_STAMP_SERIES_LAB_OFFICE_LABEL,
        "series_description_key": TextKey.DECORATION_STAMP_SERIES_LAB_OFFICE_DESCRIPTION,
        "profile_label_key": TextKey.DECORATION_STAMP_LAB_OFFICE_LABEL,
        "profile_description_key": TextKey.DECORATION_STAMP_LAB_OFFICE_DESCRIPTION,
    },
    {
        "series_id": "lab_computers",
        "profile_ids": ("lab_comp01", "lab_comp02", "lab_comp03", "lab_comp04"),
        "series_label_key": TextKey.DECORATION_STAMP_SERIES_LAB_COMPUTERS_LABEL,
        "series_description_key": TextKey.DECORATION_STAMP_SERIES_LAB_COMPUTERS_DESCRIPTION,
        "profile_label_key": TextKey.DECORATION_STAMP_LAB_COMPUTERS_LABEL,
        "profile_description_key": TextKey.DECORATION_STAMP_LAB_COMPUTERS_DESCRIPTION,
    },
    {
        "series_id": "lab_bioexp",
        "profile_ids": (
            "lab_bioexp01", "lab_bioexp02", "lab_bioexp03", "lab_bioexp04",
            "lab_bioexp05", "lab_bioexp06", "lab_bioexp07", "lab_bioexp08",
        ),
        "series_label_key": TextKey.DECORATION_STAMP_SERIES_LAB_BIOEXP_LABEL,
        "series_description_key": TextKey.DECORATION_STAMP_SERIES_LAB_BIOEXP_DESCRIPTION,
        "profile_label_key": TextKey.DECORATION_STAMP_LAB_BIOEXP_LABEL,
        "profile_description_key": TextKey.DECORATION_STAMP_LAB_BIOEXP_DESCRIPTION,
    },
    {
        "series_id": "lab_surgery",
        "profile_ids": (
            "lab_surgery01", "lab_surgery02", "lab_surgery03",
            "lab_surgery04", "lab_surgery05", "lab_surgery06",
        ),
        "series_label_key": TextKey.DECORATION_STAMP_SERIES_LAB_SURGERY_LABEL,
        "series_description_key": TextKey.DECORATION_STAMP_SERIES_LAB_SURGERY_DESCRIPTION,
        "profile_label_key": TextKey.DECORATION_STAMP_LAB_SURGERY_LABEL,
        "profile_description_key": TextKey.DECORATION_STAMP_LAB_SURGERY_DESCRIPTION,
    },
    {
        "series_id": "other_rooms",
        "profile_ids": (
            "others01", "others02", "others03", "others04", "others05",
            "others06", "others07", "others08", "others09",
        ),
        "series_label_key": TextKey.DECORATION_STAMP_SERIES_OTHER_ROOMS_LABEL,
        "series_description_key": TextKey.DECORATION_STAMP_SERIES_OTHER_ROOMS_DESCRIPTION,
        "profile_label_key": TextKey.DECORATION_STAMP_OTHER_ROOMS_LABEL,
        "profile_description_key": TextKey.DECORATION_STAMP_OTHER_ROOMS_DESCRIPTION,
    },
)


def _register_additional_decoration_stamp_series() -> None:
    """Expand compact numbered-series definitions into the UI registries."""
    for series in ADDITIONAL_DECORATION_STAMP_SERIES:
        series_id = series["series_id"]
        DECORATION_STAMP_SERIES_UI_METADATA[series_id] = {
            "label_key": series["series_label_key"],
            "description_key": series["series_description_key"],
        }

        variant_number = 1
        for profile_id in series["profile_ids"]:
            values = {"variant_number": variant_number}
            DECORATION_STAMP_UI_METADATA[profile_id] = {
                "series_id": series_id,
                "label_key": series["profile_label_key"],
                "label_values": values,
                "description_key": series["profile_description_key"],
                "description_values": values,
            }
            variant_number += 1


_register_additional_decoration_stamp_series()


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
        label_values = metadata.get("label_values", {})
        description_values = metadata.get("description_values", {})
        item["label"] = tr(metadata["label_key"], **label_values)
        item["description"] = tr(metadata["description_key"], **description_values)
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


def get_decoration_stamp_series_id(profile_id: str) -> str:
    """
    Return the series id owning one stamp profile.
    """
    return _get_stamp_ui_metadata(profile_id)["series_id"]


def get_decoration_stamp_variants(profile_id: str) -> list:
    """
    Return every stamp sharing a series with the given profile, in DLL order.
    """
    series_id = get_decoration_stamp_series_id(profile_id)

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
