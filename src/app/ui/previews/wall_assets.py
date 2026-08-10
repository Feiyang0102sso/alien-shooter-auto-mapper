"""Wall set and drawable component preview asset paths."""

from pathlib import Path

# Preview assets ship inside the app package in both source and packaged builds.
APP_PACKAGE_ROOT = Path(__file__).resolve().parents[2]
PREVIEW_ROOT = APP_PACKAGE_ROOT / "resources" / "images" / "preview"
AS1_PREVIEW_ROOT = PREVIEW_ROOT / "AS1"
AS2_PREVIEW_ROOT = PREVIEW_ROOT / "AS2"

# One representative image for each wall set card.
WALL_SET_PREVIEW_PATHS = {
    0: AS1_PREVIEW_ROOT / "standard" / "standard_wall.webp",
    1: AS1_PREVIEW_ROOT / "lab" / "lab_wall.webp",
    2: AS1_PREVIEW_ROOT / "standard_dark" / "standard_wall_dark.webp",
    3: AS2_PREVIEW_ROOT / "wall_set1" / "wall_set1_1.webp",
    4: AS2_PREVIEW_ROOT / "wall_set1" / "wall_set1_2.webp",
    5: AS2_PREVIEW_ROOT / "wall_set1" / "wall_set1_3.webp",
    6: AS2_PREVIEW_ROOT / "wall_set2" / "wall_set2.webp",
    7: AS2_PREVIEW_ROOT / "wall_set3" / "wall_set3.webp",
    8: AS2_PREVIEW_ROOT / "wall_set4" / "wall_set4.webp",
    9: AS2_PREVIEW_ROOT / "wall_set5" / "wall_set5.webp",
    10: AS2_PREVIEW_ROOT / "wall_set6" / "wall_set6.webp",
    11: AS2_PREVIEW_ROOT / "wall_set7" / "wall_set7.webp",
    12: AS2_PREVIEW_ROOT / "wall_set8" / "wall_set8.webp",
    13: AS2_PREVIEW_ROOT / "wall_set9" / "wall_set9.webp",
}

