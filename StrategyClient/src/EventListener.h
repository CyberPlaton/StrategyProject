#ifndef _CHERRYSODA_COMPONENTS_EVENT_LISTENER_H_
#define _CHERRYSODA_COMPONENTS_EVENT_LISTENER_H_

#include <CherrySoda/Components/Component.h>
#include <CherrySoda/Util/Pool.h>
#include <CherrySoda/Util/String.h>
#include <CherrySoda/Util/STL.h>

#include "Event.h"


namespace cherrysoda
{
	class EventListener : public Component
	{
	public:
		CHERRYSODA_DECLARE_COMPONENT(EventListener, Component);

		EventListener(const String& event_listen_type) : EventListener(true, false)
		{
			m_listenEventType = event_listen_type;

		}

		void operator()(Event* evnt)
		{
			// TODO	
			
		}

		const String& EventType() { return m_listenEventType; }

	private:
		CHERRYSODA_FRIEND_CLASS_POOL;

		EventListener(bool active, bool visible) : base(active, visible) {};


	private:
		String m_listenEventType;
	};
}


#endif