"""Decoration stamp preview asset paths."""

from pathlib import Path

# Preview assets ship inside the app package in both source and packaged builds.
APP_PACKAGE_ROOT = Path(__file__).resolve().parents[2]
# Stamp previews sit beside the wall previews and are named after the profile id.
STAMP_PREVIEW_ROOT = APP_PACKAGE_ROOT / "resources" / "images" / "preview" / "AS2" / "decoration"


def get_decoration_stamp_preview_path(profile_id: str) -> Path:
    """Return the preview image path for one decoration stamp."""
    return STAMP_PREVIEW_ROOT / f"{profile_id}.webp"
