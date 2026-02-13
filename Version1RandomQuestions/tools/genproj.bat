@echo off
cd %~dp0..
.\tools\premake5.exe vs2022

if not exist "%TARGET_DIR%" (
    pause > nul
    exit /b 1
)
cd "%TARGET_DIR%"

del /q *.spv
