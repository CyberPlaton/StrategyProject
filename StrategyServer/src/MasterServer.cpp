#include "MasterServer.h"

MasterServer* MasterServer::g_MasterServer = nullptr;

bool MasterServer::MasterServerCreated()
{
	return g_MasterServer != nullptr;
}

void MasterServer::OnMessage(RakNet::Packet* packet)
{
	auto addr = packet->systemAddress;
	std::string addr_string = addr.ToString(true, '|');
	auto id = (RakNet::MessageID)packet->data[0];
	
	LOG_DBG_INFO("[{:.4f}][OnMessage] Message \"{}\" from Client \"{}\".", APP_RUN_TIME, net::MessageIDTypeToString(id).C_String(),	addr_string.c_str());
	LOG_FILE_INFO("[{:.4f}][OnMessage] Message \"{}\" from Client \"{}\".", APP_RUN_TIME, net::MessageIDTypeToString(id).C_String(),	addr_string.c_str());
	LOG_MS_INFO("[%.4f][OnMessage] Message \"%s\" from Client \"%s\".", APP_RUN_TIME, net::MessageIDTypeToString(id).C_String(),		addr_string.c_str());

	switch (id)
	{
	case net::EMessageId::NET_MSG_USER_VALIDATION_DATA:
	{
		LOG_DBG_INFO("[{:.4f}][OnMessage] Received User Data from Client \"{}\".", APP_RUN_TIME,	addr_string.c_str());
		LOG_FILE_INFO("[{:.4f}][OnMessage] Received User Data from Client \"{}\".", APP_RUN_TIME, addr_string.c_str());
		LOG_MS_INFO("[%.4f][OnMessage] Received User Data from Client \"%s\".", APP_RUN_TIME,		addr_string.c_str());

		net::SClientDescription clientDesc;

		READ_MESSAGE(in, packet);
		clientDesc.Deserialize(in, true);

		// Verify Client Version.
		if (!CheckClientVersion(clientDesc))
		{
			LOG_DBG_CRITICAL("[{:.4f}][OnMessage] Client \"{}\" out of date!", APP_RUN_TIME,	addr_string.c_str());
			LOG_FILE_CRITICAL("[{:.4f}][OnMessage] Client \"{}\" out of date!", APP_RUN_TIME, addr_string.c_str());
			LOG_MS_CRITICAL("[%.4f][OnMessage] Client \"%s\" out of date!", APP_RUN_TIME,		addr_string.c_str());


			// Disconnect client.
			CREATE_MESSAGE(out, net::EMessageId::NET_MSG_CLIENT_REJECT);
			Send(out, packet->systemAddress);
		}

		if (!dbms::DBMS::GetUserDesc(clientDesc))
		{
			LOG_DBG_WARN("[{:.4f}][OnMessage] Client \"{}\" without DB entry! Attempt to create one!", APP_RUN_TIME,	addr_string.c_str());
			LOG_FILE_WARN("[{:.4f}][OnMessage] Client \"{}\" without DB entry! Attempt to create one!", APP_RUN_TIME, addr_string.c_str());
			LOG_MS_WARN("[%.4f][OnMessage] Client \"%s\" without DB entry! Attempt to create one!", APP_RUN_TIME,		addr_string.c_str());


			clientDesc.m_uuid = dbms::DBMS::CreateUser();
			if (!dbms::DBMS::UpdateUser(clientDesc))
			{
				LOG_DBG_CRITICAL("[{:.4f}][OnMessage] Failed to update DB user entry! UUID: \"{}\" for Client \"{}\"!", APP_RUN_TIME,		clientDesc.m_uuid, addr_string.c_str());
				LOG_FILE_CRITICAL("[{:.4f}][OnMessage] Failed to update DB user entry! UUID: \"{}\" for Client \"{}\"!", APP_RUN_TIME, clientDesc.m_uuid, addr_string.c_str());
				LOG_MS_CRITICAL("[%.4f][OnMessage] Failed to update DB user entry! UUID: \"%zu\" for Client \"%s\"!", APP_RUN_TIME, clientDesc.m_uuid, addr_string.c_str());


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
		// Get all the Abilities and Status Effects from DBMS and send back to requester
		// in sequence, one after another.
		std::vector< net::SStatusEffectData > status_effect_storage;
		std::vector< net::SAbilityData > ability_storage;

		bool error = false;
		if(!dbms::DBMS::GetAllStatusEffectData(status_effect_storage))
		{
			LOG_DBG_CRITICAL("[{:.4f}][OnMessage] Failed to retrieve StatusEffect Data for Client \"{}\"!", APP_RUN_TIME,	 addr_string.c_str());
			LOG_FILE_CRITICAL("[{:.4f}][OnMessage] Failed to retrieve StatusEffect Data for Client \"{}\"!", APP_RUN_TIME, addr_string.c_str());
			LOG_MS_CRITICAL("[%.4f][OnMessage] Failed to retrieve StatusEffect Data for Client \"%s\"!", APP_RUN_TIME,	 addr_string.c_str());
			error = true;
		}
		if(!dbms::DBMS::GetAllAbilityData(ability_storage))
		{
			LOG_DBG_CRITICAL("[{:.4f}][OnMessage] Failed to retrieve Ability Data for Client \"{}\"!", APP_RUN_TIME,	addr_string.c_str());
			LOG_FILE_CRITICAL("[{:.4f}][OnMessage] Failed to retrieve Ability Data for Client \"{}\"!", APP_RUN_TIME, addr_string.c_str());
			LOG_MS_CRITICAL("[%.4f][OnMessage] Failed to retrieve Ability Data for Client \"%s\"!", APP_RUN_TIME,		addr_string.c_str());
			error = true;
		}
		if(!error)
		{

			LOG_DBG_INFO("[{:.4f}][OnMessage] Successfully retrieved \"{}\" Abilities for Client \"{}\"! Sending Now...", APP_RUN_TIME, ability_storage.size(),	addr_string.c_str());
			LOG_FILE_INFO("[{:.4f}][OnMessage]  Successfully retrieved \"{}\" Abilities for Client \"{}\"! Sending Now...", APP_RUN_TIME, ability_storage.size(), addr_string.c_str());
			LOG_MS_SUCCESS("[%.4f][OnMessage] Successfully retrieved \"%d\" Abilities for Client \"%s\"! Sending Now...", APP_RUN_TIME, ability_storage.size(),	addr_string.c_str());
			// Send Ability Data.
			{
				for(auto& ability : ability_storage)
				{
					CREATE_MESSAGE(out, net::EMessageId::NET_MSG_ABILITY_DATA);
					ability.Serialize(out);
					Send(out, packet->systemAddress);
				}
			}


			LOG_DBG_INFO("[{:.4f}][OnMessage] Successfully retrieved \"{}\" StatusEffects for Client \"{}\"! Sending Now...", APP_RUN_TIME, status_effect_storage.size(),		addr_string.c_str());
			LOG_FILE_INFO("[{:.4f}][OnMessage]  Successfully retrieved \"{}\" StatusEffects for Client \"{}\"! Sending Now...", APP_RUN_TIME, status_effect_storage.size(),	addr_string.c_str());
			LOG_MS_SUCCESS("[%.4f][OnMessage] Successfully retrieved \"%d\" StatusEffects for Client \"%s\"! Sending Now...", APP_RUN_TIME, status_effect_storage.size(),		addr_string.c_str());
			// Send StatusEffects Data.
			{
				for (auto& effect : status_effect_storage)
				{
					CREATE_MESSAGE(out, net::EMessageId::NET_MSG_STATUS_EFFECTS_DATA);
					effect.Serialize(out);
					Send(out, packet->systemAddress);
				}
			}

			// Inform about end of data stream!
			{
				CREATE_MESSAGE(out, net::EMessageId::NET_MSG_ABILITY_AND_STATUS_EFFECT_DATA_COMPLETE);
				Send(out, packet->systemAddress);
			}

			break;
		}
	}


	default:
	{
		LOG_DBG_CRITICAL("[{:.4f}][OnMessage] Unrecognized Message: \"{}\" from Client \"{}\".", APP_RUN_TIME, net::MessageIDTypeToString(id).C_String(),		addr_string.c_str());
		LOG_FILE_CRITICAL("[{:.4f}][OnMessage] Unrecognized Message: \"{}\" from Client \"{}\".", APP_RUN_TIME, net::MessageIDTypeToString(id).C_String(),	addr_string.c_str());
		LOG_MS_CRITICAL("[%.4f][OnMessage] Unrecognized Message: \"%s\" from Client \"%s\".", APP_RUN_TIME, net::MessageIDTypeToString(id).C_String(),		addr_string.c_str());
		break;
	}
	}
}
void MasterServer::OnClientConnect(RakNet::Packet* packet)
{
	auto addr = packet->systemAddress;
	std::string addr_string = addr.ToString(true, ':');

	LOG_DBG_INFO("[{:.4f}][OnClientConnect] Client connected: \"{}\".", APP_RUN_TIME,		addr_string.c_str());
	LOG_FILE_INFO("[{:.4f}][OnClientConnect] Client connected: \"{}\".", APP_RUN_TIME,	addr_string.c_str());
	LOG_MS_INFO("[%.4f][OnClientConnect] Client connected: \"%s\".", APP_RUN_TIME,		addr_string.c_str());


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
	std::string addr_string = addr.ToString(true, ':');

	LOG_DBG_WARN("[{:.4f}][OnClientDisconnect] Client disconnected: \"{}\".", APP_RUN_TIME,	addr_string.c_str());
	LOG_FILE_WARN("[{:.4f}][OnClientConnect] Client disconnected: \"{}\".", APP_RUN_TIME,		addr_string.c_str());
	LOG_MS_WARN("[%.4f][OnClientConnect] Client disconnected: \"%s\".", APP_RUN_TIME,		addr_string.c_str());

	// Remote system was disconnected.
	// Remove from storage.
	m_clients.erase(SystemAddress(packet->systemAddress));
}
bool MasterServer::OnClientValidated(RakNet::Packet* packet)
{
	// RakNet validation was successful. ServerInterface validation 
	// was successful too, this is our turn.
	auto addr = packet->systemAddress;
	std::string addr_string = addr.ToString(true, ':');

	LOG_DBG_INFO("[{:.4f}][OnClientValidated] Client validated: \"{}\"", APP_RUN_TIME,	addr_string.c_str());
	LOG_FILE_INFO("[{:.4f}][OnClientValidated] Client validated: \"{}\".", APP_RUN_TIME,	addr_string.c_str());
	LOG_MS_SUCCESS("[%.4f][OnClientConnect] Client validated: \"%s\".", APP_RUN_TIME,	addr_string.c_str());


	return true;
}
bool MasterServer::CheckClientVersion(net::SClientDescription& desc)
{
	return CheckClientVersion(desc.m_version);
}

bool MasterServer::CheckClientVersion(int64_t v)
{
	return (GetVersion() == v);
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
void MasterServer::BackupClientCount(int64_t seconds)
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
