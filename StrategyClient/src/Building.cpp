#include "Building.h"

namespace cherrysoda
{
	Building::Building(const String& building_name,
		size_t player_id,
		size_t health,
		size_t tile_position_x,
		size_t tile_position_y,
		float position_x,
		float position_y) : Building(true, true)
	{
		m_buildingName = building_name;
		m_playerId = player_id;
		m_health = health;
		m_tilePositionX = tile_position_x;
		m_tilePositionY = tile_position_y;
		m_positionX = position_x;
		m_positionY = position_y;
	}
}