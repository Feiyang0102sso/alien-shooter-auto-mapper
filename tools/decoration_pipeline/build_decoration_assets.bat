@echo off
setlocal

rem One-click AS2 decoration prep: map profiles first, cropped previews second.

chcp 65001 >nul

if "%~1"=="" (
    echo Drag AS2 decoration folders onto this BAT.
    echo Each folder needs one .map per screenshot, matched in alphabetical order.
    pause
    exit /b 1
)

set "SCRIPT_DIR=%~dp0"
set "PYTHON_EXE=%SCRIPT_DIR%..\..\.venv\Scripts\python.exe"
set "PROFILE_SCRIPT=%SCRIPT_DIR%build_decoration_profile.py"
set "CROPPER_SCRIPT=%SCRIPT_DIR%decoration_preview_cropper.py"

if not exist "%PYTHON_EXE%" (
    echo [ERROR] Project Python not found: %PYTHON_EXE%
    pause
    exit /b 1
)

set "FAILED=0"

:next_input
if "%~1"=="" goto finished

if not exist "%~1\" (
    echo [ERROR] Not a folder: %~1
    set "FAILED=1"
    shift
    goto next_input
)

echo [STEP] Building decoration profiles: %~1
"%PYTHON_EXE%" "%PROFILE_SCRIPT%" "%~1"
if not "%ERRORLEVEL%"=="0" (
    set "FAILED=1"
    shift
    goto next_input
)

echo [STEP] Cropping previews: %~1
"%PYTHON_EXE%" "%CROPPER_SCRIPT%" --auto "%~1"
if not "%ERRORLEVEL%"=="0" set "FAILED=1"

shift
goto next_input

:finished
if not "%FAILED%"=="0" (
    echo [ERROR] One or more folders failed.
)
pause
exit /b %FAILED%
