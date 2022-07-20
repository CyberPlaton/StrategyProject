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

		m_data.m_behaviorTreeImplName = effect_data.m_behaviorTreeImplName;

		// Construct the BT.
		if(!_constructBehaviorTree(m_data.m_behaviorTreeImplName))
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

	bool CEntityStatusEffect::_constructBehaviorTree(const std::string& tree_name, const std::string& implementation_name)
	{
		using namespace sakura;

		std::string name = implementation_name + "::" + tree_name;
		bool construction_result;

		if(implementation_name.compare("BT_Impl_Testing_Tree") == 0)
		{
			BTBlackboard blackboard(name + "::Blackboard");
			BTFactory factory(name, &blackboard);

			m_behaviorTree = factory.Add< BTSequence >("Base Sequence")
										.Add<BTDebugCondition>("Debug Condition", 0, "SE_Default_Heal")
				.Build();


			construction_result = m_behaviorTree != nullptr;
		}
		


		if(construction_result)
		{
			LOG_DBG_INFO("[{:.4f}][CEntityStatusEffect::_constructBehaviorTree] Constructed Behavior Tree: \"{}\"!", APP_RUN_TIME, name);
			LOG_GAME_SUCCESS("[%.4f][CEntityStatusEffect::_constructBehaviorTree] Constructed Behavior Tree: \"%s\"!", APP_RUN_TIME, name);
			LOG_FILE_INFO("[{:.4f}][CEntityStatusEffect::_constructBehaviorTree] Constructed Behavior Tree: \"{}\"!", APP_RUN_TIME, name);
			return true;
		}
		else
		{
			LOG_DBG_ERROR("[{:.4f}][CEntityStatusEffect::_constructBehaviorTree] Constructing Behavior Tree failed: \"{}\"!", APP_RUN_TIME, name);
			LOG_GAME_ERROR("[%.4f][CEntityStatusEffect::_constructBehaviorTree] Constructing Behavior Tree failed: \"%s\"!", APP_RUN_TIME, name);
			LOG_FILE_ERROR("[{:.4f}][CEntityStatusEffect::_constructBehaviorTree] Constructing Behavior Tree failed: \"{}\"!", APP_RUN_TIME, name);
			return false;
		}
		LOG_DBG_ERROR("[{:.4f}][CEntityStatusEffect::_constructBehaviorTree] Unknown Behavior Tree Implementation: \"{}\"!", APP_RUN_TIME, implementation_name);
		LOG_GAME_ERROR("[%.4f][CEntityStatusEffect::_constructBehaviorTree] Unknown Behavior Tree Implementation: \"%s\"!", APP_RUN_TIME, implementation_name);
		LOG_FILE_ERROR("[{:.4f}][CEntityStatusEffect::_constructBehaviorTree] Unknown Behavior Tree Implementation: \"{}\"!", APP_RUN_TIME, implementation_name);
		return false;
	}

}