#include "Main.h"

// STATIC DATA
static bool g_bDecalDatabaseOpen = true;
static bool g_bEntityDatabaseOpen = true;
static bool g_bEntityEditorOpen = false;

static bool g_bBackgroundAudioEditorOpen = false;
static bool g_bAudioSoundChannelEditorOpen = false;
static Tree* g_SoundChannelTree = new Tree("Master");
static int g_iPlayingBackgroundAudio = 0;
static bool g_bAddingSoundSource = false;
static std::map< std::string, Entity* > g_InGameSoundSourcesMap;
static bool g_bInGameSoundSourcesMapDirty = false;
static std::string g_sSoundSource = "none";
static bool g_bRenderSoundSourceDimensions = false;
static bool g_bEditingSoundSource = false;
static Entity* g_pEditedSoundSource = nullptr;
static Entity* g_pEditedEntity = nullptr;
static Entity* g_pEditedCity = nullptr;
static bool g_bAddingTownhall = false;
static bool g_bAddingFort = false;
static bool g_bAddingBuildingSlot = false;
static bool g_bAddingTerritory = false;
static bool g_bRenderCityTerritory = true;
static bool g_bRenderCityBuildingSlots = true;
static std::string g_sDefaultCityLayer = "Building";
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
static float g_fMouseXPrevious = 0.0f;
static float g_fMouseYPrevious = 0.0f;


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
	// Ambient Sound Editor
	if (g_bBackgroundAudioEditorOpen) DisplayBackgroundAudioEditor();
	// SoundChannel Editor
	if (g_bAudioSoundChannelEditorOpen) DisplaySoundChannelEditor();
	// Sound source editing
	if (g_bEditingSoundSource) DisplaySoundSourceEditor(g_pEditedSoundSource);
	// Sound Source Map Update
	if (g_bInGameSoundSourcesMapDirty) UpdateInGameSoundSourcesMap(g_InGameSoundSourcesMap);
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

	sprite = new olc::Sprite("assets/Editor/FilledRect.png");
	decal = new olc::Decal(sprite);
	m_editorDecalDatabase.try_emplace("FilledRect", decal);
	m_editorSpriteDatabase.push_back(sprite);


	sprite = new olc::Sprite("assets/Editor/media_player_ui_button_play.png");
	decal = new olc::Decal(sprite);
	m_editorDecalDatabase.try_emplace("Play", decal);
	m_editorSpriteDatabase.push_back(sprite);

	sprite = new olc::Sprite("assets/Editor/media_player_ui_button_repeat_loop.png");
	decal = new olc::Decal(sprite);
	m_editorDecalDatabase.try_emplace("Repeat", decal);
	m_editorSpriteDatabase.push_back(sprite);

	sprite = new olc::Sprite("assets/Editor/media_player_ui_button_stop.png");
	decal = new olc::Decal(sprite);
	m_editorDecalDatabase.try_emplace("Stop", decal);
	m_editorSpriteDatabase.push_back(sprite);

	return true;
}

bool GameEditor::LoadAudioData(const std::string& filepath)
{
	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(filepath.c_str()) != tinyxml2::XMLError::XML_SUCCESS)
	{
		doc.Clear();
		return false;
	}

	auto root = doc.RootElement();
	std::string main_path = root->Attribute("path");
	auto sound = root->FirstChildElement("Sound");
	while (sound)
	{
		std::string path = main_path;

		std::string sound_path = sound->Attribute("path");
		if (sound_path.compare("") == 0)
		{
			path += "/" +std::string(sound->GetText()) + ".wav";
		}
		else
		{
			path += "/" + sound_path + "/" + std::string(sound->GetText()) + ".wav";
		}

		int id = olc::SOUND::LoadAudioSample(path);
		if (id != -1)
		{
			// Store sound.
			m_soundMap.try_emplace(std::string(sound->GetText()), std::make_pair(false, id));
		}
		else
		{
			LOG_DBG_ERROR("[{:.4f}][LoadAudioData] Failed loading Sound \"{}\" at \"{}\"!", APP_RUN_TIME, sound->GetText(), path.c_str());
		}

		sound = sound->NextSiblingElement("Sound");
	}

	return true;
}

bool GameEditor::LoadSoundChannelTreeStandalone(const std::string& filepath, Tree* tree)
{
	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(filepath.c_str()) != tinyxml2::XMLError::XML_SUCCESS)
	{
		doc.Clear();
		return false;
	}
	auto root = doc.RootElement();
	
	// Set Tree root sound channel name.
	tree->m_name = root->Attribute("root");
	auto name = tree->m_name;

	auto node = root->FirstChildElement("Node");
	while (node)
	{
		tree->Node(name)->Node(node->Attribute("name"));
		auto tree_node = tree->Node(node->Attribute("name"));

		LoadSoundChannelNode(node, tree_node);

		node = node->NextSiblingElement("Node");
	}

	return true;
}
bool GameEditor::LoadSoundChannelTreeMapData(tinyxml2::XMLElement* xml, Tree* tree)
{
	auto node = xml->FirstChildElement("Node");
	while (node)
	{
		tree->Node(tree->m_name)->Node(node->Attribute("name"));
		auto tree_node = tree->Node(node->Attribute("name"));

		LoadSoundChannelNode(node, tree_node);

		node = node->NextSiblingElement("Node");
	}

	return true;
}
void GameEditor::LoadSoundChannelNode(tinyxml2::XMLElement* xml_node, Tree* tree)
{
	auto node = xml_node->FirstChildElement("Node");
	while (node)
	{
		auto name = tree->m_name;

		tree->Node(name)->Node(node->Attribute("name"));
		auto tree_node = tree->Node(node->Attribute("name"));

		LoadSoundChannelNode(node, tree_node);

		node = node->NextSiblingElement("Node");
	}
}

