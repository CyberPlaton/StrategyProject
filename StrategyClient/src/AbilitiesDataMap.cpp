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

	void EntityAbilitiesDataMap::Data(cherrysoda::STL::Vector< SAbilityData* >& data)
	{

	}

}