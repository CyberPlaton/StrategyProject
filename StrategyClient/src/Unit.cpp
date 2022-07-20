#include "Unit.h"

namespace cherrysoda
{
	Unit::Unit(const String& unit_name, size_t player_id, size_t health, size_t armor, size_t defense,
		size_t min_attack, size_t max_attack,
		bool ranged, size_t ranged_min_attack, size_t ranged_max_attack, size_t ranged_min_range, size_t ranged_max_range,
		size_t tile_position_x, size_t tile_position_y, float position_x, float position_y) : Unit(true, true)
	{
		m_unitName = unit_name;
		m_playerId = player_id;
		m_health = health;
		m_armor = armor;
		m_defense = defense;
		m_minAttack = min_attack;
		m_maxAttack = max_attack;
		m_ranged = ranged;
		m_rangedMinAttack = ranged_min_attack;
		m_rangedMaxAttack = ranged_max_attack;
		m_rangedMinRange = ranged_min_range;
		m_rangedMaxRange = ranged_max_range;
		m_tilePositionX = tile_position_x;
		m_tilePositionY = tile_position_y;
		m_positionX = position_x;
		m_positionY = position_y;
	}




	void SelectableUnit::OnPressedLMB()
	{
		LOG_GAME_INFO("SelectableUnit selected LMB");
	}
	void SelectableUnit::OnPressedRMB()
	{
		LOG_GAME_INFO("SelectableUnit selected RMB");
	}
	void SelectableUnit::OnHoverEnter()
	{
		LOG_GAME_INFO("SelectableUnit Hover Enter");
		static bool open;
		auto position = MInput::Mouse()->Position();

		ImGui::SetNextWindowPos(ImVec2(position.x, -position.y + (float)Engine::Instance()->GetWindowHeight() / 1.5f), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_Always);
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration;
		auto unit = GetEntity()->Get< Unit >();
		ImGui::Begin("Unit Information", &open, flags);
		ImGui::Text("Name: %s", unit->GetUnitName().c_str());
		ImGui::Text("Health: %zu", unit->GetHealth());
		ImGui::Text("Armor: %zu", unit->GetArmor());
		ImGui::Text("Defense: %zu", unit->GetDefense());
		ImGui::Text("MinAttack: %zu", unit->GetMinAttack());
		ImGui::Text("MaxAttack: %zu", unit->GetMaxAttack());
		ImGui::Text("Ranged: %s", unit->GetRanged() == true ? "True" : "False");
		ImGui::Text("Ranged MinAttack: %zu", unit->GetRangedMinAttack());
		ImGui::Text("Ranged MaxAttack: %zu", unit->GetRangedMaxAttack());
		ImGui::Text("Ranged MinRange: %zu", unit->GetRangedMinRange());
		ImGui::Text("Ranged MaxRange: %zu", unit->GetRangedMaxRange());
		ImGui::Text("Level: %zu", unit->GetLevel());
		ImGui::Text("Experience: %zu", unit->GetExperience());
		ImGui::Text("Sight Radius: %zu", unit->GetSightRadius());
		ImGui::Text("Position: {%zu:%zu}", unit->GetTilePositionX(), unit->GetTilePositionY());
		ImGui::Text("MovementType: %zu", unit->GetMovementType());
		ImGui::Text("ActionPoints: %zu", unit->GetActionPoints());
		ImGui::End();
	}
	void SelectableUnit::OnHoverExit()
	{
		LOG_GAME_INFO("SelectableUnit Hover Exit");
	}

}