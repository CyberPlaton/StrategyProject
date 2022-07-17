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
		bool init;
		if (init = Logger::Initialize(); init == false)
		{
			LOG_TERMINAL_ERROR("[INIT] Could not Initialize Logger!");
		}


		if (!init)
		{
			LOG_TERMINAL_CRITICAL("[INIT] Initialization Failure...");
			return 0;
		}

		while (terminal->Update())
		{
		}
	}
	return 0;
}