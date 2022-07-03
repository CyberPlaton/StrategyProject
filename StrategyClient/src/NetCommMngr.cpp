#include "NetCommMngr.h"

CNetCommMngr* CNetCommMngr::g_NetCommMngr = nullptr;
App* CNetCommMngr::g_App = nullptr;
RakNet::SystemAddress CNetCommMngr::g_masterServerAddress;


bool CNetCommMngr::Initialize(App* app)
{
	g_App = app;
	return true;
}
void CNetCommMngr::Terminate()
{
	g_App = nullptr;
}