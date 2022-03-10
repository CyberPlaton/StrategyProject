-- cfg.buildcfg - Whether Debug, Release or Distr build.
-- cfg.system - For which Platform (Windows, Linux, etc.) to build.
-- cfg.architecture - Which processor architecture to Target (x32 or x64).
-- prj.name - Name of the Project. E.g. "CarSteering"
outputdir = "%{cfg.buildcfg}_%{cfg.system}_%{cfg.architecture}"	
-- solution name
workspace "Strategy"
	-- solution platforms
	architecture "x86_64"
	-- solution configurations
	configurations
	{
		"Debug",
		"Release",
		"Distr"
	}
	-- for which platforms to create projects
	platforms
	{
		"Win64", --"XboxOne", "Switch"
	}
	-- ##################################################################################################################################
	-- Client Application - Crossplatform (Windows, Linux, Switch, Xbox...)
	-- ##################################################################################################################################
	project "StrategyClient"
		-- specify location of Project rel. to root directory
		location "StrategyClient"
		-- what the application actually is
		kind "WindowedApp"
		-- the programming language of the project
		language "C++"
		-- where the output binaries go
		targetdir ("bin/" .. outputdir .. "/%{prj.name}")
		-- same for intermediate output files
		objdir ("intermediate/" .. outputdir .. "/%{prj.name}")
		-- use C++17 for the projects language
		cppdialect "C++17"	
		-- use latest available system version (e.g. latest Windows SDK)
		systemversion "latest"
		-- set working directory for debuging
		debugdir ("bin/" .. outputdir .. "/%{prj.name}")
		-- Specify which source files to include
		files
		{
			-- all files in the CarSteering directory with endings of .h or .cpp
			"%{prj.name}/src/**.h",
			"%{prj.name}/src/**.cpp",
		}
		-- Include thirdparty files
		includedirs
		{
			"%{prj.name}/common/include",
			"%{prj.name}/common/include/CherrySoda",	-- Engine
			"%{prj.name}/common/include/bgfx",			-- Engine
			"%{prj.name}/common/include/bx",			-- Engine
			"%{prj.name}/common/include/cgltf",			-- Engine
			"%{prj.name}/common/include/cmixer",		-- Engine
			"%{prj.name}/common/include/glm",			-- Engine
			"%{prj.name}/common/include/imgui",			-- Engine
			"%{prj.name}/common/include/rapidjson",		-- Engine
			"%{prj.name}/common/include/SDL2",			-- Engine
			"%{prj.name}/common/include/stb",			-- Engine
			"%{prj.name}/common/include/steam",			-- Release Platform
			"%{prj.name}/common/include/tinyxml2",		-- Engine
			"%{prj.name}/common/include/asio",			-- Networking
			"%{prj.name}/common/include/olc",			-- Networking
			"%{prj.name}/common/include/LuaBridge",		-- Scripting
			"%{prj.name}/common/include/spdlog",		-- Logging
		
			"StrategyServer/src"						-- For NetCommon.h include common for all projects
		}	
		-- Link thirdparty libraries for each configuration
		filter "configurations:Debug"
			libdirs{"libs", "%{prj.name}/common/lib/Debug"}
			links
			{
				"CherrySoda",
				"imgui",
				"bgfx",
				"bimg_decode",
				"bimg",
				"bx",
				"cmixer",
				"stb_vorbis",
				"SDL2",
				"SDL2main",
				"steam_api64",
				"tinyxml2",
				"Lua52",
				"spdlogd",
			}
		filter "configurations:Release"
			libdirs{"libs", "%{prj.name}/common/lib/Release"}
			links
			{
				"CherrySoda",
				"imgui",
				"bgfx",
				"bimg_decode",
				"bimg",
				"bx",
				"cmixer",
				"stb_vorbis",
				"SDL2",
				"SDL2main",
				"steam_api64",
				"tinyxml2",
				"Lua52",
				"spdlog",
			}
		filter "configurations:Distr"
			libdirs{"libs", "%{prj.name}/common/lib/Distr"}
			links
			{
				"CherrySoda",
				"imgui",
				"bgfx",
				"bimg_decode",
				"bimg",
				"bx",
				"cmixer",
				"stb_vorbis",
				"SDL2",
				"SDL2main",
				"steam_api64",
				"tinyxml2",
				"Lua52",
				"spdlog",
			}
		-- Everything defined below is only if we building on windows
		filter "system:Windows"
			system "Windows"
			-- Specify macro definitions for project in the windows system
			defines
			{
				"PLATFORM_WINDOWS"
			}
			undefines
			{
			}
			-- Enable Multithreaded Debug library only in Debug mode.
		filter {"system:Windows", "configurations:Debug"}
			staticruntime "Off"
		filter {"system:Windows", "configurations:Release"}
			staticruntime "Off"
		filter {"system:Windows", "configurations:Distr"}
			staticruntime "Off"
		-- Specify extra stuff for Debug config and each other configuration we have
		filter "configurations:Debug"
			defines 
			{
				"DEBUG",
				"_ITERATOR_DEBUG_LEVEL=0",
				"CHERRYSODA_ENABLE_DEBUG",
				"__STDC_LIMIT_MACROS",
				"__STDC_FORMAT_MACROS",
				"__STDC_CONSTANT_MACROS",
				"_HAS_EXCEPTIONS=0",
				"_SCL_SECURE=0",
				"_SECURE_SCL=0",
				"_CRT_SECURE_NO_WARNINGS",
				"_CRT_SECURE_NO_DEPRECATE",
				"BGFX_CONFIG_MAX_VIEWS=8",
				"BX_CONFIG_DEBUG=1"
			}
			symbols "On"
		filter "configurations:Release"
			defines 
			{
				"RELEASE",
				"NDEBUG",
			}
			undefines
			{
				"CHERRYSODA_ENABLE_DEBUG",
			}
			optimize "On"
		filter "configurations:Distr"
			defines
			{
				"DISTR",
				"NDEBUG",
			} 
			undefines
			{
				"CHERRYSODA_ENABLE_DEBUG",
			}
			optimize "On"
	
	
	
	
	
	
	
	
	-- ##################################################################################################################################
	-- Master-/Gameserver Application - Windows
	-- ##################################################################################################################################
	filter "system:Windows"
		project "StrategyServer"
		-- specify location of Project rel. to root directory
		location "StrategyServer"
		-- what the application actually is
		kind "ConsoleApp"
		-- the programming language of the project
		language "C++"
		-- where the output binaries go
		targetdir ("bin/" .. outputdir .. "/%{prj.name}")
		-- same for intermediate output files
		objdir ("intermediate/" .. outputdir .. "/%{prj.name}")
		-- use C++17 for the projects language
		cppdialect "C++17"	
		-- use latest available system version (e.g. latest Windows SDK)
		systemversion "latest"
		-- set working directory for debuging
		debugdir ("bin/" .. outputdir .. "/%{prj.name}")
		-- Specify which source files to include
		files
		{
			-- all files in the CarSteering directory with endings of .h or .cpp
			"%{prj.name}/src/**.h",
			"%{prj.name}/src/**.cpp",
		}
		-- Include thirdparty files
		includedirs
		{
			"%{prj.name}/common/include",
			"%{prj.name}/common/include/steam", 			-- querying Steam User data
			"%{prj.name}/common/include/tinyxml2", 			-- ...
			"%{prj.name}/common/include/asio", 				-- olcPGEX_Network
			"%{prj.name}/common/include/olc", 				-- server implementation
			"%{prj.name}/common/include/mongocxx", 			-- MongoDB intergration
			"%{prj.name}/common/include/bsoncxx", 			-- MongoDB intergration
			"C://boost_1_78_0",								-- MongoDB requires Boost
			"%{prj.name}/common/include/spdlog",			-- Logging
		}
		filter "configurations:Debug"
			-- Set working directory for debugging
			debugdir ("bin/" .. outputdir .. "/%{prj.name}")
		filter "configurations:Distr"
			-- Set working directory for distr
			debugdir ("bin/" .. outputdir .. "/%{prj.name}")
		filter "configurations:Release"
			-- Set working directory for release
			debugdir ("bin/" .. outputdir .. "/%{prj.name}")
		-- Linking libraries
		filter "configurations:Distr"
			libdirs{"libs", "%{prj.name}/common/lib/Distr"}
			links
			{
				"steam_api64",
				"tinyxml2",
				"bsoncxx",
				"mongocxx",
				"spdlog",
			}
		filter "configurations:Debug"
			libdirs{"libs", "%{prj.name}/common/lib/Debug"}
			links
			{
				"steam_api64",
				"tinyxml2",
				"bsoncxx",
				"mongocxx",
				"spdlogd",
			}
		filter "configurations:Release"
			libdirs{"libs", "%{prj.name}/common/lib/Release"}
			links
			{
				"steam_api64",
				"tinyxml2",
				"bsoncxx",
				"mongocxx",
				"spdlog",
			}
		-- Everything defined below is only if we building on windows
		filter "system:Windows"
			-- Specify macro definitions for project in the windows system
			defines
			{
				"PLATFORM_WINDOWS"
			}
			undefines
			{
			}
		-- Enable Multithreaded Debug library only in Debug mode.
		filter {"system:Windows", "configurations:Debug"}
			staticruntime "Off"
		filter {"system:Windows", "configurations:Release"}
			staticruntime "Off"
		filter {"system:Windows", "configurations:Distr"}
			staticruntime "Off"
		-- Specify extra stuff for Debug config and each other configuration we have
		filter "configurations:Debug"
			defines 
			{
				"DEBUG",
				"_ITERATOR_DEBUG_LEVEL=0",
				"_CRT_SECURE_NO_WARNINGS",
				"_CRT_SECURE_NO_DEPRECATE",
			}
			symbols "On"
		filter "configurations:Release"
			defines 
			{
				"RELEASE",
				"NDEBUG"
			} 
			optimize "On"
		filter "configurations:Distr"
			defines
			{
				"DISTR",
				"NDEBUG"
			} 
			optimize "On"