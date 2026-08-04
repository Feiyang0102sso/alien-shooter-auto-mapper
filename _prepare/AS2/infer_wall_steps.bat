@echo off
title AS2 Wall Step Inference

set "SCRIPT_DIR=%~dp0"
set "PROJECT_DIR=%SCRIPT_DIR%..\.."
set "PY_SCRIPT=%SCRIPT_DIR%infer_wall_steps.py"
set "VENV_ACTIVATE=%PROJECT_DIR%\.venv\Scripts\activate.bat"

if "%~1" == "" (
    echo Error: Please drag and drop an unpacked .json file or a wall_sets folder.
    echo Example: infer_wall_steps.bat wall_sets\wall_set1\_wall_set_1.json
    pause
    exit /b 1
)

if not exist "%PY_SCRIPT%" (
    echo Error: Could not find "%PY_SCRIPT%".
    pause
    exit /b 1
)

set "INPUT_PATH=%~1"

echo Input: "%INPUT_PATH%"
echo --------------------------------------------------

if exist "%VENV_ACTIVATE%" (
    call "%VENV_ACTIVATE%"
    python "%PY_SCRIPT%" %*
) else (
    python "%PY_SCRIPT%" %*
)

if %ERRORLEVEL% equ 0 (
    echo --------------------------------------------------
    echo Success: Wall step inference completed.
) else (
    echo --------------------------------------------------
    echo Error: Wall step inference failed.
)

echo.
pause
