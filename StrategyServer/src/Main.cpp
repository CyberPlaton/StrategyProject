#include "Main.h"

int main()
{	
	using namespace dbms;

	// Startup the Database Manager.
	DBMS::get();
	if (!DBMS::Initialized())
	{
		return 0;
	}

	// Startup the Main Server.
	uint32_t port = 60777;
	uint32_t max = 1;
	MasterServer server;
	server.Initialize(port, max);
	server.Start();
	server.Terminate();

	dbms::DBMS::del();
	return 0;
}