#include "AbilitiesDataMap.h"

namespace cherrysoda
{
	EntityAbilitiesDataMap* EntityAbilitiesDataMap::g_EntityAbilitiesDataMap = nullptr;


	bool EntityAbilitiesDataMap::Initialize()
	{
		return true;
	}

	void EntityAbilitiesDataMap::Terminate()
	{

	}

	void EntityAbilitiesDataMap::Data(net::SAbilityData& data)
	{
		m_data.AddData(data.m_abilityName.C_String(), data);
	}

	net::SAbilityData& EntityAbilitiesDataMap::Data(const std::string& name)
	{
		return m_data.RetrieveData(name.c_str());
	}

}