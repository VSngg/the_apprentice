@echo off
setlocal enabledelayedexpansion

:: If no arguments provided, default to build
if "%~1"=="" goto :build

:: Handle different commands
if "%~1"=="build" goto :build
if "%~1"=="run" goto :run
if "%~1"=="MSbuild" goto :msbuild

echo Unknown command: %~1
echo Usage: build.bat [build^|run]
exit /b 1

:msbuild
echo Using MSBuild
"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" projects/VS2022
if errorlevel 1 (
    echo Build failed
    exit /b 1
)
".\projects\VS2022\build\raylib_game\bin\x64\Debug\raylib_game.exe"
goto :eof

:build
echo Building game using cl.exe
cl src/raylib_game.c /I"../raylib-5.0_win64_msvc16/include/" /MD /link /MACHINE:X64 /OUT:"The_Apprentice.exe" "../raylib-5.0_win64_msvc16/lib/raylib.lib" opengl32.lib kernel32.lib user32.lib shell32.lib gdi32.lib winmm.lib msvcrt.lib
if errorlevel 1 (
    echo Build failed
    exit /b 1
)
goto :eof

:run
echo Using MSBuild
rem "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" projects/VS2022
cl src/raylib_game.c /I"../raylib-5.0_win64_msvc16/include/" /MD /link /MACHINE:X64 /OUT:"The_Apprentice.exe" "../raylib-5.0_win64_msvc16/lib/raylib.lib" opengl32.lib kernel32.lib user32.lib shell32.lib gdi32.lib winmm.lib msvcrt.lib
if errorlevel 1 (
    echo Build failed
    exit /b 1
)
echo --------------------------------------------------------------
echo Running Game
echo --------------------------------------------------------------

.\The_Apprentice.exe