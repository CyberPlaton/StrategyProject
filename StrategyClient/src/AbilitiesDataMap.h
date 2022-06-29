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

		/// @brief Fill the Datastructure with EntityAbility data. This requires a connection with the MasterServer to be
		// already established.
		/// @return True, if filling was successful. False, if could not retrieve ANY data, if only a part of the data
		// is retrieved, then error messages are reported but function still returns True.
		bool Initialize();

		/// @brief Free data.
		void Terminate();

	private:
		static EntityAbilitiesDataMap* g_EntityAbilitiesDataMap;
	};

}