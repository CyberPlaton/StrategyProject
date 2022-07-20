#ifndef _STATUS_EFFECT_H_
#define _STATUS_EFFECT_H_
#pragma once

#include "StatusEffectsDataMap.h"
#include "Status.h"
#include "BTInterface.h"
#include "Logging.h"


namespace cherrysoda
{

	class CEntityStatusEffect : public IEntityStatusEffect
	{
	public:
		CEntityStatusEffect();
		~CEntityStatusEffect();


		bool Initialize(const std::string& effect_name, const SStatusEffectData& effect_data, Entity* self_entity) override final;
		void Terminate() override final;


		Entity* Self() override final;
		bool OnUpdate() override final;
		String Name() override final;



	private:
		sakura::BehaviorTree* m_behaviorTree;
	};


}



#define ENTITY_ADD_STATUS_EFFECT(effect, entity) \
auto mngr = entity->Get< CEntityStatusEffectMngr >(); \
mngr->Add(effect) \


#define ENTITY_REMOVE_STATUS_EFFECT(effect, entity) \
auto mngr = entity->Get< CEntityStatusEffectMngr >(); \
mngr->Remove(effect) \



#endif