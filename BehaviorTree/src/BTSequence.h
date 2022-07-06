#pragma once

#include "BTFallback.h"

namespace bt
{
	/// @brief Sequence is a flow control Behavior Tree Node. Executes its children Tick as long as they return SUCCESS.
	// The execution order is from left (0) to right (n-1).
	class BTSequence : public BTFallback
	{
	public:

		BTSequence(std::string name) : BTFallback(name)
		{
		}
		virtual ~BTSequence()
		{
		}


		////////////////////////////////
		// Sequence Base functionality.
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
				else if (result == EBTNodeResult::FAILURE || result == EBTNodeResult::INVALID)
				{
					return EBTNodeResult::FAILURE;
				}
			}

			return EBTNodeResult::SUCCESS;
		}

		std::string Type() override{return "BTSequence";}
	protected:
	};
}
