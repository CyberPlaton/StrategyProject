#pragma once

#include "Observable.h"
#include "NetCommMngr.h"
#include "Logging.h"

namespace cherrysoda
{
	/*
	* Usage Example:
	* 
	* 0) we need an event system in the scene. Thus first create one.
	* 	auto event_system = factory->Begin(this)
		.Add(new EventSystem())
		.End();
	* 
	* 1) create an entity (a net entity): The "observable" unit which data is used for updating net data...
	* 	auto unit = factory->Begin(this)
		.Add(new Unit("Spearman", 1, 50, 200, 25, 25, 2, 1, 256.0f, 128.0f))
		.Add(new Observable(event_system->Tag()))
		.End();
	* 
	* 2) create an event listener ( a net event listener = "observer" )
	* 	auto observer = factory->Begin(this)
		.Add(new Observer("NetGameobjectUpdate", unit->Get< Observable >()->GetNetId()))
		.End();
	* 
	* 2.1) register the listener in the event system
	* 	event_system->Get< EventSystem >()->Add(observer->Get< Observer >(), "NetGameobjectUpdate");
	* 
	*/



	class Observer : public EventListener
	{
	public:
		Observer(const String& event_listen_type, size_t netId_to_listen_to) :
			EventListener(event_listen_type), m_netIdToListenTo(netId_to_listen_to)
		{
		}


		void operator()(Event* evnt) override final
		{
			LOG_DBG_INFO("[{:.4f}][Observer::operator()] Processing: \"{}\".", APP_RUN_TIME(), evnt->m_eventType);
			LOG_GAME_INFO("[%.4f][Observer::operator()] Processing: \"%s\".", APP_RUN_TIME(), evnt->m_eventType.c_str());

			if (evnt->m_eventType.compare("NetGameobjectUpdate") == 0)
			{
				auto net_event = reinterpret_cast<NetGameobjectUpdateEvent*>(evnt);

				if (net_event->m_object->m_networkId == m_netIdToListenTo)
				{
					LOG_DBG_INFO("[{:.4f}][Observer::operator()] Update net::SGameobject: \"{}\".", APP_RUN_TIME(), net_event->m_object->m_networkId);
					LOG_GAME_INFO("[%.4f][Observer::operator()] Update net::SGameobject: \"%z\".", APP_RUN_TIME(), net_event->m_object->m_networkId);
					
					// Transmit data over network.
					NetCommMngr::get()->UpdateNetGameobject(net_event->m_object);

					// Release the object from memory after sending over network.

				}
			}
		}


	private:
		size_t m_netIdToListenTo = 0;
	};
}