#include "Main.h"

// STATIC DATA
static bool g_bDecalDatabaseOpen = true;
static bool g_bEntityDatabaseOpen = true;
static bool g_bEntityEditorOpen = false;
static Entity* g_pEditedEntity = nullptr;
static std::vector< Entity* > g_vecEditedEntities;
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
static uint64_t g_iCreatedLayerCount = 1; // Used as next layer order number.
static bool g_bRenderGrid = true;



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
	if (g_bImguiDemoOpen) ImGui::ShowDemoWindow(&g_bImguiDemoOpen);

	// Decal Database
	if(g_bDecalDatabaseOpen) RenderDecalDatabase();
	// Entity Database
	if (g_bEntityDatabaseOpen) RenderEntityDatabase();
	// Rendering Layers
	if(g_bRenderingLayersOpen) RenderLayerUI();
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

	sprite = new olc::Sprite("assets/Editor/add_layer_layers.png");
	decal = new olc::Decal(sprite);
	m_editorDecalDatabase.try_emplace("AddLayer", decal);
	m_editorSpriteDatabase.push_back(sprite);

	sprite = new olc::Sprite("assets/Editor/target_dartboard_aim.png");
	decal = new olc::Decal(sprite);
	m_editorDecalDatabase.try_emplace("TargetLayer", decal);
	m_editorSpriteDatabase.push_back(sprite);

	sprite = new olc::Sprite("assets/Editor/view_eye.png");
	decal = new olc::Decal(sprite);
	m_editorDecalDatabase.try_emplace("Visible", decal);
	m_editorSpriteDatabase.push_back(sprite);

	sprite = new olc::Sprite("assets/Editor/button_circle_round_add.png");
	decal = new olc::Decal(sprite);
	m_editorDecalDatabase.try_emplace("Add", decal);
	m_editorSpriteDatabase.push_back(sprite);

	sprite = new olc::Sprite("assets/Editor/button_circle_round_remove.png");
	decal = new olc::Decal(sprite);
	m_editorDecalDatabase.try_emplace("Remove", decal);
	m_editorSpriteDatabase.push_back(sprite);

	sprite = new olc::Sprite("assets/Editor/Rect.png");
	decal = new olc::Decal(sprite);
	m_editorDecalDatabase.try_emplace("Rect", decal);
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
			if (ImGui::MenuItem("Entity Database")) ToggleMenuItem(g_bEntityDatabaseOpen);
			if (ImGui::MenuItem("Rendering Layers")) ToggleMenuItem(g_bRenderingLayersOpen);
			if (ImGui::MenuItem("Grid")) ToggleMenuItem(g_bRenderGrid);
			if (ImGui::MenuItem("Imgui Demo")) ToggleMenuItem(g_bImguiDemoOpen);
			if (ImGui::MenuItem("Save")) ExportMapData("assets/Map.xml");
			if (ImGui::MenuItem("Load")) ImportMapData("assets/Map.xml");


			ImGui::EndMenu();
		}
	}
	ImGui::EndMainMenuBar();
}
void GameEditor::RenderDecalDatabase()
{
	int i = 0;
	ImGui::Begin("DecalDatabase", &g_bDecalDatabaseOpen);
	if (ImGui::BeginTabBar("DecalDatabaseTab", ImGuiTabBarFlags_None))
	{
		if (ImGui::BeginTabItem("Forest"))
		{
			ImGui::Separator();
			RenderDecalDatabase(m_forestDecalDatabase);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Ground"))
		{
			ImGui::Separator();
			RenderDecalDatabase(m_groundDecalDatabase);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Water"))
		{
			ImGui::Separator();
			RenderDecalDatabase(m_waterDecalDatabase);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Road"))
		{
			ImGui::Separator();
			RenderDecalDatabase(m_roadDecalDatabase);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Mountain"))
		{
			ImGui::Separator();
			RenderDecalDatabase(m_mountainDecalDatabase);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Structure"))
		{
			ImGui::Separator();
			RenderDecalDatabase(m_structureDecalDatabase);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	ImGui::End();
}
void GameEditor::RenderDecalDatabase(const std::map< std::string, olc::Decal* >& db)
{
	int i = 0;
	for (auto& pair : db)
	{
		if (i == 4)
		{
			i = 0;
		}
		else
		{
			ImGui::SameLine();
		}

		if (ImGui::ImageButton((ImTextureID)pair.second->id, { 64, 64 }))
		{
			g_sSelectedMapobject = pair.first;
			ImGui::SetWindowFocus(nullptr);
		}
		i++;
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

	olc::vi2d upLeft = m_visiblePointLeftUp;
	olc::vi2d downRight = m_visiblePointDownRight;

	
	// Draw Mapobjects in reverse order to have accurate layering.
	for (auto& layer : m_sortedLayersAscending)
	{
		if (LayerVisible(layer.first) == false) continue;

		// We draw Layers from 0 in ascending order.
		const auto& layer_name = layer.second;
		const auto& layer_world = m_gameworld[layer_name];
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

	if (g_bRenderGrid)
	{
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
	}

	
	olc::vi2d mouse = GetMousePos();
	olc::vi2d point = tv.ScreenToWorld(mouse);

	// Draw selected Decal.
	if (g_sSelectedMapobject.compare("none") != 0)
	{
		tv.DrawDecal({ (float)point.x, (float)point.y }, m_decalDatabase[g_sSelectedMapobject]);
	}

	tv.DrawDecal(point, m_editorDecalDatabase["Rect"], { 1.0f, 1.0f }, olc::YELLOW);
}
void GameEditor::RenderMapobject(Entity* object)
{
	if (auto c = object->Get< ComponentSprite >("Sprite"); c)
	{
		tv.DrawDecal({ object->m_positionx, object->m_positiony }, m_decalDatabase[c->m_decal]);
	}
	else
	{
		tv.DrawStringDecal({ object->m_positionx, object->m_positiony }, object->m_name);
	}
}
bool GameEditor::LoadTilesetData(const std::string& database, const std::string& tilesetpath, const std::string& datapath)
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
			if (sprite->width == 0 && sprite->height == 0) continue;
			auto decal = new olc::Decal(sprite);

			if (database.compare("Forest") == 0)
			{
				m_forestDecalDatabase.try_emplace(name, decal);
			}
			else if (database.compare("Road") == 0)
			{
				m_roadDecalDatabase.try_emplace(name, decal);
			}
			else if (database.compare("Ground") == 0)
			{
				m_groundDecalDatabase.try_emplace(name, decal);
			}
			else if (database.compare("Water") == 0)
			{
				m_waterDecalDatabase.try_emplace(name, decal);
			}
			else if (database.compare("Mountain") == 0)
			{
				m_mountainDecalDatabase.try_emplace(name, decal);
			}
			else if (database.compare("Structure") == 0)
			{
				m_structureDecalDatabase.try_emplace(name, decal);
			}

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
			if (auto entity = GetMapobjectAt(mousex, mousey, m_currentLayer); entity != nullptr)
			{
				DeleteMapobject(entity);
			}
			g_sSelectedMapobject = "none";
		}
		if (GetMouse(0).bReleased)
		{
			if (g_sSelectedMapobject.compare("none") != 0)
			{
				CreateMapobject(point.x, point.y, g_sSelectedMapobject, g_sSelectedMapobject);
			}
			else
			{
				// Make the Selected Mapobject the one on the Tile in the current Layer.
				g_sSelectedMapobject = GetMapobjectNameAt(mousex, mousey, m_currentLayer);
			}
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
	m_visiblePointDownRight = tv.GetBottomRightTile();
	if (m_visiblePointDownRight.x > MAX_MAPSIZE_X) m_visiblePointDownRight.x = MAX_MAPSIZE_X;
	if (m_visiblePointDownRight.y > MAX_MAPSIZE_Y) m_visiblePointDownRight.y = MAX_MAPSIZE_Y;
}
std::string GameEditor::GetMapobjectNameAt(int x, int y, std::string layer)
{
	if (x < 0 ||
		y < 0 ||
		x > MAX_MAPSIZE_X - 1 ||
		y > MAX_MAPSIZE_Y - 1) return "none";

	if (m_gameworld.find(layer) != m_gameworld.end() &&
		m_gameworld[layer][x][y] != nullptr)
	{
		return m_gameworld[layer][x][y]->Get< ComponentSprite >("Sprite")->m_decal;
	}

	return "none";
}
Entity* GameEditor::GetMapobjectAt(int x, int y, std::string layer)
{
	if (x < 0 ||
		y < 0 ||
		x > MAX_MAPSIZE_X - 1 ||
		y > MAX_MAPSIZE_Y - 1) return nullptr;


	if (m_gameworld.find(layer) != m_gameworld.end() &&
		m_gameworld[layer][x][y] != nullptr)
	{
		return m_gameworld[layer][x][y];
	}

	return nullptr;
}
std::string GameEditor::CreateMapobjectName()
{
	return "Mapobject_" + std::to_string(m_mapobjectCount++);
}
void GameEditor::CreateMapobject(uint64_t x, uint64_t y, std::string layer, std::string decal, uint64_t w, uint64_t h)
{
	auto current_layer = m_currentLayer;
	m_currentLayer = layer;

	auto entity = CreateMapobject(x, y, decal, decal);
	entity->Get< ComponentSprite >("Sprite")->m_width = w;
	entity->Get< ComponentSprite >("Sprite")->m_height = h;

	m_currentLayer = current_layer;
}
Entity* GameEditor::CreateMapobject(uint64_t x, uint64_t y, std::string decal, std::string name)
{
	if (x < 0 || 
		y < 0 ||
		x > MAX_MAPSIZE_X - 1 ||
		y > MAX_MAPSIZE_Y - 1) return nullptr;

	if (name.compare("none") == 0 || IsMapobjectNameUsed(name))
	{
		name += "_" + CreateMapobjectName();
	}
	else
	{
		m_mapobjectCount++;
	}

	// Create object.
	auto object = new Entity();
	m_entities.push_back(object);
	object->Add(new ComponentSprite(decal, m_currentLayer), "Sprite");
	object->m_name = name;
	object->m_positionx = x;
	object->m_positiony = y;

	// If there is another Object already, delete it first.
	if (m_gameworld[m_currentLayer][x][y])
	{
		DeleteMapobject(m_gameworld[m_currentLayer][x][y]);
	}

	// Add Object to layer Gameworld.
	m_gameworld[m_currentLayer][x][y] = object;
	return object;
}
bool GameEditor::IsMapobjectNameUsed(const std::string& name)
{
	for (auto& e : m_entities)
	{
		if (e->m_name.compare(name.c_str()) == 0) return true;
	}
	return false;
}
void GameEditor::DeleteMapobject(Entity* object)
{
	int x, y;
	Entity* entity = nullptr;
	std::string layer = object->Get< ComponentSprite >("Sprite")->m_layer;
	for (auto it = m_entities.begin(); it != m_entities.end(); it++)
	{
		auto e = *it;
		if (e->m_name.compare(object->m_name.c_str()) == 0)
		{
			x = e->m_positionx;
			y = e->m_positiony;
			m_entities.erase(it);
			entity = e;
			break;
		}
	}

	for (auto it = g_vecEditedEntities.begin(); it != g_vecEditedEntities.end(); it++)
	{
		auto e = *it;
		if (e->m_name.compare(object->m_name.c_str()) == 0)
		{
			x = e->m_positionx;
			y = e->m_positiony;
			g_vecEditedEntities.erase(it);
			break;
		}
	}

	delete entity;
	m_gameworld[layer][x][y] = nullptr;
}
void GameEditor::UpdateEntities()
{
	for (const auto& e : m_entities)
	{
		e->Update();
	}
}
void GameEditor::RenderEntityDatabase()
{
	ImGui::Begin("EntityDatabase", &g_bEntityDatabaseOpen);
	for (const auto& e : m_entities)
	{
		// Will Entity be edited?
		bool open = false;
		if (ImGui::Button(e->m_name.c_str()))
		{
			open = true;
		}

		// Display Entity Icon for quick changes.
		ImGuiID delete_id = g_iImguiImageButtonID + (intptr_t)e + (intptr_t)"Delete";
		ImGuiID edit_id = g_iImguiImageButtonID + (intptr_t)e + (intptr_t)"Pencil";
		
		ImGui::SameLine();
		ImGui::PushID(edit_id);
		if (ImGui::ImageButton((ImTextureID)m_editorDecalDatabase["Pencil"]->id, { DEFAULT_WIDGET_IMAGE_SIZE_X, DEFAULT_WIDGET_IMAGE_SIZE_Y }))
		{
			g_bEntityEditorOpen = true;
			g_vecEditedEntities.push_back(e);
		}
		ImGui::PopID();
		
		if (!g_bEntityEditorOpen)
		{
			ImGui::SameLine();
			ImGui::PushID(delete_id);
			if (ImGui::ImageButton((ImTextureID)m_editorDecalDatabase["Delete"]->id, { DEFAULT_WIDGET_IMAGE_SIZE_X, DEFAULT_WIDGET_IMAGE_SIZE_Y }))
			{
				DeleteMapobject(e);
				ImGui::PopID();
				ImGui::End();
				return;
			}
			ImGui::PopID();
		}
	}
	ImGui::End();


	if (g_bEntityEditorOpen)
	{
		for (auto& e : g_vecEditedEntities)
		{
			DisplayEntityEditor(e);
		}
	}
}
void GameEditor::DisplayEntityEditor(Entity* e)
{
	std::string name = "Entity Edit: " + e->m_name;
	ImGui::SetNextWindowPos(ImVec2(ScreenWidth() / 2.0f - ScreenWidth() / 4.0f, ScreenHeight() / 2.0f - ScreenHeight() / 4.0f), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(ImVec2(500, 250), ImGuiCond_Appearing);
	ImGui::Begin(name.c_str(), &g_bEntityEditorOpen);
	ImGui::End();
}
void GameEditor::CreateRenderingLayer(std::string layer_name, int order)
{
	// If a layer already exists, dont do anything.
	if (m_layerOrder.find(layer_name) != m_layerOrder.end()) return;

	// Create a layer and initialize the Matrix.
	m_layerOrder[layer_name] = order;
	InitializeMatrix(m_gameworld[layer_name]);
	g_iCreatedLayerCount++;
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
void GameEditor::InitializeMatrix(std::vector< std::vector< Entity* > >& matrix)
{
	matrix.resize(MAX_MAPSIZE_X);
	for (int i = 0; i < matrix.size(); i++)
	{
		matrix[i].resize(MAX_MAPSIZE_Y);
	}
}
void GameEditor::RenderLayerUI()
{
	bool layers_dirty = false;

	ImGui::Begin("Rendering Layers", &g_bRenderingLayersOpen);

	static char layer_name_buf[64] = ""; ImGui::InputText("|", layer_name_buf, 64);
	ImGui::SameLine();

	ImGuiID add_layer_id = g_iImguiImageButtonID + (intptr_t)"AddLayer";
	ImGui::PushID(add_layer_id);
	if (ImGui::ImageButton((ImTextureID)m_editorDecalDatabase["AddLayer"]->id, { DEFAULT_WIDGET_IMAGE_SIZE_X, DEFAULT_WIDGET_IMAGE_SIZE_Y }))
	{
		CreateRenderingLayer(std::string(layer_name_buf), g_iCreatedLayerCount);
		memset(&layer_name_buf, 0, sizeof(layer_name_buf));
		layers_dirty = true;
	}
	ImGui::PopID();
	BeginTooltip("Create rendering layer");


	bool is_layer_normal;
	for (auto& layer : m_sortedLayers)
	{
		is_layer_normal = true;
		// Highlight active layer with red text.
		// Highlight unvisible layer with greyish text.
		// Highlight visible or normal layer with green text.
		if (layer.second.compare(m_currentLayer.c_str()) == 0)
		{
			if (LayerVisible(layer.first))
			{
				// Active and Visible
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.1f, 1.0f));
			}
			else
			{
				// Active but invisible
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.2f, 0.0f, 1.0f));
			}
			is_layer_normal = false;
		}
		else if (LayerVisible(layer.first) == false)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
			is_layer_normal = false;
		}
		if (is_layer_normal)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
		}
		ImGui::Text("\"%s\" [%d]", layer.second.c_str(), layer.first);
		ImGui::SameLine();
		ImGui::PopStyleColor();

		ImGuiID pencil_id = g_iImguiImageButtonID + layer.first + (intptr_t)"Pencil";
		ImGuiID slider_id = g_iImguiImageButtonID + layer.first + (intptr_t)"Slider";
		ImGuiID delete_id = g_iImguiImageButtonID + layer.first + (intptr_t)"Delete";
		ImGuiID target_id = g_iImguiImageButtonID + layer.first + (intptr_t)"Target";
		ImGuiID visible_id = g_iImguiImageButtonID + layer.first + (intptr_t)"Visible";

		// Create all the buttons with functionality for each layer.
		ImGui::PushID(pencil_id);
		if (ImGui::ImageButton((ImTextureID)m_editorDecalDatabase["Pencil"]->id, { DEFAULT_WIDGET_IMAGE_SIZE_X, DEFAULT_WIDGET_IMAGE_SIZE_Y }))
		{
			g_bChangeLayerNameOpen = true;
			g_sAlteredLayer = layer.second;
		}
		ImGui::PopID();
		BeginTooltip("Edit layer name");
		ImGui::SameLine();
		ImGui::PushID(slider_id);
		if (ImGui::ImageButton((ImTextureID)m_editorDecalDatabase["Slider"]->id, { DEFAULT_WIDGET_IMAGE_SIZE_X, DEFAULT_WIDGET_IMAGE_SIZE_Y }))
		{
			g_bChangeLayerOrderOpen = true;
			g_sAlteredLayer = layer.second;
		}
		ImGui::PopID();
		BeginTooltip("Edit layer order");
		ImGui::SameLine();
		ImGui::PushID(delete_id);
		if (ImGui::ImageButton((ImTextureID)m_editorDecalDatabase["Delete"]->id, { DEFAULT_WIDGET_IMAGE_SIZE_X, DEFAULT_WIDGET_IMAGE_SIZE_Y }))
		{
			DeleteRenderingLayer(layer.second);
			layers_dirty = true;
		}
		ImGui::PopID();
		BeginTooltip("Delete layer");

		ImGui::SameLine();
		ImGui::PushID(target_id);
		if (ImGui::ImageButton((ImTextureID)m_editorDecalDatabase["TargetLayer"]->id, { DEFAULT_WIDGET_IMAGE_SIZE_X, DEFAULT_WIDGET_IMAGE_SIZE_Y }))
		{
			m_currentLayer = layer.second;
		}
		ImGui::PopID();
		BeginTooltip("Select target layer");

		ImGui::SameLine();
		ImGui::PushID(visible_id);
		if (ImGui::ImageButton((ImTextureID)m_editorDecalDatabase["Visible"]->id, { DEFAULT_WIDGET_IMAGE_SIZE_X, DEFAULT_WIDGET_IMAGE_SIZE_Y }))
		{
			ToggleLayerVisibility(layer.first);
		}
		ImGui::PopID();
		BeginTooltip("Toggle visibility");
	}
	ImGui::End();

	if (g_bChangeLayerNameOpen)
	{
		ImGui::SetNextWindowSize({ 250, 100 }, ImGuiCond_Appearing);
		ImGui::SetNextWindowPos({ (float)GetMousePos().x, (float)GetMousePos().y}, ImGuiCond_Appearing);
		ImGui::Begin("Name Edit", &g_bChangeLayerNameOpen);
		static char layer_name_change_buf[64] = ""; ImGui::InputText(".", layer_name_change_buf, 64);
		ImGui::SameLine();
		if (ImGui::ImageButton((ImTextureID)m_editorDecalDatabase["OK"]->id, { DEFAULT_WIDGET_IMAGE_SIZE_X, DEFAULT_WIDGET_IMAGE_SIZE_Y }))
		{
			ChangeLayerName(g_sAlteredLayer, std::string(layer_name_change_buf));
			memset(&layer_name_change_buf, 0, sizeof(layer_name_change_buf));
			g_bChangeLayerNameOpen = false;
			g_sAlteredLayer = "none";
			layers_dirty = true;
		}
		ImGui::End();
	}
	if (g_bChangeLayerOrderOpen)
	{
		ImGui::SetNextWindowSize({ 250, 100 }, ImGuiCond_Appearing);
		ImGui::SetNextWindowPos({ (float)GetMousePos().x, (float)GetMousePos().y }, ImGuiCond_Appearing);
		ImGui::Begin("Order Edit", &g_bChangeLayerOrderOpen);
		static char layer_order_change_buf[64] = ""; ImGui::InputText(".", layer_order_change_buf, 64, ImGuiInputTextFlags_CharsDecimal);
		ImGui::SameLine();
		if (ImGui::ImageButton((ImTextureID)m_editorDecalDatabase["OK"]->id, { DEFAULT_WIDGET_IMAGE_SIZE_X, DEFAULT_WIDGET_IMAGE_SIZE_Y }))
		{
			ChangeLayerOrder(g_sAlteredLayer, atoi(layer_order_change_buf));
			memset(&layer_order_change_buf, 0, sizeof(layer_order_change_buf));
			g_bChangeLayerOrderOpen = false;
			g_sAlteredLayer = "none";
			layers_dirty = true;
		}
		ImGui::End();
	}


	if (layers_dirty)
	{
		UpdateLayerSorting();
	}
}
void GameEditor::ToggleLayerVisibility(int layer)
{
	m_visibleLayers[layer] = (m_visibleLayers[layer] == 1) ? 0 : 1;
}
bool GameEditor::LayerVisible(int layer)
{
	return m_visibleLayers[layer] == 1;
}
void GameEditor::BeginTooltip(const char* help_text)
{
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(help_text);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
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
std::map< int, std::string > GameEditor::SortAscending(std::map< std::string, int >& map)
{
	std::map< int, std::string > m;

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
	m_sortedLayersAscending = SortAscending(m_layerOrder);
}
bool GameEditor::ExportMapData(const std::string& filepath)
{
	tinyxml2::XMLDocument doc;
	auto root = doc.NewElement("Map");
	doc.InsertEndChild(root);

	auto layers = root->InsertNewChildElement("Layers");
	for (auto& l : m_sortedLayers)
	{
		auto layer = m_gameworld[l.second];

		auto xmlLayer = layers->InsertNewChildElement("Layer");
		xmlLayer->SetAttribute("n", l.first);
		xmlLayer->SetAttribute("name", l.second.c_str());

		for (int x = 0; x < layer.size(); x++)
		{
			for (int y = 0; y < layer[x].size(); y++)
			{
				if (layer[x][y])
				{
					auto object = xmlLayer->InsertNewChildElement("Object");

					// Insert relevant Data for each Object.
					// Position
					object->SetAttribute("x", layer[x][y]->m_positionx);
					object->SetAttribute("y", layer[x][y]->m_positiony);

					// Sprite
					object->SetAttribute("sprite", layer[x][y]->Get< ComponentSprite >("Sprite")->m_decal.c_str());

					// Width/Height
					object->SetAttribute("w", layer[x][y]->Get< ComponentSprite >("Sprite")->m_width);
					object->SetAttribute("h", layer[x][y]->Get< ComponentSprite >("Sprite")->m_height);
				}
			}
		}
	}



	doc.SaveFile(filepath.c_str());
	return true;
}
bool GameEditor::ImportMapData(const std::string& filepath)
{
	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(filepath.c_str()) != tinyxml2::XMLError::XML_SUCCESS)
	{
		doc.Clear();
		return false;
	}


	auto root = doc.RootElement();
	auto layers = root->FirstChildElement("Layers");
	auto layer = layers->FirstChildElement("Layer");
	while (layer)
	{
		// Get layer data and create Layer in Project.
		int layer_number = layer->IntAttribute("n");
		const char* layer_name = layer->Attribute("name");

		CreateRenderingLayer(std::string(layer_name), layer_number);

		auto entity = layer->FirstChildElement("Object");
		while (entity)
		{
			// Get object data and create it in Project.
			int x, y, w, h;
			x = entity->IntAttribute("x");
			y = entity->IntAttribute("y");
			w = entity->IntAttribute("w");
			h = entity->IntAttribute("h");
			const char* sprite = entity->Attribute("sprite");

			CreateMapobject(x, y, std::string(layer_name), std::string(sprite), w, h);

			entity = entity->NextSiblingElement("Object");
		}

		layer = layer->NextSiblingElement("Layer");
	}


	UpdateLayerSorting();
	return true;
}


int main()
{
	if (editor.Construct(1360, 768, 1, 1))
		editor.Start();

	return 0;
}