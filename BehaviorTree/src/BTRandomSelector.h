#pragma once

#include "BTSelector.h"
#include "Random.h"

namespace bt
{
	/// @brief Random Selector is a flow control Behavior Tree Node. It returns immediately SUCCESS when one of its children returns SUCCESS,
	// if it runs out of children it returns FAILURE. The execution order of the children Tick is randomized.
	class BTRandomSelector : public BTSelector
	{
	public:

		BTRandomSelector(std::string name) : BTSelector(name)
		{
		}
		virtual ~BTRandomSelector()
		{
		}


		////////////////////////////////
		// Random Selector Base functionality.
		////////////////////////////////
		EBTNodeResult Tick() override
		{
			// Shuffle the order of the children.
			std::vector<BTNode*> reshuffled_kids;
			for (auto& kid : m_Children)
			{
				reshuffled_kids.push_back(kid);
			}

			// Randomize the children execution sequence.
			auto rng = std::default_random_engine{};
			rng.seed((uint64_t)NULL);
			std::shuffle(std::begin(reshuffled_kids), std::end(reshuffled_kids), rng);


			for(auto& kid: m_Children)
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

		std::string Type() override { return "BTRandomSelector"; }
	protected:
	};
}