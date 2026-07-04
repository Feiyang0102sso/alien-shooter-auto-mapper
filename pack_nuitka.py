"""
Build the AutoMapper desktop app with Nuitka.
"""

import shutil
import subprocess
import sys
from pathlib import Path


APP_PROCESS_NAME = "AutoMapper.exe"
APP_OUTPUT_NAME = "AutoMapper"
BUILD_DIR = Path("build") / "mingw-release"
DIST_DIR = Path("dist")
DLL_SOURCE = BUILD_DIR / "AutoMapper.dll"
DLL_TARGET = DIST_DIR / "AutoMapper.dll"
VERSION_FILE = Path("src") / "app" / "version.py"
ENTRY_FILE = Path("src") / "app" / "main.py"
ICON_FILE = Path("assets") / "logo.ico"
STYLE_SOURCE = Path("src") / "app" / "ui" / "styles" / "dark_theme.qss"
STYLE_TARGET = "ui/styles/dark_theme.qss"


def main() -> int:
    """
    Run a clean Nuitka onefile build.
    """
    print("=========================================")
    print(" Alien Shooter Auto Mapper - Nuitka Pack ")
    print("=========================================")

    stop_old_app_process()

    version = read_version()
    if version is None:
        return 1

    print(f"[Prep] Detected app version: {version}")
    command = build_nuitka_command(version)

    print("[Nuitka] Compiling Python code...")
    result = subprocess.run(command, check=False)
    if result.returncode != 0:
        print(f"[Nuitka] Error: compilation failed with code {result.returncode}")
        return result.returncode

    copy_runtime_dll()
    print("[Finish] Nuitka packaging completed successfully.")
    return 0


def stop_old_app_process() -> None:
    """
    Kill the old app process so Nuitka can overwrite the output EXE.
    """
    command = ["taskkill", "/f", "/im", APP_PROCESS_NAME]
    result = subprocess.run(command, check=False, capture_output=True, text=True)
    if result.returncode == 0:
        print(f"[Prep] Stopped old process: {APP_PROCESS_NAME}")


def read_version() -> str | None:
    """
    Read __version__ from the app version module.
    """
    if not VERSION_FILE.exists():
        print(f"[Prep] Error: version file not found: {VERSION_FILE}")
        return None

    version_globals = {}
    version_text = VERSION_FILE.read_text(encoding="utf-8")
    exec(version_text, version_globals)
    return version_globals.get("__version__", "0.0.0")


def build_nuitka_command(version: str) -> list[str]:
    """
    Build the Nuitka command line.
    """
    command = [
        sys.executable,
        "-m",
        "nuitka",
        "--experimental=use_pefile",
        "--onefile",
        "--windows-console-mode=disable",
        "--plugin-enable=pyside6",
        "--include-qt-plugins=platforms,styles",
        "--nofollow-import-to=pytest,unittest,test,scripts",
        "--include-package-data=app",
        f"--include-data-files={STYLE_SOURCE}={STYLE_TARGET}",
        f"--windows-icon-from-ico={ICON_FILE}",
        "--company-name=Feiyang",
        "--product-name=AutoMapper",
        f"--file-version={version}",
        f"--product-version={version}",
        "--file-description=Auto Mapper UI",
        "--copyright=Copyright (c) 2026 Feiyang. All rights reserved.",
        f"--output-dir={DIST_DIR}",
        f"--output-filename={APP_OUTPUT_NAME}",
        str(ENTRY_FILE),
    ]
    return command


def copy_runtime_dll() -> None:
    """
    Copy the C++ runtime DLL next to the generated EXE.
    """
    print("[Post-Build] Copying C++ library...")
    if not DLL_SOURCE.exists():
        print(f"[Post-Build] Warning: DLL not found: {DLL_SOURCE}")
        return

    DIST_DIR.mkdir(parents=True, exist_ok=True)
    shutil.copy2(DLL_SOURCE, DLL_TARGET)
    print(f"[Post-Build] Copied DLL to: {DLL_TARGET}")


if __name__ == "__main__":
    raise SystemExit(main())
