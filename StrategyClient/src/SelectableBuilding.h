#pragma once

#include "SelectableEntity.h"
#include "Logging.h"


namespace cherrysoda
{
	class SelectableBuilding : public SelectableEntity
	{
	public:
		CHERRYSODA_DECLARE_COMPONENT(SelectableBuilding, SelectableEntity);

		SelectableBuilding(int seconds_hovering_time = 0) : SelectableEntity(seconds_hovering_time) {}

		void OnPressedLMB() override;
		void OnPressedRMB() override;
		void OnHoverEnter() override;
		void OnHoverExit() override;

	private:
	};
}