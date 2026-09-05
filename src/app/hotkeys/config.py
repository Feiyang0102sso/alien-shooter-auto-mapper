"""Single source of truth for editor hotkeys and their help text."""

from dataclasses import dataclass

from app.i18n.locale import LOCALE_EN_US, LOCALE_ZH_CN, get_locale


PAGE_FUNCTIONS = "functions"
PAGE_NAVIGATION = "navigation"
PAGE_TOOLS = "tools"
DIALOG_PREVIOUS_PAGE_KEY = "Left"
DIALOG_NEXT_PAGE_KEY = "Right"

COMMAND_NEW = "new"
COMMAND_IMPORT_JSON = "import_json"
COMMAND_EXPORT_JSON = "export_json"
COMMAND_GENERATE_MAP = "generate_map"
COMMAND_FLOOR = "floor"
COMMAND_CEILING = "ceiling"
COMMAND_DOOR_STATE = "door_state"
COMMAND_RANDOM_DIRECTION = "random_direction"
COMMAND_HOTKEY_HELP = "hotkey_help"
COMMAND_LANGUAGE = "language"
COMMAND_RESERVED_F11 = "reserved_f11"
COMMAND_RESERVED_F12 = "reserved_f12"
COMMAND_PREVIOUS_SET = "previous_set"
COMMAND_NEXT_SET = "next_set"
COMMAND_PREVIOUS_ITEM = "previous_item"
COMMAND_NEXT_ITEM = "next_item"
COMMAND_SELECT = "select"
COMMAND_POLYLINE = "polyline"
COMMAND_STRAIGHT_LINE = "straight_line"
COMMAND_RECTANGLE = "rectangle"
COMMAND_ERASER = "eraser"
COMMAND_RESERVED_5 = "reserved_5"
COMMAND_RESERVED_6 = "reserved_6"
COMMAND_RESERVED_7 = "reserved_7"
COMMAND_RESERVED_8 = "reserved_8"
COMMAND_RESERVED_9 = "reserved_9"
COMMAND_DELETE_DECORATION = "delete_decoration"


@dataclass(frozen=True)
class HotkeyBinding:
    """One configured command and every key sequence that triggers it."""

    command_id: str
    display_key: str
    key_sequences: tuple[str, ...]
    help_page: str
    enabled: bool = True


# Keep this tuple in display order. The help dialog renders each page by walking
# this same data, so documentation and runtime bindings cannot drift apart.
HOTKEY_BINDINGS = (
    HotkeyBinding(COMMAND_NEW, "F1", ("F1",), PAGE_FUNCTIONS),
    HotkeyBinding(COMMAND_IMPORT_JSON, "F2", ("F2",), PAGE_FUNCTIONS),
    HotkeyBinding(COMMAND_EXPORT_JSON, "F3", ("F3",), PAGE_FUNCTIONS),
    HotkeyBinding(COMMAND_GENERATE_MAP, "F4", ("F4",), PAGE_FUNCTIONS),
    HotkeyBinding(COMMAND_FLOOR, "F5", ("F5",), PAGE_FUNCTIONS),
    HotkeyBinding(COMMAND_CEILING, "F6", ("F6",), PAGE_FUNCTIONS),
    HotkeyBinding(COMMAND_DOOR_STATE, "F7", ("F7",), PAGE_FUNCTIONS),
    HotkeyBinding(COMMAND_RANDOM_DIRECTION, "F8", ("F8",), PAGE_FUNCTIONS),
    HotkeyBinding(COMMAND_HOTKEY_HELP, "F9", ("F9",), PAGE_FUNCTIONS),
    HotkeyBinding(COMMAND_LANGUAGE, "F10", ("F10",), PAGE_FUNCTIONS),
    HotkeyBinding(COMMAND_RESERVED_F11, "F11", ("F11",), PAGE_FUNCTIONS, False),
    HotkeyBinding(COMMAND_RESERVED_F12, "F12", ("F12",), PAGE_FUNCTIONS, False),
    HotkeyBinding(COMMAND_PREVIOUS_SET, "Q", ("Q",), PAGE_NAVIGATION),
    HotkeyBinding(COMMAND_NEXT_SET, "E", ("E",), PAGE_NAVIGATION),
    HotkeyBinding(COMMAND_PREVIOUS_ITEM, "-", ("-", "Num+-"), PAGE_NAVIGATION),
    HotkeyBinding(COMMAND_NEXT_ITEM, "+", ("Shift+=", "Num++"), PAGE_NAVIGATION),
    HotkeyBinding(COMMAND_SELECT, "0", ("0",), PAGE_TOOLS),
    HotkeyBinding(COMMAND_POLYLINE, "1", ("1",), PAGE_TOOLS),
    HotkeyBinding(COMMAND_STRAIGHT_LINE, "2", ("2",), PAGE_TOOLS),
    HotkeyBinding(COMMAND_RECTANGLE, "3", ("3",), PAGE_TOOLS),
    HotkeyBinding(COMMAND_ERASER, "4", ("4",), PAGE_TOOLS),
    HotkeyBinding(COMMAND_RESERVED_5, "5", ("5",), PAGE_TOOLS, False),
    HotkeyBinding(COMMAND_RESERVED_6, "6", ("6",), PAGE_TOOLS, False),
    HotkeyBinding(COMMAND_RESERVED_7, "7", ("7",), PAGE_TOOLS, False),
    HotkeyBinding(COMMAND_RESERVED_8, "8", ("8",), PAGE_TOOLS, False),
    HotkeyBinding(COMMAND_RESERVED_9, "9", ("9",), PAGE_TOOLS, False),
    HotkeyBinding(COMMAND_DELETE_DECORATION, "Del", ("Delete",), PAGE_TOOLS),
)


