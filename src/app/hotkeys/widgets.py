"""Small widgets used to present hotkey labels consistently."""

from PySide6.QtCore import Qt
from PySide6.QtWidgets import QCheckBox, QStyle, QStyleOptionButton, QStylePainter


class HotkeyCheckBox(QCheckBox):
    """Center a two-line label while preserving native checkbox behavior."""

    def paintEvent(self, event) -> None:
        """Draw the indicator normally and center the label in its text area."""
        del event
        option = QStyleOptionButton()
        self.initStyleOption(option)
        label_text = option.text
        option.text = ""

        painter = QStylePainter(self)
        painter.drawControl(QStyle.CE_CheckBox, option)

        text_rect = self.style().subElementRect(
            QStyle.SE_CheckBoxContents,
            option,
            self,
        )
        painter.setPen(option.palette.windowText().color())
        painter.drawText(
            text_rect,
            Qt.AlignCenter | Qt.TextShowMnemonic,
            label_text,
        )
