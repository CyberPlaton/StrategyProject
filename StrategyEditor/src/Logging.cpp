#include "Logging.h"

Timer Logger::g_AppTimer;
Logger* Logger::g_Logger = nullptr;
std::shared_ptr< spdlog::logger > Logger::g_ConsoleLog;
std::shared_ptr< spdlog::logger > Logger::g_FileLog;

bool Logger::Initialize()
{
	Logger::get();
	Logger::g_AppTimer.StartTimer();

#ifdef DEBUG
	// Complete tracing.
	// Output to file and output to real time in-game log.
	spdlog::set_pattern("%^[%n] %v%$");

	g_ConsoleLog = spdlog::stdout_color_mt("DEBUG");
#elif defined RELEASE
	// Partial tracing.
	// Output to real time in-game log.
#elif defined DISTR
	// Minimal tracing.
	// Output to file.
	spdlog::set_pattern("%^[%Y-%m-%d %H:%M:%S][%n][%l] %v%$");
	spdlog::flush_every(std::chrono::seconds(1));

	g_FileLog = spdlog::basic_logger_mt("DISTR", "Log.txt");
#endif

	LOG_DBG_INFO("[{:.4f}] Starting Application...", 0.0);
	LOG_FILE_INFO("[{:.4f}] Starting Application...", 0.0);

	return true;
}


void Logger::Terminate()
{
	LOG_DBG_INFO("[{:.4f}] Ending Application...", AppRunningTime());
	LOG_FILE_INFO("Ending Application...", AppRunningTime());
	Logger::del();
}