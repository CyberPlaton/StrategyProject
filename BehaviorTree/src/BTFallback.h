#pragma once

#include "BTBaseNode.h"

namespace bt
{
	/// @brief Sequence is a flow control Behavior Tree Node. It returns SUCCESS/RUNNING on the first kid 
	// that returns SUCCESS/RUNNING, else it keeps executing their Tick in order. If no kid return
	// one of the above, then it returns FAILURE.
	class BTFallback : public BTBaseNode
	{
	public:

		BTFallback(std::string name, const std::string& behavior_tree_name) : BTBaseNode(name, behavior_tree_name)
		{
		}
		virtual ~BTFallback()
		{
			// Delete all Children.
			while(m_Children.size() > 0)
			{
				auto kid = m_Children[0];
				m_Children.erase(m_Children.begin());
				delete kid;
			}
		}


		//////////////////////////////
		// Fallback Base functionality.
		//////////////////////////////
		EBTNodeResult Tick() override
		{
			for (auto& kid : m_Children)
			{
				EBTNodeResult result = kid->Tick();

				if (result == EBTNodeResult::RUNNING)
				{
					return EBTNodeResult::RUNNING;
				}
				else if (result == EBTNodeResult::SUCCESS)
				{
					return EBTNodeResult::SUCCESS;
				}
			}

			return EBTNodeResult::FAILURE;
		}
		BTNode* Child(std::string name) override
		{
			for(auto& kid: m_Children)
			{
				if (kid->Name().compare(name) == 0) return kid;
			}
			return nullptr;
		}
		std::vector<BTNode*>& Children() override { return m_Children; }
		void AddChild(BTNode* child) override { m_Children.push_back(child); }
		void RemoveChild(std::string name) override
		{
			BTNode* child = nullptr;
			int i = 0;
			for (auto& kid : m_Children)
			{
				if (kid->Name().compare(name) == 0) 
				{
					child = kid;
					m_Children.erase(m_Children.begin() + i);
					break;
				}
				i++;
			}

			delete child;
			child = nullptr;
		}
		void RemoveFirstChild() override
		{
			if(m_Children.size() > 1)
			{
				std::vector< BTNode* > kids;
				for (int i = 1; i < m_Children.size(); i++)
				{
					kids.push_back(m_Children[i]);
				}

				m_Children.clear();

				for (auto& kid: kids)
				{
					m_Children.push_back(kid);
				}
			}
			else if(m_Children.size() == 1)
			{
				auto kid = m_Children[0];
				m_Children.erase(m_Children.begin());
				delete kid;
				kid = nullptr;
			}
		}
		

		std::string Type() override{return "BTFallback";}
	protected:
		std::vector<BTNode*> m_Children;
	};
}