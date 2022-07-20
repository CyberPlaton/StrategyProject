#pragma once

#include "BTInterface.h"

namespace sakura
{
	/// @brief Example implementation of a Condition Node.
	/// Note the first arguments "name" and "behavior_tree_name"; the declaration and order is required as given,
	/// and only the first argument requires to be specified in Factory construction, the second is automatically applied
	/// by the Factory itself.
	/// All other arguments are specified on construction in the order you give them.
	/// 
	/// Example: 
	/// m_behaviorTree = factory.Add< BTSequence >("Base Sequence")
	///								.Add<BTDebugCondition>("Debug Condition", 0, "SE_Default_Heal")
	///	.Build();
	class BTDebugCondition : public BTCondition
	{
	public:
		BTDebugCondition(std::string name, const std::string& behavior_tree_name, size_t turn_count_left, const std::string& other_status_effect_name) : BTCondition(name, behavior_tree_name){}

		/// @brief We want to check several plausible conditions.
		/// 1) Whether the unit we are belonging to is alive.
		/// 2) Whether the turn count reached a certain value.
		/// 3) Whether we have another Status Effect applied at the same time.
		/// @return True if all conditions are satisfied. 
		bool CheckCondition() override final;
	};
}