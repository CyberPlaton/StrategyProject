#pragma once

#include "BTNodes.h"
#include "BTBlackboard.h"
#include "Any.h"

namespace bt
{
	class BTFactory;


	class BehaviorTree
	{
		friend class BTFactory;

	public:
		BehaviorTree(std::string name) : m_Name(name) {}
		~BehaviorTree()
		{
			// Recursively delete the Tree structure.
			delete m_Root; m_Root = nullptr;
			m_Name.clear();
			m_TreeNodes.clear();
		}

		BTNode::EBTNodeResult Update(){return m_Root->Tick();}
		void Root(BTNode* node){m_Root = node;}
		BTNode* Root(){return m_Root;}

	private:
		std::vector<BTNode*> m_TreeNodes;
		BTNode* m_Root = nullptr;
		std::string m_Name;
	};
}