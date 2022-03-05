#ifndef _CHERRYSODA_COMPONENTS_EVENT_SYSTEM_H_
#define _CHERRYSODA_COMPONENTS_EVENT_SYSTEM_H_

#include <CherrySoda/Components/Component.h>
#include <CherrySoda/Util/Pool.h>
#include <CherrySoda/Util/String.h>
#include <CherrySoda/Util/STL.h>
#include <CherrySoda/Scene.h>
#include <CherrySoda/Entity.h>

namespace cherrysoda
{
	class EventSystem;
	class EventListener;
	class EventEmitter;

	struct Event
	{
		Event(const String& event_type) : m_eventType(event_type) {}
		virtual ~Event(){}

		String m_eventType;
	};


	class EventEmitter : public Component
	{
	public:
		CHERRYSODA_DECLARE_COMPONENT(EventEmitter, Component);


		EventEmitter(size_t event_system_id) : base(true, false)
		{
			m_eventSystemID = event_system_id;
		}

		void Update() override final
		{
			if (EmitCheck()) EmitEvent();
		}

	protected:
		CHERRYSODA_FRIEND_CLASS_POOL;

		size_t m_eventSystemID;

	protected:

		virtual void EmitEvent() {}
		virtual bool EmitCheck() { return false; }
		EventSystem* GetEventSystem();
	};



	class EventListener : public Component
	{
	public:
		CHERRYSODA_DECLARE_COMPONENT(EventListener, Component);

		EventListener(const String& event_listen_type) : EventListener(true, false)
		{
			m_listenEventType = event_listen_type;

		}

		// The Delegate function that is called
		// on the occurance of the to listen event type.
		virtual void operator()(Event* evnt)
		{
		}

		const String& EventType() { return m_listenEventType; }

	private:
		CHERRYSODA_FRIEND_CLASS_POOL;

		EventListener(bool active, bool visible) : base(active, visible) {};


	private:
		String m_listenEventType;
	};



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