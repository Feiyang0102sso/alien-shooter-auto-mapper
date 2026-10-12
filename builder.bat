@echo off
setlocal
cd /d "%~dp0"
rem Build and test AutoMapper (MSVC Release).
rem   Output: build\msvc-release\AutoMapper.exe  (runs in place: Qt DLLs and assets\ are copied next to it)
rem   builder.bat package "<folder>"  -> also installs the portable app into <folder> (used by pack_release.bat)
set "EXIT_CODE=0"

rem Discover MSVC so builds also work outside a developer command prompt.
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" (
    echo [Error] Visual Studio Installer was not found.
    set "EXIT_CODE=1"
    goto finish
)
for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VS_INSTALL=%%i"
if not defined VS_INSTALL (
    echo [Error] MSVC x64 tools were not found.
    set "EXIT_CODE=1"
    goto finish
)
call "%VS_INSTALL%\VC\Auxiliary\Build\vcvars64.bat"
if errorlevel 1 goto failed

rem Use the CMake and Ninja shipped with Visual Studio.
set "VS_CMAKE=%VS_INSTALL%\Common7\IDE\CommonExtensions\Microsoft\CMake"
set "PATH=%VS_CMAKE%\CMake\bin;%VS_CMAKE%\Ninja;%PATH%"
rem Only one build flavour is maintained: MSVC Release.
set "BUILD_PRESET=msvc-release"

cmake --preset %BUILD_PRESET%
if errorlevel 1 goto failed
cmake --build --preset %BUILD_PRESET% --target auto_mapper_tests auto_mapper_editor auto_mapper_editor_tests
if errorlevel 1 goto failed
ctest --preset %BUILD_PRESET%
if errorlevel 1 goto failed
echo.
echo [OK] Built build\%BUILD_PRESET%\AutoMapper.exe

if /i not "%~1"=="package" goto finish
if "%~2"=="" (
    echo [Error] builder.bat package needs the install folder as its second argument.
    goto failed
)
cmake --install "build/%BUILD_PRESET%" --prefix "%~2" --component NativeEditor
if errorlevel 1 goto failed
echo [OK] Installed portable app into %~2
goto finish

:failed
set "EXIT_CODE=1"
echo.
echo [Error] Build failed. See the messages above.

:finish
rem pack_release.bat calls this script with "package" and pauses once at its own end.
if /i not "%~1"=="package" @pause
exit /b %EXIT_CODE%
