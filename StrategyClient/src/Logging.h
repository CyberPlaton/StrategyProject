#pragma once

#include <CherrySoda/Util/Time.h>
#include <CherrySoda/Util/String.h>

#include <spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "AppCommands.h"
#include "CommonDefines.h"


#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS
#include <imgui.h>


namespace cherrysoda
{
	class Logger
	{
	public:
		STATIC_GET_DEL(Logger, g_Logger);

		static bool Initialize(bool gamelog_open = false, bool limit_number_of_retained_messages = false, size_t count = 256);
		static void Terminate();
		static void ToggleGamelog() { Logger::g_GamelogOpen = (Logger::g_GamelogOpen == true) ? false : true; }
		static bool GamelogEnabled() { return Logger::g_GamelogOpen; }
		static void Update()
		{
#ifdef DEBUG || RELEASE

			// Retain a max number of last messages.
			if (g_LimitMessages)
			{
				if (g_Items.size() > g_MessageLimit)
				{
					// Pop color and message itself.
					g_Items.pop_back();
					g_Items.pop_back();
				}
			}
			if (g_GamelogOpen)
			{
				RenderLog("Gamelog", &g_GamelogOpen);
			}
#endif
		}

		// REALTIME IN-GAME LOG
		static void LogRealtime(const char* fmt, ...);

		// SPDLOG 
		static std::shared_ptr< spdlog::logger >& ConsoleLog() { return g_ConsoleLog; }
		static std::shared_ptr< spdlog::logger >& FileLog() { return g_FileLog; }


		// UTIL
		static double AppRunningTime() { return Time::GetSystemTime() - g_AppStartingTime; }

	private:
		static Logger* g_Logger;
		static bool g_LimitMessages;
		static size_t g_MessageLimit;
		static bool g_GamelogOpen;

		static double g_AppStartingTime;

		// Logging to debug console
		static std::shared_ptr< spdlog::logger > g_ConsoleLog;

		// Logging to file
		static std::shared_ptr< spdlog::logger > g_FileLog;

		// Logging to ImGui console
		static ImVector< char* > g_Items;
		static bool g_AutoScroll;
		static ImVec4 g_Color;

	private:
		Logger() = default;

		static void RenderLog(const String& title, bool* opened = nullptr);
		static char* Strdup(const char* s) { IM_ASSERT(s); size_t len = strlen(s) + 1; void* buf = malloc(len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)s, len); }
		static void ClearRealtimeLog();
	};
}

#define APP_RUN_TIME cherrysoda::Logger::AppRunningTime()
#ifdef DEBUG
// Stdout in DEBUG
#define LOG_DBG_INFO(...) cherrysoda::Logger::ConsoleLog()->info(__VA_ARGS__)
#define LOG_DBG_WARN(...) cherrysoda::Logger::ConsoleLog()->warn(__VA_ARGS__)
#define LOG_DBG_ERROR(...) cherrysoda::Logger::ConsoleLog()->error(__VA_ARGS__)
#define LOG_DBG_CRITICAL(...) cherrysoda::Logger::ConsoleLog()->critical(__VA_ARGS__)
// No File output in DEBUG.
#define LOG_FILE_INFO(...)
#define LOG_FILE_WARN(...)
#define LOG_FILE_ERROR(...)
#define LOG_FILE_CRITICAL(...)
#elif RELEASE
// No Stdout in RELEASE
#define LOG_DBG_INFO(...)
#define LOG_DBG_WARN(...)
#define LOG_DBG_ERROR(...)
#define LOG_DBG_CRITICAL(...)
// No File output in RELEASE
#define LOG_FILE_INFO(...)
#define LOG_FILE_WARN(...)
#define LOG_FILE_ERROR(...)
#define LOG_FILE_CRITICAL(...)
#elif DISTR
// No Stdout in RELEASE
#define LOG_DBG_INFO(...)
#define LOG_DBG_WARN(...)
#define LOG_DBG_ERROR(...)
#define LOG_DBG_CRITICAL(...)
// Only File output in DISTR
#define LOG_FILE_INFO(...) cherrysoda::Logger::FileLog()->info(__VA_ARGS__)
#define LOG_FILE_WARN(...) cherrysoda::Logger::FileLog()->warn(__VA_ARGS__)
#define LOG_FILE_ERROR(...) cherrysoda::Logger::FileLog()->error(__VA_ARGS__)
#define LOG_FILE_CRITICAL(...) cherrysoda::Logger::FileLog()->critical(__VA_ARGS__)
#endif

#if defined RELEASE || DEBUG
// ImGui ingame console in DEBUG and RELEASE only.
#define LOG_GAME_INFO(...)		cherrysoda::Logger::LogRealtime(__VA_ARGS__); cherrysoda::Logger::LogRealtime("[blue]")
#define LOG_GAME_SUCCESS(...)	cherrysoda::Logger::LogRealtime(__VA_ARGS__); cherrysoda::Logger::LogRealtime("[info]")
#define LOG_GAME_WARN(...)		cherrysoda::Logger::LogRealtime(__VA_ARGS__); cherrysoda::Logger::LogRealtime("[warn]")
#define LOG_GAME_ERROR(...)		cherrysoda::Logger::LogRealtime(__VA_ARGS__); cherrysoda::Logger::LogRealtime("[error]")
#define LOG_GAME_CRITICAL(...)	cherrysoda::Logger::LogRealtime(__VA_ARGS__); cherrysoda::Logger::LogRealtime("[critical]")
#else
#define LOG_GAME_INFO(...)
#define LOG_GAME_SUCCESS(...)
#define LOG_GAME_WARN(...)
#define LOG_GAME_ERROR(...)
#define LOG_GAME_CRITICAL(...)
#endif