#pragma once

#include "NetCommon.h"
#include "CommonDefines.h"
#include "Ability.h"


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

	private:
		static EntityAbilitiesDataMap* g_EntityAbilitiesDataMap;
	};

}