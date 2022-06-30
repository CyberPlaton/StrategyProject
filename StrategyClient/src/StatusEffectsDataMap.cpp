#include "StatusEffectsDataMap.h"

namespace cherrysoda
{
	EntityStatusEffectsDataMap* EntityStatusEffectsDataMap::g_EntityStatusEffectsDataMap;

	
	bool EntityStatusEffectsDataMap::Initialize()
	{
		return true;
	}

	void EntityStatusEffectsDataMap::Terminate()
	{

	}

	void EntityStatusEffectsDataMap::Data(net::SStatusEffectDataStorageObject& data)
	{

	}

}