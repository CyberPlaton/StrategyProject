#pragma once

#include "ComponentCommon.h"

namespace cherrysoda
{
	class Ability : public Component
	{
	public:
		CHERRYSODA_DECLARE_COMPONENT(Ability, Component);


		virtual void UseAbility(cherrysoda::Entity* target) = 0;

	};


}