HELP_PAGE_ORDER = (
    PAGE_FUNCTIONS,
    PAGE_NAVIGATION,
    PAGE_TOOLS,
)


HOTKEY_TEXT = {
    LOCALE_EN_US: {
        "button": "Shortcuts",
        "tooltip": "Show keyboard shortcuts",
        "dialog_title": "Keyboard Shortcuts",
        "page.functions": "Top Toolbar",
        "page.navigation": "Set Navigation",
        "page.tools": "Tools and Editing",
        "previous_page": "Previous page",
        "next_page": "Next page",
        COMMAND_NEW: "New",
        COMMAND_IMPORT_JSON: "Import JSON",
        COMMAND_EXPORT_JSON: "Export JSON",
        COMMAND_GENERATE_MAP: "Generate Map",
        COMMAND_FLOOR: "Floor",
        COMMAND_CEILING: "Ceiling",
        COMMAND_DOOR_STATE: "Door State",
        COMMAND_RANDOM_DIRECTION: "Random Direction",
        COMMAND_HOTKEY_HELP: "Keyboard Shortcuts",
        COMMAND_LANGUAGE: "Language",
        COMMAND_RESERVED_F11: "Reserved",
        COMMAND_RESERVED_F12: "Reserved",
        COMMAND_PREVIOUS_SET: "Previous Set",
        COMMAND_NEXT_SET: "Next Set",
        COMMAND_PREVIOUS_ITEM: "Previous Set Item",
        COMMAND_NEXT_ITEM: "Next Set Item",
        COMMAND_SELECT: "Select Decorations",
        COMMAND_POLYLINE: "Polyline",
        COMMAND_STRAIGHT_LINE: "Straight Line",
        COMMAND_RECTANGLE: "Rectangle",
        COMMAND_ERASER: "Eraser",
        COMMAND_RESERVED_5: "Reserved",
        COMMAND_RESERVED_6: "Reserved",
        COMMAND_RESERVED_7: "Reserved",
        COMMAND_RESERVED_8: "Reserved",
        COMMAND_RESERVED_9: "Reserved",
        COMMAND_DELETE_DECORATION: "Delete Selected Decoration",
    },
    LOCALE_ZH_CN: {
        "button": "快捷键",
        "tooltip": "显示键盘快捷键",
        "dialog_title": "键盘快捷键",
        "page.functions": "顶部功能",
        "page.navigation": "Set 导航",
        "page.tools": "工具与编辑",
        "previous_page": "上一页",
        "next_page": "下一页",
        COMMAND_NEW: "新建",
        COMMAND_IMPORT_JSON: "导入 JSON",
        COMMAND_EXPORT_JSON: "导出 JSON",
        COMMAND_GENERATE_MAP: "生成地图",
        COMMAND_FLOOR: "地板",
        COMMAND_CEILING: "天花板",
        COMMAND_DOOR_STATE: "门状态",
        COMMAND_RANDOM_DIRECTION: "随机方向",
        COMMAND_HOTKEY_HELP: "快捷键帮助",
        COMMAND_LANGUAGE: "语言",
        COMMAND_RESERVED_F11: "保留",
        COMMAND_RESERVED_F12: "保留",
        COMMAND_PREVIOUS_SET: "上一个 Set",
        COMMAND_NEXT_SET: "下一个 Set",
        COMMAND_PREVIOUS_ITEM: "上一个 Set 内项目",
        COMMAND_NEXT_ITEM: "下一个 Set 内项目",
        COMMAND_SELECT: "选择装饰物",
        COMMAND_POLYLINE: "折线",
        COMMAND_STRAIGHT_LINE: "直线",
        COMMAND_RECTANGLE: "矩形",
        COMMAND_ERASER: "橡皮擦",
        COMMAND_RESERVED_5: "保留",
        COMMAND_RESERVED_6: "保留",
        COMMAND_RESERVED_7: "保留",
        COMMAND_RESERVED_8: "保留",
        COMMAND_RESERVED_9: "保留",
        COMMAND_DELETE_DECORATION: "删除选中的装饰物",
    },
}


def get_hotkey_binding(command_id: str) -> HotkeyBinding:
    """Return one configured binding or raise for an unknown command."""
    for binding in HOTKEY_BINDINGS:
        if binding.command_id == command_id:
            return binding

    raise KeyError(f"Unknown hotkey command: {command_id}")


def get_hotkey_text(text_id: str) -> str:
    """Return hotkey-owned text in the active application language."""
    locale_name = get_locale()
    locale_text = HOTKEY_TEXT.get(locale_name)
    if locale_text is None:
        locale_text = HOTKEY_TEXT[LOCALE_EN_US]

    text = locale_text.get(text_id)
    if text is not None:
        return text

    return HOTKEY_TEXT[LOCALE_EN_US][text_id]
