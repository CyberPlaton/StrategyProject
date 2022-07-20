#include "StatusEffect.h"


namespace cherrysoda
{

	CEntityStatusEffect::CEntityStatusEffect() : m_behaviorTree(nullptr)
	{

	}

	CEntityStatusEffect::~CEntityStatusEffect()
	{
		Terminate();
	}

	bool CEntityStatusEffect::Initialize(const net::SStatusEffectData& effect_data, Entity* self_entity)
	{
		// Sanity check.
		if (!self_entity) return false;


		// Copy the data.
		m_data.m_effectName = effect_data.m_effectName;
		m_data.m_effectDisplayName = effect_data.m_effectDisplayName;
		m_data.m_effectType = effect_data.m_effectType;

		m_data.m_effectValueMin = effect_data.m_effectValueMin;
		m_data.m_effectValueMax = effect_data.m_effectValueMax;
		m_data.m_effectApplicationProbability = effect_data.m_effectApplicationProbability;

		m_data.m_effectApplicableTo = effect_data.m_effectApplicableTo;

		m_data.m_effectTimerType = effect_data.m_effectTimerType;

		m_data.m_effectTimerValue = effect_data.m_effectTimerValue;

		m_data.m_effectDesc = effect_data.m_effectDesc;

		m_data.m_behaviorTreeName = effect_data.m_behaviorTreeName;

		// Construct the BT.
		if(!_constructBehaviorTree(m_data.m_behaviorTreeName))
		{
			return false;
		}

		// Store the Entity in BT global blackboard.
		m_behaviorTree->Blackboard()->SetDataObject< cherrysoda::Entity >("Self", self_entity, "Entity");


		return true;
	}

	void CEntityStatusEffect::Terminate()
	{
		// Delete BT.
		delete m_behaviorTree;
		m_behaviorTree = nullptr;
	}

	Entity* CEntityStatusEffect::Self()
	{
		return m_behaviorTree->Blackboard()->GetDataObject< cherrysoda::Entity >("Self");
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
		return m_data.m_effectName;
	}

	bool CEntityStatusEffect::_constructBehaviorTree(const std::string& implementation_name)
	{

	}

}