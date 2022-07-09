#pragma once

#include "BTNodes.h"
#include "BehaviorTree.h"

namespace bt
{
	/// @brief The purpose of the Behavior Tree Factory is to create fast and convenient a Behavior Tree.
	class BTFactory
	{
	public:
		BTFactory(const std::string& behavior_tree_name, BTBlackboard* behavior_tree_global_blackboard) : m_Tree(new BehaviorTree(behavior_tree_name, behavior_tree_global_blackboard)) {}
		~BTFactory()
		{
			// Do not delete the Tree Here.
			// This is not our concern.
			m_LastParent = nullptr;
			m_Tree = nullptr;
			m_HasRoot = false;
		}


		/// @brief Add a node to the Behavior Tree. The node is added as a child to the last added one.
		/// @tparam NodeType Class type of the node to be added.
		/// @tparam ...Args Variadic template.
		/// @param node_name The name of the node.
		/// @param ...args Variadic template values to be added; Must match the expected data types in the Node constructor.
		/// @return Returns self, for chaining construction of the Tree.
		template < typename NodeType, class... Args >
		BTFactory& Add(const std::string& node_name, Args... args)
		{
			NodeType* node = new NodeType(node_name, m_Tree->m_Name, args...);

			if (!m_HasRoot)
			{
				// Set the Root,
				m_Tree->Root(node);
				node->GlobalBlackboard(m_Tree->m_GlobalBlackboard);
				m_HasRoot = true;
				m_LastParent = node;
			}
			else
			{
				m_LastParent->AddChild(node);
				m_LastParent = node;
			}
			m_Tree->m_TreeNodes.push_back(node);
			return *this;
		}

		/// @brief End adding to current Node and get its parent.
		/// @return Return the parent of the last node.
		BTFactory& End()
		{
			BTNode* p = m_LastParent->Parent();
			if (p != nullptr)
			{
				m_LastParent = p;
			}
			return *this;
		}


		BehaviorTree* Build()
		{
			return m_Tree;
		}


	private:
		BTNode* m_LastParent = nullptr;
		BehaviorTree* m_Tree = nullptr;
		bool m_HasRoot = false;


	private:
	};
}