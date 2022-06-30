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


		/// @brief
		/// @return
		bool Initialize();

		/// @brief Free data.
		void Terminate();

		/// @brief Store given data of each Status Effect.
		/// @param data Data vector.
		void Data(cherrysoda::STL::Vector< SStatusEffectData* >& data);

	private:
		static EntityStatusEffectsDataMap* g_EntityStatusEffectsDataMap;
	};

}