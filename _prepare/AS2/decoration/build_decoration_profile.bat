@echo off
setlocal

if "%~1"=="" (
    echo Drag an AS2 decoration .map file onto this BAT.
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

"%PYTHON_EXE%" "%PYTHON_SCRIPT%" "%~1"
set "RESULT=%ERRORLEVEL%"

if not "%RESULT%"=="0" pause
exit /b %RESULT%
