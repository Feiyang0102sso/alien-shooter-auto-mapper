"""
PySide6 application entry point.
"""
import sys
from pathlib import Path

from PySide6.QtWidgets import QApplication

from app.config import init_app_env
from app.i18n.locale import load_locale_preference, tr
from app.i18n.text_keys import TextKey
from app.logger import logger


STYLE_PATH = Path(__file__).resolve().parent / "ui" / "styles" / "dark_theme.qss"


def load_style(app: QApplication) -> None:
    """
    Load the application QSS file.
    """
    if not STYLE_PATH.exists():
        logger.warning(f"Style file not found: {STYLE_PATH}")
        return

    style_text = STYLE_PATH.read_text(encoding="utf-8")
    app.setStyleSheet(style_text)
    logger.debug(f"Loaded QSS style: {STYLE_PATH}")


def main() -> int:
    """
    Start the Auto Mapper PySide6 UI.
    """
    init_app_env()
    load_locale_preference()

    from app.ui.main_window import MainWindow

    app = QApplication(sys.argv)
    app.setApplicationName(tr(TextKey.APP_NAME))
    app.setOrganizationName(tr(TextKey.APP_NAME))

    load_style(app)

    window = MainWindow()
    window.show()

    logger.info("PySide6 UI started")
    return app.exec()


if __name__ == "__main__":
    raise SystemExit(main())
