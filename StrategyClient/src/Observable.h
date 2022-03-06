#pragma once

#include "EventSystem.h"
#include "NetCommon.h"
#include "Unit.h"

namespace cherrysoda
{
	struct NetGameobjectUpdateEvent : public Event
	{
		NetGameobjectUpdateEvent(net::NetGameobject& object) : Event("NetGameobjectUpdate")
		{
			memmove(&m_object, &object, sizeof(net::NetGameobject));
		}
		~NetGameobjectUpdateEvent()
		{
		}

		net::NetGameobject m_object;
	};


	class Observable : public EventEmitter
	{
	public:
		CHERRYSODA_DECLARE_COMPONENT(Observable, EventEmitter);

		Observable(size_t event_system_id) : EventEmitter(event_system_id)
		{
			m_networkId = net::CreateNetworkUUID();
		}

		size_t GetNetId() { return m_networkId; }

		bool EmitCheck() override final
		{
			return true;
		}
		void EmitEvent() override final
		{
			// Get Data from Entity.
			auto unit = GetEntity()->Get< Unit >();

			// Put Data into net::NetGameobject and emit
			// an event with the Data.
			net::NetGameobject object;
			object.m_netId = m_networkId;
			object.m_playerId = unit->GetPlayerId();
			object.m_objectType = net::ENetGameobject::NET_GO_UNIT;

			object.m_unitName = unit->GetUnitName();
			object.m_unitArmor = unit->GetArmor();
			object.m_unitHealth = unit->GetHealth();
			object.m_unitAttack = unit->GetAttack();
			object.m_unitDefense = unit->GetDefense();
			object.m_positionX = unit->GetPositionX();
			object.m_positionY = unit->GetPositionY();
			object.m_tilePositionX = unit->GetTilePositionX();
			object.m_tilePositionY = unit->GetTilePositionY();


			auto evnt = new NetGameobjectUpdateEvent(object);
			GetEventSystem()->Add(evnt);
		}


	private:
		size_t m_networkId = 0;
	};
}