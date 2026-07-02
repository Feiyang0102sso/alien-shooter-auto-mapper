"""Simple JSON-backed UI text translation."""

import json
from json import JSONDecodeError
from pathlib import Path

from app.config import ROOT_DIR
from app.logger import logger


DEFAULT_LOCALE = "en_US"
LOCALE_EN_US = "en_US"
LOCALE_ZH_CN = "zh_CN"
LOCALE_DIR = Path(__file__).resolve().parent / "locales"
LOCALE_SETTINGS_PATH = ROOT_DIR / "ui_settings.json"
LOCALE_SETTINGS_KEY = "locale"

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
    locale_name = DEFAULT_LOCALE

    if not LOCALE_SETTINGS_PATH.exists():
        set_locale(locale_name)
        return locale_name

    try:
        settings_text = LOCALE_SETTINGS_PATH.read_text(encoding="utf-8")
        settings = json.loads(settings_text)
    except (OSError, JSONDecodeError) as error:
        logger.warning(f"Failed to load UI settings: {error}")
        set_locale(locale_name)
        return locale_name

    saved_locale = settings.get(LOCALE_SETTINGS_KEY)
    if isinstance(saved_locale, str):
        locale_name = saved_locale

    set_locale(locale_name)
    return get_locale()


def save_locale_preference(locale_name: str) -> None:
    """Save the UI locale for the next application start."""
    if locale_name not in _get_supported_locales():
        logger.warning(f"Unsupported locale preference: {locale_name}")
        return

    settings = {
        LOCALE_SETTINGS_KEY: locale_name,
    }
    settings_text = json.dumps(settings, indent=2)
    LOCALE_SETTINGS_PATH.write_text(settings_text, encoding="utf-8")
    logger.info(f"Saved UI locale preference: {locale_name}")


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
