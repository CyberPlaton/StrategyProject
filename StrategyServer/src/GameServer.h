#pragma once

#include "olcPGEX_Network.h"// Client and Server Interface.
#include "NetCommon.h"		// Common network messages.
#include "DBMS.h"			// Database Access.
#include "Timer.h"

using net_msg = olc::net::message< net::Message >;


class GameServer
{
public:
	static GameServer& CreateGameServerCluster(size_t id, size_t port, double update_freq = 1 / 60.0);

	GameServer(size_t id, size_t port, double update_freq) : m_id(id), m_port(port), m_updateFrequency(update_freq){}
	~GameServer() {}
	bool Initialize();
	void Terminate();
	// Starts the while Loop for GameServer. 
	// This is a Blocking function.
	void Start();
	// Update the client and server side of the GameServer.
	// Also the GameServer triggers update steps in PhysicsServer and
	// AIServer over network messages.
	void Update();
	// Whether the GameServer should stop to exist.
	bool ShouldExit();

public:
	
	// Implements the connection and messaging with the MasterServer server.
	struct GameServer::ClientImpl : public olc::net::client_interface< net::Message >
	{
		GameServer::ClientImpl(GameServer* server) : olc::net::client_interface< net::Message >(), m_gameServer(server) {}
		void Update();


		GameServer* m_gameServer = nullptr;
	};
	

	// Implements the connection and messaging with PhysicsServer and AIServer clients.
	struct GameServer::ServerImpl : public olc::net::server_interface< net::Message >
	{
		GameServer::ServerImpl(GameServer* server, size_t port) : olc::net::server_interface< net::Message >(port), m_gameServer(server) {}
		void OnMessage(std::shared_ptr<olc::net::connection< net::Message >> client, olc::net::message< net::Message >& msg) override final;
		bool OnClientConnect(std::shared_ptr<olc::net::connection<net::Message>> client){return true;}

		GameServer* m_gameServer = nullptr;
	};

public:
	bool m_shouldExit = false;
	Timer m_timer;
	double m_updateFrequency = 0.0;

	size_t m_id = 0;
	size_t m_port = 0;

	ClientImpl* m_clientImpl = nullptr;
	ServerImpl* m_serverImpl = nullptr;
};