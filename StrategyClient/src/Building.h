#pragma once

#include "ComponentCommon.h"

namespace cherrysoda
{
	class Building : public Component
	{
	public:
		CHERRYSODA_DECLARE_COMPONENT(Building, Component);

		Building() : Building(true, false)
		{
		}
		Building::Building(const String& building_name,
			size_t player_id,
			size_t health,
			size_t tile_position_x,
			size_t tile_position_y,
			float position_x,
			float position_y);

		// GETTER/SETTER
		DECLARE_GET_SET(const String&, BuildingName, m_buildingName);
		DECLARE_GET_SET(size_t, PlayerId, m_playerId);
		DECLARE_GET_SET(size_t, Health, m_health);
		DECLARE_GET_SET(size_t, Level, m_level);
		DECLARE_GET_SET(size_t, SightRadius, m_sightRadius);
		DECLARE_GET_SET(size_t, TilePositionX, m_tilePositionX);
		DECLARE_GET_SET(size_t, TilePositionY, m_tilePositionY);
		DECLARE_GET_SET(float, PositionX, m_positionX);
		DECLARE_GET_SET(float, PositionY, m_positionY);

		// QUERYING
		DECLARE_QUERY(Dirty, m_isDirty);

		// COMMON FUNCTIONALITY

	private:
		String m_buildingName;
		size_t m_playerId = 0;
		size_t m_health = 0;
		size_t m_level = 1;
		size_t m_sightRadius = 1;
		size_t m_tilePositionX = 0;
		size_t m_tilePositionY = 0;
		float  m_positionX = 0.0f;
		float  m_positionY = 0.0f;

		bool m_isDirty = false;

	private:
		CHERRYSODA_FRIEND_CLASS_POOL;

		Building(bool active, bool visible) : base(active, visible) {};
	};
}