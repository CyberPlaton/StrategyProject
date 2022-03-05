#pragma once

#include "NetCommon.h"


extern class App;
class NetCommMngr
{
public:
	STATIC_GET_DEL(NetCommMngr, g_NetCommMngr);


	static bool Initialize(App* app);
	static void Terminate();
	static void UpdateNetGameobject(net::NetGameobject& object);

private:
	
	static NetCommMngr* g_NetCommMngr;
	static App* g_App;

private:
	NetCommMngr(){}
};