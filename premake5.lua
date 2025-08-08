workspace "XephMark"
architecture "x64"
    configurations { "Debug", "Release" }
    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Tester"
    location "%{prj.name}"
    kind "ConsoleApp"
    language "C++"
    targetname "%{prj.name}"
    targetdir ("bin/".. outputdir)
    objdir ("%{prj.name}/int/" .. outputdir)
    cppdialect "C++17"
    staticruntime "Off"

    files
    {
        "%{prj.name}/**.h",
        "%{prj.name}/**.c",
        "%{prj.name}/**.hpp"
,        "%{prj.name}/**.cpp"
    }

    includedirs
    {
        "%{prj.name}/include",
        "%{prj.name}/src",
		"XephMark/include"
    }

    libdirs "%{prj.name}/lib"

    links
    {
        "XephMark"
    }

    filter "system:windows"
		systemversion "latest"
		defines { "WIN32" }

	filter "configurations:Debug"
		defines { "_DEBUG", "_CONSOLE" }
		symbols "On"

    filter "configurations:Release"
		defines { "NDEBUG", "NCONSOLE" }
		optimize "On"

project "XephMark"
    location "%{prj.name}"
    kind "StaticLib"
    language "C++"
    targetname "%{prj.name}"
    targetdir ("bin/".. outputdir)
    objdir ("%{prj.name}/int/" .. outputdir)
    cppdialect "C++17"
    staticruntime "Off"

    files
    {
        "%{prj.name}/**.h",
        "%{prj.name}/**.c",
        "%{prj.name}/**.hpp"
,       "%{prj.name}/**.cpp"
    }

    includedirs
    {
        "%{prj.name}/include",
        "%{prj.name}/src",
		"%{prj.name}/ext/yaml-cpp/include"
    }

    defines
    {
        "YAML_CPP_STATIC_DEFINE"
    }

    filter "system:windows"
		systemversion "latest"
		defines { "WIN32" }

	filter "configurations:Debug"
		defines { "_DEBUG", "_CONSOLE" }
		symbols "On"

    filter "configurations:Release"
		defines { "NDEBUG", "NCONSOLE" }
		optimize "On"
