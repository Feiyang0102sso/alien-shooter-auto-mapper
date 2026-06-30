"""
src/app/config.py
universal config for the exe and relevant paths
"""
import sys
from pathlib import Path

from .logger import logger, add_file_handler


def get_app_root() -> Path:
    """
    Get application root directory.

    - bundled EXE
    - python scripts (python main.py ... or python -m package.main ...)
    - CLI Wrapper / Shim (pip install e . then enter package name in console) ** recommended **

    Returns:
        Path: application root directory
    """

    # If running as a bundled EXE, get parent dir
    if getattr(sys, 'frozen', False):
        logger.debug(f"currently running as an EXE")
        return Path(sys.executable).parent.resolve()

    # If running as a script, get dir of the main script
    import __main__
    if hasattr(__main__, "__file__"):
        main_file = Path(__main__.__file__)
        # Check if the parent is an actual directory to prevent FileExistsError!
        if main_file.parent.is_dir():
            logger.debug(f"currently running as a python script")
            return main_file.parent.resolve()

    #If running as CLI Wrapper / Shim
    logger.debug(f"currently running as a CLI Wrapper / Shim")
    return Path.cwd().resolve()


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
    return app_root


# === Path Configuration ===
ROOT_DIR = get_app_root()
RESOURCE_ROOT = get_resource_root(ROOT_DIR)
LOG_FILE_NAME = "auto_mapper.log"
LOG_FILE_PATH = ROOT_DIR / LOG_FILE_NAME

input_dir = ROOT_DIR / 'DATAS' # not used in this project
output_dir = ROOT_DIR / 'OUTPUT' # not used in this project

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
    logger.debug(f"Root Path: {ROOT_DIR}")
    logger.debug(f"Log File Path: {LOG_FILE_PATH}")

# --- Initialize on Import ---
# to avoid import problem, no longer used
# init_app_env()
