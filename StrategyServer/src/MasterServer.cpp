#include "MasterServer.h"

MasterServer* MasterServer::g_MasterServer = nullptr;

bool MasterServer::MasterServerCreated()
{
	return g_MasterServer != nullptr;
}

void MasterServer::OnMessage(RakNet::Packet* packet)
{
	auto addr = packet->systemAddress;
	auto id = (RakNet::MessageID)packet->data[0];
	LOG_DBG_INFO("[{:.4f}][OnMessage] Message \"{}\" from Client \"{}\".", APP_RUN_TIME(), net::MessageIDTypeToString(id).C_String(), addr.ToString());
	LOG_FILE_INFO("[{:.4f}][OnMessage] Message \"{}\" from Client \"{}\".", APP_RUN_TIME(), net::MessageIDTypeToString(id).C_String(), addr.ToString());
	LOG_MS_INFO("[%.4f][OnMessage] Failed to retrieve Ability Data for Client \"%s\"!", APP_RUN_TIME(), addr.ToString());

	switch (id)
	{
	case net::EMessageId::NET_MSG_USER_VALIDATION_DATA:
	{
		LOG_DBG_INFO("[{:.4f}][OnMessage] Received User Data from Client \"{}\".", APP_RUN_TIME(), addr.ToString());
		LOG_FILE_INFO("[{:.4f}][OnMessage] Received User Data from Client \"{}\".", APP_RUN_TIME(), addr.ToString());
		LOG_MS_INFO("[%.4f][OnMessage] Received User Data from Client \"%s\".", APP_RUN_TIME(), addr.ToString());

		net::SClientDescription clientDesc;

		READ_MESSAGE(in, packet);
		clientDesc.Deserialize(in, true);

		// Verify Client Version.
		if (!CheckClientVersion(clientDesc))
		{
			LOG_DBG_CRITICAL("[{:.4f}][OnMessage] Client \"{}\" out of date!", APP_RUN_TIME(), addr.ToString());
			LOG_FILE_CRITICAL("[{:.4f}][OnMessage] Client \"{}\" out of date!", APP_RUN_TIME(), addr.ToString());
			LOG_MS_CRITICAL("[%.4f][OnMessage] Client \"%s\" out of date!", APP_RUN_TIME(), addr.ToString());


			// Disconnect client.
			CREATE_MESSAGE(out, net::EMessageId::NET_MSG_CLIENT_REJECT);
			Send(out, packet->systemAddress);
		}

		if (!dbms::DBMS::GetUserDesc(clientDesc))
		{
			LOG_DBG_WARN("[{:.4f}][OnMessage] Client \"{}\" without DB entry! Attempt to create one!", APP_RUN_TIME(), addr.ToString());
			LOG_FILE_WARN("[{:.4f}][OnMessage] Client \"{}\" without DB entry! Attempt to create one!", APP_RUN_TIME(), addr.ToString());
			LOG_MS_WARN("[%.4f][OnMessage] Client \"%s\" without DB entry! Attempt to create one!", APP_RUN_TIME(), addr.ToString());


			clientDesc.m_uuid = dbms::DBMS::CreateUser();
			if (!dbms::DBMS::UpdateUser(clientDesc))
			{
				LOG_DBG_CRITICAL("[{:.4f}][OnMessage] Failed to update DB user entry! UUID: \"{}\"!", APP_RUN_TIME(), addr.ToString(), clientDesc.m_uuid);
				LOG_FILE_CRITICAL("[{:.4f}][OnMessage] Failed to update DB user entry! UUID: \"{}\"!", APP_RUN_TIME(), addr.ToString(), clientDesc.m_uuid);
				LOG_MS_CRITICAL("[%.4f][OnMessage] Failed to update DB user entry! UUID: \"%zu\"!", APP_RUN_TIME(), addr.ToString(), clientDesc.m_uuid);


				// Disconnect client.
				CREATE_MESSAGE(out, net::EMessageId::NET_MSG_CLIENT_REJECT);
				Send(out, packet->systemAddress);
			}
		}

		{
			CREATE_MESSAGE(out, net::EMessageId::NET_MSG_CLIENT_ACCEPT);
			Send(out, packet->systemAddress);
		}
		{
			CREATE_MESSAGE(out, net::EMessageId::NET_MSG_USER_DATA);
			clientDesc.Serialize(out);
			Send(out, packet->systemAddress);
		}
		break;
	}

	case net::EMessageId::NET_MSG_REQUEST_ABILITY_AND_STATUS_EFFECTS_DATA:
	{
		// Get all the Abilities and Status Effects from DBMS and send back to requester.
		net::SStatusEffectDataStorageObject status_effect_storage;
		net::SAbilityDataStorageObject ability_storage;

		bool error = false;
		if(!dbms::DBMS::GetAllStatusEffectData(status_effect_storage))
		{
			LOG_DBG_CRITICAL("[{:.4f}][OnMessage] Failed to retrieve StatusEffect Data for Client \"{}\"!", APP_RUN_TIME(), addr.ToString());
			LOG_FILE_CRITICAL("[{:.4f}][OnMessage] Failed to retrieve StatusEffect Data for Client \"{}\"!", APP_RUN_TIME(), addr.ToString());
			LOG_MS_CRITICAL("[%.4f][OnMessage] Failed to retrieve StatusEffect Data for Client \"%s\"!", APP_RUN_TIME(), addr.ToString());
			error = true;
		}
		if(!dbms::DBMS::GetAllAbilityData(ability_storage))
		{
			LOG_DBG_CRITICAL("[{:.4f}][OnMessage] Failed to retrieve Ability Data for Client \"{}\"!", APP_RUN_TIME(), addr.ToString());
			LOG_FILE_CRITICAL("[{:.4f}][OnMessage] Failed to retrieve Ability Data for Client \"{}\"!", APP_RUN_TIME(), addr.ToString());
			LOG_MS_CRITICAL("[%.4f][OnMessage] Failed to retrieve Ability Data for Client \"%s\"!", APP_RUN_TIME(), addr.ToString());
			error = true;
		}
		if(!error)
		{
			CREATE_MESSAGE(out, net::EMessageId::NET_MSG_ABILITY_AND_STATUS_EFFECTS_DATA);
			// Expected is first the Ability Data and then the Status Effect Data.
			ability_storage.Serialize(out);
			status_effect_storage.Serialize(out);
			Send(out, packet->systemAddress);
		}
		break;
	}


	default:
	{
		LOG_DBG_CRITICAL("[{:.4f}][OnMessage] Unrecognized Message: \"{}\" from \"{}\".", APP_RUN_TIME(), net::MessageIDTypeToString(id).C_String(), addr.ToString());
		LOG_FILE_CRITICAL("[{:.4f}][OnMessage] Unrecognized Message: \"{}\" from \"{}\".", APP_RUN_TIME(), net::MessageIDTypeToString(id).C_String(),  addr.ToString());
		LOG_MS_CRITICAL("[%.4f][OnMessage] Unrecognized Message: \"%s\" from \"%s\".", APP_RUN_TIME(), net::MessageIDTypeToString(id).C_String(), addr.ToString());
		break;
	}
	}

	m_instance->DeallocatePacket(packet);
}
void MasterServer::OnClientConnect(RakNet::Packet* packet)
{
	auto addr = packet->systemAddress;
	LOG_DBG_INFO("[{:.4f}][OnClientConnect] Client connected: \"{}\".", APP_RUN_TIME(), addr.ToString());
	LOG_FILE_INFO("[{:.4f}][OnClientConnect] Client connected: \"{}\".", APP_RUN_TIME(), addr.ToString());

	// RakNet and ServerInterface Validation steps successfully passed.
	// Remote system has successfully connected...
	auto client_id = AssignClientId();
	auto client_addr = SystemAddress(packet->systemAddress);
	m_clients.try_emplace(client_addr, client_id);

	// Request client and platform data for last validation step.
	CREATE_MESSAGE(out, net::EMessageId::NET_MSG_REQUEST_USER_VALIDATION_DATA);
	Send(out, packet->systemAddress);
}
void MasterServer::OnClientDisconnect(RakNet::Packet* packet)
{
	auto addr = packet->systemAddress;
	LOG_DBG_WARN("[{:.4f}][OnClientDisconnect] Client disconnected: \"{}\".", APP_RUN_TIME(), addr.ToString());
	LOG_FILE_WARN("[{:.4f}][OnClientConnect] Client disconnected: \"{}\".", APP_RUN_TIME(), addr.ToString());

	// Remote system was disconnected.
	// Remove from storage.
	m_clients.erase(SystemAddress(packet->systemAddress));
}
bool MasterServer::OnClientValidated(RakNet::Packet* packet)
{
	// RakNet validation was successful. ServerInterface validation 
	// was successful too, this is our turn.
	auto addr = packet->systemAddress;
	LOG_DBG_INFO("[{:.4f}][OnClientValidated] Client validated: \"{}\"", APP_RUN_TIME(), addr.ToString());
	LOG_FILE_INFO("[{:.4f}][OnClientValidated] Client validated: \"{}\".", APP_RUN_TIME(), addr.ToString());

	return true;
}
bool MasterServer::CheckClientVersion(net::SClientDescription& desc)
{
	return CheckClientVersion(desc.m_version);
}
bool MasterServer::CheckClientVersion(uint64_t v)
{
	return (GetVersion() == v);
}
uint64_t MasterServer::GetVersion()
{
	return 1000;
}

