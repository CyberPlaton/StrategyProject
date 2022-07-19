#pragma once

#include "ComponentCommon.h"

namespace cherrysoda
{
	// Forward decl.
	struct SAbilityData;


	/*
	* EntityAbility defines different actions available for a unit or a building.
	* We use the Command Pattern, meaning
	* the function Use(...) must be overriden with the Action defined by this ability.
	* E.g.
	* An attack ability overrides Use to hurt the target unit or building.
	* 
	* Furthermore abilities do not need an Update function. They do their thing once and stop.
	* For an Attack that causes Bleeding to the target over several turns, is created by an 
	* Attack that injects a StatusBleeding status into the target.
	*/
	class IEntityAbility
	{
	public:
		/// @brief Pointer to the entity this ability belongs to.
		/// @return Entity Pointer.
		virtual Entity* Self() = 0;

		/// @brief Use this ability on the target. The ability will the execute its functionality on it.
		/// @param target The target entity. Can be self too.
		virtual void Use(cherrysoda::Entity* target) = 0;

		/// @brief Set the data for the Ability.
		/// @param The Ability data from SAbilityData.
		virtual void Data(SAbilityData*) = 0;

		/// @brief Retrieve the Ability Name.
		/// @return Name.
		virtual String Name() = 0;
	};


	/*
	* Storage for all abilities available to an Entity.
	* Central access point to abilities queries and usage.
	* 
	* Every unit and building should have the EntityAbilityMap, otherwise they are not
	* interactable, meaning cant do things like "Move", "Produce" (another unit or building) etc.
	*/
	class EntityAbilityMap : public Component
	{
	public:
		CHERRYSODA_DECLARE_COMPONENT(EntityAbilityMap, Component);
		EntityAbilityMap() : EntityAbilityMap(true, false)
		{
		}

		void Use(const cherrysoda::String& name, cherrysoda::Entity* entity);
		void Add(const cherrysoda::String& name, IEntityAbility* ability);
		void Remove(const cherrysoda::String& name);
		bool Has(const cherrysoda::String& name);
		template < class T >
		T* Get(const cherrysoda::String& name)
		{
			return (Has(name) == true) ? reinterpret_cast< T* >( m_abilities[name] ) : nullptr;
		}


	private:
		EntityAbilityMap(bool active, bool visible) : base(active, visible) {};


	private:
		std::map< cherrysoda::String, IEntityAbility* > m_abilities;
	};


#define ENTITY_ADD_ABILITY(ability, entity) \
auto map = entity->Get< EntityAbilityMap >(); \
map->Add(ability->Name(), ability) \

#define ENTITY_REMOVE_ABILITY(ability_name, entity) \
auto map = entity->Get< EntityAbilityMap >(); \
map->Remove(ability_name) \

#define ENTITY_USE_ABILITY(ability_name, using_entity, target_entity) \
auto map = using_entity->Get< EntityAbilityMap >(); \
map->Use(ability_name, target_entity) \

}