#ifndef _ABILITY_BASE_H_
#define _ABILITY_BASE_H_
#pragma once

#include "AbilitiesDataMap.h"
#include "TurnStepMngr.h"
#include "Entity.h"
#include "Unit.h"
#include "Building.h"
#include "Logging.h"


namespace cherrysoda
{
	class CEntityAbilityBase : public IEntityAbility
	{
	public:
		CEntityAbilityBase(Entity* user_entity_of_the_ability);
		~CEntityAbilityBase();


		virtual void Use(Entity* target) override;


		Entity* Self() override final;
		void Data(net::SAbilityData* data) override final;
		virtual String Name() override;


	protected:
		/// @brief Data defining the ability and required for correct Use implementation.
		net::SAbilityData m_data;

		/// @brief Required to check that it is not used more than once per turn.
		/// Some Abilities have this constraint, others not, but it is very common.
		size_t m_last_turn_ability_was_used = 0;

	protected:
		/// @brief Check the default conditions for use of Ability. Does not
		/// allow multiple uses per turn.
		/// @param target The Entity on which to check the conditions. The target is not the unit which
		/// casts or uses the Ability, but rather the one ON which to use it. In some cases both can be the same...
		/// @return True, if all conditions are satisfied.
		bool CheckConditions(Entity* target);

		/// @brief Check the default conditions for use of Ability. Allow multiple uses per turn.
		/// @param target The Entity on which to check the conditions. The target is not the unit which
		/// casts or uses the Ability, but rather the one ON which to use it. In some cases both can be the same...
		/// @return True, if all conditions are satisfied.
		bool CheckConditionsAllowMultipleUse(Entity* target);


		/// @brief Check whether this Ability was used on current turn.
		/// @return True, if self unit has used this Ability on current turn.
		bool WasUsedThisTurn() const;

		/// @brief Check whether our self unit has enough action points to perform this ability.
		/// @return True, if self unit can perform action.
		bool HasEnoughActionPoints() const;

		/// @brief Check whether we can use the Ability on the target based
		/// on the specified usability in the data.
		/// @param target The target entity on which to perform the check.
		/// @return True, if we can use the Ability on the target.
		bool IsTargetValid(Entity* target);

		/// @brief Check whether the Target entity is ourselves.
		/// We do this by comparing the BitTagValue of both entities.
		/// @param target The target entity with which to compare.
		/// @return True if both are same; basically having the same BitTag.
		bool IsTargetSelf(Entity* target);

		/// @brief Check whether the Target entity is one of our own units.
		/// We do this by comparing the assigned Player ID.
		/// @param target The target entity which to check.
		/// @return True, if the Target is a friendly unit.
		bool IsTargetFriendly(Entity* target);
		
		/// @brief Check whether the Target entity is not one of our own units.
		/// We do this by comparing the assigned Player ID.
		/// @param target The target entity which to check.
		/// @return True, if the Target is an enemy unit.
		bool IsTargetEnemy(Entity* target);

		/// @brief Check whether this ability is applicable to the Target type of entity.
		/// E.g. some abilities can only be used on Unit types and other only on Map tile types.
		/// @param target The target entity.
		/// @return True, if we can apply ability on the Target.
		bool IsApplicableTo(Entity* target);

		/// @brief Check whether the target fulfills the Abilities range requirements.
		/// @param target The Entity on which to check.
		/// @return True, if range requirements are valid.
		bool IsTargetInRange(Entity* target);

	private:
		/// @brief The entity which uses the Ability.
		Entity* m_self = nullptr;
	};
}


#endif