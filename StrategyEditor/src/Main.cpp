#include "Main.h"

// STATIC DATA
static bool g_bDecalDatabaseOpen = true;
static bool g_bImguiDemoOpen = true;
static bool g_bImguiHasFocus = false;
static bool g_bIsPanning = false;
static bool g_bRenderingLayersOpen = true;
static bool g_bChangingLayerName = false;
static uint64_t g_iCameraSpeed = 1;
static std::string g_sSelectedMapobject = "none";
static bool g_bChangeLayerNameOpen = false;
static bool g_bChangeLayerOrderOpen = false;
static std::string g_sAlteredLayer = "none";
static uint64_t g_iImguiImageButtonID = 10000;


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
	RenderDecalDatabase();
	RenderLayerUI();
}
bool GameEditor::LoadEditorGraphicalData()
{
	auto sprite = new olc::Sprite("assets/Editor/pencil.png");
	auto decal = new olc::Decal(sprite);
	m_editorDecalDatabase.try_emplace("Pencil", decal);
	m_editorSpriteDatabase.push_back(sprite);


	sprite = new olc::Sprite("assets/Editor/sliders_options.png");
	decal = new olc::Decal(sprite);
	m_editorDecalDatabase.try_emplace("Slider", decal);
	m_editorSpriteDatabase.push_back(sprite);


	sprite = new olc::Sprite("assets/Editor/trash_bin_delete.png");
	decal = new olc::Decal(sprite);
	m_editorDecalDatabase.try_emplace("Delete", decal);
	m_editorSpriteDatabase.push_back(sprite);


	sprite = new olc::Sprite("assets/Editor/ok.png");
	decal = new olc::Decal(sprite);
	m_editorDecalDatabase.try_emplace("OK", decal);
	m_editorSpriteDatabase.push_back(sprite);


	return true;
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
void GameEditor::RenderDecalDatabase()
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

	for (auto& layer : m_layerOrder)
	{
		auto layer_name = layer.first;
		auto layer_world = m_gameworld[layer_name];

		for (int x = upLeft.x; x < downRight.x; x++)
		{
			for (int y = upLeft.y; y < downRight.y; y++)
			{
				if (layer_world[x][y])
				{
					RenderMapobject(layer_world[x][y]);
				}
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
std::string GameEditor::CreateMapobjectName()
{
	return "Mapobject_" + std::to_string(m_mapobjectCount++);
}
void GameEditor::CreateMapobject(uint64_t x, uint64_t y, std::string decal, std::string name)
{
	if (name.compare("none") == 0)
	{
		name = CreateMapobjectName();
	}
	else
	{
		m_mapobjectCount++;
	}

	// Create object.
	auto object = new Mapobject();
	object->m_decal = decal;
	object->m_hasDecal = true;
	object->m_name = name;
	object->m_positionx = x;
	object->m_positiony = y;

	// Add Object to layer Gameworld.
	m_gameworld[m_currentLayer][x][y] = object;
}
void GameEditor::CreateRenderingLayer(std::string layer_name, int order)
{
	// If a layer already exists, dont do anything.
	if (m_layerOrder.find(layer_name) != m_layerOrder.end()) return;

	// Create a layer and initialize the Matrix.
	m_layerOrder[layer_name] = order;
	InitializeMatrix(m_gameworld[layer_name]);
}
void GameEditor::ChangeLayerOrder(std::string layer_name, int order)
{
	// If a layer does not exist, dont do anything.
	if (m_layerOrder.find(layer_name) == m_layerOrder.end()) return;

	// If another layer has the order, swap them.
	// Else just change order.
	std::string sTemp;
	int iTemp;
	for (auto& pair : m_layerOrder)
	{
		if (pair.second == order)
		{
			// Other layers name.
			sTemp = pair.first;
			// Get former layers order.
			iTemp = m_layerOrder[layer_name];
			m_layerOrder[layer_name] = order;
			m_layerOrder[sTemp] = iTemp;
			return;
		}
	}


	m_layerOrder[layer_name] = order;
}
void GameEditor::InitializeMatrix(std::vector< std::vector< Mapobject* > >& matrix)
{
	matrix.resize(MAX_MAPSIZE_X);
	for (int i = 0; i < matrix.size(); i++)
	{
		matrix[i].resize(MAX_MAPSIZE_Y);
	}
}
void GameEditor::RenderLayerUI()
{
	ImGui::Begin("Rendering Layers", &g_bRenderingLayersOpen);

	static char layer_name_buf[64] = ""; ImGui::InputText("|", layer_name_buf, 64);
	ImGui::SameLine();
	
	if (ImGui::Button("Add Layer"))
	{
		CreateRenderingLayer(std::string(layer_name_buf), m_sortedLayers.size());
		memset(&layer_name_buf, 0, sizeof(layer_name_buf));
		UpdateLayerSorting();
	}
	for (auto& layer : m_sortedLayers)
	{
		ImGui::Text("\"%s\" [%d]", layer.second.c_str(), layer.first);
		ImGui::SameLine();

		ImGuiID pencil_id = g_iImguiImageButtonID + layer.first + (intptr_t)"Pencil";
		ImGuiID slider_id = g_iImguiImageButtonID + layer.first + (intptr_t)"Slider";
		ImGuiID delete_id = g_iImguiImageButtonID + layer.first + (intptr_t)"Delete";

		ImGui::PushID(pencil_id);
		if (ImGui::ImageButton((ImTextureID)m_editorDecalDatabase["Pencil"]->id, {16, 16}))
		{
			g_bChangeLayerNameOpen = true;
			g_sAlteredLayer = layer.second;
		}
		ImGui::PopID();
		ImGui::SameLine();
		ImGui::PushID(slider_id);
		if (ImGui::ImageButton((ImTextureID)m_editorDecalDatabase["Slider"]->id, { 16, 16 }))
		{
			g_bChangeLayerOrderOpen = true;
			g_sAlteredLayer = layer.second;
		}
		ImGui::PopID();
		ImGui::SameLine();
		ImGui::PushID(delete_id);
		if (ImGui::ImageButton((ImTextureID)m_editorDecalDatabase["Delete"]->id, { 16, 16 }))
		{
			DeleteRenderingLayer(layer.second);
		}
		ImGui::PopID();
	}
	ImGui::End();

	if (g_bChangeLayerNameOpen)
	{
		ImGui::SetNextWindowSize({ 250, 100 }, ImGuiCond_Appearing);
		ImGui::SetNextWindowPos({ (float)GetMousePos().x, (float)GetMousePos().y}, ImGuiCond_Appearing);
		ImGui::Begin("Layer Name Editor", &g_bChangeLayerNameOpen);
		static char layer_name_change_buf[64] = ""; ImGui::InputText("Name", layer_name_change_buf, 64);
		ImGui::SameLine();
		if (ImGui::ImageButton((ImTextureID)m_editorDecalDatabase["OK"]->id, { 16, 16 }))
		{
			ChangeLayerName(g_sAlteredLayer, std::string(layer_name_change_buf));
			memset(&layer_name_change_buf, 0, sizeof(layer_name_change_buf));
			g_bChangeLayerNameOpen = false;
			g_sAlteredLayer = "none";
			UpdateLayerSorting();
		}
		ImGui::End();
	}
	if (g_bChangeLayerOrderOpen)
	{
		ImGui::SetNextWindowSize({ 250, 100 }, ImGuiCond_Appearing);
		ImGui::SetNextWindowPos({ (float)GetMousePos().x, (float)GetMousePos().y }, ImGuiCond_Appearing);
		ImGui::Begin("Layer Order Editor", &g_bChangeLayerOrderOpen);
		static char layer_order_change_buf[64] = ""; ImGui::InputText("Order", layer_order_change_buf, 64, ImGuiInputTextFlags_CharsDecimal);
		ImGui::SameLine();
		if (ImGui::ImageButton((ImTextureID)m_editorDecalDatabase["OK"]->id, { 16, 16 }))
		{
			ChangeLayerOrder(g_sAlteredLayer, atoi(layer_order_change_buf));
			memset(&layer_order_change_buf, 0, sizeof(layer_order_change_buf));
			g_bChangeLayerOrderOpen = false;
			g_sAlteredLayer = "none";
			UpdateLayerSorting();
		}
		ImGui::End();
	}
}

std::multimap< int, std::string, std::greater<int> > GameEditor::SortDescending(std::map< std::string, int >& map)
{
	std::multimap< int, std::string, std::greater<int> > m;

	// Copy everything.
	for (auto& p : map)
	{
		m.emplace(p.second, p.first);
	}

	return m;
}
std::multimap< int, std::string > GameEditor::SortAscending(std::map< std::string, int >& map)
{
	std::multimap< int, std::string > m;

	// Copy everything.
	for (auto& p : map)
	{
		m.emplace(p.second, p.first);
	}

	return m;
}
void GameEditor::DeleteRenderingLayer(std::string layer_name)
{
	auto entry = m_layerOrder.find(layer_name);
	if (entry != m_layerOrder.end())
	{
		m_layerOrder.erase(entry);
	}
	auto world_entry = m_gameworld.find(layer_name);
	if (world_entry != m_gameworld.end())
	{
		m_gameworld.erase(world_entry);
	}
}
void GameEditor::ChangeLayerName(std::string layer_name, std::string new_name)
{
	auto entry = m_layerOrder.find(layer_name);
	if (entry != m_layerOrder.end())
	{
		auto const value = std::move(entry->second);
		m_layerOrder.erase(entry);
		m_layerOrder.insert({ new_name, std::move(value) });
	}

	auto world_entry = m_gameworld.find(layer_name);
	if (world_entry != m_gameworld.end())
	{
		auto const value = std::move(world_entry->second);
		m_gameworld.erase(world_entry);
		m_gameworld.insert({ new_name, std::move(value) });
	}

}
void GameEditor::UpdateLayerSorting()
{
	m_sortedLayers = SortDescending(m_layerOrder);
}

int main()
{
	if (editor.Construct(1360, 768, 1, 1))
		editor.Start();

	return 0;
}