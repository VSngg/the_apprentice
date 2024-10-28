@echo off
setlocal enabledelayedexpansion

:: If no arguments provided, default to build
if "%~1"=="" goto :build

:: Handle different commands
if "%~1"=="build" goto :build
if "%~1"=="run" goto :run

echo Unknown command: %~1
echo Usage: build.bat [build^|run]
exit /b 1

:build
echo Using MSBuild
"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" projects/VS2022
if errorlevel 1 (
    echo Build failed
    exit /b 1
)
goto :eof

:run
echo Using MSBuild
"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" projects/VS2022
if errorlevel 1 (
    echo Build failed
    exit /b 1
)
echo --------------------------------------------------------------
echo Running Game
echo --------------------------------------------------------------

".\projects\VS2022\build\raylib_game\bin\x64\Debug\raylib_game.exe"