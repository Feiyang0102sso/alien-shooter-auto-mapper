"""
src/app/config.py
universal config for the exe and relevant paths
"""
import sys
import __main__
import os
from pathlib import Path

try:
    from app.logger import logger, add_file_handler
except ModuleNotFoundError:
    from logger import logger, add_file_handler


def get_app_root() -> Path:
    """
    Get application root directory.

    - bundled EXE / Nuitka onefile
    - python scripts (python main.py ... or python -m package.main ...)
    - CLI Wrapper / Shim (python -m pip install -e . then enter package name in console)

    Returns:
        Path: application root directory
    """
    if is_packaged_app():
        return Path(sys.argv[0]).parent.resolve()

    if hasattr(__main__, "__file__"):
        main_file = Path(__main__.__file__)
        if main_file.parent.is_dir():
            return main_file.parent.resolve()

    return Path.cwd().resolve()


def is_packaged_app() -> bool:
    """
    Return whether the app is running from a built executable.
    """
    if getattr(sys, "frozen", False):
        return True

    if os.environ.get("NUITKA_ONEFILE_PARENT"):
        return True

    return False


def get_runtime_mode_message() -> str:
    """
    Return a short runtime mode message for logging.
    """
    if is_packaged_app():
        return "currently running as a packaged EXE"

    if hasattr(__main__, "__file__"):
        main_file = Path(__main__.__file__)
        if main_file.parent.is_dir():
            return "currently running as a python script"

    return "currently running as a CLI Wrapper / Shim"


def get_resource_root(app_root: Path) -> Path:
    """
    Get resources root directory.

    - bundled EXE, built-in static resources
        Auto extract to temporary dir (sys._MEIPASS), get this path.
    - python scripts, no sys._MEIPASS
        get actual root dir of current project.

    Returns:
        Path: resource data directory
        """
    meipass = getattr(sys, "_MEIPASS", None)
    if meipass:
        return Path(meipass).resolve()

    if is_packaged_app():
        if hasattr(__main__, "__file__"):
            main_file = Path(__main__.__file__).resolve()
            return main_file.parent

        return app_root

    return app_root


# === Path Configuration ===
ROOT_DIR = get_app_root()
RESOURCE_ROOT = get_resource_root(ROOT_DIR)
LOG_FILE_NAME = "AutoMapper.log"
LOG_FILE_PATH = ROOT_DIR / LOG_FILE_NAME


def get_app_resource_dir() -> Path:
    """
    Return the app resource directory for dev and packaged modes.
    """
    packaged_resource_dir = RESOURCE_ROOT / "app" / "resources"
    project_resource_dir = RESOURCE_ROOT / "src" / "app" / "resources"
    script_resource_dir = RESOURCE_ROOT / "resources"

    candidate_dirs = [
        packaged_resource_dir,
        project_resource_dir,
        script_resource_dir,
    ]

    for candidate_dir in candidate_dirs:
        if candidate_dir.exists():
            return candidate_dir

    if is_packaged_app():
        return packaged_resource_dir

    return project_resource_dir


APP_RESOURCE_DIR = get_app_resource_dir()

# --- DLL Path ---
DLL_NAME_EXE = "AutoMapper.dll"
DLL_NAME_DEV = "libauto_mapper.dll"
DLL_DEV_BUILD_DIR = ROOT_DIR / "build" / "mingw-release"

def get_dll_path() -> Path:
    """
    Get DLL path based on runtime environment.

    - Frozen EXE: ROOT_DIR / AutoMapper.dll
    - Dev mode: build/mingw-release / libauto_mapper.dll
    """
    if is_packaged_app():
        return ROOT_DIR / DLL_NAME_EXE

    return DLL_DEV_BUILD_DIR / DLL_NAME_DEV


DLL_PATH = get_dll_path()
input_dir = ROOT_DIR / "DATAS"  # not used in this project
output_dir = ROOT_DIR / "OUTPUT"  # not used in this project

# --- Directory Helper Methods ---
# not used in this project
def get_input_dir() -> Path:
    """
    Verify and return the source assets directory.
    """
    if not input_dir.exists():
        logger.warning(f"Assets directory not found at: {input_dir}")
    return input_dir

# not used in this project
def get_output_dir(create_if_missing: bool = True) -> Path:
    """
    Return the output directory, creating it if required.
    """
    if create_if_missing:
        output_dir.mkdir(parents=True, exist_ok=True)
    return output_dir

# intend to accept user input dirs
# no longer need to update path in this project
def update_paths(new_input: str = None, new_output: str = None):
    """
    Update global paths and re-initialize the log file.
    """
    global input_dir, output_dir

    if new_input:
        input_dir = Path(new_input).resolve()
    if new_output:
        output_dir = Path(new_output).resolve()

    # logger.debug(f"Paths updated - Input: {input_dir}, Output: {output_dir}")

def init_app_env():
    """
    Initial bootstrap with default paths.
    """
    # update_paths() # Reuses the logic to setup folders and logging

    add_file_handler(LOG_FILE_PATH)
    logger.debug(get_runtime_mode_message())
    logger.debug(f"Root Path: {ROOT_DIR}")
    logger.debug(f"Log File Path: {LOG_FILE_PATH}")
    logger.debug(f"DLL Path: {DLL_PATH}")

# --- Initialize on Import ---
# to avoid import problem, no longer used
# init_app_env()
