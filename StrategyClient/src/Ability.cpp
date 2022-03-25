#include "Ability.h"

namespace cherrysoda
{
	void Ability::Added(const cherrysoda::String& name, Ability* ability, cherrysoda::Entity* entity)
	{
		CHERRYSODA_ASSERT(entity->Get< EntityAbilityMap >() != nullptr, "Entity holding abilities requires an \"EntityAbilityMap\" component!");
		ADD_ABILITY(name, ability, entity);
	}
	void Ability::Removed(const cherrysoda::String& name, cherrysoda::Entity* entity)
	{
		CHERRYSODA_ASSERT(entity->Get< EntityAbilityMap >() != nullptr, "Entity holding abilities requires an \"EntityAbilityMap\" component!");
		REMOVE_ABILITY(name, entity);
	}
	void EntityAbilityMap::Add(const cherrysoda::String& name, Ability* ability)
	{
		m_abilities.try_emplace(name, ability);
	}
	void EntityAbilityMap::Remove(const cherrysoda::String& name)
	{
		if (m_abilities.find(name) != m_abilities.end())
		{
			m_abilities.erase(name);
		}
	}
	bool EntityAbilityMap::Has(const cherrysoda::String& name)
	{
		return m_abilities.find(name) != m_abilities.end();
	}
	void EntityAbilityMap::Use(const cherrysoda::String& name, cherrysoda::Entity* entity)
	{
		if (Has(name)) m_abilities[name]->Use(entity);
	}
}