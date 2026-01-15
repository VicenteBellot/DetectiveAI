@echo off

SET "IMGUI_PACKAGE_HASH_DEBUG=164640aad040835ac89882393a96d89200694f04"
SET "IMGUI_CACHE_PATH=C:\Users\juanj\.conan\data\imgui\1.91.4\_\_\build\%IMGUI_PACKAGE_HASH_DEBUG%\src\backends"

cd %~dp0..
conan install -if build/deps/Release -s build_type=Release -s compiler="Visual Studio" -s compiler.runtime=MD --build=missing src/build/
conan install -if build/deps/RelWithDebInfo -s build_type=RelWithDebInfo -s compiler="Visual Studio" -s compiler.runtime=MD --build=missing src/build/
conan install -if build/deps/Debug -s build_type=Debug -s compiler="Visual Studio" -s compiler.runtime=MDd --build=missing src/build/