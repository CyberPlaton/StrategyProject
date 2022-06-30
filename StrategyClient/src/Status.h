#pragma once

#include "ComponentCommon.h"
// Required for CEntityStatusEffectMngr and the system of Turn Based game.
#include "EventSystem.h"

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




	/// @brief Manages the execution and deletion of Status Effects on an Entity.
	// The execution is Turn Based and performed on Turn End.
	class CEntityStatusEffectMngr : public EventListener
	{
	public:
		CEntityStatusEffectMngr() : EventListener("TurnEnd") {}

		/// @brief Execute all status effects update function on Turn End. Removes those that are not needed anymore and remain those that should stay.
		/// @param evnt Event to be processed.
		void operator()(Event* evnt) override final
		{
			if (evnt->m_eventType.compare("TurnEnd") != 0) return;

			std::vector< EntityStatusEffect* > remaining_effects;

			// Execute and store which should remain.
			for(int i = 0; i < m_statusEffectsVec.size(); i++)
			{
				auto& effect = m_statusEffectsVec[i];

				if(effect->OnUpdate() == false)
				{
					remaining_effects.push_back(m_statusEffectsVec[i]);
				}
			}

			// Remove all Status Effects and re-append remaining.
			while(m_statusEffectsVec.size() > 0)
			{
				auto effect = m_statusEffectsVec[0];
				delete effect;
				m_statusEffectsVec.erase(m_statusEffectsVec.begin());
			}

			for(int i = 0; i < remaining_effects.size(); i++)
			{
				m_statusEffectsVec.push_back(remaining_effects[i]);
			}
		}



	private:
		std::vector< EntityStatusEffect* > m_statusEffectsVec;
	};
}