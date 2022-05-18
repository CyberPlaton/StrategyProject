#include "Terminal.h"


bool Terminal::OnUpdate()
{
	static bool show_demo_window = true;
	ImGui::ShowDemoWindow(&show_demo_window);

	if (glfwWindowShouldClose(m_window))
	{
		return false;
	}
	else
	{
		MainMenuBar();
		RenderTerminalLog();
		RenderMasterServerLog();

		return true;
	}
}


void Terminal::RenderMasterServerLog()
{
	int w, h;
	glfwGetWindowSize(m_window, &w, &h);

	ImGui::SetNextWindowSize(ImVec2(w/2, h), ImGuiCond_Always);
	ImGui::SetNextWindowPos(ImVec2(w/2 + 1, 20), ImGuiCond_Always);

	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.1f);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));

	ImGui::Begin("MasterServer Log", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoTitleBar);


	if (ImGui::Button("Clear Log"))
	{
		m_MSItems.clear();
	}
	ImGui::SameLine();
	ImGui::Checkbox("Auto Scroll", &m_MSAutoScroll);
	
	ImGui::Separator();

	ImGui::BeginChild("MSL");

	// Print log text.
	for (int i = m_MSItems.size() - 1; i > 0; i -= 2)
	{
		const char* color_item = m_MSItems[i - 1];

		if (strstr(color_item, "[info]")) { m_MSColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f); }
		else if (strstr(color_item, "[warn]")) { m_MSColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); }
		else if (strstr(color_item, "[error]")) { m_MSColor = ImVec4(1.0f, 0.3f, 0.0f, 1.0f); }
		else if (strstr(color_item, "[critical]")) { m_MSColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); }

		const char* item = m_MSItems[i];

		ImGui::PushStyleColor(ImGuiCol_Text, m_MSColor);
		ImGui::TextUnformatted(item);
		ImGui::PopStyleColor();
	}

	// Set scroll to bottom.
	if (m_MSAutoScroll) ImGui::SetScrollHereY(1.0f);


	ImGui::EndChild();
	ImGui::End();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
}

void Terminal::RenderTerminalLog()
{
	int w, h;
	glfwGetWindowSize(m_window, &w, &h);

	ImGui::SetNextWindowSize(ImVec2(w/2, h), ImGuiCond_Always);
	ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_Always);

	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.1f);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));

	ImGui::Begin("Terminal Log", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoTitleBar);

	if (ImGui::Button("Clear Log"))
	{
		m_TerItems.clear();
	}
	ImGui::SameLine();
	ImGui::Checkbox("Auto Scroll", &m_TerAutoScroll);

	ImGui::Separator();

	ImGui::BeginChild("TL");

	// Print log text.
	for (int i = m_TerItems.size() - 1; i > 0; i -= 2)
	{
		const char* color_item = m_TerItems[i - 1];

		if (strstr(color_item, "[info]")) { m_TerColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f); }
		else if (strstr(color_item, "[warn]")) { m_TerColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); }
		else if (strstr(color_item, "[error]")) { m_TerColor = ImVec4(1.0f, 0.3f, 0.0f, 1.0f); }
		else if (strstr(color_item, "[critical]")) { m_TerColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); }

		const char* item = m_TerItems[i];

		ImGui::PushStyleColor(ImGuiCol_Text, m_TerColor);
		ImGui::TextUnformatted(item);
		ImGui::PopStyleColor();
	}

	// Set scroll to bottom.
	if (m_TerAutoScroll) ImGui::SetScrollHereY(1.0f);


	ImGui::EndChild();
	ImGui::End();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
}


void Terminal::MainMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				glfwSetWindowShouldClose(m_window, GLFW_TRUE);
			}



			ImGui::EndMenu();
		}
	}
	ImGui::EndMainMenuBar();
}

bool Terminal::Initialize(TerminalDescription* desc)
{
	// Initialize GLFW and GLEW.
	if (!glfwInit()) return false;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
	glfwWindowHint(GLFW_REFRESH_RATE, GLFW_DONT_CARE);

	if (desc->m_fullscreen)
	{
		glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
		glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		
		m_window = glfwCreateWindow(mode->width, mode->height, desc->m_title.c_str(), monitor, NULL);
	}
	else
	{
		glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
		glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE);
		glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_FALSE);
		m_window = glfwCreateWindow(desc->m_width, desc->m_height, desc->m_title.c_str(), NULL, NULL);
	}

	if (!m_window)
	{
		Terminate();
		return false;
	}

	m_clearColor = desc->m_clearColor;

	glfwMakeContextCurrent(m_window);

	if(desc->m_vsync) glfwSwapInterval(1);
	else glfwSwapInterval(0);

	auto r = glewInit();
	if (r != GLEW_OK)
	{
		Terminate();
		return false;
	}


	// IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsClassic();

	if (!ImGui_ImplGlfw_InitForOpenGL(m_window, true)) return false;
	if (!ImGui_ImplOpenGL3_Init("#version 300 es")) return false;


	return true;
}

void Terminal::Terminate()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void Terminal::OnFrameBegin()
{
	// Update GLFW:
	glfwPollEvents();

	// Begin Renderer.
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void Terminal::OnRender()
{
	ImGui::Render();
	int display_w, display_h;
	glfwGetFramebufferSize(m_window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glClearColor(m_clearColor.x * m_clearColor.w, m_clearColor.y * m_clearColor.w, m_clearColor.z * m_clearColor.w, m_clearColor.w);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Terminal::OnFrameEnd()
{
	glfwSwapBuffers(m_window);
}

bool Terminal::Update()
{
	OnFrameBegin();
	if (!OnUpdate())
	{
		return false;
	}
	OnRender();
	OnFrameEnd();
}

void Terminal::LogTerminal(const char* fmt, ...) IM_FMTARGS(2)
{
	char buf[1024];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
	buf[IM_ARRAYSIZE(buf) - 1] = 0;
	va_end(args);
	m_TerItems.push_front(Strdup(buf));
}

void Terminal::LogMasterServer(const char* fmt, ...) IM_FMTARGS(2)
{
	char buf[1024];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
	buf[IM_ARRAYSIZE(buf) - 1] = 0;
	va_end(args);
	m_MSItems.push_front(Strdup(buf));
}

Terminal* Terminal::g_Terminal = nullptr;


Terminal::Terminal()
{
}

Terminal::~Terminal()
{
}

