#ifndef _CHERRYSODA_EVENT_H_
#define _CHERRYSODA_EVENT_H_


namespace cherrysoda
{
	struct Event
	{
		Event(const String& event_type) : m_eventType(event_type){}
		~Event() = default;

		String m_eventType;
	};
}


#endif