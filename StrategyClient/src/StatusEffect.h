#ifndef _STATUS_EFFECT_H_
#define _STATUS_EFFECT_H_
#pragma once

#include "StatusEffectsDataMap.h"	// Get SStatusEffectData by Name.
#include "Status.h"					// IEntityStatusEffect interface.
#include "Logging.h"


namespace cherrysoda
{

	class CEntityStatusEffect : public IEntityStatusEffect
	{
	public:
		CEntityStatusEffect();
		~CEntityStatusEffect();


		bool Initialize(const net::SStatusEffectData& effect_data, Entity* self_entity, sakura::BehaviorTree* behavior_tree) override final;
		void Terminate() override final;


		Entity* Self() override final;
		bool OnUpdate() override final;
		String Name() override final;



	private:
		sakura::BehaviorTree* m_behaviorTree;
		net::SStatusEffectData m_data;
	};


}



#define ENTITY_ADD_STATUS_EFFECT(effect, entity) \
auto mngr = entity->Get< CEntityStatusEffectMngr >(); \
mngr->Add(effect) \


#define ENTITY_REMOVE_STATUS_EFFECT(effect, entity) \
auto mngr = entity->Get< CEntityStatusEffectMngr >(); \
mngr->Remove(effect) \



#endif