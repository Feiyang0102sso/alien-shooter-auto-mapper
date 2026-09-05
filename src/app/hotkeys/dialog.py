"""Three-page keyboard shortcut reference dialog."""

from functools import partial

from PySide6.QtCore import Qt
from PySide6.QtGui import QAction, QKeySequence
from PySide6.QtWidgets import (
    QDialog,
    QFrame,
    QGridLayout,
    QHBoxLayout,
    QLabel,
    QPushButton,
    QStackedWidget,
    QVBoxLayout,
    QWidget,
)

from app.hotkeys.config import (
    COMMAND_HOTKEY_HELP,
    DIALOG_NEXT_PAGE_KEY,
    DIALOG_PREVIOUS_PAGE_KEY,
    HELP_PAGE_ORDER,
    HOTKEY_BINDINGS,
    PAGE_FUNCTIONS,
    PAGE_TOOLS,
    get_hotkey_binding,
    get_hotkey_text,
)


DIALOG_WIDTH = 1080
DIALOG_HEIGHT = 430
FUNCTION_PAGE_COLUMNS = 12
TOOL_PAGE_COLUMNS = 10
DEFAULT_PAGE_COLUMNS = 4


class HotkeyHelpDialog(QDialog):
    """Show configured shortcuts without duplicating their key definitions."""

    def __init__(self, parent=None) -> None:
        super().__init__(parent)
        self.setWindowTitle(get_hotkey_text("dialog_title"))
        self.setModal(True)
        self.setFixedSize(DIALOG_WIDTH, DIALOG_HEIGHT)
        self.setObjectName("hotkeyHelpDialog")
        self._dialog_actions = []

        layout = QVBoxLayout(self)
        layout.setContentsMargins(20, 18, 20, 16)
        layout.setSpacing(14)

        self.page_stack = QStackedWidget()
        for page_id in HELP_PAGE_ORDER:
            self.page_stack.addWidget(self._build_page(page_id))
        layout.addWidget(self.page_stack, 1)

        navigation_layout = QHBoxLayout()
        self.previous_button = QPushButton("←")
        self.previous_button.setToolTip(get_hotkey_text("previous_page"))
        self.previous_button.clicked.connect(self.show_previous_page)
        navigation_layout.addWidget(self.previous_button)

        navigation_layout.addStretch(1)
        self.page_indicator = QLabel()
        self.page_indicator.setObjectName("hotkeyPageIndicator")
        self.page_indicator.setAlignment(Qt.AlignCenter)
        navigation_layout.addWidget(self.page_indicator)
        navigation_layout.addStretch(1)

        self.next_button = QPushButton("→")
        self.next_button.setToolTip(get_hotkey_text("next_page"))
        self.next_button.clicked.connect(self.show_next_page)
        navigation_layout.addWidget(self.next_button)
        layout.addLayout(navigation_layout)

        self._add_dialog_action(DIALOG_PREVIOUS_PAGE_KEY, self.show_previous_page)
        self._add_dialog_action(DIALOG_NEXT_PAGE_KEY, self.show_next_page)
        help_binding = get_hotkey_binding(COMMAND_HOTKEY_HELP)
        self._add_dialog_action(help_binding.key_sequences[0], self.reject)

        self._apply_style()
        self._update_navigation()

    def show_previous_page(self) -> None:
        """Move left one page without wrapping."""
        current_index = self.page_stack.currentIndex()
        if current_index <= 0:
            return

        self.page_stack.setCurrentIndex(current_index - 1)
        self._update_navigation()

    def show_next_page(self) -> None:
        """Move right one page without wrapping."""
        current_index = self.page_stack.currentIndex()
        last_index = self.page_stack.count() - 1
        if current_index >= last_index:
            return

        self.page_stack.setCurrentIndex(current_index + 1)
        self._update_navigation()

    def _build_page(self, page_id: str) -> QWidget:
        """Build one keyboard-card page from ordered hotkey config."""
        page = QWidget()
        page_layout = QVBoxLayout(page)
        page_layout.setContentsMargins(0, 0, 0, 0)
        page_layout.setSpacing(14)

        title = QLabel(get_hotkey_text(f"page.{page_id}"))
        title.setObjectName("hotkeyPageTitle")
        title.setAlignment(Qt.AlignCenter)
        page_layout.addWidget(title)

        cards_layout = QGridLayout()
        cards_layout.setSpacing(8)
        column_count = DEFAULT_PAGE_COLUMNS
        if page_id == PAGE_FUNCTIONS:
            column_count = FUNCTION_PAGE_COLUMNS
        if page_id == PAGE_TOOLS:
            column_count = TOOL_PAGE_COLUMNS

        card_index = 0
        for binding in HOTKEY_BINDINGS:
            if binding.help_page != page_id:
                continue

            row = card_index // column_count
            column = card_index % column_count
            cards_layout.addWidget(self._build_key_card(binding), row, column)
            card_index += 1

        page_layout.addLayout(cards_layout)
        page_layout.addStretch(1)
        return page

    def _build_key_card(self, binding) -> QFrame:
        """Build one keycap and its localized command label."""
        card = QFrame()
        card.setObjectName("hotkeyCard")
        if not binding.enabled:
            card.setProperty("reserved", True)

        card_layout = QVBoxLayout(card)
        card_layout.setContentsMargins(6, 8, 6, 8)
        card_layout.setSpacing(7)

        key_label = QLabel(binding.display_key)
        key_label.setObjectName("hotkeyKeyLabel")
        key_label.setAlignment(Qt.AlignCenter)
        card_layout.addWidget(key_label)

        command_label = QLabel(get_hotkey_text(binding.command_id))
        command_label.setObjectName("hotkeyCommandLabel")
        command_label.setAlignment(Qt.AlignCenter)
        command_label.setWordWrap(True)
        card_layout.addWidget(command_label)
        return card

    def _add_dialog_action(self, key_sequence, callback) -> None:
        """Add one dialog-local navigation or close action."""
        action = QAction(self)
        action.setShortcut(QKeySequence(key_sequence))
        action.setShortcutContext(Qt.WidgetWithChildrenShortcut)
        action.triggered.connect(partial(self._run_dialog_callback, callback))
        self.addAction(action)
        self._dialog_actions.append(action)

    def _run_dialog_callback(self, callback, checked: bool = False) -> None:
        """Run a dialog-local action without QAction's checked argument."""
        del checked
        callback()

    def _update_navigation(self) -> None:
        """Update boundary buttons and the current page indicator."""
        current_index = self.page_stack.currentIndex()
        page_count = self.page_stack.count()
        self.previous_button.setEnabled(current_index > 0)
        self.next_button.setEnabled(current_index < page_count - 1)
        self.page_indicator.setText(f"{current_index + 1} / {page_count}")

    def _apply_style(self) -> None:
        """Keep all shortcut-help-specific styling inside the hotkeys package."""
        self.setStyleSheet(
            """
            #hotkeyHelpDialog {
                background: #121b20;
                color: #d9e8e2;
            }
            #hotkeyPageTitle {
                color: #f1f6f3;
                font-size: 20px;
                font-weight: 700;
            }
            #hotkeyCard {
                background: #182329;
                border: 1px solid #36505d;
                border-radius: 8px;
                min-height: 96px;
            }
            #hotkeyCard[reserved="true"] {
                background: #161d21;
                border-color: #2b363c;
            }
            #hotkeyKeyLabel {
                background: #24353e;
                border: 1px solid #4f7fbf;
                border-radius: 5px;
                color: #ffffff;
                font-family: monospace;
                font-size: 15px;
                font-weight: 700;
                padding: 6px 4px;
            }
            #hotkeyCommandLabel {
                color: #b8cbc4;
                font-size: 11px;
            }
            #hotkeyPageIndicator {
                color: #69f0ae;
                font-size: 14px;
                font-weight: 700;
            }
            """
        )
