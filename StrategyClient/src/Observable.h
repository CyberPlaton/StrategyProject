#pragma once

#include "EventSystem.h"
#include "NetCommon.h"
#include "Unit.h"
#include "Building.h"
#include "Timer.h"

namespace cherrysoda
{
	struct NetGameobjectUpdateEvent : public Event
	{
		NetGameobjectUpdateEvent(net::SGameobject* object) : Event("NetGameobjectUpdate"), m_object(object)
		{
		}
		~NetGameobjectUpdateEvent()
		{
			m_object = nullptr;
		}

		net::SGameobject* m_object;
	};


	class Observable : public EventEmitter
	{
	public:
		CHERRYSODA_DECLARE_COMPONENT(Observable, EventEmitter);

		Observable(size_t event_system_id) : EventEmitter(event_system_id)
		{
			m_networkId = net::CreateGameobjectNetworkUUID();
			m_timer.StartTimer();
		}


		uint32_t GetNetId() { return m_networkId; }

		bool EmitCheck() override final
		{
			printf("Observable::EmitCheck\n");
			printf("\tSeconds elapsed: %.3f\n", m_timer.SecondsElapsed());

			if (m_timer.SecondsElapsed() > 10)
			{
				printf("\t...60 seconds elapsed\n");
				m_timer.StartTimer();
				return true;
			}


			return GetEntity()->Get< Unit >()->IsDirty();
		}
		void EmitEvent() override final
		{
			printf("Observable::EmitEvent\n");

			// Get Data from Entity.
			auto unit = GetEntity()->Get< Unit >();
			// Populate SNetGameobject structure.
			auto net_object = new net::SGameobject();

			// ...




			// Create an event holding the entity data.
			auto evnt = new NetGameobjectUpdateEvent(net_object);

			// Fire the event.
			GetEventSystem()->Add(evnt);


			/*
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
			*/
		}


	private:
		uint32_t m_networkId = 0;
		Timer m_timer;
	};
}