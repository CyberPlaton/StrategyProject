#pragma once

#include "BTInterface.h"

namespace sakura
{
	/// @brief 
	class BTDebugCondition : public BTCondition
	{
	public:
		BTDebugCondition(std::string name, const std::string& behavior_tree_name) : BTCondition(name, behavior_tree_name){}

		/// @brief We want to check several plausible conditions.
		/// 1) Whether the unit we are belonging to is alive.
		/// 2) Whether the turn count reached a certain value.
		/// 3) Whether we have another Status Effect applied at the same time.
		/// @return True if all conditions are satisfied. 
		bool CheckCondition() override final;
	};
}