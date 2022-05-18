#include "Main.h"

// GLEW AND GLFW
#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "imgui/imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>

int main()
{	
	using namespace dbms;	

	// GLFW Window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
	if (window == NULL)
		return 1;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);


	// Remove Console.
	ShowWindow(GetConsoleWindow(), SW_HIDE);


	// GLEW
	if (auto r = glewInit(); r != GLEW_OK)
	{
		std::cout << glewGetErrorString(r) << std::endl;
		return -3;
	}


	// IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsClassic();

	if (!ImGui_ImplGlfw_InitForOpenGL(window, true))return -1;
	if (!ImGui_ImplOpenGL3_Init("#version 300 es")) return -2;

	bool show_demo_window = true;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();


		ImGui::ShowDemoWindow(&show_demo_window);

		ImGui::Render();

		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;



	if (!Logger::Initialize())
	{
		LOG_DBG_CRITICAL("[{:.4f}] ... unable to start Logger ...", APP_RUN_TIME());
		return 0;
	}
	LOG_DBG_INFO("[{:.4f}] ... start Logger ...", APP_RUN_TIME());


	// Startup the Database Manager.
	DBMS::get();
	if (!DBMS::Initialized())
	{
		LOG_DBG_CRITICAL("[{:.4f}] ... unable to start DBMS ...", APP_RUN_TIME());
		return 0;
	}
	LOG_DBG_INFO("[{:.4f}] ... start DBMS ...", APP_RUN_TIME());


	// Startup the Main Server.
	uint32_t port = 60777;
	uint32_t max = 1;
	MasterServer server;
	if (server.Initialize(port, max))
	{
		LOG_DBG_INFO("[{:.4f}] ... start MS ...", APP_RUN_TIME());
		LOG_DBG_INFO("[{:.4f}] ... listening on {} ...", APP_RUN_TIME(), server.SystemAddress().C_String());
		server.Start();
	}

	server.Terminate();
	dbms::DBMS::del();
	return 0;
}