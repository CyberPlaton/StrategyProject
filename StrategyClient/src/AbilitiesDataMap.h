#ifndef _ABILITIES_DATA_MAP_H_
#define _ABILITIES_DATA_MAP_H_
#pragma once

#include "CommonDefines.h"
#include "Ability.h"
#include "HashMap.h"


namespace cherrysoda
{
	/// @brief Datastructure holding the data for each in-game available EntityAbility.
	// It is filled with data once on start up and later used to retrieve data for an Ability.
	class EntityAbilitiesDataMap
	{
	public:
		STATIC_GET_DEL_WITH_INIT_AND_SHUTDOWN(EntityAbilitiesDataMap, g_EntityAbilitiesDataMap);

		/// @brief
		/// @return
		bool Initialize();

		/// @brief Free data.
		void Terminate();

		/// @brief Store given data of each Ability.
		/// @param data Data vector.
		void Data(net::SAbilityData& data);

		net::SAbilityData& Data(const std::string& name);

	private:
		static EntityAbilitiesDataMap* g_EntityAbilitiesDataMap;

		CHashMap< net::SAbilityData > m_data;
	};

}


#endif