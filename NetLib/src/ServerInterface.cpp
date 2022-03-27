#include "ServerInterface.h"

namespace net
{
	bool ServerInterface::Initialize(uint32_t port, uint32_t max_connections)
	{
		using namespace RakNet;

		m_instance = RakPeerInterface::GetInstance();

		SocketDescriptor desc(port, "192.168.178.43");
		desc.socketFamily = AF_INET;

		auto result = m_instance->Startup(max_connections, &desc, 1);
		if (result != StartupResult::RAKNET_STARTED)
		{
			return false;
		}

		m_instance->SetMaximumIncomingConnections(max_connections);
		ServerInterface::SystemAdress(desc);
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
					printf("[ServerInterface] Message: ID_CONNECTION_REQUEST\n");
					break;
				}

				case DefaultMessageIDTypes::ID_NEW_INCOMING_CONNECTION:
				{
					printf("[ServerInterface] Message: ID_NEW_INCOMING_CONNECTION\n");
					// Raknet security passed. Our Turn...
					if (Validate(packet))
					{
						if (!OnClientValidated(packet))
						{
							// Dont accept.
							// Drop connection.
						}

						OnClientConnect(packet);
					}
					break;
				}

				case DefaultMessageIDTypes::ID_DISCONNECTION_NOTIFICATION:
				{
					printf("[ServerInterface] Message: ID_DISCONNECTION_NOTIFICATION\n");
					OnClientDisconnect(packet);
					break;
				}


				default:
				{
					printf("[ServerInterface] Message: %d\n", (int)packet->data[0]);
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
		return true;
	}
	void ServerInterface::Send(RakNet::BitStream& stream, RakNet::SystemAddress& client)
	{
		m_instance->Send(&stream, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0, client, false);
	}
	void ServerInterface::Broadcast(RakNet::BitStream& stream, RakNet::SystemAddress& exception)
	{
		m_instance->Send(&stream, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0, exception, true);
	}
	RakNet::RakString ServerInterface::SystemAdress()
	{
		return m_systemAdress;
	}
	void ServerInterface::SystemAdress(RakNet::SocketDescriptor& socket)
	{
		const char* host = &socket.hostAddress[0];
		m_systemAdress.AppendBytes(host, strlen(host));

		m_systemAdress.AppendBytes("|", strlen("|"));

		std::stringstream ss;
		ss << socket.port;
		std::string port = ss.str();
		m_systemAdress.AppendBytes(port.c_str(), port.size());
	}
}