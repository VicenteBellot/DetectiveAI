conan = {}

configs = {'Debug','Release','RelWithDebInfo'}
for i = 1,3 do
    include("build/deps/"..configs[i].."/conanbuildinfo.premake.lua")
    conan[configs[i]] = {}
    local cfg = conan[configs[i]]

    cfg["build_type"] = conan_build_type
    cfg["arch"] = conan_arch
    cfg["includedirs"] = conan_includedirs
    cfg["libdirs"] = conan_libdirs
    cfg["bindirs"] = conan_bindirs
    cfg["libs"] = conan_libs
    cfg["system_libs"] = conan_system_libs
    cfg["defines"] = conan_defines
    cfg["cxxflags"] = conan_cxxflags
    cfg["cflags"] = conan_cflags
    cfg["sharedlinkflags"] = conan_sharedlinkflags
    cfg["exelinkflags"] = conan_exelinkflags
    cfg["frameworks"] = conan_frameworks
end

function conan_config_exec()
    configs = {'Debug','Release','RelWithDebInfo'}
    for i = 1,3 do    
        local cfg = conan[configs[i]]
        filter("configurations:"..configs[i])
            linkoptions { cfg["exelinkflags"] }
            includedirs{ cfg["includedirs"] }
            libdirs{ cfg["libdirs"] }
            links{ cfg["libs"] }
            links{ cfg["system_libs"] }
            links{ cfg["frameworks"] }
            defines{ cfg["defines"] }
        filter{}
    end
end

function conan_config_lib()
    configs = {'Debug','Release','RelWithDebInfo'}
    for i = 1,3 do
        local cfg = conan[configs[i]]
        filter("configurations:"..configs[i])
            linkoptions { cfg["sharedlinkflags"] }
            includedirs{ cfg["includedirs"] }
            defines{ cfg["defines"] }
        filter{}
    end
end

workspace "DetectiveAI"
    configurations { "Debug", "Release" }
    architecture "x64"
    cppdialect "C++20"
    location "build"
    startproject "Window"
    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
        runtime "Debug"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
        runtime "Release"

    filter "configurations:RelWithDebInfo"
        defines { "NDEBUG" }
        optimize "On"
        runtime "Release"
        symbols "On"
    filter {}

project "EliasThorne"
    kind "StaticLib"
    language "C++"
    targetdir "build/%{cfg.buildcfg}"
    includedirs { "include", "deps" }
    
    conan_config_lib()

    files {
        "premake5.lua",
        "src/build/conanfile.txt",
        "src/build/conan.lua",
        "include/*.hpp", 
        "src/*.cpp", 
    }

project "MainDetective"
    kind "ConsoleApp"
    language "C++"
    targetdir "build/%{prj.name}/%{cfg.buildcfg}"
    
    -- IMPORTANT: This app needs the headers to see the classes
    includedirs { "include", "deps", "deps/imgui" }

    links { "EliasThorne" } 
    conan_config_exec() -- Ensure your Conan libs link here
    conan_basic_setup()


    files {
        "example/main.cpp",
        "include/*.hpp", "src/*.cpp",
        "deps/imgui/imgui*.cpp",
    }

    -- Your data copy command
    postbuildcommands {
        '{MKDIR} "%{cfg.targetdir}/data"',
        'xcopy /E /Y /I "%{_MAIN_SCRIPT_DIR}\\example\\data" "%{cfg.targetdir}\\data\\"'
    }