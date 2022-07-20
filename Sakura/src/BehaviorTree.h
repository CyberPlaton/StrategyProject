#pragma once

#include "BTNodes.h"
#include "BTBlackboard.h"
#include "Any.h"

namespace sakura
{
	class BTFactory;


	class BehaviorTree
	{
		friend class BTFactory;

	public:
		BehaviorTree(std::string name, BTBlackboard* behavior_tree_global_blackboard) : m_Name(name), m_GlobalBlackboard(behavior_tree_global_blackboard)
		{
			m_BehaviorTreeHash = DJBHash(m_Name.c_str(), m_Name.size());
		}
		~BehaviorTree()
		{
			// Recursively delete the Tree structure.
			delete m_Root; m_Root = nullptr;
			m_Name.clear();
			m_TreeNodes.clear();
			m_BehaviorTreeHash = 0;
		}

		BTNode::EBTNodeResult Update()
		{
			auto last_running_node = BTCurrentRunningNodeMngr::get()->GetRunning(m_BehaviorTreeHash);
			if(last_running_node)
			{
				return last_running_node->Tick();
			}
			return m_Root->Tick();
		}
		
		BTBlackboard* Blackboard() { return m_GlobalBlackboard; }

	private:
		std::vector<BTNode*> m_TreeNodes;
		BTNode* m_Root = nullptr;
		BTBlackboard* m_GlobalBlackboard = nullptr;
		std::string m_Name;
		size_t m_BehaviorTreeHash = 0;



	private:
		void Root(BTNode* node) { m_Root = node; }
		BTNode* Root() { return m_Root; }
	};
}