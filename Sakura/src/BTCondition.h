#pragma once

#include "BTBaseNode.h"

namespace sakura
{
	/// @brief Condition is a Leaf Behavior Tree Node. The designer has to provide a override
	// of the CheckCondition function. If the condition returns true, the node returns SUCCESS, else FAILURE.
	class BTCondition : public BTBaseNode
	{
	public:
		BTCondition(std::string name, const std::string& behavior_tree_name) : BTBaseNode(name, behavior_tree_name)
		{
		}
		virtual ~BTCondition()
		{
		}

		/// @brief The designer implemented execution function for this condition node.
		/// @return The result of the execution.
		virtual bool CheckCondition()
		{
			return false;
		}

		//////////////////////////////
		// Condition Base functionality.
		//////////////////////////////
		EBTNodeResult Tick() override
		{
			if (CheckCondition() == true)
			{
				return SUCCESS;
			}
			else
			{
				return FAILURE;
			}
		}

		std::string Type() override{return "BTCondition";}
	private:
	};
}