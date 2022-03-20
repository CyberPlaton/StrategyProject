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

		void OnPressedLMB() override
		{
			LOG_GAME_INFO("SelectableBuilding selected LMB");
		}
		void OnPressedRMB() override
		{
			LOG_GAME_INFO("SelectableBuilding selected RMB");
		}
		void OnHoverEnter() override
		{
			LOG_GAME_INFO("SelectableBuilding Hover Enter");
			static bool open;

			auto position = MInput::Mouse()->Position();
			
			ImGui::SetNextWindowPos(ImVec2(position.x, -position.y + (float)Engine::Instance()->GetWindowHeight() / 1.5f), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_Always);
			ImGui::Begin("Building Information", &open);
			ImGui::Text("Building");
			ImGui::End();
		}
		void OnHoverExit() override
		{
			LOG_GAME_INFO("SelectableBuilding Hover Exit");
		}

	private:
	};
}