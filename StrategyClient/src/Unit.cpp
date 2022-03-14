#include "Unit.h"

namespace cherrysoda
{
	Unit::Unit(const String& unit_name,
		size_t player_id,
		size_t health,
		size_t armor,
		size_t defense,
		size_t attack,
		size_t tile_position_x,
		size_t tile_position_y,
		float position_x,
		float position_y) : Unit(true, false)
	{
		m_unitName = unit_name;
		m_playerId = player_id;
		m_health = health;
		m_armor = armor;
		m_defense = defense;
		m_attack = attack;
		m_tilePositionX = tile_position_x;
		m_tilePositionY = tile_position_y;
		m_positionX = position_x;
		m_positionY = position_y;
	}
}