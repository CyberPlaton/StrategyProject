#pragma once

#include "BTFallback.h"

namespace sakura
{
	/// @brief Selector is a flow control Behavior Tree Node. It returns immediately SUCCESS when one of its children returns SUCCESS,
	// if it runs out of children it returns FAILURE.
	class BTSelector : public BTFallback
	{
	public:

		BTSelector(std::string name, const std::string& behavior_tree_name) : BTFallback(name, behavior_tree_name)
		{
		}
		virtual ~BTSelector()
		{
		}


		////////////////////////////////
		// Selector Base functionality.
		////////////////////////////////
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

		std::string Type() override { return "BTSelector"; }
	protected:
	};
}