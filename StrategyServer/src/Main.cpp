#include "Main.h"

int main()
{	
	using namespace dbms;

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
		LOG_DBG_INFO("[{:.4f}] ... listening on {} ...", APP_RUN_TIME(), server.SystemAdress().C_String());
		server.Start();
	}

	server.Terminate();
	dbms::DBMS::del();
	return 0;
}