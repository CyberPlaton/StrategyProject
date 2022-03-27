#pragma once

#include <spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <NetLib/NetLib.h>
#include <Timer.h>

#include "CommonDefines.h"

class Logger
{
public:
	STATIC_GET_DEL(Logger, g_Logger);

	static bool Initialize();
	static void Terminate();

	// SPDLOG 
	static std::shared_ptr< spdlog::logger >& ConsoleLog() { return g_ConsoleLog; }
	static std::shared_ptr< spdlog::logger >& FileLog() { return g_FileLog; }


	// UTIL
	static double AppRunningTime() { return g_AppTimer.SecondsElapsed(); }

private:
	static Logger* g_Logger;
	static bool g_LimitMessages;
	static size_t g_MessageLimit;
	static bool g_GamelogOpen;

	static Timer g_AppTimer;
	
	// Logging to debug console
	static std::shared_ptr< spdlog::logger > g_ConsoleLog;

	// Logging to file
	static std::shared_ptr< spdlog::logger > g_FileLog;

private:
	Logger() = default;
};
#define APP_RUN_TIME() Logger::AppRunningTime()
#ifdef DEBUG
// Stdout in DEBUG
#define LOG_DBG_INFO(...) Logger::ConsoleLog()->info(__VA_ARGS__)
#define LOG_DBG_WARN(...) Logger::ConsoleLog()->warn(__VA_ARGS__)
#define LOG_DBG_ERROR(...) Logger::ConsoleLog()->error(__VA_ARGS__)
#define LOG_DBG_CRITICAL(...) Logger::ConsoleLog()->critical(__VA_ARGS__)
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
#define LOG_FILE_INFO(...)	   Logger::FileLog()->info(__VA_ARGS__)
#define LOG_FILE_WARN(...)	   Logger::FileLog()->warn(__VA_ARGS__)
#define LOG_FILE_ERROR(...)	   Logger::FileLog()->error(__VA_ARGS__)
#define LOG_FILE_CRITICAL(...) Logger::FileLog()->critical(__VA_ARGS__)
#endif