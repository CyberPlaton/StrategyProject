#ifndef _CHERRYSODA_COMPONENTS_EVENT_SYSTEM_H_
#define _CHERRYSODA_COMPONENTS_EVENT_SYSTEM_H_

#include <CherrySoda/Components/Component.h>
#include <CherrySoda/Util/Pool.h>
#include <CherrySoda/Util/String.h>
#include <CherrySoda/Util/STL.h>

#include "Event.h"
#include "EventListener.h"
#include "EventEmitter.h"


namespace cherrysoda
{
	class EventSystem : public Component
	{
	public:
		CHERRYSODA_DECLARE_COMPONENT(EventSystem, Component);

		EventSystem() : EventSystem(true, false)
		{
		}


		// Update the Event System.
		// Redirect all gathered Events to Listeners.
		void Update() override final
		{
            // Get Active Queue q.

            // For each Event e in q:
                // Get Type t of Event e.
                // Get a List l of all Listeners for the Event Type t.
                    // For each Listener lst in List l:
                        // Get a List l_ of all Delegates of the Listener.
                            // For each Delegate d in l_:
                                // Call Function d();


            // Store unprocessed events (due to time constraints) in other queue.

            // Clear Active Queue q.
		}


		// Add an Event to the Event System for Redirecting.
		void Add(Event* evnt)
		{
            m_realtimeQueue[m_activeQueue == 0 ? 1 : 0].push(evnt);
		}

		void Add(EventListener* listener, const String& event_listen_type)
		{
			STL::Add<String, EventListener*>(m_listenerMap, STL::MakePair(event_listen_type, listener));
		}

	private:
		CHERRYSODA_FRIEND_CLASS_POOL;

		EventSystem(bool active, bool visible) : base(active, visible) {};


	private:

        int m_activeQueue;
        STL::Queue<Event*> m_realtimeQueue[2];
		STL::HashMap<String, EventListener*> m_listenerMap;
	};
}


#endif