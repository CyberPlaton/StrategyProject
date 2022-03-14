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

	if (DBMS::DeleteGame("Bogdans_Test_Game"))
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