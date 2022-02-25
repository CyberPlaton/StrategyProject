#include "Main.h"

int main()
{
	/*
	auto dbms = dbms::DBMS::get();

	// Create a User
	auto user_id = dbms->AddUserTable();
	// Create a Table for the User.
	dbms->AddUserDataTable<const char*>(user_id, "SteamId", "shadowbananaz");
	

	user_id = dbms->AddUserTable();
	dbms->AddUserDataTable<const char*>(user_id, "SteamId", "tailoryia");

	user_id = dbms->AddUserTable();
	dbms->AddUserDataTable<const char*>(user_id, "SteamId", "shan");

	user_id = dbms->AddUserTable();
	dbms->AddUserDataTable<const char*>(user_id, "SteamId", "evan");

	user_id = dbms->AddUserTable();
	dbms->AddUserDataTable<const char*>(user_id, "SteamId", "hannelore");


	dbms->RemoveUserTable(1);


	dbms::DBMS::del();
	*/

	/*
	auto server = new MasterServer(65777);
	if (server->Start())
	{
		while (!server->ShouldExit())
		{
			server->Update(-1, true);
		}
	}
	*/


	/*
	size_t id = dbms::DBMS::CreateUser();

	printf("User created %d\n", id);

	net::UserDesc desc;
	desc.m_uuid = 1;
	desc.m_majorVersion = 0;
	desc.m_minorVersion = 1;
	desc.m_reviewVersion = 152;
	desc.m_steamId = 918182972718282;
	desc.m_steamName = "ShadowBananaz";
	desc.m_nintendoId = 0;
	desc.m_nintendoName = "";
	desc.m_currency = 8817;
	desc.m_achievementsVec.push_back(net::ACHIEV_TEST_ACHIEVEMENT_19);
	desc.m_achievementsVec.push_back(net::ACHIEV_TEST_ACHIEVEMENT_10);
	desc.m_achievementsVec.push_back(net::ACHIEV_TEST_ACHIEVEMENT_3);
	desc.m_achievementsVec.push_back(net::ACHIEV_TEST_ACHIEVEMENT_1);
	desc.m_serviceItemsVec.push_back(net::ITEM_TEST_ITEM_11);
	desc.m_serviceItemsVec.push_back(net::ITEM_TEST_ITEM_12);
	desc.m_serviceItemsVec.push_back(net::ITEM_TEST_ITEM_2);
	desc.m_serviceItemsVec.push_back(net::ITEM_TEST_ITEM_1);
	desc.m_serviceItemsVec.push_back(net::ITEM_TEST_ITEM_5);
	desc.m_serviceItemsVec.push_back(net::ITEM_TEST_ITEM_6);
	desc.m_serviceItemsVec.push_back(net::ITEM_TEST_ITEM_3);

	dbms::DBMS::UpdateUser(desc);
	*/

	/*
	net::UserDesc desc;
	desc.m_uuid = 1; // The uuid is crucial and is retrieved somehow...

	dbms::DBMS::GetUserDesc(desc);
	*/

	
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