@echo off
setlocal
cd /d "%~dp0"
rem Build and test AutoMapper (MSVC Release x64) with vs\AutoMapper.sln.
rem   Output: bin\x64\Release\AutoMapper.exe  (runs in place: Qt DLLs and assets\ are copied next to it)
rem   Tests:  bin\x64\Release\AutoMapper.Tests.exe, editor test details in bin\x64\Release\native-editor-results.txt
rem pack_release.bat packages what this script built; it does not build by itself.
set "EXIT_CODE=0"
set "BIN_DIR=bin\x64\Release"

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

rem MSBuild looks up the UCRT headers through the 64-bit registry first; use the SDK that vcvars found instead,
rem in case that registry entry points at a folder without headers.
set "UCRTContentRoot=%UniversalCRTSdkDir%"

msbuild vs\AutoMapper.sln /m /nologo /v:minimal /p:Configuration=Release /p:Platform=x64
if errorlevel 1 goto failed

rem Tests resolve tests\golden from the working directory (the repository root).
"%BIN_DIR%\AutoMapper.Tests.exe" -o "%BIN_DIR%\native-editor-results.txt,txt" -o "-,txt"
if errorlevel 1 goto failed
echo.
echo [OK] Built %BIN_DIR%\AutoMapper.exe
goto finish

:failed
set "EXIT_CODE=1"
echo.
echo [Error] Build failed. See the messages above.

:finish
@pause
exit /b %EXIT_CODE%
