#include "ClientInterface.h"

namespace net
{
	void ClientInterface::Send(RakNet::BitStream& stream, RakNet::SystemAddress& server)
	{
		m_instance->Send(&stream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, server, false);
	}
	bool ClientInterface::Initialize(uint32_t port, const char* host)
	{
		using namespace RakNet;

		m_instance = RakPeerInterface::GetInstance();
		
		SocketDescriptor desc;
		auto startup = m_instance->Startup(1, &desc, 1);
		if(startup != RakNet::StartupResult::RAKNET_STARTED)
		{
			return false;
		}

		auto connect = m_instance->Connect(host, port, 0, 0);
		if (connect != RakNet::ConnectionAttemptResult::CONNECTION_ATTEMPT_STARTED)
		{
			return false;
		}

		m_running = true;
	}
	void ClientInterface::Terminate()
	{
		m_running = false;
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
					printf("[ClientInterface] Message: ID_CONNECTION_REQUEST_ACCEPTED\n");
					m_connected = true;
					OnConnected(packet);
					break;
				}

				case DefaultMessageIDTypes::ID_DISCONNECTION_NOTIFICATION:
				{
					printf("[ClientInterface] Message: ID_DISCONNECTION_NOTIFICATION\n");
					m_connected = false;
					OnDisconnected(packet);
					break;
				}

				case DefaultMessageIDTypes::ID_CONNECTION_ATTEMPT_FAILED:
				{
					printf("[ClientInterface] Message: ID_CONNECTION_ATTEMPT_FAILED\n");
					break;
				}

				case DefaultMessageIDTypes::ID_NO_FREE_INCOMING_CONNECTIONS:
				{
					printf("[ClientInterface] Message: ID_NO_FREE_INCOMING_CONNECTIONS\n");
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
	RakNet::Packet* ClientInterface::PopNextMessage()
	{
		return m_instance->Receive();
	}
	void ClientInterface::DeallocateMessage(RakNet::Packet* p)
	{
		m_instance->DeallocatePacket(p);
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
	bool ClientInterface::Connected()
	{
		return m_connected;
	}
}