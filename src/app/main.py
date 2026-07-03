"""
PySide6 application entry point.
"""
import sys
from pathlib import Path

from PySide6.QtWidgets import QApplication, QDialog, QDialogButtonBox, QLabel, QRadioButton, QVBoxLayout

from app.config import init_app_env
from app.i18n.locale import (
    LOCALE_EN_US,
    LOCALE_ZH_CN,
    is_language_initialization_required,
    load_locale_preference,
    save_locale_preference,
    set_locale,
    tr,
)
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


def choose_initial_language() -> str:
    """
    Ask for the first startup language.
    """
    dialog = QDialog()
    dialog.setWindowTitle("Language / 语言")
    dialog.setModal(True)

    layout = QVBoxLayout(dialog)
    layout.setSpacing(10)

    title = QLabel("Choose language / 选择语言")
    layout.addWidget(title)

    english_radio = QRadioButton("English")
    chinese_radio = QRadioButton("中文")
    english_radio.setChecked(True)
    layout.addWidget(english_radio)
    layout.addWidget(chinese_radio)

    buttons = QDialogButtonBox(QDialogButtonBox.Ok)
    buttons.accepted.connect(dialog.accept)
    layout.addWidget(buttons)

    dialog.exec()

    if chinese_radio.isChecked():
        return LOCALE_ZH_CN

    return LOCALE_EN_US


def main() -> int:
    """
    Start the Auto Mapper PySide6 UI.
    """
    init_app_env()
    load_locale_preference()

    app = QApplication(sys.argv)

    if is_language_initialization_required():
        selected_locale = choose_initial_language()
        set_locale(selected_locale)
        save_locale_preference(selected_locale)

    app.setApplicationName(tr(TextKey.APP_NAME))
    app.setOrganizationName(tr(TextKey.APP_NAME))

    load_style(app)

    from app.ui.main_window import MainWindow

    window = MainWindow()
    window.show()

    logger.info("PySide6 UI started")
    return app.exec()


if __name__ == "__main__":
    raise SystemExit(main())
