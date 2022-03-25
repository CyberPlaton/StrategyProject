#pragma once

#include "ComponentCommon.h"
#include <CherrySoda/Entity.h>

#include <map>

namespace cherrysoda
{
	/*
	* Ability component for defining different actions available for a unit or a building.
	* We use the Command Pattern, meaning
	* the function Use(...) must be overriden with the Action defined by this ability.
	* E.g.
	* An attack ability overrides Use to hurt the target unit or building.
	*/
	class Ability : public Component
	{
	public:
		CHERRYSODA_DECLARE_COMPONENT(Ability, Component);

		virtual void Use(cherrysoda::Entity* target) = 0;

		void Added(const cherrysoda::String& name, Ability* ability, cherrysoda::Entity* entity);
		void Removed(const cherrysoda::String& name, cherrysoda::Entity* entity);
	};

	/*
	* Storage for all abilities available to an Entity.
	* Central access point to abilities queries and usage.
	*/
	class EntityAbilityMap : public Component
	{
	public:
		CHERRYSODA_DECLARE_COMPONENT(EntityAbilityMap, Component);

		void Use(const cherrysoda::String& name, cherrysoda::Entity* entity);
		void Add(const cherrysoda::String& name, Ability* ability);
		void Remove(const cherrysoda::String& name);
		bool Has(const cherrysoda::String& name);
		template < class T >
		T* Get(const cherrysoda::String& name)
		{
			return (Has(name) == true) ? reinterpret_cast< T* >( m_abilities[name] ) : nullptr;
		}

	private:

		std::map< cherrysoda::String, Ability* > m_abilities;
	};


#define ADD_ABILITY(name, ability, entity) \
auto map = entity->Get< EntityAbilityMap >(); \
map->Add(name, ability); \

#define REMOVE_ABILITY(name, entity) \
auto map = entity->Get< EntityAbilityMap >(); \
map->Remove(name); \


}