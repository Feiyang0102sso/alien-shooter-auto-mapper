"""
Drawable part options for each wall set.
"""

from app.editor.wall_profiles import get_drawable_parts as get_dll_drawable_parts


PART_WALL_BODY = "wall_body"
AS2_SET1_WALL_TYPES = {3, 4, 5}


def get_drawable_parts(wall_type: int) -> list:
    """
    Return DLL-exported drawable parts for a wall set.
    """
    return get_dll_drawable_parts(wall_type)


def get_wall_set_drawable_parts(wall_type: int) -> list:
    """
    Return the drawable choices represented by one wall set card.

    AS2 Wall Set 1 combines three internal wall profiles in one UI wall set.
    """
    drawable_parts = get_drawable_parts(wall_type)
    if wall_type not in AS2_SET1_WALL_TYPES:
        items = []
        for part_id, label in drawable_parts:
            items.append((wall_type, part_id, label))
        return items

    wall_body_label = PART_WALL_BODY
    for part_id, label in drawable_parts:
        if part_id == PART_WALL_BODY:
            wall_body_label = label
            break

    # Set1 variants: three wall body choices that switch the active wall type.
    items = []
    variant_index = 1
    for variant_wall_type in sorted(AS2_SET1_WALL_TYPES):
        variant_label = f"{wall_body_label} {variant_index}"
        items.append((variant_wall_type, PART_WALL_BODY, variant_label))
        variant_index += 1

    # Append door parts from DLL (as2_door_closed / as2_door_open).
    # Use the canonical Set1 wall type for all door entries so the door
    # auto-adapts to whichever variant is active via wall_type on the segment.
    for part_id, label in drawable_parts:
        if part_id == PART_WALL_BODY:
            continue
        items.append((wall_type, part_id, label))

    return items
