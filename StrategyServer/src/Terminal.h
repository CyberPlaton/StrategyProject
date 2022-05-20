#pragma once

#include "CommonDefines.h"

#include <iostream>
#include <thread>
#include <chrono>

// GLEW AND GLFW
#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"

// IMGUI
#include "imgui/imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


// MASTERSERVER AND DBMS
#include "DBMS.h"
#include "MasterServer.h"
#include "CommandImpl.h"

class Terminal
{
public:
	struct TerminalDescription
	{
		std::string m_title;
		uint64_t m_width;
		uint64_t m_height;
		bool m_fullscreen;
		bool m_vsync;
		ImVec4 m_clearColor;
	};

public:
	STATIC_GET_DEL(Terminal, g_Terminal);

	bool Initialize(TerminalDescription* desc);
	void Terminate();
	bool Update();


	void LogTerminal(const char* fmt, ...);
	void LogMasterServer(const char* fmt, ...);


private:
	static Terminal* g_Terminal;

	GLFWwindow* m_window;
	ImVec4 m_clearColor;

	// Terminal
	ImVector< char* > m_TerItems;
	ImVec4 m_TerColor;
	bool m_TerAutoScroll;

	// MasterServer
	ImVector< char* > m_MSItems;
	ImVec4 m_MSColor;
	bool m_MSAutoScroll;


	// MS
	std::thread m_masterServerThread;

private:
	Terminal();
	Terminal(const Terminal& lh) = delete;
	Terminal& operator=(const Terminal& lh) = delete;
	~Terminal();


private:
	void OnFrameBegin();
	void OnRender();
	void OnFrameEnd();
	bool OnUpdate();
	
	void RenderMasterServerLog();
	void RenderTerminalLog();
	char* Strdup(const char* s) { IM_ASSERT(s); size_t len = strlen(s) + 1; void* buf = malloc(len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)s, len); }

private:
	void MainMenuBar();
};

#define LOG_TERMINAL_INFO(...)		Terminal::get()->LogTerminal(__VA_ARGS__); Terminal::get()->LogTerminal("[info]")
#define LOG_TERMINAL_WARN(...)		Terminal::get()->LogTerminal(__VA_ARGS__); Terminal::get()->LogTerminal("[warn]")
#define LOG_TERMINAL_ERROR(...)		Terminal::get()->LogTerminal(__VA_ARGS__); Terminal::get()->LogTerminal("[error]")
#define LOG_TERMINAL_CRITICAL(...)  Terminal::get()->LogTerminal(__VA_ARGS__); Terminal::get()->LogTerminal("[critical]")

#define LOG_MS_INFO(...)		Terminal::get()->LogMasterServer(__VA_ARGS__); Terminal::get()->LogMasterServer("[info]")
#define LOG_MS_WARN(...)		Terminal::get()->LogMasterServer(__VA_ARGS__); Terminal::get()->LogMasterServer("[warn]")
#define LOG_MS_ERROR(...)		Terminal::get()->LogMasterServer(__VA_ARGS__); Terminal::get()->LogMasterServer("[error]")
#define LOG_MS_CRITICAL(...)	Terminal::get()->LogMasterServer(__VA_ARGS__); Terminal::get()->LogMasterServer("[critical]")