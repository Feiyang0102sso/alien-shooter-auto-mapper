@echo off
setlocal
cd /d "%~dp0"
rem Package the AutoMapper.exe already built in bin\x64\Release (no build, no tests: run builder.bat first),
rem named from the version in vs\AutoMapper.rc:
rem   Folder: bin\AutoMapper V x.y.z\      (AutoMapper.exe, assets\, Qt DLLs and plugins, VC++ runtime)
rem   Zip:    bin\AutoMapper V x.y.z.zip   (the same folder, ready to distribute)
rem assets\ and loot_presets.ini come from the repository, not from bin\x64\Release: the bin\ copies keep files
rem deleted from assets\ and any presets saved while running the exe there.
set "EXIT_CODE=0"
set "BIN_DIR=bin\x64\Release"

if not exist "%BIN_DIR%\AutoMapper.exe" (
    echo [Error] %BIN_DIR%\AutoMapper.exe was not found. Run builder.bat first.
    goto failed
)
for %%e in ("%BIN_DIR%\AutoMapper.exe") do echo Packaging %BIN_DIR%\AutoMapper.exe, built %%~te

rem Read: #define AUTO_MAPPER_VERSION_TEXT "x.y.z"
set "APP_VERSION="
for /f "tokens=3" %%v in ('findstr /b /c:"#define AUTO_MAPPER_VERSION_TEXT" "vs\AutoMapper.rc"') do set "APP_VERSION=%%~v"
if not defined APP_VERSION (
    echo [Error] AUTO_MAPPER_VERSION_TEXT was not found in vs\AutoMapper.rc.
    goto failed
)
set "RELEASE_NAME=AutoMapper V %APP_VERSION%"
set "RELEASE_DIR=bin\%RELEASE_NAME%"
set "RELEASE_ZIP=bin\%RELEASE_NAME%.zip"
echo Packaging %RELEASE_NAME%

rem vcvars provides msbuild (for the Qt path) and VCToolsRedistDir (for the CRT DLLs).
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" (
    echo [Error] Visual Studio Installer was not found.
    goto failed
)
for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VS_INSTALL=%%i"
if not defined VS_INSTALL (
    echo [Error] MSVC x64 tools were not found.
    goto failed
)
rem vcvars prints a harmless "'vswhere.exe' is not recognized" on stderr; its exit code is still checked.
call "%VS_INSTALL%\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
if errorlevel 1 goto failed

rem Qt SDK path comes from vs\qt.props, so it is written in one place only. ("^=": for /f would eat a bare "=")
for /f "usebackq tokens=*" %%q in (`msbuild vs\AutoMapper.vcxproj -nologo -getProperty:QtDir -p:Configuration^=Release -p:Platform^=x64`) do set "QT_DIR=%%q"
if not exist "%QT_DIR%bin\windeployqt.exe" (
    echo [Error] windeployqt.exe was not found under "%QT_DIR%".
    goto failed
)
rem App-local release CRT: only the DLLs the exe, Qt and its plugins import (checked with dumpbin /dependents).
for /d %%d in ("%VCToolsRedistDir%x64\Microsoft.VC*.CRT") do set "CRT_DIR=%%d"
if not defined CRT_DIR (
    echo [Error] The VC++ runtime folder was not found under "%VCToolsRedistDir%x64".
    goto failed
)

rem Start from empty: remove every previous package folder and zip in bin\.
for /d %%d in ("bin\AutoMapper V *") do rmdir /s /q "%%d"
if exist "bin\AutoMapper V *.zip" del /q "bin\AutoMapper V *.zip"
if exist "%RELEASE_DIR%" (
    echo [Error] Could not clear %RELEASE_DIR%. Close any program using it and retry.
    goto failed
)
if exist "%RELEASE_ZIP%" (
    echo [Error] Could not delete %RELEASE_ZIP%. Close any program using it and retry.
    goto failed
)

mkdir "%RELEASE_DIR%"
copy /y "%BIN_DIR%\AutoMapper.exe" "%RELEASE_DIR%\" >nul || goto failed
copy /y "loot_presets.ini" "%RELEASE_DIR%\" >nul || goto failed
xcopy /e /i /y /q "assets\preview" "%RELEASE_DIR%\assets\preview" >nul || goto failed
xcopy /e /i /y /q "assets\decoration_maps" "%RELEASE_DIR%\assets\decoration_maps" >nul || goto failed

rem The Qt runtime in bin\x64\Release also serves the tests (Qt6Test, qoffscreen), so deploy a slim set instead:
rem Qt6Core/Gui/Widgets, platforms\qwindows, imageformats\qwebp (previews), styles\qmodernwindowsstyle ("windows11").
"%QT_DIR%bin\windeployqt.exe" --release --no-translations --no-compiler-runtime --no-system-d3d-compiler ^
    --no-system-dxc-compiler --no-opengl-sw --skip-plugin-types generic,networkinformation,tls ^
    --exclude-plugins qgif,qicns,qico,qjpeg,qtga,qtiff,qwbmp "%RELEASE_DIR%\AutoMapper.exe" >nul
if errorlevel 1 goto failed

for %%f in (msvcp140.dll msvcp140_1.dll msvcp140_2.dll vcruntime140.dll vcruntime140_1.dll) do (
    copy /y "%CRT_DIR%\%%f" "%RELEASE_DIR%\" >nul || goto failed
)
echo [OK] Copied portable app into %RELEASE_DIR%

powershell -NoProfile -Command "$ErrorActionPreference = 'Stop'; Compress-Archive -Path '%RELEASE_DIR%\*' -DestinationPath '%RELEASE_ZIP%' -CompressionLevel Optimal"
if errorlevel 1 goto failed
echo [OK] Packed %RELEASE_ZIP%
goto finish

:failed
set "EXIT_CODE=1"
echo.
echo [Error] Packaging failed. See the messages above.

:finish
@pause
exit /b %EXIT_CODE%
