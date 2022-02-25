#ifndef _CHERRYSODA_COMPONENTS_EVENT_EMITTER_H_
#define _CHERRYSODA_COMPONENTS_EVENT_EMITTER_H_

#include <CherrySoda/Components/Component.h>
#include <CherrySoda/Util/Pool.h>
#include <CherrySoda/Util/String.h>

#include <CherrySoda/Util/STL.h>


namespace cherrysoda
{
	using EmitCheckAction = STL::Func<bool>;

	class EventEmitter : public Component
	{
	public:
		CHERRYSODA_DECLARE_COMPONENT(EventEmitter, Component);

		
		EventEmitter(const EmitCheckAction& action, size_t event_system_id) : base(true, false)
		{
			m_emitCheck = action;
			m_eventSystemID = event_system_id;
		}
	
		void Update() override final
		{
			if (m_emitCheck())
			{
				// EMIT TODO
				printf_s("Emit Event\n");
			}
		}

	private:
		CHERRYSODA_FRIEND_CLASS_POOL;

		EmitCheckAction m_emitCheck;
		size_t m_eventSystemID;
	};
}


#endif