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

	void EntityStatusEffectsDataMap::Data(net::SStatusEffectData& data)
	{
		m_data.AddData(data.m_effectName.C_String(), data);
	}

	net::SStatusEffectData& EntityStatusEffectsDataMap::Data(const std::string& name)
	{
		return m_data.RetrieveData(name.c_str());
	}

}