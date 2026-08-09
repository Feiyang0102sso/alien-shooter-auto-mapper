@echo off
setlocal
title AS2 VID Army Finder

set "SCRIPT_DIR=%~dp0"
set "PROJECT_DIR=%SCRIPT_DIR%..\.."
set "PY_SCRIPT=%SCRIPT_DIR%find_vid_armies.py"
set "VENV_ACTIVATE=%PROJECT_DIR%\.venv\Scripts\activate.bat"

if "%~1" == "" (
    echo Error: Please drag and drop one or more .map files or folders onto this batch file.
    pause
    exit /b 1
)

if not exist "%PY_SCRIPT%" (
    echo Error: Could not find "%PY_SCRIPT%".
    pause
    exit /b 1
)

echo --------------------------------------------------
echo Starting recursive VID Army lookup...

if exist "%VENV_ACTIVATE%" (
    call "%VENV_ACTIVATE%"
)

python "%PY_SCRIPT%" %*
set "EXIT_CODE=%ERRORLEVEL%"

if %EXIT_CODE% equ 0 (
    echo --------------------------------------------------
    echo Success: VID Army report completed.
    echo Report: "%SCRIPT_DIR%_vid_army_report.txt"
) else (
    echo --------------------------------------------------
    echo Error: VID Army lookup failed. Check the messages above.
)

echo.
pause
exit /b %EXIT_CODE%
