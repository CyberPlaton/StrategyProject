#ifndef _STATUS_EFFECT_H_
#define _STATUS_EFFECT_H_
#pragma once

#include "StatusEffectsDataMap.h"	// Get SStatusEffectData by Name.
#include "Status.h"					// IEntityStatusEffect interface.
#include "Logging.h"

// Include all conditions required for BT creation.
#include "BTDebugCondition.h"

namespace cherrysoda
{

	class CEntityStatusEffect : public IEntityStatusEffect
	{
	public:
		CEntityStatusEffect();
		~CEntityStatusEffect();


		bool Initialize(const net::SStatusEffectData& effect_data, Entity* self_entity) override final;
		void Terminate() override final;


		Entity* Self() override final;
		bool OnUpdate() override final;
		String Name() override final;



	private:
		sakura::BehaviorTree* m_behaviorTree;
		net::SStatusEffectData m_data;


	private:
		/// @brief Construct an appropriate BT. Which one will be constructed depends on the impl. name, which
		/// is defined in the Status Effect data.
		/// @param implementation_name The Behavior Tree implementation.
		bool _constructBehaviorTree(const std::string& tree_name, const std::string& implementation_name);
	};


}



#define ENTITY_ADD_STATUS_EFFECT(effect_name, entity) \
auto se = new cherrysoda::CEntityStatusEffect(); \
if(se->Initialize(cherrysoda::EntityStatusEffectsDataMap::get()->Data(effect_name), entity)) \
{ \
	auto mngr = entity->Get< cherrysoda::CEntityStatusEffectMngr >(); \
	mngr->Add(effect) \
} \
else \
{ \
	delete se; \
} \


#define ENTITY_REMOVE_STATUS_EFFECT(effect, entity) \
auto mngr = entity->Get< CEntityStatusEffectMngr >(); \
mngr->Remove(effect) \


#endif