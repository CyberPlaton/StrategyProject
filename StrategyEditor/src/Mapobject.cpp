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

void ComponentCity::RemoveBuildingSlot(int x, int y)
{
	for (auto it = m_buildingSlots.begin(); it != m_buildingSlots.end(); it++)
	{
		auto pair = *it;
		if (pair.first == x && pair.second == y)
		{
			m_buildingSlots.erase(it);
			return;
		}
	}
}
void ComponentCity::RemoveTerritory(int x, int y)
{
	for (auto it = m_territory.begin(); it != m_territory.end(); it++)
	{
		auto pair = *it;
		if (pair.first == x && pair.second == y)
		{
			m_territory.erase(it);
			return;
		}
	}
}

bool ComponentCity::HasBuildingSlot(int x, int y)
{
	for (auto it = m_buildingSlots.begin(); it != m_buildingSlots.end(); it++)
	{
		auto pair = *it;
		if (pair.first == x && pair.second == y)
		{
			return true;
		}
	}
	return false;
}
bool ComponentCity::HasTerritory(int x, int y)
{
	for (auto it = m_territory.begin(); it != m_territory.end(); it++)
	{
		auto pair = *it;
		if (pair.first == x && pair.second == y)
		{
			return true;
		}
	}
	return false;
}