#pragma once

#include "NetCommon.h"
#include "CommonDefines.h"

#include "Status.h"

namespace cherrysoda
{
	/// @brief Datastructure holding the data for each in-game available EntityStatusEffect.
	// It is filled with data once on start up and later used to retrieve data for a StatusEffect.
	class EntityStatusEffectsDataMap
	{
	public:
		STATIC_GET_DEL_WITH_INIT_AND_SHUTDOWN(EntityStatusEffectsDataMap, g_EntityStatusEffectsDataMap);

		/// @brief Fill the Datastructure with StatusEffect data. This requires a connection with the MasterServer to be
		// already established.
		/// @return True, if filling was successful. False, if could not retrieve ANY data, if only a part of the data
		// is retrieved, then error messages are reported but function still returns True.
		bool Initialize();

		/// @brief Free data.
		void Terminate();

	private:
		static EntityStatusEffectsDataMap* g_EntityStatusEffectsDataMap;
	};

}