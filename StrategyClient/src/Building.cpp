#include "Building.h"

namespace cherrysoda
{
	Building::Building(const String& building_name,
		size_t player_id,
		size_t health,
		size_t tile_position_x,
		size_t tile_position_y,
		float position_x,
		float position_y) : Building(true, true)
	{
		m_buildingName = building_name;
		m_playerId = player_id;
		m_health = health;
		m_tilePositionX = tile_position_x;
		m_tilePositionY = tile_position_y;
		m_positionX = position_x;
		m_positionY = position_y;
	}



	void SelectableBuilding::OnPressedLMB()
	{
		LOG_GAME_INFO("SelectableBuilding selected LMB");
	}
	void SelectableBuilding::OnPressedRMB()
	{
		LOG_GAME_INFO("SelectableBuilding selected RMB");
	}
	void SelectableBuilding::OnHoverEnter()
	{
		LOG_GAME_INFO("SelectableBuilding Hover Enter");
		static bool open;

		auto position = MInput::Mouse()->Position();

		ImGui::SetNextWindowPos(ImVec2(position.x, -position.y + (float)Engine::Instance()->GetWindowHeight() / 1.5f), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_Always);
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration;
		auto building = GetEntity()->Get< Building >();
		ImGui::Begin("Building Information", &open, flags);
		ImGui::Text("Name: %s", building->GetBuildingName());
		ImGui::Text("Health: %zu", building->GetHealth());
		ImGui::Text("Level: %zu", building->GetLevel());
		ImGui::Text("Sight Radius: %zu", building->GetSightRadius());
		ImGui::Text("Position: {%zu:%zu}", building->GetTilePositionX(), building->GetTilePositionY());

		ImGui::End();
	}
	void SelectableBuilding::OnHoverExit()
	{
		LOG_GAME_INFO("SelectableBuilding Hover Exit");
	}
}