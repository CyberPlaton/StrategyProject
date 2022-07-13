#pragma once

#include "BTDecorator.h"

namespace sakura
{
	/// @brief Inverter is a flow control Behavior Tree Node. It runs its Tick only if the limit has not been reached.
	class BTLimit : public BTDecorator
	{
	public:
		BTLimit(std::string name, const std::string& behavior_tree_name, size_t limit) : BTDecorator(name, behavior_tree_name), m_Limit(limit)
		{
		}
		virtual ~BTLimit()
		{
		}


		//////////////////////////////
		// Limit Base functionality.
		//////////////////////////////
		EBTNodeResult Tick() override final
		{
			if(m_Runs <= m_Limit)
			{
				EBTNodeResult result = m_Child->Tick();

				m_Runs++;
				return result;
			}
			else
			{
				return EBTNodeResult::FAILURE;
			}
		}
		std::string Type() override
		{
			return "BTLimit";
		}
	protected:
		size_t m_Runs = 0;
		size_t m_Limit = 0;
	};
}