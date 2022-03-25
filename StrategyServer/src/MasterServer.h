#pragma once

#include <algorithm>
#include <vector>
#include <map>

#include <NetLib/NetLib.h>
#include <NetLib/ServerInterface.h>
#include <NetLib/Timer.h>
#include "DBMS.h"


#define MASTER_SERVER_MAJOR_VERSION 0
#define MASTER_SERVER_MINOR_VERSION 1
#define MASTER_SERVER_REVIEW_VERSION 0


class MasterServer : public net::ServerInterface
{
public:
	MasterServer() : net::ServerInterface()
	{
		m_timer.StartTimer();
	}

	// SERVER INTERFACE OVERRIDES
	void OnMessage(RakNet::Packet* packet) override final;
	void OnClientConnect(RakNet::Packet* packet) override final;
	void OnClientDisconnect(RakNet::Packet* packet) override final;
	bool OnClientValidated(RakNet::Packet* packet) override final;
	

	// COMMON SERVER FUNCTIONS
	bool ShouldExit() { return m_shouldExit; }
	void Exit() { m_shouldExit = true; }
	void BackupUserNumber();
	bool CheckClientVersion(uint16_t maj, uint16_t min, uint16_t rev);


	// UTIL
	std::string MapdataToText(tinyxml2::XMLDocument& doc);
	tinyxml2::XMLDocument& MapdataFromText(std::string& maptext);

private:

	Timer m_timer;

	bool m_shouldExit = false;

	std::map< size_t, size_t > m_clientIdMap;


private:
};