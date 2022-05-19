#include "ServerInterface.h"

namespace net
{
	std::string ServerInterface::GetErrorString(RakNet::StartupResult result)
	{
		switch (result)
		{
		case RakNet::RAKNET_ALREADY_STARTED:
			return "RakNet Already Started";
		case RakNet::INVALID_SOCKET_DESCRIPTORS:
			return "Invalid Socket Descriptors";
		case RakNet::INVALID_MAX_CONNECTIONS:
			return "Invalid Max Connections";
		case RakNet::SOCKET_FAMILY_NOT_SUPPORTED:
			return "Socket Family Not Supported";
		case RakNet::SOCKET_PORT_ALREADY_IN_USE:
			return "Socket Port Already In Use";
		case RakNet::SOCKET_FAILED_TO_BIND:
			return "Socket Failed To Bind";
		case RakNet::SOCKET_FAILED_TEST_SEND:
			return "Socket Failed Test Send";
		case RakNet::PORT_CANNOT_BE_ZERO:
			return "Port Cannot Be Zero";
		case RakNet::FAILED_TO_CREATE_NETWORK_THREAD:
			return "Failed To Create Network Thread";
		case RakNet::COULD_NOT_GENERATE_GUID:
			return "Could Not Generate GUID";
		case RakNet::STARTUP_OTHER_FAILURE:
			return "Startup Other Failure";
		default:
			return "Unknown Startup Error";
		}
	}

	bool ServerInterface::Initialize(uint32_t port, uint32_t max_connections, std::string* error_message)
	{
		using namespace RakNet;

		m_instance = RakPeerInterface::GetInstance();

		SocketDescriptor desc(port, "192.168.178.43");
		desc.socketFamily = AF_INET;

		auto result = m_instance->Startup(max_connections, &desc, 1);
		if (result != StartupResult::RAKNET_STARTED)
		{
			error_message->append(GetErrorString(result));
			return false;
		}

		m_instance->SetMaximumIncomingConnections(max_connections);
		ServerInterface::SystemAddress(desc);
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
			OnUpdate();
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
				case DefaultMessageIDTypes::ID_NEW_INCOMING_CONNECTION:
				{
					// Raknets security passed. Our Turn...
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

				case DefaultMessageIDTypes::ID_CONNECTION_LOST:
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
		return true;
	}
	void ServerInterface::Send(RakNet::BitStream& stream, RakNet::SystemAddress& client)
	{
		m_instance->Send(&stream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, client, false);
	}
	void ServerInterface::Broadcast(RakNet::BitStream& stream, RakNet::SystemAddress& exception)
	{
		m_instance->Send(&stream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, exception, true);
	}
	RakNet::RakString ServerInterface::SystemAddress()
	{
		return m_systemAddress;
	}
	void ServerInterface::SystemAddress(RakNet::SocketDescriptor& socket)
	{
		const char* host = &socket.hostAddress[0];
		m_systemAddress.AppendBytes(host, strlen(host));

		m_systemAddress.AppendBytes("|", strlen("|"));

		std::stringstream ss;
		ss << socket.port;
		std::string port = ss.str();
		m_systemAddress.AppendBytes(port.c_str(), port.size());
	}
	RakNet::RakString ServerInterface::SystemAddress(RakNet::SystemAddress& addr)
	{
		return addr.ToString();
	}
}