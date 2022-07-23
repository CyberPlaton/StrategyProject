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
	-- Behavior Tree Library  - Crossplatform (Windows, Linux, Switch, Xbox...)
	-- ##################################################################################################################################
	project "Sakura"
		-- specify location of Project rel. to root directory
		location "Sakura"
		-- what the application actually is
		kind "StaticLib"
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
			"%{prj.name}/common/include/tinyxml2",
		}	
		-- Link thirdparty libraries for each configuration
		filter "configurations:Debug"
			libdirs{"libs", "%{prj.name}/common/lib/Debug"}
			links
			{
				"tinyxml2",
			}
		filter "configurations:Release"
			libdirs{"libs", "%{prj.name}/common/lib/Release"}
			links
			{
				"tinyxml2",
			}
		filter "configurations:Distr"
			libdirs{"libs", "%{prj.name}/common/lib/Distr"}
			links
			{
				"tinyxml2",
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
				"_CRT_SECURE_NO_WARNINGS",
				"_CRT_SECURE_NO_DEPRECATE",
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
			}
			optimize "On"	



	-- ##################################################################################################################################
	-- Networking Library  - Crossplatform (Windows, Linux, Switch, Xbox...)
	-- ##################################################################################################################################
	project "NetLib"
		-- specify location of Project rel. to root directory
		location "NetLib"
		-- what the application actually is
		kind "StaticLib"
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
			"%{prj.name}/common/include/RakNet",	-- RakNet
		}	
		-- Link thirdparty libraries for each configuration
		filter "configurations:Debug"
			libdirs{"libs", "%{prj.name}/common/lib/Debug"}
			links
			{
				"ws2_32",
				"RakNetLibStatic",
			}
		filter "configurations:Release"
			libdirs{"libs", "%{prj.name}/common/lib/Release"}
			links
			{
				"ws2_32",
				"RakNetLibStatic",
			}
		filter "configurations:Distr"
			libdirs{"libs", "%{prj.name}/common/lib/Distr"}
			links
			{
				"ws2_32",
				"RakNetLibStatic",
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
				"_CRT_SECURE_NO_WARNINGS",
				"_CRT_SECURE_NO_DEPRECATE",
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
			}
			optimize "On"



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
			"%{prj.name}/common/include/spdlog",		-- Logging
		

			"NetLib/common/include/RakNet",				-- Networking
			"NetLib/src",								-- Networking

			"Sakura/src",								-- Behavior Tree
		}	



		filter "configurations:Debug"
			libdirs{"libs", "bin/" .. outputdir .. "/NetLib"} -- Networking Library
			links{"ws2_32", "NetLib"}
			libdirs{"libs", "bin/" .. outputdir .. "/Sakura"} -- BehaviorTree Library
			links{"Sakura"}
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
				"spdlogd",
			}
		filter "configurations:Release"
			libdirs{"libs", "bin/" .. outputdir .. "/NetLib"} -- Networking Library
			links{"ws2_32", "NetLib"}
			libdirs{"libs", "bin/" .. outputdir .. "/Sakura"} -- BehaviorTree Library
			links{"Sakura"}
			libdirs{"libs", "%{prj.name}/common/lib/Release"}
			links
			{
				"ws2_32",
				"NetLib",
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
				"spdlog",
			}
		filter "configurations:Distr"
			libdirs{"libs", "bin/" .. outputdir .. "/NetLib"} -- Networking Library
			links{"ws2_32", "NetLib"}
			libdirs{"libs", "bin/" .. outputdir .. "/Sakura"} -- BehaviorTree Library
			links{"Sakura"}
			libdirs{"libs", "%{prj.name}/common/lib/Distr"}
			links
			{
				"ws2_32",
				"NetLib",
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
				"_HAS_EXCEPTIONS=1", -- Exceptions are required for Any implementation to link to std::bad_cast libraries. We dont throw them however, just ignore.
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
				"_ITERATOR_DEBUG_LEVEL=0",
				"__STDC_LIMIT_MACROS",
				"__STDC_FORMAT_MACROS",
				"__STDC_CONSTANT_MACROS",
				"_HAS_EXCEPTIONS=1", -- Exceptions are required for Any implementation to link to std::bad_cast libraries. We dont throw them however, just ignore.
				"_SCL_SECURE=0",
				"_SECURE_SCL=0",
				"_CRT_SECURE_NO_WARNINGS",
				"_CRT_SECURE_NO_DEPRECATE",
				"BGFX_CONFIG_MAX_VIEWS=8",
				"BX_CONFIG_DEBUG=0"
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
				"_ITERATOR_DEBUG_LEVEL=0",
				"__STDC_LIMIT_MACROS",
				"__STDC_FORMAT_MACROS",
				"__STDC_CONSTANT_MACROS",
				"_HAS_EXCEPTIONS=1", -- Exceptions are required for Any implementation to link to std::bad_cast libraries. We dont throw them however, just ignore.
				"_SCL_SECURE=0",
				"_SECURE_SCL=0",
				"_CRT_SECURE_NO_WARNINGS",
				"_CRT_SECURE_NO_DEPRECATE",
				"BGFX_CONFIG_MAX_VIEWS=8",
				"BX_CONFIG_DEBUG=0"
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
			-- all files in the Project src directory with endings of .h or .cpp
			"%{prj.name}/src/**.h",
			"%{prj.name}/src/**.cpp",

			-- all imgui files also need to be included in order to build correctly
			"%{prj.name}/common/include/imgui/**.h",
			"%{prj.name}/common/include/imgui/**.cpp",
		}
		-- Include thirdparty files
		includedirs
		{
			"%{prj.name}/common/include",
			"%{prj.name}/common/include/steam", 			-- querying Steam User data
			"%{prj.name}/common/include/tinyxml2", 			-- Serializing Mapdata
			"%{prj.name}/common/include/mongocxx", 			-- MongoDB intergration
			"%{prj.name}/common/include/bsoncxx", 			-- MongoDB intergration
			"C://boost_1_78_0",								-- MongoDB requires Boost
			"%{prj.name}/common/include/spdlog",			-- Logging


			"%{prj.name}/common/include/imgui",				-- MasterServer GUI
			"%{prj.name}/common/include/GL",				-- ImGui
			"%{prj.name}/common/include/GLFW",				-- ImGui

			"NetLib/common/include/RakNet",					-- Networking
			"NetLib/src",									-- Networking

			"%{prj.name}/common/include/LuaBridge",			-- Serverside Scripting

			"Sakura/src",									-- Behavior Tree
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
			libdirs{"libs", "bin/" .. outputdir .. "/NetLib"} -- Networking Library
			links{"ws2_32", "NetLib"}
			libdirs{"libs", "bin/" .. outputdir .. "/Sakura"} -- BehaviorTree Library
			links{"Sakura"}

			libdirs{"libs", "%{prj.name}/common/lib/Distr"}
			links
			{
				"steam_api64",
				"tinyxml2",
				"bsoncxx",
				"mongocxx",
				"spdlog",
				"glew32",
				"glfw3",
				"opengl32",
				"Lua52"
			}
		filter "configurations:Debug"
			libdirs{"libs", "bin/" .. outputdir .. "/NetLib"}
			links{"ws2_32", "NetLib"}
			libdirs{"libs", "bin/" .. outputdir .. "/Sakura"} -- BehaviorTree Library
			links{"Sakura"}

			libdirs{"libs", "%{prj.name}/common/lib/Debug"}
			links
			{
				"steam_api64",
				"tinyxml2",
				"bsoncxx",
				"mongocxx",
				"spdlogd",
				"glew32",
				"glfw3",
				"opengl32",
				"Lua52"
			}
		filter "configurations:Release"
			libdirs{"libs", "bin/" .. outputdir .. "/NetLib"}
			links{"ws2_32", "NetLib"}
			libdirs{"libs", "bin/" .. outputdir .. "/Sakura"} -- BehaviorTree Library
			links{"Sakura"}

			libdirs{"libs", "%{prj.name}/common/lib/Release"}
			links
			{
				"steam_api64",
				"tinyxml2",
				"bsoncxx",
				"mongocxx",
				"spdlog",
				"glew32",
				"glfw3",
				"opengl32",
				"Lua52"
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





	-- ##################################################################################################################################
	-- Gameeditor - Windows
	-- ##################################################################################################################################
	project "StrategyEditor"
		-- specify location of Project rel. to root directory
		location "StrategyEditor"
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
			-- all files in the project src directory with endings of .h or .cpp
			"%{prj.name}/src/**.h",
			"%{prj.name}/src/**.cpp",

			-- all imgui files also need to be included in order to build correctly
			"%{prj.name}/common/include/imgui/**.h",
			"%{prj.name}/common/include/imgui/**.cpp",

		}
		-- Include thirdparty files
		includedirs
		{
			"%{prj.name}/common/include",
			"%{prj.name}/common/include/olc", 				-- Graphics etc. the base of the Editor
			"%{prj.name}/common/include/tinyxml2", 			-- Serializing Mapdata
			"%{prj.name}/common/include/rapidjson", 		-- Reading TilesetData
			"%{prj.name}/common/include/imgui",				-- Editor GUI
			"%{prj.name}/common/include/GL",				-- ImGui
			"%{prj.name}/common/include/GLFW",				-- ImGui
			"%{prj.name}/common/include/FMOD",				-- FMOD
			"%{prj.name}/common/include/spdlog",			-- SPDLOG
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
				"glew32",
				"glfw3",
				"opengl32",
				"tinyxml2",
				"imgui",

				"fmod_vc",
				"fsbank_vc",
				"fmodstudio_vc",

				"spdlog"
			}
		filter "configurations:Debug"
			libdirs{"libs", "%{prj.name}/common/lib/Debug"}
			links
			{
				"glew32",
				"glfw3",
				"opengl32",
				"tinyxml2",
				"imgui",

				"fmod_vc",
				"fsbank_vc",
				"fmodstudio_vc",

				"spdlogd"
			}
		filter "configurations:Release"
			libdirs{"libs", "%{prj.name}/common/lib/Release"}
			links
			{
				"glew32",
				"glfw3",
				"opengl32",
				"tinyxml2",
				"imgui",

				"fmod_vc",
				"fsbank_vc",
				"fmodstudio_vc",

				"spdlog"
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
