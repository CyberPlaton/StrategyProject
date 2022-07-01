#pragma once

#include <algorithm>
#include <vector>

#include "NetLib.h"
#include "ServerInterface.h"

#include "Logging.h"
#include "DBMS.h"

#include "ICommand.h"
#include "ThreadSafeQueue.h"

#include "Terminal.h"

class MasterServer : public net::ServerInterface
{
public:
	// STATIC HELPERS
	STATIC_GET_DEL(MasterServer, g_MasterServer);
	static bool MasterServerCreated();


	//////////////////////////////
	// SERVER INTERFACE OVERRIDES
	//////////////////////////////

	/// @brief Process incoming messages from clients. Note: You must NOT deallocate the packets, this is done in Interface.
	/// @param packet The next message to be processed.
	void OnMessage(RakNet::Packet* packet) override final;
	
	void OnClientConnect(RakNet::Packet* packet) override final;
	void OnClientDisconnect(RakNet::Packet* packet) override final;
	bool OnClientValidated(RakNet::Packet* packet) override final;
	void OnUpdate() override final;


	//////////////////////////
	// COMMON SERVER FUNCTIONS
	//////////////////////////


	bool ShouldExit() { return !Running(); }
	bool CheckClientVersion(int64_t v);
	bool CheckClientVersion(net::SClientDescription& desc);

	//////////////////////////
	// UTIL
	//////////////////////////

	std::string MapdataToText(tinyxml2::XMLDocument& doc);
	tinyxml2::XMLDocument& MapdataFromText(std::string& maptext);
	inline int64_t GetVersion() { return 1000; }
	void AddCommand(ICommand* cmd);
	std::string RetrieveNextOutput();


private:
	static MasterServer* g_MasterServer;

	Timer m_timer;

	std::map< RakNet::RakString, int64_t > m_clients;
	int64_t m_nextId = 10000;

	// Terminal Command Handling
	tsqueue< std::string > m_outputQueue;
	tsqueue< ICommand* > m_commandQueue;

private:
	MasterServer() : net::ServerInterface()
	{
		m_timer.StartTimer();
	}

	inline int64_t AssignClientId() { return m_nextId++; }
	void BackupClientCount(int64_t seconds);
	void ExecuteTerminalCommands();
};


#define LOG_MS_INFO(...)		Terminal::get()->LogMasterServer(__VA_ARGS__); Terminal::get()->LogMasterServer("[blue]")
#define LOG_MS_SUCCESS(...)		Terminal::get()->LogMasterServer(__VA_ARGS__); Terminal::get()->LogMasterServer("[green]")
#define LOG_MS_WARN(...)		Terminal::get()->LogMasterServer(__VA_ARGS__); Terminal::get()->LogMasterServer("[warn]")
#define LOG_MS_ERROR(...)		Terminal::get()->LogMasterServer(__VA_ARGS__); Terminal::get()->LogMasterServer("[error]")
#define LOG_MS_CRITICAL(...)	Terminal::get()->LogMasterServer(__VA_ARGS__); Terminal::get()->LogMasterServer("[critical]")
#define LOG_MS_FMT(color, ...)	Terminal::get()->LogMasterServer(__VA_ARGS__); Terminal::get()->LogMasterServer( "[" ##color "]" )
