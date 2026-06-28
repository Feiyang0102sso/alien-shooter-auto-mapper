@echo off
title AS1 Map Unpacker

:: Check if a file was actually dragged and dropped
if "%~1" == "" (
    echo Error: Please drag and drop a .map file onto this batch file.
    pause
    exit /b
)

:: Get the directory where this batch script is located
set "SCRIPT_DIR=%~dp0"
set "PY_SCRIPT=%SCRIPT_DIR%as1_map_unpack.py"

:: Verify if the Python script exists in the same directory
if not exist "%PY_SCRIPT%" (
    echo Error: Could not find "as1_map_unpack.py" in the same folder.
    echo Expected path: "%PY_SCRIPT%"
    pause
    exit /b
)

:: Extract input file path and define output JSON path in the same directory
set "INPUT_FILE=%~1"
set "OUTPUT_FILE=%~dpn1.json"

echo Processing file: "%INPUT_FILE%"
echo Output target:    "%OUTPUT_FILE%"
echo --------------------------------------------------

:: Execute the Python script
python "%PY_SCRIPT%" "%INPUT_FILE%" --output "%OUTPUT_FILE%"

if %ERRORLEVEL% equ 0 (
    echo --------------------------------------------------
    echo Success: Extraction completed.
) else (
    echo --------------------------------------------------
    echo Error: The extraction process failed.
)

echo.
pause