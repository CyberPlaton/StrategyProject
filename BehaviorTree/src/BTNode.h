#pragma once

#include "BTCommon.h"

namespace bt
{
	class BTBlackboard;


	class BTNode
	{
	public:
		enum EBTNodeResult
		{
			INVALID = -1,
			FAILURE = 0,
			SUCCESS = 1,
			RUNNING = 2
		};
	public:
		/// @brief Base node function executed on each Behavior Tree tick.
		/// @return Result of the node execution.
		virtual EBTNodeResult Tick() = 0;

		/// @brief Basic init function.
		/// @return True if init was successful.
		virtual bool Initialize() = 0;

		/// @brief Terminate and release used memory.
		virtual void Terminate() = 0;

		/// @brief Get Parent node.
		/// @return Pointer to parent.
		virtual BTNode* Parent() = 0;

		/// @brief Set Parent node.
		/// @param  Pointer to parent node.
		virtual void Parent(BTNode*) = 0;

		/// @brief Get Child by name.
		/// @param Name of the Child.
		/// @return Pointer to Child node.
		virtual BTNode* Child(std::string) = 0;

		/// @brief Get all children.
		/// @return Vector of all children.
		virtual std::vector<BTNode*>&  Children() = 0;

		/// @brief Add child to node.
		/// @param  Pointer to child node.
		virtual void AddChild(BTNode*) = 0;

		/// @brief Remove one child if it exists.
		/// @param Name of the child.
		virtual void RemoveChild(std::string) = 0;

		/// @brief Remove the first child. Removes recursively all children.
		virtual void RemoveFirstChild() = 0;

		/// @brief Check whether a blackboard has been set.
		/// @return True, if node has a blackboard.
		virtual bool HasBlackboard() = 0;

		/// @brief Set the Blackboard for the node.
		/// @param Pointer to Blackboard.
		virtual void SetBlackboard(BTBlackboard*) = 0;

		/// @brief Get the Blackboard that has been set if it exists.
		/// @return Pointer to Blackboard.
		virtual BTBlackboard* Blackboard() = 0;


		virtual void LastRunningNode() = 0;

		/// @brief Get the name of the Node.
		/// @return Name of the node.
		virtual std::string Name() = 0;

		/// @brief Get the type of the node.
		/// @return Name of the type.
		virtual std::string Type() = 0;
	};
}