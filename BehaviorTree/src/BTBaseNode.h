#pragma once

#include "BTNode.h"

namespace bt
{
	class BTBaseNode : public BTNode
	{
	public:
		/// @brief Ctor designated to "normal" nodes (NOT ROOT).
		/// @param name Name of the node.
		/// @param behavior_tree_name Name of the Behavior Tree (Required for Hash).
		BTBaseNode(std::string name, const std::string& behavior_tree_name) : m_Name(name)
		{
			m_BehaviorTreeHash = DJBHash(behavior_tree_name.c_str(), behavior_tree_name.size());
			Initialize();
		}

		/// @brief Ctor for a Root Node.
		/// @param name Name of the Node.
		/// @param behavior_tree_name Name of the Behavior Tree (Required for Hash).
		/// @param behavior_tree_global_blackboard An optional Behavior Tree Global Blackboard.
		BTBaseNode(std::string name, const std::string& behavior_tree_name, BTBlackboard* behavior_tree_global_blackboard) : m_Name(name), m_GlobalBlackboard(behavior_tree_global_blackboard)
		{
			m_BehaviorTreeHash = DJBHash(behavior_tree_name.c_str(), behavior_tree_name.size());
			Initialize();
		}
		virtual ~BTBaseNode()
		{
			Terminate();
		}

		//////////////////////////////
		// Node Base functionality.
		//////////////////////////////
		EBTNodeResult Tick() override
		{
			return INVALID;
		}
		bool Initialize() override { return true; }
		void Terminate() override
		{
			m_Name.clear();
			m_Parent = nullptr;
		}

		void LastRunningNode() override 
		{
			BTCurrentRunningNodeMngr::get()->SetRunning(m_BehaviorTreeHash, this);
		}

		void ResetLastRunning() override
		{
			BTCurrentRunningNodeMngr::get()->ResetRunning(m_BehaviorTreeHash);
		}
		
		//////////////////////////////
		// Node Base blackboard query.
		//////////////////////////////
		bool HasBlackboard() override { return m_Blackboard != nullptr; }
		void SetBlackboard(BTBlackboard* b) override { m_Blackboard = b; }
		BTBlackboard* Blackboard() override { return m_Blackboard; }
		BTBlackboard* GlobalBlackboard() override;
		void GlobalBlackboard(BTBlackboard* b) override;
		
		//////////////////////////////
		// Node Base parent query.
		//////////////////////////////
		BTNode* Parent() override { return m_Parent; }
		void Parent(BTNode* node) override { m_Parent = node; }
		
		//////////////////////////////
		// Node Base children query.
		//////////////////////////////
		BTNode* Child(std::string) override { return nullptr; }
		std::vector<BTNode*>& Children() override { std::vector<BTNode*> v; return v; }
		void AddChild(BTNode*) override { return; }
		void RemoveChild(std::string) override { return; }
		void RemoveFirstChild() override { return; }

		//////////////////////////////
		// Node Base information.
		//////////////////////////////
		std::string Name() override { return m_Name; }
		std::string Type() override { return "BTBaseNode"; }

	protected:
		BTBlackboard* m_GlobalBlackboard = nullptr;
		BTBlackboard* m_Blackboard = nullptr;
		BTNode* m_Parent = nullptr;
		std::string m_Name;
		size_t m_BehaviorTreeHash = 0;
	};
}