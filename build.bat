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
echo ----------------------------
echo Generating atlas header file
echo ----------------------------

cl src\gen_atlas.c /Fe"src/gen_atlas.exe" -I ..\raylib-5.0_win64_msvc16\include\ /link ..\raylib-5.0_win64_msvc16\lib\raylib.lib opengl32.lib kernel32.lib user32.lib shell32.lib gdi32.lib winmm.lib msvcrt.lib
if errorlevel 1 (
    echo atlas generation failed
    exit /b 1
)

echo ---------------------------------
echo Building and running using cl.exe
echo ---------------------------------

cl src/raylib_game.c /I"../raylib-5.0_win64_msvc16/include/" /MD /DEBUG /Zi /Od /link /DEBUG /MACHINE:X64 /OUT:"The_Apprentice.exe" "../raylib-5.0_win64_msvc16/lib/raylib.lib" opengl32.lib kernel32.lib user32.lib shell32.lib gdi32.lib winmm.lib msvcrt.lib
if errorlevel 1 (
    echo Build failed
    exit /b 1
)
goto :eof

:run
echo ----------------------------
echo Generating atlas header file
echo ----------------------------

cl src\gen_atlas.c /Fe"gen_atlas.exe" -I ..\raylib-5.0_win64_msvc16\include\ /link ..\raylib-5.0_win64_msvc16\lib\raylib.lib opengl32.lib kernel32.lib user32.lib shell32.lib gdi32.lib winmm.lib msvcrt.lib
gen_atlas.exe
if errorlevel 1 (
    echo atlas generation failed
    exit /b 1
)

echo ---------------------------------
echo Building and running using cl.exe
echo ---------------------------------

cl src/raylib_game.c /I"../raylib-5.0_win64_msvc16/include/" /MD /DEBUG /Zi /Od /link /DEBUG /MACHINE:X64 /OUT:"src\The_Apprentice.exe" "../raylib-5.0_win64_msvc16/lib/raylib.lib" opengl32.lib kernel32.lib user32.lib shell32.lib gdi32.lib winmm.lib msvcrt.lib
if errorlevel 1 (
    echo Build failed
    exit /b 1
)
echo --------------------------------------------------------------
echo Running Game
echo --------------------------------------------------------------

pushd src
.\The_Apprentice.exe
