#pragma once

#include <algorithm>
#include <vector>

#include "NetLib.h"
#include "ServerInterface.h"

#include "Logging.h"
#include "DBMS.h"

#include "ICommand.h"
#include "ThreadSafeQueue.h"

class Terminal;

class MasterServer : public net::ServerInterface
{
public:
	// STATIC HELPERS
	STATIC_GET_DEL(MasterServer, g_MasterServer);
	static bool MasterServerCreated();



	// SERVER INTERFACE OVERRIDES
	void OnMessage(RakNet::Packet* packet) override final;
	void OnClientConnect(RakNet::Packet* packet) override final;
	void OnClientDisconnect(RakNet::Packet* packet) override final;
	bool OnClientValidated(RakNet::Packet* packet) override final;
	void OnUpdate() override final;

	// COMMON SERVER FUNCTIONS
	bool ShouldExit() { return !Running(); }
	bool CheckClientVersion(uint64_t v);
	bool CheckClientVersion(net::SClientDescription& desc);


	// UTIL
	std::string MapdataToText(tinyxml2::XMLDocument& doc);
	tinyxml2::XMLDocument& MapdataFromText(std::string& maptext);
	uint64_t GetVersion();
	void AddCommand(ICommand* cmd);
	std::string RetrieveNextOutput();
	void LogToTerminal(const char* fmt, ...);

private:
	static MasterServer* g_MasterServer;

	Timer m_timer;

	std::map< RakNet::RakString, uint32_t > m_clients;
	uint32_t m_nextId = 10000;

	// Terminal Command Handling
	tsqueue< std::string > m_outputQueue;
	tsqueue< ICommand* > m_commandQueue;

	// MasterServer Log Messages Output


private:
	MasterServer() : net::ServerInterface()
	{
		m_timer.StartTimer();
	}
	uint32_t AssignClientId();
	void BackupClientCount(uint32_t seconds);
	void ExecuteTerminalCommands();
};


#define LOG_MS_INFO(...)		MasterServer::get()->LogToTerminal(__VA_ARGS__); MasterServer::get()->LogToTerminal("[info]")
#define LOG_MS_WARN(...)		MasterServer::get()->LogToTerminal(__VA_ARGS__); MasterServer::get()->LogToTerminal("[warn]")
#define LOG_MS_ERROR(...)		MasterServer::get()->LogToTerminal(__VA_ARGS__); MasterServer::get()->LogToTerminal("[error]")
#define LOG_MS_CRITICAL(...)	MasterServer::get()->LogToTerminal(__VA_ARGS__); MasterServer::get()->LogToTerminal("[critical]")
#define LOG_MS_FMT(color, ...)	MasterServer::get()->LogToTerminal(__VA_ARGS__); MasterServer::get()->LogToTerminal( "[" ##color "]" )
