#pragma once

#include "ComponentCommon.h"

namespace cherrysoda
{
	// Forward decl.
	struct SStatusEffectData;


	/// @brief 
	class EntityStatusEffect
	{
	public:
		/// @brief Pointer to the entity this effect is applied to.
		/// @return Entity Pointer.
		virtual Entity* Self() = 0;

		/// @brief Perform an update.
		/// @return True, if this effect stays to the next update; False if it can be removed and deleted.
		virtual bool OnUpdate() = 0;

		/// @brief Set the data for the status effect.
		/// @param The status effect data from SStatusEffectData.
		virtual void Data(SStatusEffectData*) = 0;
	};
}