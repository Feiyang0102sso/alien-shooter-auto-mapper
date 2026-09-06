@echo off
setlocal

set "PROJECT_ROOT=%~dp0"
set "PYTHON_EXE=%PROJECT_ROOT%.venv\Scripts\python.exe"

cd /d "%PROJECT_ROOT%"

taskkill /f /im AutoMapper.exe 2>nul

if not exist "%PYTHON_EXE%" (
    echo [Error] Project virtual environment was not found:
    echo %PYTHON_EXE%
    pause
    exit /b 1
)

echo [Prep] Using project Python:
echo %PYTHON_EXE%
"%PYTHON_EXE%" -c "import sys; print(sys.executable)"

"%PYTHON_EXE%" -m pip install nuitka ordered-set
if errorlevel 1 (
    echo [Error] Failed to install packaging dependencies.
    pause
    exit /b 1
)

"%PYTHON_EXE%" "%PROJECT_ROOT%pack_nuitka.py"
if errorlevel 1 (
    echo [Error] Nuitka packaging failed.
    pause
    exit /b 1
)

echo [Finish] Packaging completed successfully.
pause

endlocal
