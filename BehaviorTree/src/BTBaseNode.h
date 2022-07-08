#pragma once

#include "BTNode.h"

namespace bt
{
	class BTBaseNode : public BTNode
	{
	public:
		BTBaseNode(std::string name) : m_Name(name)
		{
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
		
		//////////////////////////////
		// Node Base blackboard query.
		//////////////////////////////
		bool HasBlackboard() override { return m_Blackboard != nullptr; }
		void SetBlackboard(BTBlackboard* b) override { m_Blackboard = b; }
		BTBlackboard* Blackboard() override { return m_Blackboard; }
		
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
		BTBlackboard* m_Blackboard = nullptr;
		BTNode* m_Parent = nullptr;
		std::string m_Name;
	};
}