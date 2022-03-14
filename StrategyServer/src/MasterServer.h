#pragma once

#include <algorithm>
#include <vector>
#include <map>

#include <olcPGEX_Network.h>

#include "DBMS.h"
#include "NetCommon.h"
#include "Timer.h"

#define MASTER_SERVER_MAJOR_VERSION 0
#define MASTER_SERVER_MINOR_VERSION 1
#define MASTER_SERVER_REVIEW_VERSION 0


class MasterServer : public olc::net::server_interface<net::Message>
{
public:
	MasterServer(uint16_t port) : server_interface(port)
	{
		m_timer.StartTimer();
	}

	// SERVER INTERFACE OVERRIDES
	void OnMessage(std::shared_ptr<olc::net::connection<net::Message>> client, olc::net::message<net::Message>& msg) override final;
	bool OnClientConnect(std::shared_ptr<olc::net::connection<net::Message>> client);
	void OnClientDisconnect(std::shared_ptr<olc::net::connection<net::Message>> client);
	void OnClientValidated(std::shared_ptr<olc::net::connection<net::Message>> client);

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

	std::vector< net::UserDesc* > m_userVec;			// Holding client information.
	//std::vector< net::PlayerCarDesc* > m_playerCarVec;	// Holding game car related information.

private:
};