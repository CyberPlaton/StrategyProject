#pragma once

#include "NetCommon.h"
#include "Timer.h"

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>
#include <RakNetTypes.h>
#include <RakNetTime.h>
#include <GetTime.h>

namespace net
{
	class ClientInterface
	{
	public:
		ClientInterface() = default;
		~ClientInterface()
		{
			Terminate();
		}
		bool Initialize(uint32_t port, const char* host);
		void Terminate();
		void Update();
		void Update(double milliseconds);
		void Send(RakNet::BitStream& stream, RakNet::SystemAddress& server);
		void Exit();
		bool Connected();

	protected:
		RakNet::RakPeerInterface* m_instance;

		bool m_running;
		bool m_connected;

	protected:
		virtual void OnConnected(RakNet::Packet* packet) = 0;
		virtual void OnDisconnected(RakNet::Packet* packet) = 0;
		virtual void OnMessage(RakNet::Packet* packet) = 0;


	private:


	private:
		bool Running();
	};
}