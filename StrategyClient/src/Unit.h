#pragma once

#include <CherrySoda/Components/Component.h>
#include <CherrySoda/Util/Pool.h>
#include <CherrySoda/Util/String.h>
#include <CherrySoda/Util/STL.h>


#define CONCAT(a, b) a##b
#define DECLARE_GET_SET(returntype, name, member) \
returntype CONCAT(Get, name)() \
{ \
	return member; \
} \
void CONCAT(Set, name)(returntype value) \
{ \
	member = value; \
} \


namespace cherrysoda
{
	class Unit : public Component
	{
	public:
		CHERRYSODA_DECLARE_COMPONENT(Unit, Component);

		Unit() : Unit(true, false)
		{
		}




		// GETTER/SETTER
		DECLARE_GET_SET(const String&, UnitName, m_unitName);
		DECLARE_GET_SET(size_t, PlayerId, m_playerId);
		DECLARE_GET_SET(size_t, Health, m_health);
		DECLARE_GET_SET(size_t, Armor, m_armor);
		DECLARE_GET_SET(size_t, Defense, m_defense);
		DECLARE_GET_SET(size_t, Attack, m_attack);
		DECLARE_GET_SET(size_t, TilePositionX, m_tilePositionX);
		DECLARE_GET_SET(size_t, TilePositionY, m_tilePositionY);
		DECLARE_GET_SET(float, PositionX, m_positionX);
		DECLARE_GET_SET(float, PositionY, m_positionY);

	private:
		String m_unitName;
		size_t m_playerId = 0;
		size_t m_health = 0;
		size_t m_armor = 0;
		size_t m_defense = 0;
		size_t m_attack = 0;
		size_t m_tilePositionX = 0;
		size_t m_tilePositionY = 0;
		float m_positionX = 0;
		float m_positionY = 0;

	private:
		CHERRYSODA_FRIEND_CLASS_POOL;

		Unit(bool active, bool visible) : base(active, visible) {};
	};
}


