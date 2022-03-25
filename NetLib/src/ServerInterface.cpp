#include "ServerInterface.h"

namespace net
{
	bool ServerInterface::Initialize(uint32_t port, uint32_t max_connections)
	{
		using namespace RakNet;

		m_instance = RakPeerInterface::GetInstance();

		SocketDescriptor desc(port, 0);
		desc.socketFamily = AF_INET;

		auto result = m_instance->Startup(max_connections, &desc, 1);
		if (result != StartupResult::RAKNET_STARTED)
		{
			Terminate();
			return false;
		}

		m_instance->SetMaximumIncomingConnections(max_connections);

		m_running = true;
		return true;
	}
	void ServerInterface::Terminate()
	{
		RakNet::RakPeerInterface::DestroyInstance(m_instance);
	}
	void ServerInterface::Start()
	{
		while (Running())
		{
			Update(RakNet::GetTime());
		}
	}
	void ServerInterface::Update(RakNet::Time&& currentTime)
	{
		auto packet = m_instance->Receive();
		if (packet)
		{
			switch (packet->data[0])
			{
				case DefaultMessageIDTypes::ID_CONNECTION_REQUEST:
				{
					if (Validate(packet))
					{
						OnClientValidated(packet);
					}
					break;
				}

				case DefaultMessageIDTypes::ID_NEW_INCOMING_CONNECTION:
				{
					OnClientConnect(packet);
					break;
				}

				case DefaultMessageIDTypes::ID_DISCONNECTION_NOTIFICATION:
				{
					OnClientDisconnect(packet);
					break;
				}


				default:
				{
					OnMessage(packet);
					break;
				}
			}
		}
		m_instance->DeallocatePacket(packet);
	}		
	void ServerInterface::Exit()
	{
		m_running = false;
	}
	bool ServerInterface::Running()
	{
		return m_running;
	}
	bool ServerInterface::Validate(RakNet::Packet* packet)
	{
		return false;
	}
	void ServerInterface::Send(RakNet::BitStream& stream, RakNet::SystemAddress& client)
	{
		m_instance->Send(&stream, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0, client, false);
	}
	void ServerInterface::Broadcast(RakNet::BitStream& stream, RakNet::SystemAddress& exception)
	{
		m_instance->Send(&stream, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0, exception, true);
	}

}