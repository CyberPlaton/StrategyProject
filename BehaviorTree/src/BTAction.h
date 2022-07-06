#pragma once

#include "BTBaseNode.h"

namespace bt
{
	/// @brief Action is a Leaf Behavior Tree Node. It requires the designer to override it and provide
	// a correct implementation of the Command function, which is executed on the Node Tick.
	class BTAction : public BTBaseNode
	{
	public:
		BTAction(std::string name) : BTBaseNode(name)
		{
		}
		virtual ~BTAction()
		{
		}
		/// @brief The designer implemented execution function for this action node.
		/// @return The result of the execution.
		virtual EBTNodeResult Command()
		{
			return INVALID;
		}

		//////////////////////////////
		// Action Base functionality.
		//////////////////////////////
		EBTNodeResult Tick() override
		{
			return Command();
		}

		std::string Type() override{return "BTAction";}
	protected:
	};
}