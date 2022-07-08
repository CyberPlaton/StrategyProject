#pragma once

#include "BTSequence.h"

namespace bt
{

	/// @brief Parallel is a flow control Behavior Tree Node. Generally, behaves as a sequence, which returns
	// SUCCESS or FAILURE based on the defined policy.
	// E.g. can return SUCCESS only if all kids returned SUCCESS etc.
	class BTParallel : public BTSequence
	{
	public:
		enum EPolicy
		{
			P_INVALID = -1,
			P_REQUIRE_ALL = 0,
			P_REQUIRE_ONE = 1
		};
	public:

		BTParallel(std::string name, const std::string& behavior_tree_name, EPolicy success, EPolicy fail) : BTSequence(name, behavior_tree_name), m_FailPolicy(fail), m_SuccessPolicy(success) {}
		BTParallel(std::string name, const std::string& behavior_tree_name, int success, int fail) : BTSequence(name, behavior_tree_name), m_FailPolicy(BTParallel::EPolicy(fail)), m_SuccessPolicy(BTParallel::EPolicy(success)) {}
		virtual ~BTParallel()
		{
		}

		
		//////////////////////////////
		// Parallel Base functionality.
		//////////////////////////////
		EBTNodeResult Tick() override
		{
			if (m_FailPolicy == EPolicy::P_INVALID) return INVALID;
			if (m_SuccessPolicy == EPolicy::P_INVALID) return INVALID;

			int successful_ticks = 0, failed_ticks = 0;


			for (auto& kid : m_Children)
			{

				EBTNodeResult result = kid->Tick();

				if (result == SUCCESS)
				{
					successful_ticks++;

					if (m_SuccessPolicy == P_REQUIRE_ONE)
					{
						return SUCCESS;
					}
				}
				else if (result == FAILURE)
				{
					failed_ticks++;

					if (m_FailPolicy == P_REQUIRE_ONE)
					{
						return SUCCESS;
					}
				}
				else
				{
					return INVALID;
				}
			}



			if (m_SuccessPolicy == P_REQUIRE_ALL &&
				successful_ticks == m_Children.size())
			{
				return SUCCESS;
			}

			if (m_FailPolicy == P_REQUIRE_ALL &&
				failed_ticks == m_Children.size())
			{
				return FAILURE;
			}

			// Should never reach.
			return INVALID;
		}
		
		std::string Type() override{return "BTParallel";}
	protected:
		EPolicy m_FailPolicy = EPolicy::P_INVALID;
		EPolicy m_SuccessPolicy = EPolicy::P_INVALID;
	};
}