#include "NetCommMngr.h"

NetCommMngr* NetCommMngr::g_NetCommMngr = nullptr;
App* NetCommMngr::g_App = nullptr;


bool NetCommMngr::Initialize(App* app)
{
	g_App = app;
	return true;
}
void NetCommMngr::Terminate()
{
	g_App = nullptr;
}