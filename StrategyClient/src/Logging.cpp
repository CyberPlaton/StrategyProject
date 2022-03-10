#include "Logging.h"

namespace cherrysoda
{
	Logger* Logger::g_Logger = nullptr;
	bool Logger::g_LimitMessages = false;
	size_t Logger::g_MessageLimit = 256;
	bool Logger::g_GamelogOpen = false;
	ImVector< char* > Logger::g_Items;
	bool Logger::g_AutoScroll = true;
	ImVec4 Logger::g_Color(0.0f, 0.0f, 0.0f, 0.0f);
	double Logger::g_AppStartingTime = 0.0;
	std::shared_ptr< spdlog::logger > Logger::g_ConsoleLog;
	std::shared_ptr< spdlog::logger > Logger::g_FileLog;

	bool Logger::Initialize(bool gamelog_open, bool limit_number_of_retained_messages, size_t count)
	{
		Logger::get();
		g_AppStartingTime = Time::GetSystemTime();

		g_LimitMessages = limit_number_of_retained_messages;
		g_MessageLimit = count;
		g_GamelogOpen = gamelog_open;

#ifdef DEBUG
		// Complete tracing.
		// Output to file and output to realtime ingame log.
		spdlog::set_pattern("%^[%Y-%m-%d %H:%M:%S][%n] %v%$");

		g_ConsoleLog = spdlog::stdout_color_mt("DEBUG");
#elif defined RELEASE
		// Partial tracing.
		// Output to realtime ingame log.
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
	void Logger::RenderLog(const String& title, bool* opened)
	{
		ImGui::SetNextWindowSize(ImVec2(300, 180), ImGuiCond_FirstUseEver);

		ImGui::Begin(title.c_str(), opened);

		
		ImGui::Checkbox("Autoscroll", &g_AutoScroll);
		ImGui::SameLine();
		ImGui::Checkbox("Messagelimit", &g_LimitMessages);
		ImGui::SameLine();
		if (ImGui::Button("Clear"))
		{
			ClearRealtimeLog();
		}

		ImGui::BeginChild("Scrolling", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));

		// Print log text.
		for (int i = g_Items.size() - 1; i > 0; i-=2)
		{
			const char* color_item = g_Items[i - 1];

			if (strstr(color_item, "[info]")) { g_Color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f); }
			else if (strstr(color_item, "[warn]")) { g_Color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); }
			else if (strstr(color_item, "[error]")) { g_Color = ImVec4(1.0f, 0.3f, 0.0f, 1.0f); }
			else if (strstr(color_item, "[critical]")) { g_Color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); }

			const char* item = g_Items[i];

			ImGui::PushStyleColor(ImGuiCol_Text, g_Color);
			ImGui::TextUnformatted(item);
			ImGui::PopStyleColor();
		}

		// Set scroll to bottom.
		if (g_AutoScroll) ImGui::SetScrollHereY(1.0f);
		
		ImGui::PopStyleVar();
		ImGui::EndChild();
		ImGui::End();
	}
	void Logger::LogRealtime(const char* fmt, ...) IM_FMTARGS(2)
	{
		char buf[1024];
		va_list args;
		va_start(args, fmt);
		vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
		buf[IM_ARRAYSIZE(buf) - 1] = 0;
		va_end(args);
		g_Items.push_front(Strdup(buf));
	}
	void Logger::ClearRealtimeLog()
	{
		g_Items.clear_delete();
	}
}