#include "EventSystem.h"

namespace cherrysoda
{
	EventSystem* EventEmitter::GetEventSystem()
	{
		Scene* scene = GetScene();
		for (auto& e : scene->GetEntitiesByTagMask(m_eventSystemID))
		{
			if (e->Tag() == m_eventSystemID)
			{
				return e->Get< EventSystem >();
			}
		}

		return nullptr;
	}
}

