#include "Main.h"

// STATIC DATA
static bool g_bDecalDatabaseOpen = true;
static bool g_bImguiDemoOpen = true;
static bool g_bImguiHasFocus = false;
static bool g_bIsPanning = false;
static uint64_t g_iCameraSpeed = 1;
static std::string g_sSelectedMapobject = "none";;

GameEditor editor;
void MainRender(GameEditor* editor)
{
	editor->OnUserUpdate(editor->GetFPS());
}
void GameEditor::RenderGUI()
{
	// Update imgui focus.
	if (ImGui::IsAnyItemHovered() ||
		ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow) ||
		ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
	{
		g_bImguiHasFocus = true;
	}
	else
	{
		g_bImguiHasFocus = false;
	}

	// Menu Bar.
	RenderMainMenu();

	// Imgui Demo
	ImGui::ShowDemoWindow(&g_bImguiDemoOpen);

	// Decal Database
	RenderDecalDatabase(g_bDecalDatabaseOpen);
}
void GameEditor::RenderMainMenu()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{
			if (ImGui::MenuItem("Decal Database")) ToggleMenuItem(g_bDecalDatabaseOpen);
			if (ImGui::MenuItem("Imgui Demo")) ToggleMenuItem(g_bImguiDemoOpen);


			ImGui::EndMenu();
		}
	}
	ImGui::EndMainMenuBar();
}
void GameEditor::RenderDecalDatabase(bool open)
{
	if (open)
	{
		ImGui::Begin("DecalDatabase", &g_bDecalDatabaseOpen);
		for (auto& pair : m_decalDatabase)
		{
			if (ImGui::ImageButton((ImTextureID)pair.second->id, { 128, 128 }))
			{
				g_sSelectedMapobject = pair.first;
				ImGui::SetWindowFocus(nullptr);
			}
		}
		ImGui::End();
	}
}
void GameEditor::ToggleMenuItem(bool& item)
{
	item = (item == true) ? false : true;
}
void GameEditor::RenderMainFrame()
{
	// Draw Grid
	olc::vi2d topleft = tv.GetTopLeftTile().max({ 0, 0 });
	olc::vi2d bottomright = tv.GetBottomRightTile().min({ MAX_MAPSIZE_X, MAX_MAPSIZE_Y });
	olc::vi2d tile;

	// Draw Grid.
	olc::Pixel color = { 255, 255, 255, 50 };
	for (tile.y = topleft.y; tile.y < bottomright.y; tile.y++)
	{
		for (tile.x = topleft.x; tile.x < bottomright.x; tile.x++)
		{
			tv.DrawLineDecal(tile, tile + olc::vf2d(0.0f, 1.0f), color);
			tv.DrawLineDecal(tile, tile + olc::vf2d(1.0f, 0.0f), color);
		}
	}

	// Draw Mapobjects
	olc::vi2d upLeft = m_visiblePointLeftUp;
	olc::vi2d downRight = m_visiblePointDownRight;
	for (int x = upLeft.x; x < downRight.x; x++)
	{
		for (int y = upLeft.y; y < downRight.y; y++)
		{
			if (m_gameworld[x][y])
			{
				RenderMapobject(m_gameworld[x][y]);
			}
		}
	}

	// Draw selected Decal.
	if (g_sSelectedMapobject.compare("none") == 0)
	{
		return;
	}
	else
	{
		tv.DrawDecal({ 0, 0 }, m_decalDatabase[g_sSelectedMapobject]);
	}
}
void GameEditor::RenderMapobject(Mapobject* object)
{
	if (object->m_hasDecal)
	{
		tv.DrawDecal({ object->m_positionx, object->m_positiony }, m_decalDatabase[object->m_decal]);
	}
	else
	{
		tv.DrawStringDecal({ object->m_positionx, object->m_positiony }, object->m_name);
	}
}
bool GameEditor::LoadTilesetData(const std::string& tilesetpath, const std::string& datapath)
{
	rapidjson::Document doc;
	std::ifstream ifs(datapath.c_str());
	rapidjson::IStreamWrapper isw(ifs);
	doc.ParseStream(isw);

	const auto& at = doc["textures"];
	for (const auto& tex : at.GetArray())
	{
		//std::string name = tex["name"].GetString();

		const auto& imgs = tex["images"];
		for (const auto& img : imgs.GetArray())
		{
			std::string name = img["n"].GetString();

			// Here we only need the Name and the path in order to load the image
			// as an OLC sprite and create a decal...
			auto sprite = new olc::Sprite(tilesetpath + "/" + name + ".png");
			auto decal = new olc::Decal(sprite);

			m_decalDatabase.try_emplace(name, decal);
			m_spriteDatabase.push_back(sprite);
		}
	}
	return true;
}
void GameEditor::HandleInput()
{
	olc::vf2d point = tv.ScreenToWorld({ (float)GetMouseX(), (float)GetMouseY() });
	float mousex = point.x;
	float mousey = point.y;

	olc::vi2d topLeft = tv.GetTileUnderScreenPos({ 0, 0 });
	olc::vi2d bottomDown = tv.GetBottomRightTile();
	olc::vi2d middle = { bottomDown.x / 2, bottomDown.y / 2 };

	if (!g_bImguiHasFocus)
	{
		if (GetMouse(1).bReleased)
		{
			g_sSelectedMapobject = "none";
		}

		if (GetMouse(2).bPressed)
		{
			g_bIsPanning = true;
			tv.StartPan(GetMousePos());
		}
		if (GetMouse(2).bHeld) tv.UpdatePan(GetMousePos());
		if (GetMouse(2).bReleased)
		{
			g_bIsPanning = false;
			tv.EndPan(GetMousePos());
		}
		if (GetMouseWheel() > 0) tv.ZoomAtScreenPos(2.0f, GetMousePos());
		if (GetMouseWheel() < 0) tv.ZoomAtScreenPos(0.5f, GetMousePos());

		if (!g_bIsPanning)
		{
			// WASD Movement,
			// for when we do not use the Mouse for panning.
			if (GetKey(olc::SHIFT).bHeld)
			{
				g_iCameraSpeed = 4;
			}
			else
			{
				g_iCameraSpeed = 1;
			}
			if (GetKey(olc::W).bPressed || GetKey(olc::S).bPressed ||
				GetKey(olc::A).bPressed || GetKey(olc::D).bPressed)
			{
				m_camerax = tv.GetTileUnderScreenPos({ 0, 0 }).x;
				m_cameray = tv.GetTileUnderScreenPos({ 0, 0 }).y;
				tv.StartPan(olc::vf2d{ m_camerax, m_cameray });
			}
			if (GetKey(olc::W).bHeld || GetKey(olc::S).bHeld ||
				GetKey(olc::A).bHeld || GetKey(olc::D).bHeld)
			{
				if (GetKey(olc::W).bHeld)
				{
					m_cameray += 3 * g_iCameraSpeed;
					tv.UpdatePan(olc::vf2d{ m_camerax, m_cameray });
				}
				if (GetKey(olc::S).bHeld)
				{
					m_cameray -= 3 * g_iCameraSpeed;
					tv.UpdatePan(olc::vf2d{ m_camerax, m_cameray });
				}
				if (GetKey(olc::A).bHeld)
				{
					m_camerax += 3 * g_iCameraSpeed;
					tv.UpdatePan(olc::vf2d{ m_camerax, m_cameray });
				}
				if (GetKey(olc::D).bHeld)
				{
					m_camerax -= 3 * g_iCameraSpeed;
					tv.UpdatePan(olc::vf2d{ m_camerax, m_cameray });
				}
			}
			else
			{
				tv.UpdatePan(olc::vf2d{ m_camerax, m_cameray });
			}
		}
	}
}
void GameEditor::UpdateVisibleRect()
{
	m_visiblePointLeftUp = tv.GetTileUnderScreenPos({ 0, 0 });
	if (m_visiblePointLeftUp.x < 0) m_visiblePointLeftUp.x = 0;
	if (m_visiblePointLeftUp.y < 0) m_visiblePointLeftUp.y = 0;
	m_visiblePointLeftUp = tv.GetBottomRightTile();
	if (m_visiblePointLeftUp.x > MAX_MAPSIZE_X) m_visiblePointLeftUp.x = MAX_MAPSIZE_X;
	if (m_visiblePointLeftUp.y > MAX_MAPSIZE_Y) m_visiblePointLeftUp.y = MAX_MAPSIZE_Y;
}


int main()
{
	if (editor.Construct(1360, 768, 1, 1))
		editor.Start();

	return 0;
}