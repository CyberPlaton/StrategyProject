#pragma once

#include "NetCommon.h"
#include "Timer.h"

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>
#include <RakNetTypes.h>
#include <RakNetTime.h>
#include <GetTime.h>


#include <sstream>
#include <string>
#include <map>

namespace net
{
	class ServerInterface
	{
	public:
		ServerInterface() = default;
		~ServerInterface()
		{
		}
		bool Initialize(uint32_t port, uint32_t max_connections, std::string* error_message);
		void Terminate();

		void Start();
		void Exit();


		void Send(RakNet::BitStream& stream, RakNet::SystemAddress& client);
		void Broadcast(RakNet::BitStream& stream, RakNet::SystemAddress& exception);		
	

		RakNet::RakString SystemAddress();
		void SystemAddress(RakNet::SocketDescriptor& socket);
		RakNet::RakString SystemAddress(RakNet::SystemAddress& addr);



	protected:
		RakNet::RakPeerInterface* m_instance;

		RakNet::RakString m_systemAddress;

		Timer timer;

	protected:
		virtual void OnUpdate() = 0;
		virtual void OnMessage(RakNet::Packet* packet) = 0;
		virtual void OnClientConnect(RakNet::Packet* packet) = 0;
		virtual void OnClientDisconnect(RakNet::Packet* packet) = 0;
		virtual bool OnClientValidated(RakNet::Packet* packet) = 0;

	protected:
		bool Running();

	private:

		bool m_running;

	private:
		void Update(RakNet::Time&& currentTime);
		bool Validate(RakNet::Packet* packet);
		std::string GetErrorString(RakNet::StartupResult result);
	};


}