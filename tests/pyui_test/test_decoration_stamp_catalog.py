"""Tests for the complete AS2 decoration stamp catalog."""

import unittest

from PySide6.QtGui import QImageReader

from app.editor.decoration_stamps import (
    ADDITIONAL_DECORATION_STAMP_SERIES,
    DECORATION_STAMP_SERIES_UI_METADATA,
    DECORATION_STAMP_UI_METADATA,
)
from app.i18n.locale import get_locale, set_locale, tr
from app.ui.previews.decoration_assets import (
    get_decoration_stamp_canvas_path,
    get_decoration_stamp_series_thumbnail_path,
    get_decoration_stamp_thumbnail_path,
)


class DecorationStampCatalogTest(unittest.TestCase):
    """Verify metadata, translations, and preview assets for every new stamp."""

    def test_every_additional_profile_has_metadata_and_preview_assets(self) -> None:
        """All 90 generated profiles should be ready for the shelf and canvas."""
        profile_ids = set()

        for series in ADDITIONAL_DECORATION_STAMP_SERIES:
            series_id = series["series_id"]
            self.assertIn(series_id, DECORATION_STAMP_SERIES_UI_METADATA)
            series_thumbnail_path = get_decoration_stamp_series_thumbnail_path(series_id)
            self.assertTrue(series_thumbnail_path.is_file(), series_thumbnail_path)
            series_thumbnail_size = QImageReader(str(series_thumbnail_path)).size()
            self.assertEqual(series_thumbnail_size.width(), 300, series_thumbnail_path)
            self.assertEqual(series_thumbnail_size.height(), 200, series_thumbnail_path)

            for profile_id in series["profile_ids"]:
                self.assertNotIn(profile_id, profile_ids)
                profile_ids.add(profile_id)

                metadata = DECORATION_STAMP_UI_METADATA[profile_id]
                self.assertEqual(metadata["series_id"], series_id)

                canvas_path = get_decoration_stamp_canvas_path(series_id, profile_id)
                thumbnail_path = get_decoration_stamp_thumbnail_path(series_id, profile_id)
                self.assertTrue(canvas_path.is_file(), canvas_path)
                self.assertTrue(thumbnail_path.is_file(), thumbnail_path)

        self.assertEqual(len(profile_ids), 90)
        self.assertEqual(len(DECORATION_STAMP_UI_METADATA), 116)
        self.assertEqual(len(DECORATION_STAMP_SERIES_UI_METADATA), 17)

    def test_additional_profile_templates_format_in_both_locales(self) -> None:
        """Numbered labels should resolve without leaking format placeholders."""
        original_locale = get_locale()

        try:
            for locale_name in ("en_US", "zh_CN"):
                set_locale(locale_name)

                for series in ADDITIONAL_DECORATION_STAMP_SERIES:
                    first_profile_id = series["profile_ids"][0]
                    metadata = DECORATION_STAMP_UI_METADATA[first_profile_id]
                    label = tr(metadata["label_key"], **metadata["label_values"])
                    description = tr(
                        metadata["description_key"],
                        **metadata["description_values"],
                    )

                    self.assertNotIn("{variant_number", label)
                    self.assertNotIn("{variant_number", description)
                    self.assertIn("01", label)
        finally:
            set_locale(original_locale)


if __name__ == "__main__":
    unittest.main()