# Component previews use the same part IDs exported by the DLL-backed wall profiles.
WALL_COMPONENT_PREVIEW_PATHS = {
    (0, "wall_body"): AS1_PREVIEW_ROOT / "standard" / "standard_wall.webp",
    (0, "active_door"): AS1_PREVIEW_ROOT / "standard" / "standard_wall_door_active.webp",
    (0, "dead_door_closed"): AS1_PREVIEW_ROOT / "standard" / "standard_wall_door_dead_closed.webp",
    (0, "dead_door_jammed"): AS1_PREVIEW_ROOT / "standard" / "standard_wall_door_dead_jammed.webp",
    (0, "dead_door_open"): AS1_PREVIEW_ROOT / "standard" / "standard_wall_door_dead_open.webp",
    (1, "wall_body"): AS1_PREVIEW_ROOT / "lab" / "lab_wall.webp",
    (1, "lab_laser_closed"): AS1_PREVIEW_ROOT / "lab" / "lab_wall_door_laser_on.webp",
    (1, "lab_laser_open"): AS1_PREVIEW_ROOT / "lab" / "lab_wall_door_laser_off.webp",
    (1, "lab_decoration_door"): AS1_PREVIEW_ROOT / "lab" / "lab_wall_door_decoration.webp",
    (2, "wall_body"): AS1_PREVIEW_ROOT / "standard_dark" / "standard_wall_dark.webp",
    (2, "active_door"): AS1_PREVIEW_ROOT / "standard_dark" / "standard_wall_dark_door_active.webp",
    (2, "dead_door_closed"): AS1_PREVIEW_ROOT / "standard_dark" / "standard_wall_dark_door_dead_closed.webp",
    (2, "dead_door_jammed"): AS1_PREVIEW_ROOT / "standard_dark" / "standard_wall_dark_door_dead_jammed.webp",
    (2, "dead_door_open"): AS1_PREVIEW_ROOT / "standard_dark" / "standard_wall_dark_door_dead_open.webp",
    (3, "wall_body"): AS2_PREVIEW_ROOT / "wall_set1" / "wall_set1_1.webp",
    (4, "wall_body"): AS2_PREVIEW_ROOT / "wall_set1" / "wall_set1_2.webp",
    (5, "wall_body"): AS2_PREVIEW_ROOT / "wall_set1" / "wall_set1_3.webp",
    (3, "as2_door_closed"): AS2_PREVIEW_ROOT / "wall_set1" / "wall_set1_door_close.webp",
    (4, "as2_door_closed"): AS2_PREVIEW_ROOT / "wall_set1" / "wall_set1_door_close.webp",
    (5, "as2_door_closed"): AS2_PREVIEW_ROOT / "wall_set1" / "wall_set1_door_close.webp",
    (3, "as2_door_open"): AS2_PREVIEW_ROOT / "wall_set1" / "wall_set1_door_open.webp",
    (4, "as2_door_open"): AS2_PREVIEW_ROOT / "wall_set1" / "wall_set1_door_open.webp",
    (5, "as2_door_open"): AS2_PREVIEW_ROOT / "wall_set1" / "wall_set1_door_open.webp",
    (6, "wall_body"): AS2_PREVIEW_ROOT / "wall_set2" / "wall_set2.webp",
    (6, "as2_door_closed"): AS2_PREVIEW_ROOT / "wall_set2" / "wall_set2_door_close.webp",
    (6, "as2_door_open"): AS2_PREVIEW_ROOT / "wall_set2" / "wall_set2_door_open.webp",
    (7, "wall_body"): AS2_PREVIEW_ROOT / "wall_set3" / "wall_set3.webp",
    (7, "as2_door_closed"): AS2_PREVIEW_ROOT / "wall_set3" / "wall_set3_door_close.webp",
    (7, "as2_door_open"): AS2_PREVIEW_ROOT / "wall_set3" / "wall_set3_door_open.webp",
    (8, "wall_body"): AS2_PREVIEW_ROOT / "wall_set4" / "wall_set4.webp",
    (8, "as2_door_closed"): AS2_PREVIEW_ROOT / "wall_set4" / "wall_set4_door_close.webp",
    (8, "as2_door_open"): AS2_PREVIEW_ROOT / "wall_set4" / "wall_set4_door_open.webp",
    (9, "wall_body"): AS2_PREVIEW_ROOT / "wall_set5" / "wall_set5.webp",
    (9, "as2_door_closed"): AS2_PREVIEW_ROOT / "wall_set5" / "wall_set5_door_close.webp",
    (9, "as2_door_open"): AS2_PREVIEW_ROOT / "wall_set5" / "wall_set5_door_open.webp",
    (10, "wall_body"): AS2_PREVIEW_ROOT / "wall_set6" / "wall_set6.webp",
    (10, "as2_door_closed"): AS2_PREVIEW_ROOT / "wall_set6" / "wall_set6_door_close.webp",
    (10, "as2_door_open"): AS2_PREVIEW_ROOT / "wall_set6" / "wall_set6_door_open.webp",
    (11, "wall_body"): AS2_PREVIEW_ROOT / "wall_set7" / "wall_set7.webp",
    (11, "as2_door_closed"): AS2_PREVIEW_ROOT / "wall_set7" / "wall_set7_door_close.webp",
    (11, "as2_door_open"): AS2_PREVIEW_ROOT / "wall_set7" / "wall_set7_door_open.webp",
    (12, "wall_body"): AS2_PREVIEW_ROOT / "wall_set8" / "wall_set8.webp",
    (12, "as2_door_closed"): AS2_PREVIEW_ROOT / "wall_set8" / "wall_set8_door_close.webp",
    (12, "as2_door_open"): AS2_PREVIEW_ROOT / "wall_set8" / "wall_set8_door_open.webp",
    (13, "wall_body"): AS2_PREVIEW_ROOT / "wall_set9" / "wall_set9.webp",
    (13, "as2_door_closed"): AS2_PREVIEW_ROOT / "wall_set9" / "wall_set9_door_close.webp",
    (13, "as2_door_open"): AS2_PREVIEW_ROOT / "wall_set9" / "wall_set9_door_open.webp",
}


def get_wall_set_preview_path(wall_type: int) -> Path | None:
    """Return the representative preview path for one wall set."""
    return WALL_SET_PREVIEW_PATHS.get(wall_type)


def get_wall_component_preview_path(wall_type: int, part_id: str) -> Path | None:
    """Return the preview path for one drawable wall component."""
    lookup_key = (wall_type, part_id)
    return WALL_COMPONENT_PREVIEW_PATHS.get(lookup_key)
