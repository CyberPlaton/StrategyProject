#pragma once

#include "NetCommon.h"

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>
#include <RakNetTypes.h>
#include <RakNetTime.h>
#include <GetTime.h>


namespace net
{
	class ServerInterface
	{
	public:
		ServerInterface() = default;
		~ServerInterface()
		{
			Terminate();
		}
		bool Initialize(uint32_t port, uint32_t max_connections);
		void Terminate();
		void Start();
		void Send(RakNet::BitStream& stream, RakNet::SystemAddress& client);
		void Broadcast(RakNet::BitStream& stream, RakNet::SystemAddress& exception);
		void Exit();


	protected:
		RakNet::RakPeerInterface* m_instance;


	protected:
		virtual void OnMessage(RakNet::Packet* packet) = 0;
		virtual void OnClientConnect(RakNet::Packet* packet) = 0;
		virtual void OnClientDisconnect(RakNet::Packet* packet) = 0;
		virtual bool OnClientValidated(RakNet::Packet* packet) = 0;


	private:

		bool m_running;

	private:
		void Update(RakNet::Time&& currentTime);
		bool Running();
		bool Validate(RakNet::Packet* packet);
	};


}