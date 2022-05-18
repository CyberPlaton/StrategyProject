#include "Main.h"


int main()
{	
	using namespace dbms;	

	Terminal::TerminalDescription desc;
	desc.m_clearColor = { 0.1f, 0.1f, 0.1f, 1.0f};
	desc.m_fullscreen = false;
	desc.m_height = 720;
	desc.m_width = 1096;
	desc.m_title = "MasterServer Terminal";
	desc.m_vsync = true;

	auto terminal = Terminal::get();
	if (terminal->Initialize(&desc))
	{
		while (terminal->Update())
		{
			LOG_TERMINAL_INFO("Some Info");
			LOG_TERMINAL_WARN("Warning!");
			LOG_TERMINAL_ERROR("Uh, an Error!!!");
			LOG_TERMINAL_CRITICAL("CRITICAL FAILURE");


			LOG_MS_INFO("Beep");
			LOG_MS_WARN("Warning");
			LOG_MS_ERROR("Some Error occured at xyz");
			LOG_MS_CRITICAL("A critical issue %d, %s", 100291982, "\"Error String\"");

		}
	}
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