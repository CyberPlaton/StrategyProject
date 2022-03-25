#pragma once

#include "Ability.h"

namespace cherrysoda
{
	class MeeleAttackAbility : public Ability
	{
	public:
		CHERRYSODA_DECLARE_COMPONENT(MeeleAttackAbility, Ability);

		virtual void Use(cherrysoda::Entity* target) override
		{
		}

		void Added(cherrysoda::Entity* entity) override final
		{
			base::Added("Meele Attack", this, entity);
		}
		void Removed(Entity* entity) override final
		{
			base::Removed("Meele Attack", entity);
		}

	};


	class RangeAttackAbility : public Ability
	{
	public:
		CHERRYSODA_DECLARE_COMPONENT(RangeAttackAbility, Ability);

		virtual void Use(cherrysoda::Entity* target)
		{

		}

		void Added(cherrysoda::Entity* entity) override final
		{
			base::Added("Range Attack", this, entity);
		}
		void Removed(Entity* entity) override final
		{
			base::Removed("Range Attack", entity);
		}

	};

}