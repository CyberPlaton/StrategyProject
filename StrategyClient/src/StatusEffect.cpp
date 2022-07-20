#include "StatusEffect.h"


namespace cherrysoda
{



	CEntityStatusEffect::CEntityStatusEffect()
	{

	}

	CEntityStatusEffect::~CEntityStatusEffect()
	{

	}

	bool CEntityStatusEffect::Initialize(const std::string& effect_name, const SStatusEffectData& effect_data, Entity* self_entity)
	{

	}

	void CEntityStatusEffect::Terminate()
	{

	}

	Entity* CEntityStatusEffect::Self()
	{

	}

	bool CEntityStatusEffect::OnUpdate()
	{
		if(!m_behaviorTree)
		{
			// Error occurred, just remove us.
			LOG_DBG_ERROR("[{:.4f}][CEntityStatusEffect::OnUpdate] Status Effect \"{}\" has no Behavior Implemented!", APP_RUN_TIME, Name());
			LOG_GAME_ERROR("[%.4f][CEntityStatusEffect::OnUpdate] Status Effect \"%s\" has no Behavior Implemented!", APP_RUN_TIME, Name());
			LOG_FILE_ERROR("[{:.4f}][CEntityStatusEffect::OnUpdate] Status Effect \"{}\" has no Behavior Implemented!", APP_RUN_TIME, Name());
			return false;
		}

		// Perform Logic update.
		auto r = m_behaviorTree->Update();

		// Decide what to do with the Status Effect.
		if(r == sakura::BTNode::EBTNodeResult::SUCCESS || 
		   r == sakura::BTNode::EBTNodeResult::RUNNING)
		{
			// Status Effect stays until next update.
			return true;
		}
		

		// Status Effect will be removed on 
		// FAILURE and INVALID.
		return false;
	}

	cherrysoda::String CEntityStatusEffect::Name()
	{

	}

}