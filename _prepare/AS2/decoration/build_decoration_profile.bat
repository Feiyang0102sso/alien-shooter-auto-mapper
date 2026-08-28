@echo off
setlocal

if "%~1"=="" (
    echo Drag AS2 decoration .map files or folders onto this BAT.
    pause
    exit /b 1
)

set "SCRIPT_DIR=%~dp0"
set "PYTHON_EXE=%SCRIPT_DIR%..\..\..\.venv\Scripts\python.exe"
set "PYTHON_SCRIPT=%SCRIPT_DIR%build_decoration_profile.py"

if not exist "%PYTHON_EXE%" (
    echo [ERROR] Project Python not found: %PYTHON_EXE%
    pause
    exit /b 1
)

set "FAILED=0"

:next_input
if "%~1"=="" goto finished
"%PYTHON_EXE%" "%PYTHON_SCRIPT%" "%~1"
if not "%ERRORLEVEL%"=="0" set "FAILED=1"
shift
goto next_input

:finished
if not "%FAILED%"=="0" pause
exit /b %FAILED%
