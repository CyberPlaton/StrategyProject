#ifndef _REST_ABILITY_H_
#define _REST_ABILITY_H_
#pragma once

#include "AbilityBase.h"
#include "StatusEffect.h"

namespace cherrysoda
{
	class CEntityAbilityRest : public CEntityAbilityBase
	{
	public:
		CEntityAbilityRest(Entity* user_entity_of_the_ability) : CEntityAbilityBase(user_entity_of_the_ability){}


		void Use(Entity* target) override final
		{
			// Check for all conditions.
			if (!CheckConditions(target)) return;

			// Everything in order. Do the Ability.
			
			// If the unit has a Status Effect applied that can be removed by Resting,
			// remove it and return.
			auto se_vec = target->Get< CEntityStatusEffectMngr >()->GetAllAppliedStatusEffects();
			for(auto& se: se_vec)
			{
				if(se->TriviallyRemovable())
				{
					ENTITY_REMOVE_STATUS_EFFECT(se->Name(), target);
					return;
				}
			}
			
			// Else just increase his Health by 5.
			target->Get< Unit >()->SetHealth(target->Get< Unit >()->GetHealth() + 5);


			// At the end, call the Base Ability to get the current turn number.
			CEntityAbilityBase::Use(target);
		}

		String Name() override final {return "ABL_Rest";}

	private:
		
	};
}

#endif