#include "EventSystem.h"

namespace cherrysoda
{
	EventSystem* EventEmitter::GetEventSystem()
	{
		auto scene = GetScene();
		auto entities = scene->GetEntitiesByTagMask((cherrysoda::BitTagValueType)m_eventSystemID);
		auto entity = entities.front();
		return entity->Get< EventSystem >();
	}
}