void GameEditor::RenderMainMenu()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{
			if (ImGui::MenuItem("Imgui Demo")) ToggleMenuItem(g_bImguiDemoOpen);
			if (ImGui::MenuItem("Save")) ExportMapData("assets/Map.xml");
			if (ImGui::MenuItem("Load"))
			{
				ImportMapData("assets/Map.xml");
				SetAllLayersVisible();
			}
			if (ImGui::MenuItem("Exit"))
			{
				olc_Terminate();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Project"))
		{
			if (ImGui::MenuItem("Decal Database"))
			{
				ToggleMenuItem(g_bDecalDatabaseOpen);
			}
			ImGui::SameLine();
			ImGui::Checkbox("Open", &g_bDecalDatabaseOpen);

			if (ImGui::MenuItem("Entity Database"))
			{
				ToggleMenuItem(g_bEntityDatabaseOpen);
			}
			ImGui::SameLine();
			ImGui::Checkbox("Open", &g_bEntityDatabaseOpen);

			if (ImGui::MenuItem("Rendering Grid"))
			{
				ToggleMenuItem(g_bRenderGrid);
			}
			ImGui::SameLine();
			ImGui::Checkbox("Open", &g_bRenderGrid);

			if (ImGui::MenuItem("Rendering City Territory"))
			{
				ToggleMenuItem(g_bRenderCityTerritory);
			}
			ImGui::SameLine();
			ImGui::Checkbox("Open", &g_bRenderCityTerritory);

			if (ImGui::MenuItem("Rendering City Buildind Slots"))
			{
				ToggleMenuItem(g_bRenderCityBuildingSlots);
			}
			ImGui::SameLine();
			ImGui::Checkbox("Open", &g_bRenderCityBuildingSlots);

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Map"))
		{
			if (ImGui::BeginMenu("Add"))
			{
				if (ImGui::MenuItem("Townhall"))
				{
					g_bAddingTownhall = true;
				}
				if (ImGui::MenuItem("Fort"))
				{
					g_bAddingFort = true;
				}
				if (ImGui::MenuItem("Building Slot"))
				{
					g_bAddingBuildingSlot = true;
				}
				if (ImGui::MenuItem("Territory"))
				{
					g_bAddingTerritory = true;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}


		if (ImGui::BeginMenu("Audio"))
		{
			if (ImGui::MenuItem("Edit Sources"))
			{
				ToggleMenuItem(g_bBackgroundAudioEditorOpen);
			}
			if (ImGui::MenuItem("Edit SoundChannels"))
			{
				ToggleMenuItem(g_bAudioSoundChannelEditorOpen);
			}

			if (ImGui::MenuItem("Display Source Dimensions"))
			{
				ToggleMenuItem(g_bRenderSoundSourceDimensions);
			}
			ImGui::SameLine();
			ImGui::Checkbox("Open", &g_bRenderSoundSourceDimensions);

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
		if (ImGui::BeginTabItem("Sea"))
		{
			ImGui::Separator();
			RenderDecalDatabase(m_waterDecalDatabase);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Bridge"))
		{
			ImGui::Separator();
			RenderDecalDatabase(m_bridgeDecalDatabase);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("River"))
		{
			ImGui::Separator();
			RenderDecalDatabase(m_riverDecalDatabase);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Wall"))
		{
			ImGui::Separator();
			RenderDecalDatabase(m_wallDecalDatabase);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Road"))
		{
			ImGui::Separator();
			RenderDecalDatabase(m_roadDecalDatabase);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Hill"))
		{
			ImGui::Separator();
			RenderDecalDatabase(m_hillDecalDatabase);
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
		if (ImGui::BeginTabItem("Unit"))
		{
			ImGui::Separator();
			RenderDecalDatabase(m_unitDecalDatabase);
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

	
	struct MapCell
	{
		olc::Pixel color;
		olc::vf2d position;

	};
	std::vector< MapCell > territory_cells;
	std::vector< MapCell > building_slot_cells;
	
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

					olc::Pixel slot_color = { 0, 255, 0, 255 };
					olc::Pixel terr_color = { 230, 0, 0, 100 };

					auto e = layer_world[x][y];

					// Render City/Fort specific data.
					if (e->Has("Townhall") || e->Has("Fort"))
					{
						bool townhall = true;
						auto component = e->Get< ComponentCity >("Townhall");
						if (!component)
						{
							component = e->Get< ComponentCity >("Fort");
							townhall = false;
						}

						olc::vf2d rect_size = { 3.0f, 2.0f };
						if (!townhall) rect_size.x = 2.0f;

						tv.DrawDecal(olc::vf2d(x, y), m_editorDecalDatabase["Rect"], rect_size, olc::RED);
						tv.DrawStringDecal(olc::vf2d(x + 0.5f, y + 0.5f), component->m_name.c_str(), olc::RED, { 2.0f, 2.0f });

						for (auto& slot : component->m_territory)
						{
							MapCell cell;
							cell.position = olc::vf2d(slot.first, slot.second);
							cell.color = terr_color;
							territory_cells.push_back(cell);
						}
						for (auto& slot : component->m_buildingSlots)
						{
							MapCell cell;
							cell.position = olc::vf2d(slot.first, slot.second);
							cell.color = slot_color;
							building_slot_cells.push_back(cell);
						}
					}

					// Render Audio Source specific data.
					if (g_bRenderSoundSourceDimensions)
					{
						if (e->Has("Sound"))
						{
							auto sound = e->Get< ComponentSound >("Sound");
							auto color = olc::Pixel(sound->r, sound->g, sound->b, sound->a);
							float xpos = e->m_positionx - sound->w;
							float ypos = e->m_positiony - sound->h;
							float w = sound->w * 2 + 1;
							float h = sound->h * 2 + 1;
							tv.DrawDecal({xpos, ypos}, m_editorDecalDatabase["FilledRect"], { w, h }, color);
						}
					}
				}
			}
		}
	}


	auto terr_cell_decal = m_editorDecalDatabase["FilledRect"];
	auto build_cell_decal = m_editorDecalDatabase["Rect"];
	if (g_bRenderCityTerritory)
	{
		// Draw City Territory
		for (auto& c : territory_cells)
		{
			tv.DrawDecal(c.position, terr_cell_decal, olc::vf2d(1.0f, 1.0f), c.color);
		}
	}
	if (g_bRenderCityBuildingSlots)
	{
		// Draw City Building Slots
		for (auto& c : building_slot_cells)
		{
			tv.DrawDecal(c.position, build_cell_decal, olc::vf2d(1.0f, 1.0f), c.color);
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


	// Draw some hint text for editing.
	if (g_pEditedCity == nullptr && ( g_bAddingBuildingSlot || g_bAddingTerritory ))
	{
		DrawStringDecal({ ScreenWidth() - ScreenWidth() * 0.75f, 50.0f }, "Select Townhall or Fort...", olc::RED, { 2.5f, 2.5f });
	}
	if (g_pEditedCity != nullptr && (g_bAddingBuildingSlot || g_bAddingTerritory))
	{
		DrawStringDecal({ ScreenWidth() - ScreenWidth() * 0.75f, 50.0f }, "Press ESC to Stop Editing...", olc::RED, { 2.5f, 2.5f });
	}

	// Draw Camera Position
	std::string position = std::to_string(m_camerax) + ":" + std::to_string(m_cameray);
	std::string height = std::to_string(m_cameraHeigth);
	DrawStringDecal({10.0f, 45.0f }, position);
	DrawStringDecal({ 10.0f, 60.0f }, height);


	// Draw Tile Number under Mouse.
	position = std::to_string(m_tilex) + ":" + std::to_string(m_tiley);
	tv.DrawStringDecal({ (float)m_tilex + 0.5f, (float)m_tiley + 0.5f }, position, olc::RED);
}
void GameEditor::RenderMapobject(Entity* object)
{
	if (auto c = object->Get< ComponentSprite >("Sprite"); c)
	{
		// Offset for Units which are 180x180.
		float offx = 0.0f, offy = 0.0f;
		float scalex = 1.0f, scaley = 1.0f;
		if (object->Get< ComponentUnit >("Unit"))
		{
			offy = DEFAULT_UNIT_DECAL_OFFSET_Y;
			offx = DEFAULT_UNIT_DECAL_OFFSET_X;
		}

		tv.DrawDecal({ object->m_positionx + offx, object->m_positiony + offy }, m_decalDatabase[c->m_decal], { scalex, scaley });
	}
	else
	{
		tv.DrawStringDecal({ object->m_positionx, object->m_positiony }, object->m_name, olc::WHITE, { 1.0f, 1.0f });
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
		const auto& imgs = tex["images"];
		for (const auto& img : imgs.GetArray())
		{
			std::string name = img["n"].GetString();

			// Here we only need the Name and the path in order to load the image
			// as an OLC sprite and create a decal...
			auto sprite = new olc::Sprite(tilesetpath + "/" + name + ".png");
			if (sprite->width == 0 && sprite->height == 0) continue;
			auto decal = new olc::Decal(sprite);

			SetDecalSize(name, sprite->width, sprite->height);

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
			else if (database.compare("Sea") == 0)
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
			else if (database.compare("Bridge") == 0)
			{
				m_bridgeDecalDatabase.try_emplace(name, decal);
			}
			else if (database.compare("Wall") == 0)
			{
				m_wallDecalDatabase.try_emplace(name, decal);
			}
			else if (database.compare("River") == 0)
			{
				m_riverDecalDatabase.try_emplace(name, decal);
			}
			else if (database.compare("Hill") == 0)
			{
				m_hillDecalDatabase.try_emplace(name, decal);
			}
			else if (database.compare("Unit") == 0)
			{
				m_unitDecalDatabase.try_emplace(name, decal);
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
	m_tilex = (uint64_t)mousex;
	m_tiley = (uint64_t)mousey;
	
	olc::vi2d topLeft = tv.GetTileUnderScreenPos({ 0, 0 });
	olc::vi2d bottomDown = tv.GetBottomRightTile();
	olc::vi2d middle = { bottomDown.x / 2, bottomDown.y / 2 };

	m_camerax = m_tilex;
	m_cameray = m_tiley;


	if (!g_bImguiHasFocus)
	{
		if (g_pEditedCity)
		{
			if (GetKey(olc::ESCAPE).bReleased)
			{
				g_pEditedCity = nullptr;
				g_bAddingBuildingSlot = false;
				g_bAddingTerritory = false;
			}
			if (GetMouse(0).bReleased)
			{
				if (g_bAddingBuildingSlot)
				{
					AddBuildingSlotToCity(g_pEditedCity, mousex, mousey);
				}
				else if (g_bAddingTerritory)
				{
					AddTerritoryToCity(g_pEditedCity, mousex, mousey);
				}
			}
			if (GetMouse(1).bReleased)
			{
				if (g_bAddingBuildingSlot)
				{
					RemoveBuildingSlotFromCity(g_pEditedCity, mousex, mousey);
				}
				else if (g_bAddingTerritory)
				{
					RemoveTerritoryFromCity(g_pEditedCity, mousex, mousey);
				}
			}
		}
		if (g_bAddingBuildingSlot || g_bAddingTerritory)
		{
			if (g_pEditedCity == nullptr)
			{
				if (GetMouse(0).bReleased)
				{
					auto e = GetMapobjectAt(mousex, mousey, g_sDefaultCityLayer);
					if (e)
					{
						g_pEditedCity = e;
					}
				}
			}
		}
		if (g_bAddingTownhall)
		{
			if (GetMouse(0).bReleased)
			{
				MakeMapobjectTownhall(mousex, mousey, g_sDefaultCityLayer);
				g_bAddingTownhall = false;
			}
		}
		if (g_bAddingFort)
		{
			if (GetMouse(0).bReleased)
			{
				MakeMapobjectFort(mousex, mousey, g_sDefaultCityLayer);
				g_bAddingFort = false;
			}
		}

		// Delete Mapobject.
		if (GetMouse(1).bHeld && (g_fMouseXPrevious != mousex) && (g_fMouseYPrevious != mousey))
		{
			g_fMouseXPrevious = mousex;
			g_fMouseYPrevious = mousey;

			if (auto entity = GetMapobjectAt(mousex, mousey, m_currentLayer); entity != nullptr)
			{
				DeleteMapobject(entity);
			}
			g_sSelectedMapobject = "none";
		}

		// Create new Mapobject.
		if (GetMouse(0).bHeld && (g_fMouseXPrevious != mousex) && (g_fMouseYPrevious != mousey))
		{
			g_fMouseXPrevious = mousex;
			g_fMouseYPrevious = mousey;

			if (g_sSelectedMapobject.compare("none") != 0)
			{
				CreateMapobjectEx(point.x, point.y, m_currentLayer, g_sSelectedMapobject);
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
		if (GetMouseWheel() > 0)
		{
			// Do not allow to zoom in too much.
			if (m_cameraHeigth > DEFAULT_MIN_ZOOM_DISTANCE)
			{
				tv.ZoomAtScreenPos(2.0f, GetMousePos());
				m_cameraHeigth -= DEFAULT_MAP_HEIGHT_STEP;
			}
		}
		if (GetMouseWheel() < 0)
		{
			// Do not allow to zoom out too much.
			if (m_cameraHeigth < DEFAULT_MAX_ZOOM_DISTANCE)
			{
				tv.ZoomAtScreenPos(0.5f, GetMousePos());
				m_cameraHeigth += DEFAULT_MAP_HEIGHT_STEP;
			}
		}

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


	// Handle input despite Imgui having focus.
	if (g_bAddingSoundSource)
	{
		if (GetMouse(0).bReleased)
		{
			auto sound_source = CreateMapobjectAudioSource(mousex, mousey, 2.0f, 2.0f, g_sSoundSource);
			g_bAddingSoundSource = false;
			g_sSoundSource = "none";


			auto sound_name = sound_source->Get< ComponentSound >("Sound")->m_soundName;
			auto name = sound_source->m_name + " (" + sound_name + ")";
			g_InGameSoundSourcesMap.try_emplace(name, sound_source);
		}
		if (GetMouse(1).bReleased || GetKey(olc::ESCAPE).bReleased)
		{
			g_bAddingSoundSource = false;
			g_sSoundSource = "none";
		}
	}
	if (!g_bAddingSoundSource)
	{
		if (GetMouse(0).bReleased)
		{
			auto sound_source = GetMapobjectAt(mousex, mousey, "AudioSourceLayer");
			if (sound_source)
			{
				g_bEditingSoundSource = true;
				g_pEditedSoundSource = sound_source;
			}
		}
		if (GetMouse(1).bReleased)
		{
			auto sound_source = GetMapobjectAt(mousex, mousey, "AudioSourceLayer");
			if (sound_source)
			{
				// Remove Sound Source from Project.
				auto sound_name = sound_source->Get< ComponentSound >("Sound")->m_soundName;
				auto name = sound_source->m_name + " (" + sound_name + ")";
				if (g_InGameSoundSourcesMap.find(name) != g_InGameSoundSourcesMap.end())
				{
					g_InGameSoundSourcesMap.erase(name);
				}
				DeleteMapobjectAudioSource(sound_source);
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

void GameEditor::SetDecalSize(const std::string& name, uint64_t w, uint64_t h)
{
	m_decalSizeDatabase.try_emplace(name, std::make_pair(w, h));
}

uint64_t GameEditor::GetDecalWidth(const std::string& name)
{
	if (m_decalSizeDatabase.find(name) != m_decalSizeDatabase.end())
	{
		return m_decalSizeDatabase[name].first;
	}
}

uint64_t GameEditor::GetDecalHeight(const std::string& name)
{
	if (m_decalSizeDatabase.find(name) != m_decalSizeDatabase.end())
	{
		return m_decalSizeDatabase[name].second;
	}
}

olc::Pixel GameEditor::GetRandomColor(uint64_t alpha /*= 255*/)
{
	int r = rand() % 255;
	int g = rand() % 255;
	int b = rand() % 255;

	return olc::Pixel(r, g, b, alpha);
}

bool GameEditor::CreateAndSubmitSoundChannelTree(Tree* tree)
{
	auto system = SoundSystem::get();

	// First release all channel groups if there are any.
	system->ReleaseAllChannelGroups();

	// Create the Master channel.
	bool result = system->CreateMasterChannelGroup(g_SoundChannelTree->m_name);

	// Create Channel Groups.
	for (auto& kid : g_SoundChannelTree->m_children)
	{
		result &= CreateAndSubmitSoundChannelNode(kid, g_SoundChannelTree->m_name);
	}

	return result;
}

bool GameEditor::CreateAndSubmitSoundChannelNode(Tree* tree, const std::string& parent)
{
	auto system = SoundSystem::get();

	bool result = system->CreateChannelGroup(tree->m_name, parent);

	for (auto& kid : tree->m_children)
	{
		result &= CreateAndSubmitSoundChannelNode(kid, tree->m_name);
	}

	return result;
}

void GameEditor::UpdateInGameSoundSourcesMap(std::map< std::string, Entity* >& map)
{
	std::map< std::string, std::string > changes;

	for (auto& pair : map)
	{
		// Check for different names.
		if (pair.first.compare(pair.second->m_name) != 0)
		{
			// Mem the change required.
			changes.emplace(pair.first, pair.second->m_name);
		}
	}

	// Apply the changes.
	for (auto& pair : changes)
	{
		auto node = map.extract(pair.first);

		node.key() = pair.second;

		map.insert(std::move(node));
	}
}

std::string GameEditor::CreateMapobjectName()
{
	return "Mapobject_" + std::to_string(m_mapobjectCount++);
}
Entity* GameEditor::CreateMapobjectAudioSource(uint64_t x, uint64_t y, uint64_t w, uint64_t h, const std::string& soundname)
{
	// Ensure Boundaries.
	if (x < 0 ||
		y < 0 ||
		x > MAX_MAPSIZE_X - 1 ||
		y > MAX_MAPSIZE_Y - 1) return nullptr;

	std::string name = "SoundSource_" + soundname;

	// Create a default Entity with 
	// a visual representation.
	//auto entity = CreateMapobject(x, y, "AudioSourceLayer", "AudioOn", soundname);
	if (name.compare("none") == 0 || IsMapobjectNameUsed(name))
	{
		name += "_" + CreateMapobjectName();
	}
	else
	{
		m_mapobjectCount++;
	}
	// Create object.
	auto entity = new Entity(name);
	m_entities.push_back(entity);
	entity->Add(new ComponentSprite("AudioOff", "AudioSourceLayer"), "Sprite");
	entity->m_positionx = x;
	entity->m_positiony = y;
	entity->Get< ComponentSprite >("Sprite")->m_width = DEFAULT_DECAL_SIZE_X;
	entity->Get< ComponentSprite >("Sprite")->m_height = DEFAULT_DECAL_SIZE_Y;
	
	auto sound_component = new ComponentSound(w, h, soundname);
	olc::Pixel color = GetRandomColor(150);
	sound_component->r = color.r;
	sound_component->g = color.g;
	sound_component->b = color.b;
	sound_component->a = color.a;
	entity->Add(sound_component, "Sound");
	

	// If there is another Object already, delete it first.
	if (m_gameworld["AudioSourceLayer"][x][y])
	{
		DeleteMapobject(m_gameworld["AudioSourceLayer"][x][y]);
	}

	// Add Object to layer Gameworld.
	m_gameworld["AudioSourceLayer"][x][y] = entity;
	return entity;
}
Entity* GameEditor::CreateMapobjectEx(uint64_t x, uint64_t y, std::string layer, std::string decal, std::string name)
{
	auto current_layer = m_currentLayer;
	m_currentLayer = layer;

	bool is_unit = false;
	if (m_unitDecalDatabase.find(decal) != m_unitDecalDatabase.end())
	{
		is_unit = true;
	}

	auto entity = CreateMapobject(x, y, decal, is_unit, name);
	if (entity == nullptr) return nullptr;

	float w, h;
	if (is_unit)
	{
		h = w = 180;
	}
	else
	{
		w = DEFAULT_DECAL_SIZE_X;
		h = DEFAULT_DECAL_SIZE_Y;
	}

	entity->Get< ComponentSprite >("Sprite")->m_width = w;
	entity->Get< ComponentSprite >("Sprite")->m_height = h;

	m_currentLayer = current_layer;
	return entity;
}
Entity* GameEditor::CreateMapobject(uint64_t x, uint64_t y, std::string decal, bool unit, std::string name)
{
	// Ensure Boundaries.
	if (x < 0 || 
		y < 0 ||
		x > MAX_MAPSIZE_X - 1 ||
		y > MAX_MAPSIZE_Y - 1) return nullptr;

	// Ensure Current layer selected valid.
	if (m_gameworld.find(m_currentLayer) == m_gameworld.end()) return nullptr;


	if (name.compare("none") == 0 || IsMapobjectNameUsed(name))
	{
		name = "Entity_" + CreateMapobjectName();
	}
	else
	{
		m_mapobjectCount++;
	}

	// Create object.
	auto object = new Entity(name);
	m_entities.push_back(object);
	object->Add(new ComponentSprite(decal, m_currentLayer), "Sprite");
	object->m_positionx = x;
	object->m_positiony = y;

	if (unit)
	{
		object->Add(new ComponentUnit(), "Unit");
	}

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

void GameEditor::DeleteMapobjectAudioSource(Entity* object)
{
	auto sound_component = object->Get< ComponentSound >("Sound");
	// Stop according sample from playing.
	m_soundMap[sound_component->m_soundName].first = false;
	olc::SOUND::StopSample(m_soundMap[sound_component->m_soundName].second);

	// Close the editing window if it is on.
	g_bEditingSoundSource = false;
	g_pEditedSoundSource = nullptr;
	g_sSoundSource = "none";

	// General delete of the Object.
	DeleteMapobject(object);
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
		BeginTooltip("Edit the Entity. Add/Remove components and/or alter data of them");

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
			BeginTooltip("Delete the Entity from the Map");
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

void GameEditor::DisplayBackgroundAudioEditor()
{
	static const char* name = "Audio Editor";
	ImGui::SetNextWindowPos(ImVec2(ScreenWidth() / 2.0f - ScreenWidth() / 4.0f, ScreenHeight() / 2.0f - ScreenHeight() / 4.0f), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(ImVec2(500, 250), ImGuiCond_Appearing);
	ImGui::Begin(name, &g_bBackgroundAudioEditorOpen);

	bool tree_open = ImGui::TreeNode("Audio Assets");
	ImGui::SameLine(); HelpMarker("Currently loaded and available audio assets");
	if (tree_open)
	{
		for (auto& p : m_soundMap)
		{
			if (ImGui::Button(p.first.c_str()))
			{
				// Selected a sound source to be placed on the map.
				g_bAddingSoundSource = true;
				g_sSoundSource = p.first;
			}
		}
		ImGui::TreePop();
	}

	tree_open = ImGui::TreeNode("Sound Sources");
	ImGui::SameLine(); HelpMarker("Sound sources currently placed on the map");
	if (tree_open)
	{
		for (auto& pair : g_InGameSoundSourcesMap)
		{
			if (ImGui::Button(pair.first.c_str()))
			{
				g_bEditingSoundSource = true;
				g_pEditedSoundSource = pair.second;
			}
		}

		ImGui::TreePop();
	}

	ImGui::End();
}

void GameEditor::DisplaySoundChannelEditor()
{
	static const char* name = "SoundChannel Editor";
	ImGui::SetNextWindowPos(ImVec2(ScreenWidth() / 2.0f - ScreenWidth() / 4.0f, ScreenHeight() / 2.0f - ScreenHeight() / 4.0f), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(ImVec2(500, 250), ImGuiCond_Appearing);
	ImGui::Begin(name, &g_bAudioSoundChannelEditorOpen);
	
	// The Tree Itself.
	if (ImGui::TreeNode(g_SoundChannelTree->m_name.c_str()))
	{
		for (int i = 0; i < g_SoundChannelTree->m_children.size(); i++)
		{
			DisplaySoundChannelNode(g_SoundChannelTree->m_children[i]);
		}
		ImGui::TreePop();
	}

	ImGui::Separator();
	if (ImGui::SmallButton("Create and Submit Tree"))
	{
		if (CreateAndSubmitSoundChannelTree(g_SoundChannelTree))
		{
			LOG_DBG_INFO("[{:.4f}][CreateAndSubmitSoundChannelTree] Success!", APP_RUN_TIME);
		}
		else
		{
			LOG_DBG_ERROR("[{:.4f}][CreateAndSubmitSoundChannelTree] Failed!", APP_RUN_TIME);
			SoundSystem::get()->ReleaseAllChannelGroups();
		}
	}
	if (ImGui::SmallButton("Reload Tree"))
	{
		if (LoadSoundChannelTreeStandalone("assets/Audio/SoundChannelTree.xml", g_SoundChannelTree))
		{
			LOG_DBG_INFO("[{:.4f}][LoadSoundChannelTree] Success!", APP_RUN_TIME);
		}
		else
		{
			LOG_DBG_ERROR("[{:.4f}][LoadSoundChannelTree] Failed!", APP_RUN_TIME);
		}
	}
	ImGui::End();
}

void GameEditor::DisplaySoundChannelNode(Tree* tree)
{
	if (ImGui::TreeNode(tree->m_name.c_str()))
	{
		for (int i = 0; i < tree->m_children.size(); i++)
		{
			DisplaySoundChannelNode(tree->m_children[i]);
		}
		ImGui::TreePop();
	}
}

void GameEditor::DisplaySoundSourceEditor(Entity* e)
{
	auto sound_component = e->Get< ComponentSound >("Sound");
	auto sound_name = sound_component->m_soundName;

	std::string window_title = "Sound Source Edit: " + e->m_name;
	ImGui::SetNextWindowPos(ImVec2(ScreenWidth() / 2.0f - ScreenWidth() / 4.0f, ScreenHeight() / 2.0f - ScreenHeight() / 4.0f), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(ImVec2(500, 250), ImGuiCond_Appearing);
	ImGui::Begin(window_title.c_str(), &g_bEditingSoundSource);

	ImGuiID play_sound_id = g_iImguiImageButtonID + strlen(sound_name.c_str()) + (intptr_t)"Play";
	ImGuiID stop_sound_id = g_iImguiImageButtonID + strlen(sound_name.c_str()) + (intptr_t)"Stop";
	ImGuiID loop_sound_id = g_iImguiImageButtonID + strlen(sound_name.c_str()) + (intptr_t)"Repeat";


	// Play or Stop Sound source and Set Looping.
	ImGui::PushID(loop_sound_id);
	if (ImGui::ImageButton((ImTextureID)m_editorDecalDatabase["Repeat"]->id, { DEFAULT_WIDGET_IMAGE_SIZE_X, DEFAULT_WIDGET_IMAGE_SIZE_Y }))
	{
		if (m_soundMap[sound_name].first == false)
		{
			olc::SOUND::PlaySample(m_soundMap[sound_name].second, true);
			m_soundMap[sound_name].first = true;
		}
		else
		{
			olc::SOUND::StopSample(m_soundMap[sound_name].second);
			olc::SOUND::PlaySample(m_soundMap[sound_name].second, true);
		}
	}
	ImGui::PopID();
	HelpMarkerWithoutQuestion("Play the Sound Source in a loop");
	
	
	ImGui::SameLine();
	ImGui::PushID(play_sound_id);
	if (ImGui::ImageButton((ImTextureID)m_editorDecalDatabase["Play"]->id, { DEFAULT_WIDGET_IMAGE_SIZE_X, DEFAULT_WIDGET_IMAGE_SIZE_Y }))
	{
		if (m_soundMap[sound_name].first == false)
		{
			olc::SOUND::PlaySample(m_soundMap[sound_name].second);
			m_soundMap[sound_name].first = true;
		}
		else
		{
			olc::SOUND::StopSample(m_soundMap[sound_name].second);
			olc::SOUND::PlaySample(m_soundMap[sound_name].second);
		}
	}
	ImGui::PopID();
	HelpMarkerWithoutQuestion("Play the Sound Source once");


	ImGui::SameLine();
	ImGui::PushID(stop_sound_id);
	if (ImGui::ImageButton((ImTextureID)m_editorDecalDatabase["Stop"]->id, { DEFAULT_WIDGET_IMAGE_SIZE_X, DEFAULT_WIDGET_IMAGE_SIZE_Y }))
	{
		if (m_soundMap[sound_name].first)
		{
			olc::SOUND::StopSample(m_soundMap[sound_name].second);
			m_soundMap[sound_name].first = false;
		}
	}
	ImGui::PopID();
	HelpMarkerWithoutQuestion("Stop the Sound Source");


	// Change Sound source name
	static char sound_source_name_buf[64] = ""; 
	ImGui::InputText("|", sound_source_name_buf, 64);
	HelpMarkerWithoutQuestion("Change the unique name of the Sound Source. The name does not change the sound file being played");
	ImGui::SameLine();
	if (ImGui::SmallButton("OK"))
	{
		auto name = std::string(sound_source_name_buf);

		// Do not Allow empty names.
		// Check whether the new name is a duplicate.
		if (name.size() != 0 && g_InGameSoundSourcesMap.find(name) == g_InGameSoundSourcesMap.end())
		{
			// Change the Entity Name.
			e->m_name = std::string(sound_source_name_buf);

			memset(&sound_source_name_buf, 0, sizeof(sound_source_name_buf));

			// Update the Map to adjust to changes.
			g_bInGameSoundSourcesMapDirty = true;
		}
		else
		{
			LOG_DBG_ERROR("[{:.4f}][DisplaySoundSourceEditor] Error changing Sound-Source-Entity name from \"{}\" to \"{}\"!", APP_RUN_TIME, e->m_name, name);
		}
	}
	
	// Change Channel Group
	DisplayChannelGroupChanger(e);

	// Change width and height.


	// Change color.

	
	ImGui::End();
}

void GameEditor::DisplayChannelGroupChanger(Entity* e)
{
	// Create an array for the imgui combo
	std::vector< std::string > sound_channel_group_vec;
	
	sound_channel_group_vec.push_back(g_SoundChannelTree->m_name);
	for (auto& kid : g_SoundChannelTree->m_children)
	{
		AddSoundChannelGroupToVec(kid, sound_channel_group_vec);
	}

	// Find our current item index in the vector.
	auto sound_component = e->Get< ComponentSound >("Sound");
	auto sound_channel_group = sound_component->m_soundChannelGroup;
	int current_item_index = 0;
	bool changed = false;
	for (int i = 0; i < sound_channel_group_vec.size(); i++)
	{
		if (sound_channel_group_vec[i].compare(sound_channel_group) == 0)
		{
			current_item_index = i;
			break;
		}
	}

	// Create the Combo window.
	if (ImGui::BeginCombo("SoundChannelGroup", sound_channel_group.c_str()))
	{
		for (int i = 0; i < sound_channel_group_vec.size(); i++)
		{
			const bool is_selected = (current_item_index == i);

			if (ImGui::Selectable(sound_channel_group_vec[i].c_str(), is_selected))
			{
				current_item_index = i;
				LOG_DBG_INFO("[{:.4f}][DisplayChannelGroupChanger] Changed Sound Channel Group to \"{}\"", APP_RUN_TIME, sound_channel_group_vec[current_item_index]);
				changed = true;
			}

			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	HelpMarkerWithoutQuestion("Change the channel group on which the sound will be played. The change will find place on the next \"Play\" command for the sound source");

	// Apply the change.
	if (changed) sound_component->m_soundChannelGroup = sound_channel_group_vec[current_item_index];
}


void GameEditor::AddSoundChannelGroupToVec(Tree* tree, std::vector< std::string >& vec)
{
	vec.push_back(tree->m_name);
	for (auto& kid : tree->m_children)
	{
		AddSoundChannelGroupToVec(kid, vec);
	}
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
	bool is_layer_uneditable;
	for (auto& layer : m_sortedLayers)
	{
		is_layer_uneditable = false;
		// Find whether this layer is permanent and uneditable.
		for (auto& n : m_PermanentLayersVec)
		{
			if (n == layer.first)
			{
				is_layer_uneditable = true;
				break;
			}
		}

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
		// Except the layer is permanent, then we do not create ALL of them.
		if (is_layer_uneditable == false)
		{
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

		}

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
void GameEditor::SetAllLayersVisible()
{
	for (auto& layer : m_layerOrder)
	{
		m_visibleLayers[layer.second] = 1;
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

		// Start Layer.
		auto xmlLayer = layers->InsertNewChildElement("Layer");
		xmlLayer->SetAttribute("n", l.first);
		xmlLayer->SetAttribute("name", l.second.c_str());

		// Start Entity Export.
		for (int x = 0; x < layer.size(); x++)
		{
			for (int y = 0; y < layer[x].size(); y++)
			{
				if (layer[x][y])
				{
					auto xml = xmlLayer->InsertNewChildElement("Object");
					auto entity = layer[x][y];
					ExportEntity(xml, entity);
				}
			}
		}
	}

	doc.SaveFile(filepath.c_str());
	return true;
}

void GameEditor::ExportEntity(tinyxml2::XMLElement* xml, Entity* entity)
{
	// Export default data.
	// Position
	xml->SetAttribute("x", entity->m_positionx);
	xml->SetAttribute("y", entity->m_positiony);

	// Sprite
	xml->SetAttribute("sprite", entity->Get< ComponentSprite >("Sprite")->m_decal.c_str());

	// Width/Height
	xml->SetAttribute("w", entity->Get< ComponentSprite >("Sprite")->m_width);
	xml->SetAttribute("h", entity->Get< ComponentSprite >("Sprite")->m_height);


	// Export component data.
	bool has_fort = entity->Has("Fort");
	bool has_townhall = entity->Has("Townhall");
	bool has_unit = entity->Has("Unit");
	bool has_sound = entity->Has("Sound");

	if (has_fort) ExportEntityComponentFort(xml, entity);
	if (has_townhall) ExportEntityComponentTownhall(xml, entity);
	if (has_unit) ExportEntityComponentUnit(xml, entity);
	if (has_sound) ExportEntityComponentSound(xml, entity);
}
void GameEditor::ExportEntityComponentSound(tinyxml2::XMLElement* xml, Entity* entity)
{
	auto sound_component = entity->Get< ComponentSound >("Sound");
	auto sound_xml = xml->InsertNewChildElement("Sound");

	sound_xml->SetAttribute("w", sound_component->w);
	sound_xml->SetAttribute("h", sound_component->h);
	sound_xml->SetAttribute("r", sound_component->r);
	sound_xml->SetAttribute("g", sound_component->g);
	sound_xml->SetAttribute("b", sound_component->b);
	sound_xml->SetAttribute("a", sound_component->a);
	sound_xml->SetAttribute("soundName", sound_component->m_soundName.c_str());
	sound_xml->SetAttribute("soundChannelGroupName", sound_component->m_soundChannelGroup.c_str());

	// Special for the sound component we export the custom given name,
	// this may cause problems on loading duplicate or errorous names.
	sound_xml->SetAttribute("entityName", entity->m_name.c_str());
}
void GameEditor::ExportEntityComponentFort(tinyxml2::XMLElement* xml, Entity* entity)
{
	auto fort = xml->InsertNewChildElement("Fort");
	auto building_slots = xml->InsertNewChildElement("BuildingSlots");
	auto territory = xml->InsertNewChildElement("Territory");

	auto component = entity->Get< ComponentTownhall >("Fort");

	for (auto bs : component->m_buildingSlots)
	{
		auto slot = building_slots->InsertNewChildElement("Slot");
		slot->SetAttribute("x", bs.first);
		slot->SetAttribute("y", bs.second);
	}
	for (auto tr : component->m_territory)
	{
		auto terr = territory->InsertNewChildElement("Slot");
		terr->SetAttribute("x", tr.first);
		terr->SetAttribute("y", tr.second);
	}
}
void GameEditor::ExportEntityComponentTownhall(tinyxml2::XMLElement* xml, Entity* entity)
{
	auto th = xml->InsertNewChildElement("Townhall");
	auto building_slots = xml->InsertNewChildElement("BuildingSlots");
	auto territory = xml->InsertNewChildElement("Territory");

	auto component = entity->Get< ComponentTownhall >("Townhall");

	for (auto bs : component->m_buildingSlots)
	{
		auto slot = building_slots->InsertNewChildElement("Slot");
		slot->SetAttribute("x", bs.first);
		slot->SetAttribute("y", bs.second);
	}
	for (auto tr : component->m_territory)
	{
		auto terr = territory->InsertNewChildElement("Slot");
		terr->SetAttribute("x", tr.first);
		terr->SetAttribute("y", tr.second);
	}
}
void GameEditor::ExportEntityComponentUnit(tinyxml2::XMLElement* xml, Entity* entity)
{
	LOG_DBG_ERROR("[{:.4f}][ExportEntityComponentUnit] Serializing ComponentUnit data not supported!", APP_RUN_TIME);
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

	// LOAD SOUND CHANNEL TREE
	auto sound_channel_tree = root->FirstChildElement("SoundChannelTree");
	if (sound_channel_tree)
	{
		if (LoadSoundChannelTreeMapData(sound_channel_tree, g_SoundChannelTree))
		{
			LOG_DBG_INFO("[{:.4f}][LoadSoundChannelTreeMapData] Success loading SoundChannelTree!", APP_RUN_TIME);
		}
		else
		{
			LOG_DBG_ERROR("[{:.4f}][LoadSoundChannelTreeMapData] Failed loading SoundChannelTree!", APP_RUN_TIME);
		}
	}


	// LOAD LAYER DATA.
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
			auto obj = ImportEntity(entity, layer_name);


			auto townhall = entity->FirstChildElement("Townhall");
			auto fort = entity->FirstChildElement("Fort");
			auto sound = entity->FirstChildElement("Sound");


			if (townhall)
			{
				ImportEntityComponentTownhall(townhall, obj);
			}
			if (fort)
			{
				ImportEntityComponentFort(fort, obj);
			}
			if (sound)
			{
				ImportEntityComponentSound(sound, obj);
			}


			entity = entity->NextSiblingElement("Object");
		}

		layer = layer->NextSiblingElement("Layer");
	}


	UpdateLayerSorting();
	LOG_DBG_INFO("[{:.4f}][ImportMapData] Success!", APP_RUN_TIME);
	return true;
}

Entity* GameEditor::ImportEntity(tinyxml2::XMLElement* xml, const std::string& layer)
{
	int x, y, w, h;

	x = xml->IntAttribute("x");
	y = xml->IntAttribute("y");
	w = xml->IntAttribute("w");
	h = xml->IntAttribute("h");

	const char* sprite = xml->Attribute("sprite");

	auto object = CreateMapobjectEx(x, y, layer, sprite);

	return object;
}

void GameEditor::ImportEntityComponentSound(tinyxml2::XMLElement* xml, Entity* entity)
{
	auto w = xml->IntAttribute("w", 1);
	auto h = xml->IntAttribute("h", 1);
	auto r = xml->IntAttribute("r", 1);
	auto g = xml->IntAttribute("g", 1);
	auto b = xml->IntAttribute("b", 1);
	auto a = xml->IntAttribute("a", 1);
	auto sound_name = xml->Attribute("soundName");
	auto group = xml->Attribute("soundChannelGroupName");
	auto sound_entity_name = xml->Attribute("entityName");

	entity->Add(new ComponentSound(w, h, r, g, b, a, sound_name, group), "Sound");
	entity->m_name = sound_entity_name;
	
	g_InGameSoundSourcesMap.try_emplace(entity->m_name, entity);
}

void GameEditor::ImportEntityComponentFort(tinyxml2::XMLElement* xml, Entity* entity)
{
	entity->Add(new ComponentFort(entity->m_positionx, entity->m_positiony), "Fort");
	auto fort = entity->Get< ComponentFort >("Fort");

	// Get Building slots and Territory defined for Entity.
	auto bs = xml->FirstChildElement("BuildingSlots");
	auto bslot = bs->FirstChildElement("Slot");
	while (bslot)
	{
		fort->AddBuildingSlot(bslot->IntAttribute("x"), bslot->IntAttribute("y"));

		bslot = bslot->NextSiblingElement("Slot");
	}
	auto ts = xml->FirstChildElement("Territory");
	auto tslot = ts->FirstChildElement("Slot");
	while (tslot)
	{
		fort->AddTerritory(tslot->IntAttribute("x"), tslot->IntAttribute("y"));

		tslot = tslot->NextSiblingElement("Slot");
	}
}

void GameEditor::ImportEntityComponentTownhall(tinyxml2::XMLElement* xml, Entity* entity)
{
	entity->Add(new ComponentTownhall(entity->m_positionx, entity->m_positiony), "Townhall");
	auto townhall = entity->Get< ComponentCity >("Townhall");

	// Get Building slots and Territory defined for Entity.
	auto bs = xml->FirstChildElement("BuildingSlots");
	auto bslot = bs->FirstChildElement("Slot");
	while (bslot)
	{
		townhall->AddBuildingSlot(bslot->IntAttribute("x"), bslot->IntAttribute("y"));

		bslot = bslot->NextSiblingElement("Slot");
	}
	auto ts = xml->FirstChildElement("Territory");
	auto tslot = ts->FirstChildElement("Slot");
	while (tslot)
	{
		townhall->AddTerritory(tslot->IntAttribute("x"), tslot->IntAttribute("y"));

		tslot = tslot->NextSiblingElement("Slot");
	}
}

void GameEditor::ImportEntityComponentUnit(tinyxml2::XMLElement* xml, Entity* entity)
{
	LOG_DBG_ERROR("[{:.4f}][ImportEntityComponentUnit] Serializing ComponentUnit data not supported!", APP_RUN_TIME);
}

void GameEditor::MakeMapobjectTownhall(int x, int y, std::string layer)
{
	if (x < 0 ||
		y < 0 ||
		x > MAX_MAPSIZE_X - 1 ||
		y > MAX_MAPSIZE_Y - 1) return;

	auto e = m_gameworld[layer][x][y];
	if (e)
	{
		e->Add(new ComponentTownhall(x, y), "Townhall");
	}
}
void GameEditor::MakeMapobjectFort(int x, int y, std::string layer)
{
	if (x < 0 ||
		y < 0 ||
		x > MAX_MAPSIZE_X - 1 ||
		y > MAX_MAPSIZE_Y - 1) return;

	auto e = m_gameworld[layer][x][y];
	if (e)
	{
		e->Add(new ComponentFort(x, y), "Fort");
	}

}
void GameEditor::AddTerritoryToCity(Entity* e, int slotx, int sloty)
{
	if (e->Has("Townhall"))
	{
		e->Get< ComponentTownhall >("Townhall")->AddTerritory(slotx, sloty);
	}
	else if (e->Has("Fort"))
	{
		e->Get< ComponentFort >("Fort")->AddTerritory(slotx, sloty);
	}
}
void GameEditor::AddBuildingSlotToCity(Entity* e, int slotx, int sloty)
{
	if (e->Has("Townhall"))
	{
		e->Get< ComponentTownhall >("Townhall")->AddBuildingSlot(slotx, sloty);
	}
	else if (e->Has("Fort"))
	{
		e->Get< ComponentFort >("Fort")->AddBuildingSlot(slotx, sloty);
	}
}
void GameEditor::RemoveTerritoryFromCity(Entity* e, int slotx, int sloty)
{
	if (e->Has("Townhall"))
	{
		e->Get< ComponentTownhall >("Townhall")->RemoveTerritory(slotx, sloty);
	}
	else if (e->Has("Fort"))
	{
		e->Get< ComponentFort >("Fort")->RemoveTerritory(slotx, sloty);
	}
}
void GameEditor::RemoveBuildingSlotFromCity(Entity* e, int slotx, int sloty)
{
	if (e->Has("Townhall"))
	{
		e->Get< ComponentTownhall >("Townhall")->RemoveBuildingSlot(slotx, sloty);
	}
	else if (e->Has("Fort"))
	{
		e->Get< ComponentFort >("Fort")->RemoveBuildingSlot(slotx, sloty);
	}
}


int main()
{
	if (editor.Construct(1600, 900, 1, 1, true, true, false))
		editor.Start();

	return 0;
}
