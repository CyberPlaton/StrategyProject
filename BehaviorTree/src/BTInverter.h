#pragma once

#include "BTDecorator.h"

namespace bt
{
	/// @brief Inverter is a flow control Behavior Tree Node. It inverts the returned Tick value of its Child.
	class BTInverter : public BTDecorator
	{
	public:
		BTInverter(std::string name, const std::string& behavior_tree_name) : BTDecorator(name, behavior_tree_name)
		{
		}
		virtual ~BTInverter()
		{
		}


		//////////////////////////////
		// Inverter Base functionality.
		//////////////////////////////
		EBTNodeResult Tick() override final
		{
			EBTNodeResult result = m_Child->Tick();

			if (result == EBTNodeResult::SUCCESS)
			{
				return FAILURE;
			}
			else
			{
				return SUCCESS;
			}
		}
		std::string Type() override
		{
			return "BTInverter";
		}
	protected:
	};
}