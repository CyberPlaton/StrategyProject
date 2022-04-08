#include "Mapobject.h"


void Entity::Update()
{
	if (Dirty())
	{
		if (m_children.size() > 0)
		{
			// Update children position relative to own position.
			for (auto& k : m_children)
			{
				k->m_positionx = m_positionx + (m_positionx - k->m_positionx);
				k->m_positiony = m_positiony + (m_positiony - k->m_positiony);
			}

		}
		m_dirty = false;
	}

	// Update components.
	for (auto& c : m_components)
	{
		c.second->Update();
	}

	// Update children.
	for (auto& k : m_children)
	{
		k->Update();
	}
}


Component* Entity::Get(std::string component)
{
	if (Has(component))
	{
		return m_components[component];
	}
	return false;
}

bool Entity::Has(std::string component)
{
	if (m_components.find(component) != m_components.end()) return true;
	return false;
}