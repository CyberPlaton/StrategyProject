#include "GameServer.h"

GameServer& GameServer::CreateGameServerCluster(size_t id, size_t port, double update_freq)
{
	// Create and start the server.
	GameServer server(id, port, update_freq);
	server.Initialize();
	return server;
}
bool GameServer::ShouldExit()
{
	return m_shouldExit;
}
bool GameServer::Initialize()
{
	// Create Client and connect to MasterServer.
	m_clientImpl = new GameServer::ClientImpl(this);
	bool client_connect = m_clientImpl->Connect("100.81.181.154", 55777);

	// Create Server and start listening.
	m_serverImpl = new GameServer::ServerImpl(this, m_port);
	bool server_start = m_serverImpl->Start();

	return client_connect && server_start;
}
void GameServer::Terminate()
{
	delete m_serverImpl;
	delete m_clientImpl;
	m_serverImpl = nullptr;
	m_clientImpl = nullptr;
}
void GameServer::Update()
{
	// Client update.
	m_clientImpl->Update();
	
	// Update Server once.
	m_serverImpl->Update(-1, false);
}
void GameServer::Start()
{
	m_timer.StartTimer();
	while (!ShouldExit())
	{
		Update();
	}
}
void GameServer::ServerImpl::OnMessage(std::shared_ptr<olc::net::connection< net::Message >> client, olc::net::message< net::Message >& msg)
{
	if (m_gameServer->m_shouldExit)
	{
		return;
	}

	switch (msg.header.id)
	{
	case net::NET_MSG_UPDATE_CAR:
	{
		// Received Car updates from PhysicsServer.
		// Redirect back to MasterServer.
		net::PlayerCarDesc car;
		msg >> car;

		net_msg msmsg;
		msmsg.header.id = net::NET_MSG_UPDATE_CAR;

		msmsg << car;

		m_gameServer->m_clientImpl->Send(msmsg);
	}
	break;



	default:
		break;
	}
}
void GameServer::ClientImpl::Update()
{
	if (!IsConnected() || m_gameServer->m_shouldExit)
	{
		m_gameServer->m_shouldExit = true;
		return;
	}

	// Trigger Physics and AI servers to do an update.
	if (m_gameServer->m_timer.MillisecondsElapsed() > m_gameServer->m_updateFrequency)
	{
		net_msg psmsg;
		psmsg.header.id = net::NET_MSG_PHYSICS_AI_SERVER_STEP_SIMULATION;

		m_gameServer->m_serverImpl->MessageAllClients(psmsg);
	}




	if (!Incoming().empty())
	{
		auto msg = Incoming().pop_front().msg;
		switch (msg.header.id)
		{
		case net::NET_MSG_REQUEST_USER_VALIDATION_DATA:
		{
			// We were validated by MasterServer and he wants to check our "PlayerData",
			// which we dont have as we are a GameServer.
			// Thus inform him that we are a GameServer and not Player.

			net::GameServerDesc desc;
			desc.id = m_gameServer->m_id;

			net_msg msmsg;
			msmsg.header.id = net::NET_MSG_SERVER_STARTUP;

			msmsg << desc;

			m_gameServer->m_clientImpl->Send(msmsg);
		}
		break;


		case net::NET_MSG_UPDATE_CAR:
		{
			// Received Car update from MasterServer.
			// Redirect to PhysicsServer and AIServer.

			net::PlayerCarDesc car;
			msg >> car;

			net_msg psmsg;
			psmsg.header.id = net::NET_MSG_UPDATE_CAR;

			psmsg << car;

			m_gameServer->m_serverImpl->MessageAllClients(psmsg);
		}
		break;


		default:
			break;
		}
	}
}