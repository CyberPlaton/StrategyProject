#pragma once

#include "SceneCommon.h"
#include "GamerService.h"

namespace cherrysoda
{
	class InitializationScene : public State
	{
	public:
		DECLARE_SCENE_COMMON(InitializationScene);


	private:
		bool m_abilityDataDownloadComplete = false;
		bool m_statusEffectsDataDownloadComplete = false;
	};
}
