#pragma once

#include "BTSequence.h"
#include "Random.h"

namespace bt
{
	/// @brief Random Sequence is a flow control Behavior Tree Node. Executes children Tick in a random order as long as 
	// their Tick returns SUCCESS, else it returns the return value.
	class BTRandomSequence : public BTSequence
	{
	public:
		BTRandomSequence(std::string name, const std::string& behavior_tree_name) : BTSequence(name, behavior_tree_name)
		{
		}
		virtual ~BTRandomSequence()
		{
		}


		////////////////////////////////////////
		// Random Sequence Base functionality.
		////////////////////////////////////////
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


			for (auto& kid : reshuffled_kids)
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

		std::string Type() override
		{
			return "BTRandomSequence";
		}
	protected:
	};
}