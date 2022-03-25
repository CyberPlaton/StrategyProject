#include "ClientInterface.h"

namespace net
{
	void ClientInterface::Send(RakNet::BitStream& stream, RakNet::SystemAddress& server)
	{
		m_instance->Send(&stream, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0, server, false);
	}
	bool ClientInterface::Initialize(uint32_t port, const char* host)
	{
		using namespace RakNet;

		m_instance = RakPeerInterface::GetInstance();
		
		SocketDescriptor desc;
		auto startup = m_instance->Startup(1, &desc, 1);
		if(startup != RakNet::StartupResult::RAKNET_STARTED)
		{
			Terminate();
			return false;
		}

		auto connect = m_instance->Connect(host, port, 0, 0);
		if (connect != RakNet::ConnectionAttemptResult::CONNECTION_ATTEMPT_STARTED)
		{
			Terminate();
			return false;
		}

		m_running = true;
	}
	void ClientInterface::Terminate()
	{
		RakNet::RakPeerInterface::DestroyInstance(m_instance);
	}
	void ClientInterface::Update()
	{
		auto packet = m_instance->Receive();
		if (packet)
		{
			switch (packet->data[0])
			{
				case DefaultMessageIDTypes::ID_CONNECTION_REQUEST_ACCEPTED:
				{
					m_connected = true;
					OnConnected(packet);
					break;
				}

				case DefaultMessageIDTypes::ID_DISCONNECTION_NOTIFICATION:
				{
					m_connected = false;
					OnDisconnected(packet);
					break;
				}

				default:
				{
					OnMessage(packet);
					break;
				}
			}
		}
	}
	void ClientInterface::Update(double milliseconds)
	{
		Timer timer;
		timer.StartTimer();
		while (timer.MillisecondsElapsed() < milliseconds && Running())
		{
			Update();
		}
	}
	bool ClientInterface::Running()
	{
		return m_running;
	}
	void ClientInterface::Exit()
	{
		m_running = false;
	}
	bool ClientInterface::Connected()
	{
		return m_connected;
	}
}