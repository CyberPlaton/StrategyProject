#include "Ability.h"

namespace cherrysoda
{
	void EntityAbilityMap::Add(const cherrysoda::String& name, IEntityAbility* ability)
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