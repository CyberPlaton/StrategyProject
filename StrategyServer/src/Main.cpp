#include "Main.h"

int main()
{	
	// Startup the Database Manager.
	dbms::DBMS::get();
	if (!dbms::DBMS::Initialized())
	{
		return 0;
	}

	// Startup the Main Server.
	auto server = new MasterServer(60777);
	if (server->Start())
	{
		printf("[MasterServer] Listening on \"%s:%d\"", server->GetIPv4Address().c_str(), server->GetPort());

		while (!server->ShouldExit())
		{
			server->BackupUserNumber();
			server->Update(-1, false);
		}
	}

	dbms::DBMS::del();
	return 0;
}