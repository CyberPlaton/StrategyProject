#include "MasterServer.h"

bool MasterServer::CheckClientVersion(uint16_t maj, uint16_t min, uint16_t rev)
{
	return (maj >= MASTER_SERVER_MAJOR_VERSION &&
			min >= MASTER_SERVER_MINOR_VERSION &&
			rev >= MASTER_SERVER_REVIEW_VERSION);
}
bool MasterServer::OnClientConnect(std::shared_ptr<olc::net::connection<net::Message>> client)
{
	return true;
}
void MasterServer::OnClientDisconnect(std::shared_ptr<olc::net::connection<net::Message>> client)
{

}
void MasterServer::BackupUserNumber()
{
	int elapsed = m_timer.SecondsElapsed();
	if (elapsed > 10)
	{
		printf("[MasterServer] Back up user count\n");
		dbms::DBMS::BackupUserNumber();
		m_timer.StartTimer();
	}
}
void MasterServer::OnClientValidated(std::shared_ptr<olc::net::connection<net::Message>> client)
{
	using namespace olc::net;
	using namespace net;

	printf("[MasterServer::OnClientValidated] Client \"%d\" Validated!\n", client->GetID());

	message< Message > msg;
	msg.header.id = Message::NET_MSG_REQUEST_USER_VALIDATION_DATA;	
	client->Send(msg);
}
void MasterServer::OnMessage(std::shared_ptr<olc::net::connection<net::Message>> client, olc::net::message<net::Message>& msg)
{
	using namespace olc::net;
	using namespace dbms;
	using namespace net;

	switch (msg.header.id)
	{
	// Validation of Client.
	case Message::NET_MSG_USER_VALIDATION_DATA:
	{
		printf("[MasterServer::OnMessage] Received user validation data! Validating...\n");

		UserDesc user_desc;
		ClientAppDesc client_desc;

		msg >> user_desc;
		msg >> client_desc;

		DecryptMessage(user_desc);
		DecryptMessage(client_desc);

		// Verify client version.
		bool uptodate = CheckClientVersion(client_desc.m_majorVersion, client_desc.m_majorVersion, client_desc.m_majorVersion);


		// Try getting user data.
		if (!DBMS::GetUserDesc(user_desc))
		{
			// Try creating new user entry.
			size_t uuid = DBMS::CreateUser();
			user_desc.m_uuid = uuid;
			if (!DBMS::UpdateUser(user_desc))
			{
				// Something is really wrong. Reject the client.
				olc::net::message< net::Message > message;
				message.header.id = net::NET_MSG_CLIENT_REJECT;
				MessageClient(client, message);
			}
		}

		// Assign the Network id.
		user_desc.m_netId = GetLastConnectedClientId();

		EncryptMessage(user_desc);

		olc::net::message< net::Message > message;
		message.header.id = net::NET_MSG_USER_DATA;
		message << user_desc;


		MessageClient(client, message);


	}
	break;




	// If a client requests to update his data in the database we trust him
	// and transfer this to the DB. For this the Client must be connected to us.
	// Probably blindly trusting is not the best solution...
	case Message::NET_MSG_REQUEST_USER_DATA_UPDATE:
	{
		printf("[MasterServer::OnMessage] Client requesting update of user data!\n");

		// Get the User desc to be updated.
		UserDesc desc;
		msg >> desc;


		// Update user in own storage.
		if (auto user = std::find(m_userVec.begin(), m_userVec.end(), &desc); user != m_userVec.end())
		{
			UserDesc store = desc;

			// Request to update the user from Database.
			DBMS::UpdateUser(desc);
		}

	}
		break;





	case net::Message::NET_MSG_GAMEOBJECT_UPDATE:
	{
		printf("[MasterServer::OnMessage] Received Gameobject\n");

		// Get Gameobject.
		net::NetGameobject entity;
		msg >> entity;

		printf("\tName: %s \n", entity.m_name.c_str());
		printf("\tUnitName: %s \n", entity.m_unitName.c_str());
		printf("\tNetId: %zu \n", entity.m_netId);
		printf("\tType: %d \n", entity.m_objectType);
		printf("\tArmor: %zu \n", entity.m_unitArmor);
		printf("\tAttack: %zu \n", entity.m_unitAttack);
		printf("\tDefense: %zu \n", entity.m_unitDefense);
		printf("\tHealth: %zu \n", entity.m_unitHealth);

		// TODO
		// Store/Update object in appropriate Game in MongoDB
		// and redirect update to all other connected clients...
	}
	break;

	case net::Message::NET_MSG_MAPDATA:
	{
		printf("[MasterServer::OnMessage] Client requests Mapdata\n");

		// TODO
		// FOR EACH stored game entity in appropriate MongoDB collection entry
		// create a Netgameobject with according data and send to requesting client.
	}
	break;

	default:
		printf("[MasterServer::OnMessage] Unrecognized Client Message: MsgId=\"%d\", ClientId=\"%d\"! \n", (int)msg.header.id, client->GetID());
		return;
	}
}

std::string MasterServer::MapdataToText(tinyxml2::XMLDocument& doc)
{
	using namespace tinyxml2;

	XMLPrinter p;
	doc.Accept(&p);
	return p.CStr();
}
tinyxml2::XMLDocument& MasterServer::MapdataFromText(std::string& maptext)
{
	using namespace tinyxml2;

	XMLDocument doc;
	doc.Parse(maptext.c_str());
	return doc;
}
