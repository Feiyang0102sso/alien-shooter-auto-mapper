"""Simple JSON-backed UI text translation."""

import json
from pathlib import Path

from app.config import ROOT_DIR
from app.logger import logger


DEFAULT_LOCALE = "en_US"
LOCALE_EN_US = "en_US"
LOCALE_ZH_CN = "zh_CN"
LOCALE_DIR = Path(__file__).resolve().parent / "locales"
APP_CONFIG_PATH = ROOT_DIR / "AutoMapper.cfg"
LANGUAGE_CONFIG_KEY = "language"
LANGUAGE_UNSET = 0
LANGUAGE_EN_US = 1
LANGUAGE_ZH_CN = 2
LANGUAGE_TO_LOCALE = {
    LANGUAGE_EN_US: LOCALE_EN_US,
    LANGUAGE_ZH_CN: LOCALE_ZH_CN,
}
LOCALE_TO_LANGUAGE = {
    LOCALE_EN_US: LANGUAGE_EN_US,
    LOCALE_ZH_CN: LANGUAGE_ZH_CN,
}

_current_locale = DEFAULT_LOCALE
_text_cache = {}


def set_locale(locale_name: str) -> None:
    """Set the active UI locale."""
    global _current_locale

    if locale_name not in _get_supported_locales():
        logger.warning(f"Unsupported locale: {locale_name}")
        locale_name = DEFAULT_LOCALE

    _current_locale = locale_name
    _load_locale(locale_name)


def get_locale() -> str:
    """Return the active UI locale."""
    return _current_locale


def load_locale_preference() -> str:
    """Load the saved UI locale and make it active."""
    language_value = load_language_config_value()
    locale_name = LANGUAGE_TO_LOCALE.get(language_value)
    if locale_name is None:
        locale_name = DEFAULT_LOCALE
        set_locale(locale_name)
        return locale_name

    set_locale(locale_name)
    return get_locale()


def save_locale_preference(locale_name: str) -> None:
    """Save the UI locale for the next application start."""
    if locale_name not in _get_supported_locales():
        logger.warning(f"Unsupported locale preference: {locale_name}")
        return

    language_value = LOCALE_TO_LANGUAGE[locale_name]
    save_language_config_value(language_value)
    logger.info(f"Saved UI locale preference: {locale_name}")


def is_language_initialization_required() -> bool:
    """Return whether startup should ask the user to choose a language."""
    language_value = load_language_config_value()
    return language_value == LANGUAGE_UNSET


def load_language_config_value() -> int:
    """Read the language value from AutoMapper.cfg."""
    if not APP_CONFIG_PATH.exists():
        save_language_config_value(LANGUAGE_UNSET)
        return LANGUAGE_UNSET

    try:
        config_text = APP_CONFIG_PATH.read_text(encoding="utf-8")
    except OSError as error:
        logger.warning(f"Failed to read app config: {error}")
        return LANGUAGE_UNSET

    for line in config_text.splitlines():
        stripped_line = line.strip()
        if not stripped_line:
            continue
        if stripped_line.startswith("#"):
            continue

        key, separator, value_text = stripped_line.partition("=")
        if not separator:
            continue
        if key.strip() != LANGUAGE_CONFIG_KEY:
            continue

        try:
            return int(value_text.strip())
        except ValueError:
            logger.warning(f"Invalid language config value: {value_text.strip()}")
            return LANGUAGE_UNSET

    save_language_config_value(LANGUAGE_UNSET)
    return LANGUAGE_UNSET


def save_language_config_value(language_value: int) -> None:
    """Write the language value to AutoMapper.cfg."""
    language_line = f"{LANGUAGE_CONFIG_KEY} = {language_value}"
    if not APP_CONFIG_PATH.exists():
        config_lines = [
            "# Auto Mapper configuration",
            "# language: 0 = ask on startup, 1 = English, 2 = Chinese",
            language_line,
            "",
        ]
        config_text = "\n".join(config_lines)
        APP_CONFIG_PATH.write_text(config_text, encoding="utf-8")
        return

    try:
        config_text = APP_CONFIG_PATH.read_text(encoding="utf-8")
    except OSError as error:
        logger.warning(f"Failed to update app config: {error}")
        return

    config_lines = []
    has_language_line = False
    for line in config_text.splitlines():
        key, separator, _ = line.partition("=")
        if separator and key.strip() == LANGUAGE_CONFIG_KEY:
            config_lines.append(language_line)
            has_language_line = True
            continue

        config_lines.append(line)

    if not has_language_line:
        config_lines.append(language_line)

    config_lines.append("")
    config_text = "\n".join(config_lines)
    APP_CONFIG_PATH.write_text(config_text, encoding="utf-8")


def tr(text_key: str, **values: object) -> str:
    """Return translated UI text for the active locale."""
    texts = _load_locale(_current_locale)
    template = texts.get(text_key)

    if template is None:
        logger.warning(f"Missing i18n text key: {text_key}")
        template = text_key

    if not values:
        return template

    return template.format(**values)


def _load_locale(locale_name: str) -> dict:
    """Load one locale JSON file with a small in-memory cache."""
    cached_texts = _text_cache.get(locale_name)
    if cached_texts is not None:
        return cached_texts

    locale_path = LOCALE_DIR / f"{locale_name}.json"
    text = locale_path.read_text(encoding="utf-8")
    texts = json.loads(text)

    _text_cache[locale_name] = texts
    logger.debug(f"Loaded i18n locale: {locale_name}")
    return texts


def _get_supported_locales() -> tuple:
    """Return locales currently shipped with the app."""
    return LOCALE_EN_US, LOCALE_ZH_CN