void MasterServer::AddCommand(ICommand* cmd)
{
	m_commandQueue.push_back(cmd);
}

std::string MasterServer::RetrieveNextOutput()
{
	if (!m_outputQueue.empty())
	{
		auto s = m_outputQueue.pop_front();
		return s;
	}
	return "";
}

void MasterServer::OnUpdate()
{
	BackupClientCount(10);
	ExecuteTerminalCommands();
}
void MasterServer::BackupClientCount(uint32_t seconds)
{
	if (m_timer.SecondsElapsed() > seconds)
	{
		dbms::DBMS::BackupUserNumber();
		m_timer.StartTimer();
	}
}

void MasterServer::ExecuteTerminalCommands()
{
	tsqueue< ICommand* > reoccurring;
	while (!m_commandQueue.empty())
	{
		auto cmd = m_commandQueue.pop_front();
		cmd->Execute();
		m_outputQueue.push_back(cmd->RetrieveCommandOutput());
		
		if (cmd->Persistent())
		{
			reoccurring.push_back(cmd);
		}
	}

	while (!reoccurring.empty())
	{
		m_commandQueue.push_back(reoccurring.pop_front());
	}
}


uint32_t MasterServer::AssignClientId()
{
	return m_nextId++;
}
/*
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
		net::GameDesc game;
		net::NetGameobject entity;
		msg >> entity;
		msg >> game;

		// Store/Update object in appropriate Game in MongoDB
		// and redirect update to all other connected clients...
		if (!dbms::DBMS::TryEmplaceNetGameobject(entity, game.m_id))
		{
			printf("[MasterServer::OnMessage] Failed to update Gameobject:\n");
			printf("\tGame: \"%s\", Gameobject: name - \"%s\", netId - \"%zu\"\n", game.m_id.c_str(), entity.m_name.c_str(), entity.m_netId);
		}
		else
		{
			// Redirect to clients...
			olc::net::message< net::Message > message;
			message.header.id = net::Message::NET_MSG_GAMEOBJECT_UPDATE;
			message << entity;
			MessageAllClients(message, client);
		}
	}
	break;

	case net::Message::NET_MSG_MAPDATA:
	{
		printf("[MasterServer::OnMessage] Client requests Mapdata\n");

		// FOR EACH stored game entity in appropriate MongoDB collection entry
		// create a Netgameobject with according data and send to requesting client.
		net::GameDesc game;
		msg >> game;

		std::vector< std::shared_ptr< net::NetGameobject > > entities;
		if (dbms::DBMS::GetNetGameobjects(game.m_id, entities))
		{
			for (auto& e : entities)
			{
				olc::net::message< net::Message > message;
				message.header.id = net::Message::NET_MSG_MAPDATA;
				message << e;
				MessageClient(client, message);
			}

			// As we go out of scope,
			// Gameobjects should be released by smart pointer.
		}
		else
		{
			printf("[MasterServer::OnMessage] Failed to retrieve Gameobjects for Game:\n");
			printf("\tGame: \"%s\"\n", game.m_id.c_str());
		}
	}
	break;

	case net::Message::NET_MSG_CREATE_GAME:
	{
		// Create a Game in DB.
		// Send an acknowledgement to client to inform of the success.
		net::GameDesc game;
		msg >> game;

		dbms::DBMS::CreateGame(game.m_id);

		olc::net::message< net::Message > message;
		message.header.id = net::Message::NET_MSG_CREATE_GAME;

		MessageClient(client, message);
	}
	break;

	case net::Message::NET_MSG_DELETE_GAME:
	{
		// Delete a Game from DB.
		// Send an acknowledgement to client to inform of the success.
		net::GameDesc game;
		msg >> game;

		dbms::DBMS::DeleteGame(game.m_id);

		olc::net::message< net::Message > message;
		message.header.id = net::Message::NET_MSG_DELETE_GAME;

		MessageClient(client, message);
	}
	break;

	default:
		printf("[MasterServer::OnMessage] Unrecognized Client Message: MsgId=\"%d\", ClientId=\"%d\"! \n", (int)msg.header.id, client->GetID());
		return;
	}
}
*/

std::string MasterServer::MapdataToText(tinyxml2::XMLDocument& doc)
{
	using namespace tinyxml2;

	XMLPrinter p;
	doc.Accept(&p);
	return p.CStr();
}
tinyxml2::XMLDocument& MasterServer::MapdataFromText(std::string& maptext)
{
	tinyxml2::XMLDocument doc;
	doc.Parse(maptext.c_str());
	return doc;
}
