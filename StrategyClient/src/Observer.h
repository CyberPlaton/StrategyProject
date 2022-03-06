#pragma once

#include "Observable.h"
#include "NetCommMngr.h"

namespace cherrysoda
{
	class Observer : public EventListener
	{
	public:
		Observer(const String& event_listen_type, size_t netId_to_listen_to) :
			EventListener(event_listen_type), m_netIdToListenTo(netId_to_listen_to)
		{
		}


		void operator()(Event* evnt) override final
		{
			printf("Observer::operator()\n");

			if (evnt->m_eventType.compare("NetGameobjectUpdate") == 0)
			{
				auto net_event = reinterpret_cast<NetGameobjectUpdateEvent*>(evnt);

				if (net_event->m_object.m_netId == m_netIdToListenTo)
				{
					printf("\tUpdate net gameobject\n");

					// Object to update is our responsibility.
					// Transmit data over network.
					NetCommMngr::get()->UpdateNetGameobject(net_event->m_object);
				}
			}
		}


	private:
		size_t m_netIdToListenTo = 0;
	};
}