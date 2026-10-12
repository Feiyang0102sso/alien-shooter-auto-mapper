@echo off
setlocal
cd /d "%~dp0"
rem Build, test and package the portable AutoMapper release, named from the version in src\editor\editor.rc:
rem   Folder: build\AutoMapper V x.y.z\      (AutoMapper.exe, assets\, Qt DLLs and plugins, VC++ runtime)
rem   Zip:    build\AutoMapper V x.y.z.zip   (the same folder, ready to distribute)
set "EXIT_CODE=0"

rem Read: #define AUTO_MAPPER_VERSION_TEXT "x.y.z"
set "APP_VERSION="
for /f "tokens=3" %%v in ('findstr /b /c:"#define AUTO_MAPPER_VERSION_TEXT" "src\editor\editor.rc"') do set "APP_VERSION=%%~v"
if not defined APP_VERSION (
    echo [Error] AUTO_MAPPER_VERSION_TEXT was not found in src\editor\editor.rc.
    goto failed
)
set "RELEASE_NAME=AutoMapper V %APP_VERSION%"
set "RELEASE_DIR=build\%RELEASE_NAME%"
set "RELEASE_ZIP=build\%RELEASE_NAME%.zip"
echo Packaging %RELEASE_NAME%

rem Start from empty: remove every previous package folder and zip in build\.
for /d %%d in ("build\AutoMapper V *") do rmdir /s /q "%%d"
if exist "build\AutoMapper V *.zip" del /q "build\AutoMapper V *.zip"
if exist "%RELEASE_DIR%" (
    echo [Error] Could not clear %RELEASE_DIR%. Close any program using it and retry.
    goto failed
)
if exist "%RELEASE_ZIP%" (
    echo [Error] Could not delete %RELEASE_ZIP%. Close any program using it and retry.
    goto failed
)

call "%~dp0builder.bat" package "%~dp0%RELEASE_DIR%"
if errorlevel 1 goto failed

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
