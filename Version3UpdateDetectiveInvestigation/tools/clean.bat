@echo off
cd %~dp0..
IF EXIST .\build\*.sln del /s /q .\build\*.sln
IF EXIST .\build\*.vcxproj del /s /q .\build\*.vcxproj
IF EXIST .\build\*.filters del /s /q .\build\*.filters
IF EXIST .\build\*.user del /s /q .\build\*.user
