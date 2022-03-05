#pragma once

#include "EventSystem.h"
#include "NetCommon.h"

namespace cherrysoda
{
	struct NetGameobjectUpdateEvent : public Event
	{
		NetGameobjectUpdateEvent(net::NetGameobject& object) : Event("NetGameobjectUpdate"), m_object( object )
		{
		}
		~NetGameobjectUpdateEvent()
		{
		}

		net::NetGameobject m_object;
	};


	class Observable : public EventEmitter
	{
	public:
		CHERRYSODA_DECLARE_COMPONENT(Observable, EventEmitter);

		Observable(size_t event_system_id) : EventEmitter(event_system_id)
		{
		}


		bool EmitCheck() override final
		{
			return true;
		}
		void EmitEvent() override final
		{
			// Get Data from Entity.

			// Put Data into net::NetGameobject and emit
			// an event with the Data.
		}

	};
}