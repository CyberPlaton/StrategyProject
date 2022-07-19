#ifndef _UNIT_H_
#define _UNIT_H_
#pragma once

#include "ComponentCommon.h"

// Selectable allows for defining actions to be taken on hovering/selecting/clicking on a unit
#include "SelectableUnit.h"

// Abilities definition and implementation for Unit and Building entities.
#include "Ability.h"


namespace cherrysoda
{
	class StatusEffectScriptingEngine;


	class Unit : public Component
	{
		friend class StatusEffectScriptingEngine;

	public:
		CHERRYSODA_DECLARE_COMPONENT(Unit, Component);

		Unit() : Unit(true, true)
		{
		}
		Unit::Unit(const String& unit_name,
						  size_t player_id,
						  size_t health,
						  size_t armor,
						  size_t defense,
						  size_t attack,
						  size_t tile_position_x,
						  size_t tile_position_y,
						  float position_x,
						  float position_y);

		// GETTER/SETTER
		DECLARE_GET_SET(const String&,  UnitName,			m_unitName);
		DECLARE_GET_SET(size_t,			PlayerId,			m_playerId);
		DECLARE_GET_SET(size_t,			Health,				m_health);
		DECLARE_GET_SET(size_t,			Armor,				m_armor);
		DECLARE_GET_SET(size_t,			Defense,			m_defense);
		DECLARE_GET_SET(size_t,			MinAttack,			m_minAttack);
		DECLARE_GET_SET(size_t,			MaxAttack,			m_maxAttack);
		DECLARE_GET_SET(bool,			Ranged,				m_ranged);
		DECLARE_GET_SET(size_t,			RangedMinAttack,	 m_rangedMinAttack);
		DECLARE_GET_SET(size_t,			RangedMaxAttack,	 m_rangedMaxAttack);
		DECLARE_GET_SET(size_t,			RangedMinRange,		m_rangedMinRange);
		DECLARE_GET_SET(size_t,			RangedMaxRange,		m_rangedMaxRange);
		DECLARE_GET_SET(size_t,			Level,				m_level);
		DECLARE_GET_SET(size_t,			Experience,			m_experience);
		DECLARE_GET_SET(size_t,			SightRadius,		m_sightRadius);
		DECLARE_GET_SET(size_t,			TilePositionX,		m_tilePositionX);
		DECLARE_GET_SET(size_t,			TilePositionY,		m_tilePositionY);
		DECLARE_GET_SET(float,			PositionX,			m_positionX);
		DECLARE_GET_SET(float,			PositionY,			m_positionY);

		// QUERYING
		DECLARE_QUERY(Dirty, m_isDirty);

		// COMMON FUNCTIONALITY
		
	private:
		String m_unitName;
		size_t m_playerId = 0;

		/// @brief Amount of damage the unit can take before dying.
		size_t m_health = 0;
		size_t m_armor = 0;
		size_t m_defense = 0;
		
		size_t m_minAttack = 0;
		size_t m_maxAttack = 0;
		bool m_ranged = false;
		size_t m_rangedMinAttack = 0;
		size_t m_rangedMaxAttack = 0;
		size_t m_rangedMinRange = 0;
		size_t m_rangedMaxRange = 0;

		size_t m_level = 1;
		size_t m_experience = 0;
		size_t m_sightRadius = 1;
		size_t m_tilePositionX = 0;
		size_t m_tilePositionY = 0;
		float  m_positionX = 0.0f;
		float  m_positionY = 0.0f;

		/// @brief 0=INVALID, 1=WALK, 2=SWIM, 3=RIDING
		size_t m_movementType = 0;

		/// @brief Basically the Action Points of the unit. Define how many actions it can perform in one round.
		size_t m_actionPoints = 0;

		bool m_isDirty = false;

	private:
		CHERRYSODA_FRIEND_CLASS_POOL;

		Unit(bool active, bool visible) : base(active, visible) {};
	};
}


#endif