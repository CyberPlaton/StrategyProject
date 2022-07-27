#include "Main.h"

// STATIC DATA
#ifdef DEBUG
static std::string g_sConfiguration = "DEBUG";
#endif
#ifdef RELEASE
static std::string g_sConfiguration = "RELEASE";
#endif
#ifdef DISTR
static std::string g_sConfiguration = "DISTR";
#endif


static bool g_bDecalDatabaseOpen = true;
static bool g_bPrefabDatabaseOpen = true;
static bool g_bEntityDatabaseOpen = true;
static bool g_bEntityEditorOpen = false;
static bool g_bStatusEffectEditorOpen = false;

// Status Effect Static Data
static SStatusEffect* g_pCurrentEditedStatusEffect = nullptr;
static SStatusEffect* g_pExportedStatusEffect = nullptr;
static std::string g_sCurrentEditedStatusEffectFilePath = "none";
static std::string g_sStatusEffectEditorCurrentIconSprite = "none";
static bool g_bExportingStatusEffect = false;
static bool g_bImportingStatusEffect = false;
static std::string g_sStatusEffectCacheFilepath = "assets/TilesetData/StatusEffect/StrategyEditor_StatusEffectCache.xml";

static char g_cStatusEffectName[64] = "";
static char g_cStatusEffectDisplayName[64] = "";
static char g_cStatusEffectDescription[512] = "";
static char g_cStatusEffectBehaviorTreeImplName[64] = "";
static char g_cStatusEffectTimerType[64] = "";

// Ability Static Data
static SAbility* g_pCurrentEditedAbility = nullptr;
static SAbility* g_pExportedAbility = nullptr;
static bool g_bExportingAbility = false;
static bool g_bImportingAbility = false;
static bool g_bAbilityEditorOpen = false;
static char g_cAbilityName[64] = "";
static char g_cAbilityDisplayName[64] = "";
static char g_cAbilityAppliedStatusEffectName[64] = "";
static char g_cAbilityDescription[512] = "";

static std::string g_sCurrentEditedAbilityFilePath = "none";
static std::string g_sAbilityCacheFilepath = "assets/TilesetData/Ability/StrategyEditor_AbilityCache.xml";


// Prefab Static Data
static char g_cPrefabName[64] = "";
static char g_cPrefabLayoutTemplateName[128] = "";
static char g_cPrefabRequiredBuildingName[64] = "";
static char g_cPrefabStartingStatusName[128] = "";
static char g_cPrefabAbilityName[128] = "";
static char g_cPrefabBuildingPredecessorName[128] = "";

static std::string g_sMapDataCacheFilepath = "assets/StrategyEditor_MapCache.xml";
static bool g_bSavingMapDataDialog = false;
static bool g_bLoadingMapDataDialog = false;

static std::string g_sPrefabCacheFilepath = "assets/TilesetData/UnitPrefab/StrategyEditor_PrefabCache.xml";
static std::string g_sCurrentEditedPrefabFilePath = "none";
static bool g_bPrefabEditorEditingUnits = true;
static bool g_bPrefabEditorEditingBuildings = false;
static bool g_bPlacingPrefabedMapobjectOnMap = false;
static std::string g_sSelectedPrefab = "none";
static bool g_bExportingUnitPrefab = false;
static bool g_bImportingUnitPrefab = false;
static SPrefab* g_pCurrentEditedPrefab = nullptr;
static SPrefab* g_pExportedPrefab = nullptr;
static ImGuiID g_idUnitEditorElementID = 20000;
static const ImU32 u32_one = 1;
static bool g_bSavingPrefabLayoutTemplate = false;
static bool g_bLoadPrefabLayoutTemplate = false;
static bool g_bAddingPrefabElementType = false;
static std::vector< std::string > g_PrefabElementTypeVec;
static int g_iCurrentSelectedShape = 0;
static bool g_bShapeWindowOpen = true;
static bool g_bShapePropertyWindowOpen = false;
static SShape* g_pCurrentDisplayedShape = nullptr;
static bool g_bLayoutTemplateEditorOpen = false;
static bool g_bUnitEditorOpen = false;
static std::string g_sUnitEditorCurrentUnitSprite = "none";


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
static bool g_bImguiDemoOpen = false;
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
static bool g_bCameraPositionAtMousePosition = false;


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


	// Unit Editor
	if (g_bLayoutTemplateEditorOpen)
	{
		// Show chooseable shapes
		DisplayShapesWindow();
		// Show chosen shape properties
		if(g_bShapePropertyWindowOpen) DisplayShapePropertiesEditor(g_pCurrentDisplayedShape);
		// Prefab Element Type Adding
		if (g_bAddingPrefabElementType) DisplayAddingPrefabElementType();
		// Saving/Loading Prefab Layout Template
		if (g_bSavingPrefabLayoutTemplate) DisplaySavingPrefabLayoutElement();
		if (g_bLoadPrefabLayoutTemplate) DisplayLoadingPrefabLayoutElement();
	}
	else
	{
		// Saving/Loading Map Data
		if (g_bSavingMapDataDialog) DisplayMapExportDialog();
		if (g_bLoadingMapDataDialog) DisplayMapImportDialog();


		// Decal Database
		if (g_bDecalDatabaseOpen) RenderDecalDatabase();
		//Prefab Database
		if (g_bPrefabDatabaseOpen) RenderPrefabDatabase();
		// Entity Database
		if (g_bEntityDatabaseOpen) RenderEntityDatabase();
		// Rendering Layers
		if (g_bRenderingLayersOpen) RenderLayerUI();
		// Unit Editor
		if(g_bUnitEditorOpen)
		{
			DisplayUnitEditor();
			// Prefab Export Menu.
			if (g_bExportingUnitPrefab)  DisplayUnitPrefabExportWindow(g_pExportedPrefab);
			// Prefab Import Menu.
			if (g_bImportingUnitPrefab) DisplayUnitPrefabImportWindow();
		}
		if (g_bStatusEffectEditorOpen)
		{
			DisplayStatusEffectEditor();
			// Status Effect Export Menu.
			if (g_bExportingStatusEffect)  DisplayStatusEffectExportWindow(g_pExportedStatusEffect);
			// Status Effect Import Menu.
			if (g_bImportingStatusEffect) DisplayStatusEffectImportWindow();
		}
		if (g_bAbilityEditorOpen)
		{
			DisplayAbilityEditor();
			// Ability Export Menu.
			if (g_bExportingAbility)  DisplayAbilityEditorExportWindow(g_pExportedAbility);
			// Ability Import Menu.
			if (g_bImportingAbility) DisplayAbilityEditorImportWindow();
		}

	}
}


bool GameEditor::OnUserCreate()
{
#ifdef DISTR
	// Remove console in distribution build (Windows only).
	ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif

	// Initialize Random.
	srand(time(0));

	// Initialize Logging.
	if (!Logger::Initialize()) return false;
	// Initialize Sound.
	if (!sound::SoundSystem::get()->Initialize()) return false;

	// Initialize Layered rendering.
	m_GUILayer = CreateLayer();
	EnableLayer(m_GUILayer, true);
	SetLayerCustomRenderFunction(0, std::bind(&GameEditor::DrawUI, this));

	// Initialize renderer
	tv = olc::TileTransformedView({ ScreenWidth(), ScreenHeight() }, { DEFAULT_DECAL_SIZE_X, DEFAULT_DECAL_SIZE_Y });

	m_visibleLayers.resize(256);
	CreateRenderingLayer("Default", 0);
	m_currentLayer = "Default";
	m_visibleLayers[0] = 1;

	// Create the default audio source layer.
	CreateRenderingLayer("AudioSourceLayer", 99);
	m_visibleLayers[99] = 0;
	m_PermanentLayersVec.push_back(99);

	UpdateLayerSorting();




	// Load Assets
	bool loaded = LoadTilesetData("Forest", "assets/Tileset/Forest", "assets/TilesetData/Forest.json");
	loaded &= LoadTilesetData("Ground", "assets/Tileset/Ground", "assets/TilesetData/Ground.json");
	loaded &= LoadTilesetData("Mountain", "assets/Tileset/Mountain", "assets/TilesetData/Mountain.json");
	loaded &= LoadTilesetData("Road", "assets/Tileset/Road", "assets/TilesetData/Road.json");
	loaded &= LoadTilesetData("Sea", "assets/Tileset/Sea", "assets/TilesetData/Sea.json");
	loaded &= LoadTilesetData("Bridge", "assets/Tileset/Bridge", "assets/TilesetData/Bridge.json");
	loaded &= LoadTilesetData("River", "assets/Tileset/River", "assets/TilesetData/River.json");
	loaded &= LoadTilesetData("Structure", "assets/Tileset/Structure", "assets/TilesetData/Structure.json");
	loaded &= LoadTilesetData("Wall", "assets/Tileset/Wall", "assets/TilesetData/Wall.json");
	loaded &= LoadTilesetData("Hill", "assets/Tileset/Hill", "assets/TilesetData/Hill.json");
	loaded &= LoadTilesetData("Unit", "assets/Tileset/Unit", "assets/TilesetData/Units.json");
	loaded &= LoadTilesetData("StatusEffect", "assets/Tileset/StatusEffect", "assets/TilesetData/StatusEffect.json");

	loaded &= LoadEditorGraphicalData();

	// Load special Asset data.
	auto sprite = new olc::Sprite("assets/Tileset/Structure/Fort.png");
	auto decal = new olc::Decal(sprite);
	m_structureDecalDatabase.emplace("Fort", decal);
	m_decalDatabase.emplace("Fort", decal);
	m_spriteDatabase.push_back(sprite);
	m_decalSizeDatabase.try_emplace("Fort", std::make_pair(256, 256));

	sprite = new olc::Sprite("assets/Tileset/Structure/Fort_2.png");
	decal = new olc::Decal(sprite);
	m_structureDecalDatabase.emplace("Fort_2", decal);
	m_decalDatabase.emplace("Fort_2", decal);
	m_spriteDatabase.push_back(sprite);
	m_decalSizeDatabase.try_emplace("Fort_2", std::make_pair(256, 256));


	// LOAD FOR TEST THE 2 SAMURAI UNITS.
	sprite = new olc::Sprite("assets/Tileset/Unit/HUMAN/SPECIAL CARD UNITS/figure_180x180_framed_standard_154_samurai.png");
	decal = new olc::Decal(sprite);
	m_unitDecalDatabase.emplace("KATANA SAMURAI", decal);
	m_decalDatabase.emplace("KATANA SAMURAI", decal);
	m_spriteDatabase.push_back(sprite);
	m_decalSizeDatabase.try_emplace("KATANA SAMURAI", std::make_pair(180, 180));

	sprite = new olc::Sprite("assets/Tileset/Unit/HUMAN/SPECIAL CARD UNITS/figure_180x260_framed_standard_155_yari_ashigaru.png");
	decal = new olc::Decal(sprite);
	m_unitDecalDatabase.emplace("YARI ASHIGARU", decal);
	m_decalDatabase.emplace("YARI ASHIGARU", decal);
	m_spriteDatabase.push_back(sprite);
	m_decalSizeDatabase.try_emplace("YARI ASHIGARU", std::make_pair(180, 256));



	sprite = new olc::Sprite("assets/Editor/speaker_audio_sound_loud.png");
	decal = new olc::Decal(sprite);
	m_editorDecalDatabase.try_emplace("AudioOn", decal);
	m_editorSpriteDatabase.push_back(sprite);
	m_decalDatabase.emplace("AudioOn", decal);
	m_spriteDatabase.push_back(sprite);

	sprite = new olc::Sprite("assets/Editor/speaker_audio_sound_off.png");
	decal = new olc::Decal(sprite);
	m_editorDecalDatabase.try_emplace("AudioOff", decal);
	m_editorSpriteDatabase.push_back(sprite);
	m_decalDatabase.emplace("AudioOff", decal);
	m_spriteDatabase.push_back(sprite);

	// Load Audio assets
	loaded &= LoadAudioData("assets/Audio/LoadDefinition.xml");


	// Load Map Data Cache.
	loaded &= ImportMapDataCache(g_sMapDataCacheFilepath);


	
	// Test Sound System.
	// FIRST
	sound::SoundSource::Data data;
	data.m_SoundName = "THEME_music_sound";
	data.m_Soundfilepath = m_soundPathMap["main_theme_forest"];
	data.m_X = 3.0f;
	data.m_Y = 5.0f;
	data.m_Z = 1.0f;
	data.m_VolumeFalloffFactor = 2.0f;
	data.m_Radius = 50.0f;
	data.m_FadeoutRadius = 10.0f;
	data.m_ChannelGroup = "Master";
	data.m_Loop = true;

	sound::SoundSystem::get()->CreateSound(&data);
	sound::SoundSystem::get()->PlaySound("THEME_music_sound", "Master", true);


	// SECOND
	data.m_SoundName = "AMBIENT_forest_sound_01";
	data.m_Soundfilepath = m_soundPathMap["ambient_birds_in_forest"];
	data.m_X = 44.0f;
	data.m_Y = 33.0f;
	data.m_Z = 1.0f;
	data.m_VolumeFalloffFactor = 2.7f;
	data.m_Radius = 40.0f;
	data.m_FadeoutRadius = 10.0f;
	data.m_ChannelGroup = "Master";
	data.m_Loop = true;

	sound::SoundSystem::get()->CreateSound(&data);
	sound::SoundSystem::get()->PlaySound("AMBIENT_forest_sound_01", "Master", true);

	// THIRD
	data.m_SoundName = "AMBIENT_river_sound_01";
	data.m_Soundfilepath = m_soundPathMap["sound_river"];
	data.m_X = 10.0f;
	data.m_Y = 15.0f;
	data.m_Z = 1.0f;
	data.m_VolumeFalloffFactor = 5.0f;
	data.m_Radius = 40.0f;
	data.m_FadeoutRadius = 10.0f;
	data.m_ChannelGroup = "Master";
	data.m_Loop = true;

	sound::SoundSystem::get()->CreateSound(&data);
	sound::SoundSystem::get()->PlaySound("AMBIENT_river_sound_01", "Master", true);


	// FOURTH
	data.m_SoundName = "AMBIENT_cold_wind_sound_01";
	data.m_Soundfilepath = m_soundPathMap["wind_2"];
	data.m_X = 51.0f;
	data.m_Y = 4.0f;
	data.m_Z = 1.0f;
	data.m_VolumeFalloffFactor = 3.7f;
	data.m_Radius = 30.0f;
	data.m_FadeoutRadius = 10.0f;
	data.m_ChannelGroup = "Master";
	data.m_Loop = true;

	sound::SoundSystem::get()->CreateSound(&data);
	sound::SoundSystem::get()->PlaySound("AMBIENT_cold_wind_sound_01", "Master", true);
	
	// FIFTH
	data.m_SoundName = "AMBIENT_jungle_sound_01";
	data.m_Soundfilepath = m_soundPathMap["sound_jungle"];
	data.m_X = 64.0f;
	data.m_Y = 36.0f;
	data.m_Z = 1.0f;
	data.m_VolumeFalloffFactor = 5.5f;
	data.m_Radius = 30.0f;
	data.m_FadeoutRadius = 10.0f;
	data.m_ChannelGroup = "Master";
	data.m_Loop = true;

	sound::SoundSystem::get()->CreateSound(&data);
	sound::SoundSystem::get()->PlaySound("AMBIENT_jungle_sound_01", "Master", true);

	// SIXTH
	data.m_SoundName = "AMBIENT_beach_sound_01";
	data.m_Soundfilepath = m_soundPathMap["ambient_beach_waves_and_seagull"];
	data.m_X = 27.0f;
	data.m_Y = 59.0f;
	data.m_Z = 1.0f;
	data.m_VolumeFalloffFactor = 3.0f;
	data.m_Radius = 30.0f;
	data.m_FadeoutRadius = 15.0f;
	data.m_ChannelGroup = "Master";
	data.m_Loop = true;

	sound::SoundSystem::get()->CreateSound(&data);
	sound::SoundSystem::get()->PlaySound("AMBIENT_beach_sound_01", "Master", true);

	// SEVENTH
	data.m_SoundName = "AMBIENT_beach_sound_02";
	data.m_Soundfilepath = m_soundPathMap["ambient_beach_waves_and_seagull"];
	data.m_X = 52.0f;
	data.m_Y = 55.0f;
	data.m_Z = 1.0f;
	data.m_VolumeFalloffFactor = 3.0f;
	data.m_Radius = 30.0f;
	data.m_FadeoutRadius = 15.0f;
	data.m_ChannelGroup = "Master";
	data.m_Loop = true;

	sound::SoundSystem::get()->CreateSound(&data);
	sound::SoundSystem::get()->PlaySound("AMBIENT_beach_sound_02", "Master", true);


	return loaded;
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

	sprite = new olc::Sprite("assets/Editor/Circle.png");
	decal = new olc::Decal(sprite);
	m_editorDecalDatabase.try_emplace("Circle", decal);
	m_editorSpriteDatabase.push_back(sprite);

	sprite = new olc::Sprite("assets/Editor/Triangle.png");
	decal = new olc::Decal(sprite);
	m_editorDecalDatabase.try_emplace("Triangle", decal);
	m_editorSpriteDatabase.push_back(sprite);


	sprite = new olc::Sprite("assets/Editor/FilledRect.png");
	decal = new olc::Decal(sprite);
	m_editorDecalDatabase.try_emplace("FilledRect", decal);
	m_editorSpriteDatabase.push_back(sprite);

	sprite = new olc::Sprite("assets/Editor/FilledCircle.png");
	decal = new olc::Decal(sprite);
	m_editorDecalDatabase.try_emplace("FilledCircle", decal);
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
		std::string sound_name = sound->GetText();
		if (sound_path.compare("") == 0)
		{
			path += "/" + sound_name;
		}
		else
		{
			path += "/" + sound_path + "/" + sound_name;
		}
	
		// Remove extension (e.g. .wav)
		std::string final_sound_name = sound_name.substr(0, sound_name.find("."));

		m_soundPathMap.emplace(final_sound_name, path);
	
		LOG_DBG_INFO("[{:.4f}][LoadAudioData] Loading Sound \"{}\" at \"{}\"!", APP_RUN_TIME, final_sound_name, path);
		LOG_FILE_INFO("[{:.4f}][LoadAudioData] Loading Sound \"{}\" at \"{}\"!", APP_RUN_TIME, final_sound_name, path);

		sound = sound->NextSiblingElement("Sound");
	}

	return true;
}


void GameEditor::RenderMainMenu()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{
			if (ImGui::MenuItem("Imgui Demo")) ToggleMenuItem(g_bImguiDemoOpen);
			if (ImGui::MenuItem("Save As..."))
			{
				// Saving Map Data Window.
				g_bSavingMapDataDialog = true;
			}
			if (ImGui::MenuItem("Load From..."))
			{
				// Loading Map Data Window.
				g_bLoadingMapDataDialog = true;
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

			if (ImGui::MenuItem("Prefab Database"))
			{
				ToggleMenuItem(g_bPrefabDatabaseOpen);
			}
			ImGui::SameLine();
			ImGui::Checkbox("Open", &g_bPrefabDatabaseOpen);


			if (ImGui::MenuItem("Rendering Layers"))
			{
				ToggleMenuItem(g_bRenderingLayersOpen);
			}
			ImGui::SameLine();
			ImGui::Checkbox("Open", &g_bRenderingLayersOpen);

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

			if (ImGui::MenuItem("Camera Position At Mouse Position"))
			{
				ToggleMenuItem(g_bCameraPositionAtMousePosition);
			}
			ImGui::SameLine();
			ImGui::Checkbox("Open", &g_bCameraPositionAtMousePosition);

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
		}



		if (ImGui::BeginMenu("Unit"))
		{
			if (ImGui::BeginMenu("Unit Layout"))
			{
				if (ImGui::MenuItem("Layout Template Editor"))
				{
					ToggleMenuItem(g_bLayoutTemplateEditorOpen);
				}

				if (ImGui::MenuItem("Shape Window"))
				{
					ToggleMenuItem(g_bShapeWindowOpen);
				}
				ImGui::SameLine();
				ImGui::Checkbox("Open", &g_bShapeWindowOpen);
				if (ImGui::MenuItem("Shape Properties Window"))
				{
					ToggleMenuItem(g_bShapePropertyWindowOpen);
				}
				ImGui::SameLine();
				ImGui::Checkbox("Open", &g_bShapePropertyWindowOpen);

				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Prefab Editor"))
			{
				ToggleMenuItem(g_bUnitEditorOpen);
				if (!m_prefabCacheLoaded)
				{
					// Load Prefab Cache.
					if (!ImportUnitPrefabCache(g_sPrefabCacheFilepath))
					{
						LOG_DBG_CRITICAL("[{:.4f}][RenderMainMenu] Importing Prefab Cache Failed \"{}\"!", APP_RUN_TIME, g_sPrefabCacheFilepath);
						LOG_FILE_CRITICAL("[{:.4f}][RenderMainMenu] Importing Prefab Cache Failed \"{}\"!", APP_RUN_TIME, g_sPrefabCacheFilepath);
					}
				}
			}


			if (ImGui::MenuItem("Status Effect Editor"))
			{
				ToggleMenuItem(g_bStatusEffectEditorOpen);
				if (!m_statusEffectCacheLoaded)
				{
					// Load Status Effect Cache.
					if (!ImportStatusEffectCache(g_sStatusEffectCacheFilepath))
					{
						LOG_DBG_CRITICAL("[{:.4f}][RenderMainMenu] Importing Status Effect Cache Failed \"{}\"!", APP_RUN_TIME, g_sStatusEffectCacheFilepath);
						LOG_FILE_CRITICAL("[{:.4f}][RenderMainMenu] Importing Status Effect Cache Failed \"{}\"!", APP_RUN_TIME, g_sStatusEffectCacheFilepath);
					}
				}
			}


			if (ImGui::MenuItem("Ability Editor"))
			{
				ToggleMenuItem(g_bAbilityEditorOpen);
				if (!m_abilityCacheLoaded)
				{
					// Load Ability Cache.
					if (!ImportAbilityCache(g_sAbilityCacheFilepath))
					{
						LOG_DBG_CRITICAL("[{:.4f}][RenderMainMenu] Importing Ability Cache Failed \"{}\"!", APP_RUN_TIME, g_sAbilityCacheFilepath);
						LOG_FILE_CRITICAL("[{:.4f}][RenderMainMenu] Importing Ability Cache Failed \"{}\"!", APP_RUN_TIME, g_sAbilityCacheFilepath);
					}
				}
			}


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
		if (ImGui::BeginTabItem("StatusEffect"))
		{
			ImGui::Separator();
			RenderDecalDatabase(m_statusEffectDecalDatabase);
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
		HelpMarkerWithoutQuestion(pair.first.c_str());
		i++;
	}
}

void GameEditor::RenderPrefabDatabase()
{
	ImGui::Begin("PrefabDatabase", &g_bPrefabDatabaseOpen);

	int i = 0;
	for (auto& prefab : m_prefabDecalDatabase) // This Function is REALLY slow.
	{
		auto decal = m_decalDatabase[prefab.second];

		if (i == 4)
		{
			i = 0;
		}
		else
		{
			ImGui::SameLine();
		}
		
		if (ImGui::ImageButton((ImTextureID)decal->id, { 64, 64 }))
		{
			g_sSelectedMapobject = prefab.second;
			g_sSelectedPrefab = prefab.first;
			g_bPlacingPrefabedMapobjectOnMap = true;
			ImGui::SetWindowFocus(nullptr);
		}
		HelpMarkerWithoutQuestion(prefab.first.c_str());
		i++;
	}

	ImGui::End();
}

void GameEditor::ToggleMenuItem(bool& item)
{
	item = (item == true) ? false : true;
}

bool GameEditor::OnUserUpdate(float fElapsedTime)
{
	Clear(olc::BLANK);
	SetDrawTarget((uint8_t)m_GUILayer);

	// User Input Handling.
	HandleInput();
	
	// Camera Update.
	UpdateVisibleRect();
	// Sound Update.
	sound::SoundSystem::get()->Update(m_camerax, m_cameray, m_cameraHeigth);
	// Entity Update.
	UpdateEntities();

	// Render.
	if(g_bLayoutTemplateEditorOpen)
	{
		RenderMainFrameForUnitEditor();
	}
	else
	{
		RenderMainFrame();
	}

	DrawStringDecal(olc::vf2d(5, 25), "FPS: " + std::to_string(GetFPS()));
	DrawStringDecal(olc::vf2d(5, 45), "BUILD: " + g_sConfiguration);

	return true;
}


void GameEditor::RenderMainFrameForUnitEditor()
{
	olc::vi2d topleft = tv.GetTopLeftTile().max({ 0, 0 });
	olc::vi2d bottomright = tv.GetBottomRightTile().min({ MAX_MAPSIZE_X, MAX_MAPSIZE_Y });
	olc::vi2d tile;
	olc::vi2d upLeft = m_visiblePointLeftUp;
	olc::vi2d downRight = m_visiblePointDownRight;

	// Render Current Added Shapes.
	for(auto& shape: m_currentLayoutTemplateVec)
	{
		shape->Draw(this);
	}

	// Render the grid on top of all.
	if (g_bRenderGrid)
	{
		olc::Pixel color = { 255, 255, 255, 50 };
		for (tile.y = topleft.y; tile.y < downRight.y; tile.y++)
		{
			for (tile.x = topleft.x; tile.x < downRight.x; tile.x++)
			{
				tv.DrawLineDecal(tile, tile + olc::vf2d(0.0f, 1.0f), color);
				tv.DrawLineDecal(tile, tile + olc::vf2d(1.0f, 0.0f), color);
			}
		}
	}
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
	if (g_pEditedCity == nullptr && (g_bAddingBuildingSlot || g_bAddingTerritory))
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
	DrawStringDecal({ 10.0f, 55.0f }, position);
	DrawStringDecal({ 10.0f, 65.0f }, height);


	// Draw Tile Number under Mouse.
	position = std::to_string(m_tileUnderMouseX) + ":" + std::to_string(m_tileUnderMouseY);
	tv.DrawStringDecal({ (float)m_tileUnderMouseX + 0.5f, (float)m_tileUnderMouseY + 0.5f }, position, olc::RED, { 2.0f, 2.0f });
	position = std::to_string(m_camerax) + ":" + std::to_string(m_cameray);
	// Draw Camera position indicator on the Maptile.
	tv.DrawStringDecal({ (float)m_camerax + 0.5f, (float)m_cameray + 0.5f }, position, olc::YELLOW, { 2.0f, 2.0f });
}
void GameEditor::RenderMapobject(Entity* object)
{
	if (auto c = object->Get< ComponentSprite >("Sprite"); c)
	{
		// Get the correct offset.
		float offx = 0.0f, offy = 0.0f;
		float scalex = 1.0f, scaley = 1.0f;
		if (object->Get< ComponentUnit >("Unit"))
		{
			bool normal_180x_180 = true;

			auto sprite_name = c->m_decal;
			auto sprite_dim = m_decalSizeDatabase[sprite_name];
			if(sprite_dim.second > 180)
			{
				normal_180x_180 = false;
			}

			// Offset for Units which are 180x180.
			if(normal_180x_180)
			{
				offy = DEFAULT_UNIT_DECAL_OFFSET_Y;
				offx = DEFAULT_UNIT_DECAL_OFFSET_X;
			}
			// Offset for Units which are 180x260.
			else
			{
				offy = BIGGER_UNIT_DECAL_OFFSET_Y;
				offx = BIGGER_UNIT_DECAL_OFFSET_X;
			}
		}

		tv.DrawDecal({ object->m_positionx + offx, object->m_positiony + offy }, m_decalDatabase[c->m_decal], { scalex, scaley });
	}
	else
	{
		tv.DrawStringDecal({ object->m_positionx, object->m_positiony }, object->m_name, olc::WHITE, { 1.0f, 1.0f });
	}
}
bool GameEditor::LoadTilesetData(const std::string& database, const std::string& tilesetpath, const std::string& datapath, bool verbose)
{
	rapidjson::Document doc;
	std::ifstream ifs(datapath.c_str());
	rapidjson::IStreamWrapper isw(ifs);
	doc.ParseStream(isw);

	if(verbose)
	{
		LOG_DBG_INFO("[{:.4f}][LoadTilesetData] Loading Tileset \"{}\"!", APP_RUN_TIME, tilesetpath);
		LOG_FILE_INFO("[{:.4f}][LoadTilesetData] Loading Tileset \"{}\"!", APP_RUN_TIME, tilesetpath);
	}

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
			if (sprite->width == 0 && sprite->height == 0)
			{
				if(verbose)
				{
					LOG_DBG_ERROR("[{:.4f}][LoadTilesetData] Error loading sprite \"{}\" at \"{}\"!", APP_RUN_TIME, name, tilesetpath + "/" + name + ".png");
					LOG_FILE_ERROR("[{:.4f}][LoadTilesetData] Error loading sprite \"{}\" at \"{}\"!", APP_RUN_TIME, name, tilesetpath + "/" + name + ".png");
				}
				continue;
			}


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
			else if (database.compare("StatusEffect") == 0)
			{
				m_statusEffectDecalDatabase.try_emplace(name, decal);
			}



			m_decalDatabase.try_emplace(name, decal);
			m_spriteDatabase.push_back(sprite);
			m_decalSizeDatabase.try_emplace(name, std::make_pair(sprite->width, sprite->height));

			if(verbose)
			{
				LOG_DBG_INFO("[{:.4f}][LoadTilesetData] Success loading sprite \"{}\" at \"{}\"!", APP_RUN_TIME, name, tilesetpath + "/" + name + ".png");
				LOG_FILE_INFO("[{:.4f}][LoadTilesetData] Success loading sprite \"{}\" at \"{}\"!", APP_RUN_TIME, name, tilesetpath + "/" + name + ".png");
			}
		}
	}
	return true;
}
void GameEditor::HandleInput()
{
	olc::vf2d point = tv.ScreenToWorld({ (float)GetMouseX(), (float)GetMouseY() });
	float mousex = point.x;
	float mousey = point.y;
	
	m_tileUnderMouseX = (uint64_t)mousex;
	m_tileUnderMouseY = (uint64_t)mousey;
	
	olc::vi2d topLeft = tv.GetTileUnderScreenPos({ 0, 0 });
	olc::vi2d bottomDown = tv.GetBottomRightTile();
	olc::vi2d middle = { (bottomDown.x + topLeft.x) / 2, (bottomDown.y + topLeft.y) / 2 };

	if (g_bCameraPositionAtMousePosition)
	{
		m_camerax = m_tileUnderMouseX;
		m_cameray = m_tileUnderMouseY;
	}
	else
	{
		m_camerax = middle.x;
		m_cameray = middle.y;
	}


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
				// React to when we are creatin a Prefabed Mapobject.
				if(g_bPlacingPrefabedMapobjectOnMap)
				{
					// BUG: If selected Layer does not exists -> Hard Crash.
					CreatePrefabedMapobject(point.x, point.y, m_currentLayer, g_sSelectedMapobject);
				}
				else
				{
					CreateMapobjectEx(point.x, point.y, m_currentLayer, g_sSelectedMapobject);
				}
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
			if (GetKey(olc::W).bHeld || GetKey(olc::S).bHeld ||
				GetKey(olc::A).bHeld || GetKey(olc::D).bHeld)
			{
				
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


	// Unit Template Layout Editing.
	if(g_bLayoutTemplateEditorOpen)
	{
		if(!g_bImguiHasFocus)
		{
			if (GetMouse(0).bReleased && g_iCurrentSelectedShape > -1)
			{
				// Create new shape at mouse position.
				switch (g_iCurrentSelectedShape)
				{
				case 0:
				{
					// Circle. Removed
					break;
				}
				case 1:
				{
					// Rectangle.
					CreateShapeAtMousePosition(1, (uint64_t)mousex, (uint64_t)mousey);
					break;
				}
				default:
				{
					// Error.
					LOG_DBG_ERROR("[{:.4f}][HandleInput] Could not create unrecognized Shape for Layout Template Editor: Shape index \"{}\"!", APP_RUN_TIME, g_iCurrentSelectedShape);
					LOG_FILE_ERROR("[{:.4f}][HandleInput] Could not create unrecognized Shape for Layout Template Editor: Shape index \"{}\"!", APP_RUN_TIME, g_iCurrentSelectedShape);
				}
				}
			}
			if (GetMouse(1).bReleased)
			{
				// Remove shape at current mouse position.
				DeleteShapeAtMousePosition(mousex, mousey);
				g_bShapePropertyWindowOpen = false;
				g_pCurrentDisplayedShape = nullptr;
			}
			if(g_iCurrentSelectedShape == -1)
			{
				if(GetMouse(0).bReleased)
				{
					auto shape = GetShapeAtMousePosition(mousex, mousey);
					if(shape)
					{
						g_pCurrentDisplayedShape = shape;
						g_bShapePropertyWindowOpen = true;
					}
					else
					{
						g_bShapePropertyWindowOpen = false;
						g_pCurrentDisplayedShape = nullptr;
					}
				}
			}
		}
	}

}
void GameEditor::UpdateVisibleRect()
{
	m_visiblePointLeftUp = tv.GetTileUnderScreenPos({ 0, 0 });

	// Increase draw distance by several maptiles.
	if (m_visiblePointLeftUp.x > 10) m_visiblePointLeftUp.x -= 10;
	if (m_visiblePointLeftUp.y > 10) m_visiblePointLeftUp.y -= 10;
	if (m_visiblePointLeftUp.x < 0) m_visiblePointLeftUp.x = 0;
	if (m_visiblePointLeftUp.y < 0) m_visiblePointLeftUp.y = 0;

	m_visiblePointDownRight = tv.GetBottomRightTile();
	
	// Increase draw distance by several maptiles.
	if (m_visiblePointDownRight.x < MAX_MAPSIZE_X - 11) m_visiblePointDownRight.x += 10;
	if (m_visiblePointDownRight.y < MAX_MAPSIZE_Y - 11) m_visiblePointDownRight.y += 10;

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



uint32_t GameEditor::ConvertColorToPixel(ImVec4 color)
{
	return ImGui::ColorConvertFloat4ToU32(color);
}

std::string GameEditor::CreateMapobjectName()
{
	return "Mapobject_" + std::to_string(m_mapobjectCount++);
}

Entity* GameEditor::CreateMapobjectEx(uint64_t x, uint64_t y, std::string layer, std::string decal, std::string name)
{
	// Create Entities only on Valid Layers.
	if(!DoesLayerExist(layer))
	{
		return nullptr;
	}

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

Entity* GameEditor::CreatePrefabedMapobject(uint64_t x, uint64_t y, std::string layer, std::string decal, std::string name /*= "none"*/)
{
	if (g_sSelectedPrefab.compare("none") == 0) return nullptr;

	auto e = CreateMapobjectEx(x, y, layer, decal, name);
	
	if(e)
	{
		if (auto c = e->Get < ComponentUnit >("Unit"); c)
		{
			c->m_prefabFilepath = g_sSelectedPrefab;
		}
		else if (auto c = e->Get < ComponentTownhall >("Townhall"); c)
		{
			c->m_prefabFilepath = g_sSelectedPrefab;
		}
		else if (auto c = e->Get < ComponentFort >("Fort"); c)
		{
			c->m_prefabFilepath = g_sSelectedPrefab;
		}

		return e;
	}

	return nullptr;
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

bool GameEditor::DoesLayerExist(const std::string& layer_name)
{
	return m_layerOrder.find(layer_name) != m_layerOrder.end();
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

void GameEditor::CopyStringToCharArray(const std::string& s, char arr[], uint64_t arr_size)
{
	if(s.size() < arr_size)
	{
		for(int i = 0; i < s.size(); i++)
		{
			char c = s[i];
			arr[i] = c;
		}
	}
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
bool GameEditor::ExportMapData(const std::string& filepath, const std::string& map_name)
{
	tinyxml2::XMLDocument doc;
	auto root = doc.NewElement("Map");
	doc.InsertEndChild(root);


	// Export World Layer Data.
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

	if (doc.SaveFile(filepath.c_str()) != tinyxml2::XMLError::XML_SUCCESS)
	{
		LOG_DBG_ERROR("[{:.4f}][ExportMapData] Failed to export Map Data \"{}\"!", APP_RUN_TIME, filepath);
		LOG_FILE_ERROR("[{:.4f}][ExportMapData] Failed to export Map Data \"{}\"!", APP_RUN_TIME, filepath);

		doc.Clear();
		return false;
	}


	// Add self to Map Data Cache.
	tinyxml2::XMLDocument cache_doc;
	if (cache_doc.LoadFile(g_sMapDataCacheFilepath.c_str()) != tinyxml2::XMLError::XML_SUCCESS)
	{
		LOG_DBG_ERROR("[{:.4f}][ExportMapData] Failed to load Map Data Cache \"{}\"!", APP_RUN_TIME, g_sMapDataCacheFilepath);
		LOG_FILE_ERROR("[{:.4f}][ExportMapData] Failed to load Map Data Cache \"{}\"!", APP_RUN_TIME, g_sMapDataCacheFilepath);

		cache_doc.Clear();
		return false;
	}
	 
	auto cache_root = cache_doc.RootElement();
	auto map = cache_root->InsertNewChildElement("Map");
	map->SetAttribute("name", map_name.c_str());
	map->SetAttribute("path", map_name.c_str());
	

	if (cache_doc.SaveFile(g_sMapDataCacheFilepath.c_str()) != tinyxml2::XMLError::XML_SUCCESS)
	{
		LOG_DBG_ERROR("[{:.4f}][ExportMapData] Error saving Map Data Cache: \"{}\"!", APP_RUN_TIME, g_sMapDataCacheFilepath);
		LOG_FILE_ERROR("[{:.4f}][ExportMapData] Error saving Map Data Cache: \"{}\"!", APP_RUN_TIME, g_sMapDataCacheFilepath);

		cache_doc.Clear();
		return false;
	}


	LOG_DBG_INFO("[{:.4f}][ExportMapData] Successfully added map to Map Data Cache as: \"{}\" with path \"{}\"!", APP_RUN_TIME, map_name, map_name);
	LOG_FILE_INFO("[{:.4f}][ExportMapData] Successfully added map to Map Data Cache as: \"{}\" with path \"{}\"!", APP_RUN_TIME, map_name, map_name);

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
}


void GameEditor::ExportEntityComponentFort(tinyxml2::XMLElement* xml, Entity* entity)
{
	auto fort = xml->InsertNewChildElement("Fort");
	auto building_slots = xml->InsertNewChildElement("BuildingSlots");
	auto territory = xml->InsertNewChildElement("Territory");

	auto component = entity->Get< ComponentTownhall >("Fort");

	xml->SetAttribute("prefab", component->m_prefabFilepath.c_str());

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
void GameEditor::ExportEntityComponentTownhall(tinyxml2::XMLElement* xml, Entity* entity) // THIS HAS A BUG: EXPORTING BuidlginSlots should go INSIDE the Townhall Xml.
{
	auto th = xml->InsertNewChildElement("Townhall");
	auto building_slots = th->InsertNewChildElement("BuildingSlots");
	auto territory = th->InsertNewChildElement("Territory");

	auto component = entity->Get< ComponentTownhall >("Townhall");

	th->SetAttribute("prefab", component->m_prefabFilepath.c_str());

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
	auto unitXml = xml->InsertNewChildElement("Unit");

	auto c = entity->Get< ComponentUnit >("Unit");
	unitXml->SetAttribute("prefab", c->m_prefabFilepath.c_str());
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
			auto unit = entity->FirstChildElement("Unit");


			if (townhall)
			{
				ImportEntityComponentTownhall(townhall, obj);
			}
			if (fort)
			{
				ImportEntityComponentFort(fort, obj);
			}
			if (unit)
			{
				ImportEntityComponentUnit(unit, obj);
			}


			entity = entity->NextSiblingElement("Object");
		}

		layer = layer->NextSiblingElement("Layer");
	}


	UpdateLayerSorting();
	LOG_DBG_INFO("[{:.4f}][ImportMapData] Success!", APP_RUN_TIME);
	return true;
}

bool GameEditor::ImportMapDataCache(const std::string& filepath)
{
	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(filepath.c_str()) != tinyxml2::XMLError::XML_SUCCESS)
	{
		LOG_DBG_ERROR("[{:.4f}][ImportMapDataCache] Failed loading Map Data Cache \"{}\"!", APP_RUN_TIME, filepath);
		LOG_FILE_ERROR("[{:.4f}][ImportMapDataCache] Failed loading Map Data Cache \"{}\"!", APP_RUN_TIME, filepath);
		doc.Clear();
		return false;
	}

	auto root = doc.RootElement();
	auto map = root->FirstChildElement("Map");
	while(map)
	{

		std::string map_full_path = "assets/" + std::string(map->Attribute("path")) + ".xml";
		m_mapDataCacheMap.emplace(map_full_path, map->Attribute("name"));

		map = map->NextSiblingElement("Map");
	}

	doc.Clear();
	m_mapDataCacheLoaded = true;

	LOG_DBG_INFO("[{:.4f}][ImportMapDataCache] Successfully loaded Map Data Cache \"{}\" with size \"{}\"!", APP_RUN_TIME, filepath, m_mapDataCacheMap.size());
	LOG_FILE_INFO("[{:.4f}][ImportMapDataCache] Successfully loaded Map Data Cache \"{}\" with size \"{}\"!", APP_RUN_TIME, filepath, m_mapDataCacheMap.size());

	return true;
}

void GameEditor::DisplayMapExportDialog()
{
	static char map_filepath_name[64] = "";
	std::string path = "assets/";
	std::string extension = ".xml";
	
	ImGui::SetNextWindowPos(ImVec2(ScreenWidth() / 2.0f - ScreenWidth() / 4.0f, ScreenHeight() / 2.0f - ScreenHeight() / 4.0f), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(ImVec2(500, 250), ImGuiCond_Appearing);

	ImGui::Begin("Export Map Data", &g_bSavingMapDataDialog);
	ImGui::InputText("|", map_filepath_name, 64);
	HelpMarkerWithoutQuestion("Name of the file that will be created. The file will be saved in \"assets/\". It is not necessary to add the XML or MAP extension");
	ImGui::SameLine();
	if (ImGui::SmallButton("OK"))
	{
		// Check for sanity.
		std::string name = std::string(map_filepath_name);
		std::string full_filepath = path + name + extension;

		bool length = name.length() > 0;
		bool result = false;
		if (length)
		{
			result = ExportMapData(full_filepath, name);
		}

		if (!length)
		{
			LOG_DBG_ERROR("[{:.4f}][DisplayMapExportDialog] Error exporting Map Data \"{}\": Map Name has 0 length!", APP_RUN_TIME, name);
			LOG_FILE_ERROR("[{:.4f}][DisplayMapExportDialog]  Error exporting Map Data \"{}\": Map Name has 0 length!", APP_RUN_TIME, name);
		}
		if (result)
		{
			LOG_DBG_INFO("[{:.4f}][DisplayMapExportDialog] Success exporting Map Data \"{}\" into \"{}\"!", APP_RUN_TIME, name, full_filepath);
			LOG_FILE_INFO("[{:.4f}][DisplayMapExportDialog]  Success exporting Map Data \"{}\" into \"{}\"!", APP_RUN_TIME, name, full_filepath);

			// Reload the Map Data Cache.
			ImportMapDataCache(g_sMapDataCacheFilepath);
		}
		else
		{
			LOG_DBG_ERROR("[{:.4f}][DisplayMapExportDialog] Error exporting Map Data \"{}\" into \"{}\"!", APP_RUN_TIME, name, full_filepath);
			LOG_FILE_ERROR("[{:.4f}][DisplayMapExportDialog]  Error exporting Map Data \"{}\" into \"{}\"!", APP_RUN_TIME, name, full_filepath);
		}

		memset(&map_filepath_name, 0, sizeof(map_filepath_name));
		g_bSavingMapDataDialog = false;
	}

	ImGui::End();
}

void GameEditor::DisplayMapImportDialog()
{
	ImGui::SetNextWindowPos(ImVec2(ScreenWidth() / 2.0f - ScreenWidth() / 4.0f, ScreenHeight() / 2.0f - ScreenHeight() / 4.0f), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(ImVec2(500, 250), ImGuiCond_Appearing);

	ImGui::Begin("Import Map Data", &g_bLoadingMapDataDialog);

	auto open = ImGui::CollapsingHeader("Quick Load");
	HelpMarkerWithoutQuestion("Select a Map previously created to load into the editor");

	if (open)
	{
		for (auto& map : m_mapDataCacheMap)
		{
			if (ImGui::Button(map.second.c_str()))
			{
				bool success = ImportMapData(map.first);
				g_bLoadingMapDataDialog = false;
				SetAllLayersVisible();

				if(!success)
				{
					LOG_DBG_ERROR("[{:.4f}][DisplayMapImportDialog] Error loading Map Data \"{}\" from \"{}\"!", APP_RUN_TIME, map.second, map.first);
					LOG_FILE_ERROR("[{:.4f}][DisplayMapImportDialog] Error loading Map Data \"{}\" from \"{}\"!", APP_RUN_TIME, map.second, map.first);
				}
				else
				{
					LOG_DBG_INFO("[{:.4f}][DisplayMapImportDialog] Successfully loaded Map Data \"{}\" from \"{}\"!", APP_RUN_TIME, map.second, map.first);
					LOG_FILE_INFO("[{:.4f}][DisplayMapImportDialog] Successfully loaded Map Data \"{}\" from \"{}\"!", APP_RUN_TIME, map.second, map.first);
				}
			}
		}
	}

	ImGui::End();
}

void GameEditor::FreeMapData()
{
	// Remove all Entities from Gameworld and from Entity Database.
	while(m_entities.size() > 0)
	{
		auto e = m_entities[0];
		delete e;
		m_entities.erase(m_entities.begin());
	}

	// Clear all Rendering Layers.
	for(auto& layer : m_gameworld)
	{
		DeleteRenderingLayer(layer.first);
	}

	// Reset all Rendering Layer related data.
	m_layerOrder.clear();
	m_sortedLayers.clear();
	m_sortedLayersAscending.clear();
	m_visibleLayers.clear();
	m_mapobjectCount = 0;

	m_mapobjectCount = 0;
	m_currentLayer = "none";
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


void GameEditor::ImportEntityComponentFort(tinyxml2::XMLElement* xml, Entity* entity)
{
	entity->Add(new ComponentFort(entity->m_positionx, entity->m_positiony), "Fort");
	auto fort = entity->Get< ComponentFort >("Fort");

	fort->m_prefabFilepath = xml->Attribute("prefab");

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
	
	townhall->m_prefabFilepath = xml->Attribute("prefab");

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
	auto c = entity->Add< ComponentUnit >(new ComponentUnit(), "Unit");
	c->m_prefabFilepath = xml->Attribute("prefab");
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
		LOG_DBG_INFO("[{:.4f}][AddTerritoryToCity] Add Territory to Townhall \"{}\"!", APP_RUN_TIME, e->m_name);
		LOG_FILE_INFO("[{:.4f}][AddTerritoryToCity] Add Territory to Townhall \"{}\"!", APP_RUN_TIME, e->m_name);

	}
	else if (e->Has("Fort"))
	{
		e->Get< ComponentFort >("Fort")->AddTerritory(slotx, sloty);
		LOG_DBG_INFO("[{:.4f}][AddTerritoryToCity] Add Territory to Fort \"{}\"!", APP_RUN_TIME, e->m_name);
		LOG_FILE_INFO("[{:.4f}][AddTerritoryToCity] Add Territory to Fort \"{}\"!", APP_RUN_TIME, e->m_name);
	}
	else
	{
		LOG_DBG_ERROR("[{:.4f}][AddTerritoryToCity] Failed adding Territory to City \"{}\": Entity does not have Townhall or Fort component!", APP_RUN_TIME, e->m_name);
		LOG_FILE_ERROR("[{:.4f}][AddTerritoryToCity] Failed adding Territory to City \"{}\": Entity does not have Townhall or Fort component!", APP_RUN_TIME, e->m_name);
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


bool GameEditor::ExportUnitPrefab(const std::string& filepath, SPrefab* prefab)
{
	tinyxml2::XMLDocument doc;
	auto xmlRoot = doc.NewElement("UnitPrefab");
	doc.InsertEndChild(xmlRoot);

	auto data = xmlRoot->InsertNewChildElement("Data");

	// Export default data.
	data->SetAttribute("gold_cost", prefab->gold_cost);
	data->SetAttribute("sprite", prefab->sprite.c_str());
	data->SetAttribute("name", prefab->prefab_name.c_str());
	data->SetAttribute("layout_template", prefab->layout_template_name.c_str());
	data->SetAttribute("defense", prefab->defense);
	data->SetAttribute("health", prefab->health);
	data->SetAttribute("level", prefab->level);
	data->SetAttribute("sight_range", prefab->sight_range);
	data->SetAttribute("detect_hidden", prefab->can_detect_hidden_units);

	// Export unit prefab specific data.
	if(g_bPrefabEditorEditingUnits)
	{
		data->SetAttribute("unit_prefab", true);
		data->SetAttribute("action_points", prefab->action_points);
		data->SetAttribute("armor", prefab->armor);
		data->SetAttribute("attack_min", prefab->attack_min);
		data->SetAttribute("attack_max", prefab->attack_max);
		data->SetAttribute("movement_type", prefab->movement_type);
		data->SetAttribute("race", prefab->race);
		data->SetAttribute("building_name", prefab->building_name.c_str());
		data->SetAttribute("building_level", prefab->building_level);
		data->SetAttribute("level_progression_type", prefab->unit_progression_type);

		data->SetAttribute("armor_piercing_weapon", prefab->armor_piercing_weapon);
		data->SetAttribute("anti_cavalry", prefab->anti_cavalry);
		data->SetAttribute("anti_cavalry_bonus", prefab->anti_cavalry_bonus);
		data->SetAttribute("flanking_bonus", prefab->flanking_bonus);
		data->SetAttribute("backstab_bonus", prefab->backstab_bonus);

		auto ranged = prefab->can_attack_ranged;
		data->SetAttribute("ranged_unit", ranged);
		if(ranged)
		{
			data->SetAttribute("ranged_attack_min", prefab->ranged_attack_min);
			data->SetAttribute("ranged_attack_max", prefab->ranged_attack_max);
			data->SetAttribute("ranged_range_min", prefab->ranged_range_min);
			data->SetAttribute("ranged_range_max", prefab->ranged_range_max);
		}

		
		// Add starting statuses.
		auto xmlStatus = data->InsertNewChildElement("StartingStatus");
		for (auto& s : prefab->starting_status_vec)
		{
			auto element = xmlStatus->InsertNewChildElement("Status");
			element->SetAttribute("name", s.c_str());
		}
		// Add abilities.
		auto xmlAbility = data->InsertNewChildElement("Abilities");
		for (auto& ab : prefab->abilities_vec)
		{
			auto element = xmlAbility->InsertNewChildElement("Ability");
			element->SetAttribute("name", ab.c_str());
		}

	}
	// Export building specific data.
	else if (g_bPrefabEditorEditingBuildings)
	{
		data->SetAttribute("building_prefab", true);
		data->SetAttribute("gold_production", prefab->gold_production);
		data->SetAttribute("research_points_production", prefab->research_points_production);
		data->SetAttribute("visibility_distance", prefab->visibility_distance_in_tiles);
		data->SetAttribute("predecessor_building", prefab->predecessor_building_for_upgrade.c_str());
	}


	std::string path = "assets/TilesetData/UnitPrefab/" + filepath + ".xml";
	if(doc.SaveFile(path.c_str()) != tinyxml2::XMLError::XML_SUCCESS)
	{
		LOG_DBG_ERROR("[{:.4f}][ExportUnitPrefab] Could not save Prefab: \"{}\"!", APP_RUN_TIME, path);
		LOG_FILE_ERROR("[{:.4f}][ExportUnitPrefab] Could not save Prefab: \"{}\"!", APP_RUN_TIME, path);
		doc.Clear();
		return false;
	}

	LOG_DBG_INFO("[{:.4f}][ExportUnitPrefab] Exported Prefab: \"{}\" as \"{}\"!", APP_RUN_TIME, prefab->prefab_name, path);
	LOG_FILE_INFO("[{:.4f}][ExportUnitPrefab] Exported Prefab: \"{}\" as \"{}\"!", APP_RUN_TIME, prefab->prefab_name, path);


	// Store the Prefab in the Prefab Cache.
	// Load.
	std::string cache_path = "assets/TilesetData/UnitPrefab/StrategyEditor_PrefabCache.xml";
	tinyxml2::XMLDocument cache;
	if (cache.LoadFile(cache_path.c_str()) != tinyxml2::XMLError::XML_SUCCESS)
	{
		LOG_DBG_ERROR("[{:.4f}][ExportUnitPrefab] Could not load Prefab Cache: \"{}\"!", APP_RUN_TIME, cache_path);
		LOG_FILE_ERROR("[{:.4f}][ExportUnitPrefab] Could not load Prefab Cache: \"{}\"!", APP_RUN_TIME, cache_path);
		cache.Clear();
		return false;
	}

	// Add the new Prefab Element.
	auto cache_root = cache.RootElement();
	auto prefab_element = cache_root->InsertNewChildElement("Prefab");
	prefab_element->SetAttribute("name", prefab->prefab_name.c_str());
	prefab_element->SetAttribute("path", filepath.c_str());
	prefab_element->SetAttribute("sprite", prefab->sprite.c_str());


	// Unload.
	if (cache.SaveFile(cache_path.c_str()) != tinyxml2::XMLError::XML_SUCCESS)
	{
		LOG_DBG_ERROR("[{:.4f}][ExportUnitPrefab] Could not save Prefab in Prefab Cache: \"{}\" as \"{}\"!", APP_RUN_TIME, prefab->prefab_name, filepath);
		LOG_FILE_ERROR("[{:.4f}][ExportUnitPrefab] Could not save Prefab in Prefab Cache: \"{}\" as \"{}\"!", APP_RUN_TIME, prefab->prefab_name, filepath);
		cache.Clear();
		return false;
	}

	LOG_DBG_INFO("[{:.4f}][ExportUnitPrefab] Saved Prefab in Prefab Cache: \"{}\" as \"{}\"!", APP_RUN_TIME, prefab->prefab_name, filepath);
	LOG_FILE_INFO("[{:.4f}][ExportUnitPrefab] Saved Prefab in Prefab Cache: \"{}\" as \"{}\"!", APP_RUN_TIME, prefab->prefab_name, filepath);

	return true;
}

bool GameEditor::ImportUnitPrefabCache(const std::string& filepath)
{
	tinyxml2::XMLDocument cache;
	if (cache.LoadFile(filepath.c_str()) != tinyxml2::XMLError::XML_SUCCESS)
	{
		LOG_DBG_ERROR("[{:.4f}][ExportUnitPrefab] Could not load Prefab Cache: \"{}\"!", APP_RUN_TIME, filepath);
		LOG_FILE_ERROR("[{:.4f}][ExportUnitPrefab] Could not load Prefab Cache: \"{}\"!", APP_RUN_TIME, filepath);
		cache.Clear();
		return false;
	}

	auto root = cache.RootElement();
	auto prefab = root->FirstChildElement("Prefab");
	while(prefab)
	{
		m_prefabCacheMap.emplace(prefab->Attribute("path"), prefab->Attribute("name"));
		m_prefabDecalDatabase.emplace(prefab->Attribute("path"), prefab->Attribute("sprite"));
		prefab = prefab->NextSiblingElement("Prefab");
	}



	LOG_DBG_INFO("[{:.4f}][ImportUnitPrefabCache] Prefab Cache loaded: \"{}\" with size: \"{}\"!", APP_RUN_TIME, filepath, m_prefabCacheMap.size());
	LOG_FILE_INFO("[{:.4f}][ImportUnitPrefabCache] Prefab Cache loaded: \"{}\" with size: \"{}\"!", APP_RUN_TIME, filepath, m_prefabCacheMap.size());
	m_prefabCacheLoaded = true;
}

SPrefab* GameEditor::ImportUnitPrefab(const std::string& filepath)
{
	std::string path = "assets/TilesetData/UnitPrefab/" + filepath + ".xml";

	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(path.c_str()) != tinyxml2::XMLError::XML_SUCCESS)
	{
		LOG_DBG_ERROR("[{:.4f}][ImportUnitPrefab] Could not load file: \"{}\"!", APP_RUN_TIME, path);
		LOG_FILE_ERROR("[{:.4f}][ImportUnitPrefab] Could not load file: \"{}\"!", APP_RUN_TIME, path);
		doc.Clear();
		return nullptr;
	}

	SPrefab* prefab = new SPrefab();

	auto xmlRoot = doc.RootElement();
	auto data = xmlRoot->FirstChildElement("Data");

	auto unit_prefab = data->BoolAttribute("unit_prefab", false);
	auto building_prefab = data->BoolAttribute("building_prefab", false);

	auto sprite = data->Attribute("sprite");
	prefab->sprite = sprite;

	if(unit_prefab)
	{
		auto prefab_name = data->Attribute("name");
		auto layout_template_name = data->Attribute("layout_template");
		auto health = data->Int64Attribute("health");
		auto action_points = data->Int64Attribute("action_points");
		auto level = data->Int64Attribute("level");
		auto armor = data->Int64Attribute("armor");
		auto defense = data->Int64Attribute("defense");
		auto attack_min = data->Int64Attribute("attack_min");
		auto attack_max = data->Int64Attribute("attack_max");
		auto movement_type = data->Int64Attribute("movement_type");
		auto race = data->Int64Attribute("race");
		auto building_name = data->Attribute("building_name");
		auto building_level = data->Int64Attribute("building_level");
		auto gold_cost = data->Int64Attribute("gold_cost");
		auto ranged = data->BoolAttribute("ranged_unit");
		auto unit_progression_type = data->Int64Attribute("level_progression_type");
		auto detect_hidden = data->BoolAttribute("detect_hidden");
		auto sight_range = data->Int64Attribute("sight_range");

		auto arm_pierc = data->BoolAttribute("armor_piercing_weapon");
		auto anti_cav = data->BoolAttribute("anti_cavalry");
		auto anti_cav_bonus = data->Int64Attribute("anti_cavalry_bonus");
		auto flank_bonus = data->Int64Attribute("flanking_bonus");
		auto backstab_bonus = data->Int64Attribute("backstab_bonus");

		if(ranged)
		{
			prefab->ranged_attack_min = data->Int64Attribute("ranged_attack_min");
			prefab->ranged_attack_max = data->Int64Attribute("ranged_attack_max");
			prefab->ranged_range_min = data->Int64Attribute("ranged_range_min");
			prefab->ranged_range_max = data->Int64Attribute("ranged_range_max");
		}
		prefab->prefab_name = prefab_name;
		prefab->layout_template_name = layout_template_name;
		prefab->health = health;
		prefab->action_points = action_points;
		prefab->level = level;
		prefab->unit_progression_type = unit_progression_type;
		prefab->armor = armor;
		prefab->defense = defense;
		prefab->attack_min = attack_min;
		prefab->attack_max = attack_max;
		prefab->movement_type = movement_type;
		prefab->race = race;
		prefab->building_name = building_name;
		prefab->building_level = building_level;
		prefab->gold_cost = gold_cost;
		prefab->can_attack_ranged = ranged;
		prefab->can_detect_hidden_units = detect_hidden;
		prefab->armor_piercing_weapon = arm_pierc;
		prefab->anti_cavalry = anti_cav;
		prefab->anti_cavalry_bonus = anti_cav_bonus;
		prefab->flanking_bonus = flank_bonus;
		prefab->backstab_bonus = backstab_bonus;
		prefab->sight_range = sight_range;
		
		
		auto statuses = data->FirstChildElement("StartingStatus");
		auto abilities = data->FirstChildElement("Abilities");

		auto stat = statuses->FirstChildElement("Status");
		while (stat)
		{
			auto status_name = stat->Attribute("name");
			prefab->starting_status_vec.push_back(status_name);

			stat = stat->NextSiblingElement("Status");
		}

		auto abl = abilities->FirstChildElement("Ability");
		while (abl)
		{
			auto ability_name = abl->Attribute("name");
			prefab->abilities_vec.push_back(ability_name);

			abl = abl->NextSiblingElement("Ability");
		}
	}
	

	if(building_prefab)
	{
		// TODO
		LOG_DBG_CRITICAL("[{:.4f}][ImportUnitPrefab] Loading Building Prefabs not implemented: \"{}\"!", APP_RUN_TIME, path);
		LOG_FILE_CRITICAL("[{:.4f}][ImportUnitPrefab] Loading Building Prefabs not implemented: \"{}\"!", APP_RUN_TIME, path);
	}

	if(prefab)
	{
		LOG_DBG_INFO("[{:.4f}][ImportUnitPrefab] Loaded Prefab: \"{}\"!", APP_RUN_TIME, path);
		LOG_FILE_INFO("[{:.4f}][ImportUnitPrefab] Loaded Prefab: \"{}\"!", APP_RUN_TIME, path);
	}
	else
	{
		LOG_DBG_ERROR("[{:.4f}][ImportUnitPrefab] Could not load Prefab: \"{}\"!", APP_RUN_TIME, path);
		LOG_FILE_ERROR("[{:.4f}][ImportUnitPrefab] Could not load Prefab: \"{}\"!", APP_RUN_TIME, path);
	}

	return prefab;
}

void GameEditor::DisplayUnitPrefabExportWindow(SPrefab* prefab)
{
	static char prefab_filepath_name[64] = "";
	if (prefab)
	{
		ImGui::SetNextWindowPos(ImVec2(ScreenWidth() / 2.0f - ScreenWidth() / 4.0f, ScreenHeight() / 2.0f - ScreenHeight() / 4.0f), ImGuiCond_Appearing);
		ImGui::SetNextWindowSize(ImVec2(500, 250), ImGuiCond_Appearing);

		ImGui::Begin("Prefab Export", &g_bExportingUnitPrefab);
		ImGui::InputText("|", prefab_filepath_name, 64);
		HelpMarkerWithoutQuestion("Name of the file that will be created. The file will be saved in \"assets/TilesetData/UnitPrefab\". It is not necessary to add the XML extension");
		ImGui::SameLine();
		if (ImGui::SmallButton("OK"))
		{
			// Check for sanity.
			std::string name = std::string(prefab_filepath_name);

			bool length = name.length() > 0;
			bool result = false;
			if (length)
			{
				result = ExportUnitPrefab(name, prefab);
			}

			if (!length)
			{
				LOG_DBG_ERROR("[{:.4f}][DisplayUnitPrefabExportWindow] Error exporting Unit Prefab \"{}\": Filepath has 0 length!", APP_RUN_TIME, prefab->prefab_name);
				LOG_FILE_ERROR("[{:.4f}][DisplayUnitPrefabExportWindow]  Error exporting Unit Prefab \"{}\": Filepath has 0 length!", APP_RUN_TIME, prefab->prefab_name);
			}
			if(result)
			{
				LOG_DBG_INFO("[{:.4f}][DisplayUnitPrefabExportWindow] Success exporting Unit Prefab \"{}\"!", APP_RUN_TIME, prefab->prefab_name);
				LOG_FILE_INFO("[{:.4f}][DisplayUnitPrefabExportWindow]  Success exporting Unit Prefab \"{}\"!", APP_RUN_TIME, prefab->prefab_name);

				// Reload the Prefab Cache.
				ImportUnitPrefabCache(g_sPrefabCacheFilepath);
			}
			else 
			{
				LOG_DBG_ERROR("[{:.4f}][DisplayUnitPrefabExportWindow] Error exporting Unit Prefab \"{}\"!", APP_RUN_TIME, prefab->prefab_name);
				LOG_FILE_ERROR("[{:.4f}][DisplayUnitPrefabExportWindow]  Error exporting Unit Prefab \"{}\"!", APP_RUN_TIME, prefab->prefab_name);
			}

			memset(&prefab_filepath_name, 0, sizeof(prefab_filepath_name));
			g_bExportingUnitPrefab = false;
			g_pExportedPrefab = nullptr;
		}

		ImGui::End();
	}
}

void GameEditor::DisplayUnitPrefabImportWindow()
{
	static char prefab_import_filepath_name[256] = "";
	
	ImGui::SetNextWindowPos(ImVec2(ScreenWidth() / 2.0f - ScreenWidth() / 4.0f, ScreenHeight() / 2.0f - ScreenHeight() / 4.0f), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(ImVec2(500, 250), ImGuiCond_Appearing);

	ImGui::Begin("Prefab Import", &g_bImportingUnitPrefab);
	ImGui::InputText("|", prefab_import_filepath_name, 64);
	HelpMarkerWithoutQuestion("Name of the file that will be imported. The file will be loaded from \"assets/TilesetData/UnitPrefab\". It is not necessary to add the XML extension");
	ImGui::SameLine();
	if (ImGui::SmallButton("OK"))
	{
		// Check for sanity.
		std::string name = std::string(prefab_import_filepath_name);

		bool length = name.length() > 0;
		bool result = false;
		if (length)
		{
			g_pCurrentEditedPrefab = ImportUnitPrefab(name);
			result = (g_pCurrentEditedPrefab != nullptr);
		}

		if (!length)
		{
			LOG_DBG_ERROR("[{:.4f}][DisplayUnitPrefabImportWindow] Error importing Unit Prefab: Filepath has 0 length!", APP_RUN_TIME);
			LOG_FILE_ERROR("[{:.4f}][DisplayUnitPrefabImportWindow]  Error importing Unit Prefab: Filepath has 0 length!", APP_RUN_TIME);
		}
		if (result)
		{
			g_sUnitEditorCurrentUnitSprite = g_pCurrentEditedPrefab->sprite;
			g_sCurrentEditedPrefabFilePath = name;

			LOG_DBG_INFO("[{:.4f}][DisplayUnitPrefabImportWindow] Success importing Unit Prefab \"{}\"!", APP_RUN_TIME, g_pCurrentEditedPrefab->prefab_name);
			LOG_FILE_INFO("[{:.4f}][DisplayUnitPrefabImportWindow]  Success importing Unit Prefab \"{}\"!", APP_RUN_TIME, g_pCurrentEditedPrefab->prefab_name);
		}
		else
		{
			LOG_DBG_ERROR("[{:.4f}][DisplayUnitPrefabImportWindow] Error importing Unit Prefab \"{}\"!", APP_RUN_TIME, name);
			LOG_FILE_ERROR("[{:.4f}][DisplayUnitPrefabImportWindow]  Error importing Unit Prefab \"{}\"!", APP_RUN_TIME, name);
		}

		memset(&prefab_import_filepath_name, 0, sizeof(prefab_import_filepath_name));
		g_bImportingUnitPrefab = false;
	}

	DisplayUnitEditorPrefabQuickLoadDropDown();

	ImGui::End();
}


void GameEditor::DisplayUnitEditorPrefabQuickLoadDropDown()
{
	auto open = ImGui::CollapsingHeader("Quick Load");
	HelpMarkerWithoutQuestion("Select a Prefab to load from all available in the Prefab Cache. The Prefab Cache consists of all saved Unit Prefabs");

	if(open)
	{
		for(auto& prefab: m_prefabCacheMap)
		{
			if(ImGui::Button(prefab.first.c_str()))
			{
				g_pCurrentEditedPrefab = ImportUnitPrefab(prefab.first);
				if(g_pCurrentEditedPrefab)
				{
					g_sUnitEditorCurrentUnitSprite = g_pCurrentEditedPrefab->sprite;
					g_bImportingUnitPrefab = false;
					g_sCurrentEditedPrefabFilePath = prefab.first;
				}
				return;
			}
		}
	}
}


void GameEditor::DisplayUnitEditor()
{
	std::string name = "Prefab Editor";
	auto scene_edit_window_width = 600.f;
	auto prefab_preview_window_width = 700.0f;
	auto prefab_element_window_width = 300.f;
	auto main_window_height = DEFAULT_WINDOW_HEIGHT - 50.0f;

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration;

	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(ImVec2(scene_edit_window_width + prefab_preview_window_width + prefab_element_window_width + 35.0f, main_window_height), ImGuiCond_Appearing);
	ImGui::Begin(name.c_str(), &g_bUnitEditorOpen, flags);

	// Display all editing windows.
	
	ImGui::BeginChild("Stats", ImVec2(scene_edit_window_width, main_window_height - 1.0f), true);
	
	DisplayUnitEditorMainMenu();
	
	if(g_bPrefabEditorEditingUnits)
	{
		DisplayUnitEditorNameEdit();
		DisplayUnitEditorLayoutTemplateNameEdit();
		DisplayUnitEditorHealthEdit();
		DisplayUnitEditorActionPointsEdit();
		if(g_pCurrentEditedPrefab)
		{
			// Sight Ranged
			DisplayEditFieldNumber(g_pCurrentEditedPrefab->sight_range, 1, 100, "Sight Range", "Change how far the unit can see through fog of war", 90000, 90001);
			// Whether detects hidden
			DisplayEditFieldBoolean(g_pCurrentEditedPrefab->can_detect_hidden_units, "Detect Hidden", "Change whether unit can detect invisible units, such as spies and assassins", 99000);
			
			// Armor Piercing
			DisplayEditFieldBoolean(g_pCurrentEditedPrefab->armor_piercing_weapon, "Armor Piercing Weapon", "Change whether units weapon ignores the enemies armor", 99001);

			// Anti Cavalry
			DisplayEditFieldBoolean(g_pCurrentEditedPrefab->anti_cavalry, "Anti Cavalry", "Change whether unit is Anti-Cavalry, dealing additional damage to mounted enemy units", 99002);
			if(g_pCurrentEditedPrefab->anti_cavalry)
			{
				DisplayEditFieldNumber(g_pCurrentEditedPrefab->anti_cavalry_bonus, 1, 100, "Anti Cavalry Damage Bonus", "Change how much of additional damage the unit deals to enemy Cavalry units", 90003, 90004);
			}

			// Flanking Bonus
			DisplayEditFieldNumber(g_pCurrentEditedPrefab->flanking_bonus, 0, 100, "Flanking Damage Bonus", "Change how much of additional damage the unit deals on flanking an enemy unit", 90005, 90006);

			// Backstab Bonus
			DisplayEditFieldNumber(g_pCurrentEditedPrefab->backstab_bonus, 0, 100, "Backstab Damage Bonus", "Change how much of additional damage the unit deals on attacking an enemy unit from behind", 90007, 90008);
		}
		DisplayUnitEditorLevelEdit();
		DisplayUnitEditorArmorEdit();
		DisplayUnitEditorAttackEdit();
		DisplayUnitEditorDefenseEdit();
		DisplayUnitEditorMovementTypeEdit();
		DisplayUnitEditorRaceEdit();
		DisplayUnitEditorBuildingRequirementsEdit();
		DisplayUnitEditorGoldCostEdit();
		DisplayUnitEditorStartingStatusEdit();
		DisplayUnitEditorAbilitiesEdit();
		DisplayUnitEditorUnitSpriteEdit();
	}
	if(g_bPrefabEditorEditingBuildings)
	{
		DisplayBuildingEditorNameEdit();
		DisplayBuildingEditorLayoutTemplateEdit();
		if (g_pCurrentEditedPrefab)
		{
			// Sight Ranged
			DisplayEditFieldNumber(g_pCurrentEditedPrefab->sight_range, 1, 100, "Sight Range", "Change how far the building can see through fog of war", 20000, 20001);
			// Whether detects hidden
			DisplayEditFieldBoolean(g_pCurrentEditedPrefab->can_detect_hidden_units, "Detect Hidden", "Change whether building can detect invisible units, such as spies and assassins", 30000);
		}
		DisplayUnitEditorLevelEdit();
		DisplayBuildingEditorProducerEdit();
		DisplayBuildingEditorDefenseEdit();
		DisplayBuildingEditorVisibilityEdit();
		DisplayBuildingEditorRequirementsEdit();
		DisplayPrefabEditorBuildingSpriteEdit();
	}
	
	ImGui::EndChild();

	DisplayUnitEditorUnitSprite();

	ImGui::End();
}


void GameEditor::DisplayUnitEditorMainMenu()
{
	auto quick_save = g_sCurrentEditedPrefabFilePath.compare("none") != 0 && g_pCurrentEditedPrefab != nullptr;


	if (ImGui::SmallButton("New"))
	{
		g_pCurrentEditedPrefab = new SPrefab();

		// Reset static data.
		// Prefab Name.
		memset(&g_cPrefabName, 0, sizeof(g_cPrefabName));
		// Layout Template Name.
		memset(&g_cPrefabLayoutTemplateName, 0, sizeof(g_cPrefabLayoutTemplateName));
		// Building Name.
		memset(&g_cPrefabRequiredBuildingName, 0, sizeof(g_cPrefabRequiredBuildingName));
		// Current Prefab Sprite.
		g_sUnitEditorCurrentUnitSprite = "none";
		g_sCurrentEditedPrefabFilePath = "none";
	}
	if(quick_save)
	{
		ImGui::SameLine();
		if (ImGui::SmallButton("Quick Save..."))
		{
			ExportUnitPrefab(g_sCurrentEditedPrefabFilePath, g_pCurrentEditedPrefab);
		}
	}
	ImGui::SameLine();
	if(ImGui::SmallButton("Save As..."))
	{
		// Export from g_pCurrentEditedPrefab into XML.
		g_bExportingUnitPrefab = true;
		g_pExportedPrefab = g_pCurrentEditedPrefab;
	}
	ImGui::SameLine();
	if (ImGui::SmallButton("Load From..."))
	{
		// Import from XML into g_pCurrentEditedPrefab.
		g_bImportingUnitPrefab = true;
	}
	ImGui::SameLine();
	if(ImGui::SmallButton("Unit Editor"))
	{
		// Start Editing a Unit prefab.
		// If we worked on a Building, remove it...
		delete g_pCurrentEditedPrefab;
		g_pCurrentEditedPrefab = nullptr;
		g_sUnitEditorCurrentUnitSprite = "none";

		g_bPrefabEditorEditingUnits = true;
		g_bPrefabEditorEditingBuildings = false;
	}
	ImGui::SameLine();
	if (ImGui::SmallButton("Building Editor"))
	{
		// Start Editing a Building prefab.
		// If we worked on a Unit, remove it...
		delete g_pCurrentEditedPrefab;
		g_pCurrentEditedPrefab = nullptr;
		g_sUnitEditorCurrentUnitSprite = "none";

		g_bPrefabEditorEditingUnits = false;
		g_bPrefabEditorEditingBuildings = true;
	}



	ImGui::SameLine();
	ImGui::SetCursorPosX(ImGui::GetWindowSize().x - 25.0f);
	if (ImGui::SmallButton("X"))
	{
		ToggleMenuItem(g_bUnitEditorOpen);
	}
	ImGui::Separator();
	ImGui::Separator();
}

void GameEditor::DisplayUnitEditorNameEdit()
{
	if(ImGui::CollapsingHeader("Name"))
	{
		ImGui::InputText("|", g_cPrefabName, sizeof(g_cPrefabName)); 
		if(g_pCurrentEditedPrefab)
		{
			HelpMarkerWithoutQuestion(g_pCurrentEditedPrefab->prefab_name.c_str());
		}
		ImGui::SameLine();
		if (ImGui::SmallButton("OK"))
		{
			// Check for sanity.
			std::string name = std::string(g_cPrefabName);

			auto length = name.size() > 0;

			if (length && g_pCurrentEditedPrefab != nullptr)
			{
				LOG_DBG_INFO("[{:.4f}][DisplayUnitEditorNameEdit] Set prefab name to \"{}\"!", APP_RUN_TIME, name);
				LOG_FILE_INFO("[{:.4f}][DisplayUnitEditorNameEdit] Set prefab name to \"{}\"!", APP_RUN_TIME, name);
			
				// Set the name.
				g_pCurrentEditedPrefab->prefab_name = name;

				memset(&g_cPrefabName, 0, sizeof(g_cPrefabName));
			}
			else if(!length)
			{
				LOG_DBG_ERROR("[{:.4f}][DisplayUnitEditorNameEdit] Could not set prefab name: Empty name!", APP_RUN_TIME);
				LOG_FILE_ERROR("[{:.4f}][DisplayUnitEditorNameEdit] Could not set prefab name: Empty name!", APP_RUN_TIME);
			}
			else if(!g_pCurrentEditedPrefab)
			{
				LOG_DBG_ERROR("[{:.4f}][DisplayUnitEditorNameEdit] Could not set prefab name: No prefab in work. Hit the \"New\" button Sherlock!", APP_RUN_TIME);
				LOG_FILE_ERROR("[{:.4f}][DisplayUnitEditorNameEdit] Could not set prefab name: No prefab in work. Hit the \"New\" button Sherlock!", APP_RUN_TIME);
			}
		}
		HelpMarkerWithoutQuestion("Set the Name of the unit. This name will be displayed to the player in-game. This is not the exported file name");
	}
}

void GameEditor::DisplayUnitEditorLayoutTemplateNameEdit()
{
	ImGuiID input_text_id = g_idUnitEditorElementID + 100;
	ImGuiID input_text_ok_id = g_idUnitEditorElementID + 101;

	if (ImGui::CollapsingHeader("Layout Template Name"))
	{
		ImGui::PushID(input_text_id);
		ImGui::InputText("|", g_cPrefabLayoutTemplateName, sizeof(g_cPrefabLayoutTemplateName));
		if (g_pCurrentEditedPrefab)
		{
			HelpMarkerWithoutQuestion(g_pCurrentEditedPrefab->layout_template_name.c_str());
		}
		ImGui::PopID();
		
		ImGui::SameLine();
		ImGui::PushID(input_text_ok_id);
		if (ImGui::SmallButton("OK"))
		{
			// Check for sanity.
			std::string name = std::string(g_cPrefabLayoutTemplateName);

			auto length = name.size() > 0;

			if (length && g_pCurrentEditedPrefab != nullptr)
			{
				LOG_DBG_INFO("[{:.4f}][DisplayUnitEditorLayoutTemplateNameEdit] Set Layout Template Name to \"{}\"!", APP_RUN_TIME, name);
				LOG_FILE_INFO("[{:.4f}][DisplayUnitEditorLayoutTemplateNameEdit] Set Layout Template Name to \"{}\"!", APP_RUN_TIME, name);

				// Set the layout template name for the prefab.
				g_pCurrentEditedPrefab->layout_template_name = name;

				memset(&g_cPrefabLayoutTemplateName, 0, sizeof(g_cPrefabLayoutTemplateName));
			}
			else if(!length)
			{
				LOG_DBG_ERROR("[{:.4f}][DisplayUnitEditorLayoutTemplateNameEdit] Could not set Layout Template Name: Empty name!", APP_RUN_TIME);
				LOG_FILE_ERROR("[{:.4f}][DisplayUnitEditorLayoutTemplateNameEdit] Could not set Layout Template Name: Empty name!", APP_RUN_TIME);
			}
			else if(!g_pCurrentEditedPrefab)
			{
				LOG_DBG_ERROR("[{:.4f}][DisplayUnitEditorNameEdit] Could not set Layout Template Name: No prefab in work. Hit the \"New\" button Sherlock!", APP_RUN_TIME);
				LOG_FILE_ERROR("[{:.4f}][DisplayUnitEditorNameEdit] Could not set Layout Template Name: No prefab in work. Hit the \"New\" button Sherlock!", APP_RUN_TIME);
			}
		}
		ImGui::PopID();
		HelpMarkerWithoutQuestion("Set the Layout Template for the unit");
	}
}

void GameEditor::DisplayUnitEditorHealthEdit()
{
	ImGuiID health_id = g_idUnitEditorElementID;
	ImGuiID health_scalar_id = g_idUnitEditorElementID + 1;

	if (ImGui::CollapsingHeader("Health"))
	{
		if(g_pCurrentEditedPrefab)
		{
			int health = g_pCurrentEditedPrefab->health;
			ImGui::PushID(health_id);
			ImGui::SliderInt("Int", &health, 1, 1000, "%d");
			ImGui::PopID();
			HelpMarkerWithoutQuestion("The Health amount of the unit/building. Defines how much damage the unit/building can take before dying");
			ImGui::SameLine();
			ImGui::PushID(health_scalar_id);
			ImGui::InputScalar("ScalarInt", ImGuiDataType_U32, &health, &u32_one);
			ImGui::PopID();
			HelpMarkerWithoutQuestion("The Health amount of the unit/building.  Defines how much damage the unit/building can take before dying");

			g_pCurrentEditedPrefab->health = health;
		}
	}
}


void GameEditor::DisplayEditFieldNumber(int& value, int min, int max, const char* field_name, const char* field_desc, ImGuiID slider_id, ImGuiID scalar_id)
{
	if(ImGui::CollapsingHeader(field_name))
	{
		int previous_value = value;
		ImGui::PushID(slider_id);
		ImGui::SliderInt("Int", &previous_value, min, max, "%d");
		ImGui::PopID();
		HelpMarkerWithoutQuestion(field_desc);
		ImGui::SameLine();
		ImGui::PushID(scalar_id);
		ImGui::InputScalar("ScalarInt", ImGuiDataType_U32, &previous_value, &u32_one);
		ImGui::PopID();
		HelpMarkerWithoutQuestion(field_desc);

		value = previous_value;
	}
}



void GameEditor::DisplayEditFieldBoolean(bool& value, const char* field_name, const char* field_desc, ImGuiID checkbox_id)
{
	ImGui::PushID(checkbox_id);
	ImGui::Checkbox(field_name, &value);
	ImGui::PopID();
	HelpMarkerWithoutQuestion(field_desc);
}

bool GameEditor::DisplayEditFieldString(char* string, int string_size, std::string& current_string, const char* field_name, const char* field_desc, ImGuiID text_input_id, ImGuiID text_input_ok_id)
{
	bool result = false;

	if (ImGui::CollapsingHeader(field_name))
	{
		ImGui::PushID(text_input_ok_id);
		ImGui::InputText("|", string, string_size);
		ImGui::PopID();

		HelpMarkerWithoutQuestion(current_string.c_str());

		ImGui::SameLine();
		ImGui::PushID(text_input_id);
		if (ImGui::SmallButton("OK"))
		{
			// Check for sanity.
			std::string name = std::string(string);

			auto length = name.size() > 0;

			if (length)
			{
				LOG_DBG_INFO("[{:.4f}][DisplayEditFieldString] Set string to \"{}\"!", APP_RUN_TIME, name);
				LOG_FILE_INFO("[{:.4f}][DisplayEditFieldString] Set string to \"{}\"!", APP_RUN_TIME, name);

				// Set the name.
				current_string = name;
				result = true;
			}
			else if (!length)
			{
				LOG_DBG_ERROR("[{:.4f}][DisplayEditFieldString] Could not set string: Empty name!", APP_RUN_TIME);
				LOG_FILE_ERROR("[{:.4f}][DisplayEditFieldString] Could not set string: Empty name!", APP_RUN_TIME);
			}
		}
		ImGui::PopID();
		HelpMarkerWithoutQuestion(field_desc);
	}

	return result;
}

void GameEditor::DisplayEditFieldIcon(const char* field_name, const char* field_desc, std::map< std::string, olc::Decal* >& data_map, std::string& current_icon)
{
	std::vector< std::string > icon_vec;

	// Find our current item index in the vector.
	int current_item_index = 0;
	int index = 0;
	bool changed = false;
	for (auto& pair : data_map)
	{
		icon_vec.push_back(pair.first);
	}
	for (int i = 0; i < icon_vec.size(); i++)
	{
		if (icon_vec[i].compare(current_icon) == 0)
		{
			current_item_index = i;
			break;
		}
	}

	// Create the Combo window.
	if (ImGui::BeginCombo(field_name, current_icon.c_str()))
	{
		for (int i = 0; i < icon_vec.size(); i++)
		{
			const bool is_selected = (current_item_index == i);

			if (ImGui::Selectable(icon_vec[i].c_str(), is_selected))
			{
				current_item_index = i;
				changed = true;
			}

			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	HelpMarkerWithoutQuestion(field_desc);

	// Apply the change.
	if (changed)
	{
		current_icon = icon_vec[current_item_index];

		LOG_DBG_INFO("[{:.4f}][DisplayEditFieldIcon] Icon changed to \"{}\"", APP_RUN_TIME, icon_vec[current_item_index]);
		LOG_FILE_INFO("[{:.4f}][DisplayEditFieldIcon] Icon changed to \"{}\"", APP_RUN_TIME, icon_vec[current_item_index]);
	}
}

void GameEditor::DisplayUnitEditorActionPointsEdit()
{
	ImGuiID ap_id = g_idUnitEditorElementID + 2;
	ImGuiID ap_scalar_id = g_idUnitEditorElementID + 3;

	if (ImGui::CollapsingHeader("Action Points"))
	{
		if (g_pCurrentEditedPrefab)
		{
			int ap = g_pCurrentEditedPrefab->action_points;
			ImGui::PushID(ap_id);
			ImGui::SliderInt("Int", &ap, 1, 500, "%d");
			ImGui::PopID();
			HelpMarkerWithoutQuestion("The Action Points of the unit. Defines how many actions the unit can take, like moving and attacking or casting a spell or healing etc.");
			ImGui::SameLine();
			ImGui::PushID(ap_scalar_id);
			ImGui::InputScalar("ScalarInt", ImGuiDataType_U32, &ap, &u32_one);
			ImGui::PopID();
			HelpMarkerWithoutQuestion("The Action Points of the unit. Defines how many actions the unit can take, like moving and attacking or casting a spell or healing etc.");

			g_pCurrentEditedPrefab->action_points = ap;
		}
	}
}

void GameEditor::DisplayUnitEditorLevelEdit()
{
	ImGuiID lvl_id = g_idUnitEditorElementID + 4;
	ImGuiID lvl_scalar_id = g_idUnitEditorElementID + 5;

	if (ImGui::CollapsingHeader("Level"))
	{
		if (g_pCurrentEditedPrefab)
		{
			int lvl = g_pCurrentEditedPrefab->level;
			ImGui::PushID(lvl_id);
			ImGui::SliderInt("Int", &lvl, 1, 10, "%d");
			ImGui::PopID();
			HelpMarkerWithoutQuestion("The starting level of the unit/building. The unit/building will have this level on instancing in-game");
			ImGui::SameLine();
			ImGui::PushID(lvl_scalar_id);
			ImGui::InputScalar("ScalarInt", ImGuiDataType_U32, &lvl, &u32_one);
			ImGui::PopID();
			HelpMarkerWithoutQuestion("The starting level of the unit/building. The unit/building will have this level on instancing in-game");

			g_pCurrentEditedPrefab->level = lvl;


			DisplayEditFieldNumber(g_pCurrentEditedPrefab->unit_progression_type, 0, 2, "Level Progression Type", "The level progression type can be 0=Melee, 1=Ranged, 2=Mage. This field is required in order to level-up the unit automatically in-game", lvl_id + 100010, lvl_scalar_id + 100011);
		}
	}
}

void GameEditor::DisplayUnitEditorArmorEdit()
{
	ImGuiID arm_id = g_idUnitEditorElementID + 6;
	ImGuiID arm_scalar_id = g_idUnitEditorElementID + 7;

	if (ImGui::CollapsingHeader("Armor"))
	{
		if (g_pCurrentEditedPrefab)
		{
			int arm = g_pCurrentEditedPrefab->armor;
			ImGui::PushID(arm_id);
			ImGui::SliderInt("Int", &arm, 1, 60, "%d");
			ImGui::PopID();
			HelpMarkerWithoutQuestion("The Armor value of the unit. This affects how much damage the unit will take on being attacked; E.g. with an armor value of 50, 50% of the incoming damage will be removed");
			ImGui::SameLine();
			ImGui::PushID(arm_scalar_id);
			ImGui::InputScalar("ScalarInt", ImGuiDataType_U32, &arm, &u32_one);
			ImGui::PopID();
			HelpMarkerWithoutQuestion("The Armor value of the unit. This affects how much damage the unit will take on being attacked; E.g. with an armor value of 50, 50% of the incoming damage will be removed");

			g_pCurrentEditedPrefab->armor = arm;
		}
	}
}

void GameEditor::DisplayUnitEditorAttackEdit()
{
	ImGuiID attack_id = g_idUnitEditorElementID + 8;
	ImGuiID attack_scalar_id = g_idUnitEditorElementID + 9;
	ImGuiID ranged_attack_id = g_idUnitEditorElementID + 8000;
	ImGuiID ranged_attack_scalar_id = g_idUnitEditorElementID + 9000;
	ImGuiID ranged_range_id = g_idUnitEditorElementID + 10000;
	ImGuiID ranged_range_scalar_id = g_idUnitEditorElementID + 10001;


	if (ImGui::CollapsingHeader("Melee Attack"))
	{
		if (g_pCurrentEditedPrefab)
		{
			int attack_min = g_pCurrentEditedPrefab->attack_min;
			int attack_max = g_pCurrentEditedPrefab->attack_max;

			ImGui::PushID(attack_id);
			ImGui::SliderInt("Min", &attack_min, 1, 500, "%d");
			ImGui::PopID();
			HelpMarkerWithoutQuestion("The Minimal Melee Attack Value that this unit can do");
			ImGui::SameLine();
			ImGui::PushID(attack_scalar_id);
			ImGui::InputScalar("ScalarIntMin", ImGuiDataType_U32, &attack_min, &u32_one);
			ImGui::PopID();
			HelpMarkerWithoutQuestion("The Minimal Melee Attack Value that this unit can do");

			ImGui::PushID(attack_id + 1);
			ImGui::SliderInt("Max", &attack_max, 1, 500, "%d");
			ImGui::PopID();
			HelpMarkerWithoutQuestion("The Maximal Melee Attack Value that this unit can do");
			ImGui::SameLine();
			ImGui::PushID(attack_scalar_id + 1);
			ImGui::InputScalar("ScalarIntMax", ImGuiDataType_U32, &attack_max, &u32_one);
			ImGui::PopID();
			HelpMarkerWithoutQuestion("The Maximal Melee Attack Value that this unit can do");


			g_pCurrentEditedPrefab->attack_min = attack_min;
			g_pCurrentEditedPrefab->attack_max = attack_max;
		}
	}
	
	if(g_pCurrentEditedPrefab)
	{
		ImGui::Checkbox("Ranged Unit", &g_pCurrentEditedPrefab->can_attack_ranged);

		if(g_pCurrentEditedPrefab->can_attack_ranged)
		{
			// Attack values
			if (ImGui::CollapsingHeader("Ranged Attack"))
			{
				if (g_pCurrentEditedPrefab)
				{
					int attack_min = g_pCurrentEditedPrefab->ranged_attack_min;
					int attack_max = g_pCurrentEditedPrefab->ranged_attack_max;

					ImGui::PushID(ranged_attack_id);
					ImGui::SliderInt("Min", &attack_min, 1, 500, "%d");
					ImGui::PopID();
					HelpMarkerWithoutQuestion("The Minimal Ranged Attack Value that this unit can do");
					ImGui::SameLine();
					ImGui::PushID(ranged_attack_scalar_id);
					ImGui::InputScalar("ScalarIntMin", ImGuiDataType_U32, &attack_min, &u32_one);
					ImGui::PopID();
					HelpMarkerWithoutQuestion("The Minimal Ranged Attack Value that this unit can do");

					ImGui::PushID(ranged_attack_id + 1);
					ImGui::SliderInt("Max", &attack_max, 1, 500, "%d");
					ImGui::PopID();
					HelpMarkerWithoutQuestion("The Maximal Ranged Attack Value that this unit can do");
					ImGui::SameLine();
					ImGui::PushID(ranged_attack_scalar_id + 1);
					ImGui::InputScalar("ScalarIntMax", ImGuiDataType_U32, &attack_max, &u32_one);
					ImGui::PopID();
					HelpMarkerWithoutQuestion("The Maximal Ranged Attack Value that this unit can do");

					g_pCurrentEditedPrefab->ranged_attack_min = attack_min;
					g_pCurrentEditedPrefab->ranged_attack_max = attack_max;
				}
			}

			// Attack range
			if (ImGui::CollapsingHeader("Ranged Attack Range"))
			{
				if (g_pCurrentEditedPrefab)
				{
					int range_min = g_pCurrentEditedPrefab->ranged_range_min;
					int range_max = g_pCurrentEditedPrefab->ranged_range_max;

					ImGui::PushID(ranged_range_id);
					ImGui::SliderInt("Min", &range_min, 1, 20, "%d");
					ImGui::PopID();
					HelpMarkerWithoutQuestion("The Minimal Range that the unit can execute a ranged attack");
					ImGui::SameLine();
					ImGui::PushID(ranged_range_scalar_id);
					ImGui::InputScalar("ScalarIntMin", ImGuiDataType_U32, &range_min, &u32_one);
					ImGui::PopID();
					HelpMarkerWithoutQuestion("The Minimal Range that the unit can execute a ranged attack");

					ImGui::PushID(ranged_range_id + 1);
					ImGui::SliderInt("Max", &range_max, 1, 20, "%d");
					ImGui::PopID();
					HelpMarkerWithoutQuestion("The Maximal Range that the unit can execute a ranged attack");
					ImGui::SameLine();
					ImGui::PushID(ranged_range_scalar_id + 1);
					ImGui::InputScalar("ScalarIntMax", ImGuiDataType_U32, &range_max, &u32_one);
					ImGui::PopID();
					HelpMarkerWithoutQuestion("The Maximal Range that the unit can execute a ranged attack");

					g_pCurrentEditedPrefab->ranged_range_min = range_min;
					g_pCurrentEditedPrefab->ranged_range_max = range_max;
				}
			}
		}
	}
}

void GameEditor::DisplayUnitEditorDefenseEdit()
{
	ImGuiID def_id = g_idUnitEditorElementID + 10;
	ImGuiID def_scalar_id = g_idUnitEditorElementID + 11;

	if (ImGui::CollapsingHeader("Defense"))
	{
		if (g_pCurrentEditedPrefab)
		{
			int def = g_pCurrentEditedPrefab->defense;
			ImGui::PushID(def_id);
			ImGui::SliderInt("Int", &def, 1, 60, "%d");
			ImGui::PopID();
			HelpMarkerWithoutQuestion("The Defense value of the unit/building. This affects how much damage the unit/building will take on being attacked; E.g. with a defense value of 50, 50% of the incoming damage will be removed");
			ImGui::SameLine();
			ImGui::PushID(def_scalar_id);
			ImGui::InputScalar("ScalarInt", ImGuiDataType_U32, &def, &u32_one);
			ImGui::PopID();
			HelpMarkerWithoutQuestion("The Defense value of the unit/building. This affects how much damage the unit/building will take on being attacked; E.g. with a defense value of 50, 50% of the incoming damage will be removed");

			g_pCurrentEditedPrefab->defense = def;
		}
	}
}

void GameEditor::DisplayUnitEditorMovementTypeEdit()
{
	ImGuiID mov_id = g_idUnitEditorElementID + 12;
	ImGuiID mov_scalar_id = g_idUnitEditorElementID + 13;

	if (ImGui::CollapsingHeader("Movement Type"))
	{
		if (g_pCurrentEditedPrefab)
		{
			int mov = g_pCurrentEditedPrefab->movement_type;
			ImGui::PushID(mov_id);
			ImGui::SliderInt("Int", &mov, 1, 3, "%d");
			ImGui::PopID();
			HelpMarkerWithoutQuestion("The Movement Type of the unit. 1=Walk, 2=Flying, 3=Swimming, 4=Mounted"); // TODO Add Mounted to slider, export and import.
			ImGui::SameLine();
			ImGui::PushID(mov_scalar_id);
			ImGui::InputScalar("ScalarInt", ImGuiDataType_U32, &mov, &u32_one);
			ImGui::PopID();
			HelpMarkerWithoutQuestion("The Movement Type of the unit. 1=Walk, 2=Flying, 3=Swimming, 4=Mounted");

			g_pCurrentEditedPrefab->movement_type = mov;
		}
	}
}

void GameEditor::DisplayUnitEditorRaceEdit()
{
	ImGuiID race_id = g_idUnitEditorElementID + 14;
	ImGuiID race_scalar_id = g_idUnitEditorElementID + 15;

	if (ImGui::CollapsingHeader("Race"))
	{
		if (g_pCurrentEditedPrefab)
		{
			int race = g_pCurrentEditedPrefab->race;
			ImGui::PushID(race_id);
			ImGui::SliderInt("Int", &race, 1, 4, "%d");
			ImGui::PopID();
			HelpMarkerWithoutQuestion("The Race of the unit. 1=Human, 2=High Elf, 3=Dark Elf, 4=Orc");
			ImGui::SameLine();
			ImGui::PushID(race_scalar_id);
			ImGui::InputScalar("ScalarInt", ImGuiDataType_U32, &race, &u32_one);
			ImGui::PopID();
			HelpMarkerWithoutQuestion("The Race of the unit. 1=Human, 2=High Elf, 3=Dark Elf, 4=Orc");

			g_pCurrentEditedPrefab->race = race;
		}
	}
}

void GameEditor::DisplayUnitEditorBuildingRequirementsEdit()
{
	ImGuiID input_text_id = g_idUnitEditorElementID + 200;
	ImGuiID input_text_ok_id = g_idUnitEditorElementID + 201;

	if (ImGui::CollapsingHeader("Building Requirements"))
	{
		if (ImGui::CollapsingHeader("Building Name"))
		{
			ImGui::PushID(input_text_id);
			ImGui::InputText("|", g_cPrefabRequiredBuildingName, sizeof(g_cPrefabRequiredBuildingName));
			if (g_pCurrentEditedPrefab)
			{
				HelpMarkerWithoutQuestion(g_pCurrentEditedPrefab->building_name.c_str());
			}
			ImGui::PopID();
			ImGui::SameLine();
			ImGui::PushID(input_text_ok_id);
			if (ImGui::SmallButton("OK"))
			{
				// Check for sanity.
				std::string name = std::string(g_cPrefabRequiredBuildingName);

				auto length = name.size() > 0;

				if (length && g_pCurrentEditedPrefab != nullptr)
				{
					LOG_DBG_INFO("[{:.4f}][DisplayUnitEditorBuildingRequirementsEdit] Set required building name to \"{}\"!", APP_RUN_TIME, name);
					LOG_FILE_INFO("[{:.4f}][DisplayUnitEditorBuildingRequirementsEdit] Set required building name to \"{}\"!", APP_RUN_TIME, name);

					// Set the layout template name for the prefab.
					g_pCurrentEditedPrefab->building_name = name;

					memset(&g_cPrefabRequiredBuildingName, 0, sizeof(g_cPrefabRequiredBuildingName));
				}
				else if (!length)
				{
					LOG_DBG_ERROR("[{:.4f}][DisplayUnitEditorBuildingRequirementsEdit] Could not set required building name: Empty name!", APP_RUN_TIME);
					LOG_FILE_ERROR("[{:.4f}][DisplayUnitEditorBuildingRequirementsEdit] Could not set required building name: Empty name!", APP_RUN_TIME);
				}
				else if (!g_pCurrentEditedPrefab)
				{
					LOG_DBG_ERROR("[{:.4f}][DisplayUnitEditorBuildingRequirementsEdit] Could not set required building name: No prefab in work. Hit the \"New\" button Sherlock!", APP_RUN_TIME);
					LOG_FILE_ERROR("[{:.4f}][DisplayUnitEditorBuildingRequirementsEdit] Could not set required building name: No prefab in work. Hit the \"New\" button Sherlock!", APP_RUN_TIME);
				}
			}
			ImGui::PopID();
			HelpMarkerWithoutQuestion("Set the required building name. The building name will be searched for in-game and has to match. In that building this unit will be produced");
		}

		if (ImGui::CollapsingHeader("Building Level"))
		{
			ImGuiID build_id = g_iImguiImageButtonID + strlen("BuildingLevelInt") + 1;
			ImGuiID build_scalar_id = g_iImguiImageButtonID + strlen("BuildingLevelScalarInt") + 1;

			if (g_pCurrentEditedPrefab)
			{
				int build = g_pCurrentEditedPrefab->building_level;
				ImGui::PushID(build_id);
				ImGui::SliderInt("Int", &build, 1, 10, "%d");
				ImGui::PopID();
				HelpMarkerWithoutQuestion("The required building level. The building has to be of that level in order for the unit to be able to be produced");
				ImGui::SameLine();
				ImGui::PushID(build_scalar_id);
				ImGui::InputScalar("ScalarInt", ImGuiDataType_U32, &build, &u32_one);
				ImGui::PopID();
				HelpMarkerWithoutQuestion("The required building level. The building has to be of that level in order for the unit to be able to be produced");

				g_pCurrentEditedPrefab->building_level = build;
			}
		}
	}
}

void GameEditor::DisplayUnitEditorGoldCostEdit()
{
	if (ImGui::CollapsingHeader("Gold Cost"))
	{
		ImGuiID gold_id = g_idUnitEditorElementID + 16;
		ImGuiID gold_scalar_id = g_idUnitEditorElementID + 17;

		if (g_pCurrentEditedPrefab)
		{
			int gold = g_pCurrentEditedPrefab->gold_cost;
			ImGui::PushID(gold_id);
			ImGui::SliderInt("Int", &gold, 1, 1000, "%d");
			ImGui::PopID();
			HelpMarkerWithoutQuestion("The required gold to produce this unit/building. The player has to spend the specified amount of gold in-game to create this unit/building");
			ImGui::SameLine();
			ImGui::PushID(gold_scalar_id);
			ImGui::InputScalar("ScalarInt", ImGuiDataType_U32, &gold, &u32_one);
			ImGui::PopID();
			HelpMarkerWithoutQuestion("The required gold to produce this unit/building. The player has to spend the specified amount of gold in-game to create this unit/building");

			g_pCurrentEditedPrefab->gold_cost = gold;
		}
	}
}

void GameEditor::DisplayUnitEditorStartingStatusEdit()
{
	ImGuiID input_text_id = g_idUnitEditorElementID + 300;
	ImGuiID input_text_ok_id = g_idUnitEditorElementID + 301;
	ImGuiID remove_button_id = g_idUnitEditorElementID + 9001;
	std::string to_be_removed;
	
	if (ImGui::CollapsingHeader("Starting Status"))
	{
		// Show all already added starting statuses.
		if(g_pCurrentEditedPrefab)
		{
			// Assume there are not duplicate statuses.
			int i = 0;
			for (auto& status : g_pCurrentEditedPrefab->starting_status_vec)
			{
				ImGui::BulletText(status.c_str());
				ImGui::SameLine();
				ImGui::PushID(remove_button_id + i);
				if (ImGui::SmallButton("x"))
				{
					to_be_removed = status;
				}
				ImGui::PopID();
				i++;
			}
		}

		if (to_be_removed.size() > 0)
		{
			auto it = std::find(g_pCurrentEditedPrefab->starting_status_vec.begin(), g_pCurrentEditedPrefab->starting_status_vec.end(), to_be_removed);
			g_pCurrentEditedPrefab->starting_status_vec.erase(it);
		}

		ImGui::PushID(input_text_id);
		ImGui::InputText("|", g_cPrefabStartingStatusName, sizeof(g_cPrefabStartingStatusName));
		ImGui::PopID();
		HelpMarkerWithoutQuestion("Add a starting status to the unit. The unit will start with the given status and the name must match an existing status name in DB. A status can be e.g. \"Poison\" or \"Levitate\"");
		ImGui::SameLine();
		ImGui::PushID(input_text_ok_id);
		if (ImGui::SmallButton("OK"))
		{
			// Check for sanity.
			std::string name = std::string(g_cPrefabStartingStatusName);

			auto length = name.size() > 0;
			auto duplicate = false;
			
			if (length && g_pCurrentEditedPrefab != nullptr)
			{

				duplicate = std::find(g_pCurrentEditedPrefab->starting_status_vec.begin(), g_pCurrentEditedPrefab->starting_status_vec.end(), name) != g_pCurrentEditedPrefab->starting_status_vec.end();

				if(!duplicate)
				{
					LOG_DBG_INFO("[{:.4f}][DisplayUnitEditorStartingStatusEdit] Add starting status \"{}\"!", APP_RUN_TIME, name);
					LOG_FILE_INFO("[{:.4f}][DisplayUnitEditorStartingStatusEdit] Add starting status \"{}\"!", APP_RUN_TIME, name);

					// Set the layout template name for the prefab.
					g_pCurrentEditedPrefab->starting_status_vec.push_back(name);
				}

				memset(&g_cPrefabStartingStatusName, 0, sizeof(g_cPrefabStartingStatusName));
			}
			
			// Error messages.
			if (!length)
			{
				LOG_DBG_ERROR("[{:.4f}][DisplayUnitEditorStartingStatusEdit] Could not add starting status: Empty name!", APP_RUN_TIME);
				LOG_FILE_ERROR("[{:.4f}][DisplayUnitEditorStartingStatusEdit] Could not add starting status: Empty name!", APP_RUN_TIME);
			}
			else if(duplicate)
			{
				LOG_DBG_ERROR("[{:.4f}][DisplayUnitEditorStartingStatusEdit] Could not add starting status: Duplicate!", APP_RUN_TIME);
				LOG_FILE_ERROR("[{:.4f}][DisplayUnitEditorStartingStatusEdit] Could not add starting status: Duplicate!", APP_RUN_TIME);
			}
			else if (!g_pCurrentEditedPrefab)
			{
				LOG_DBG_ERROR("[{:.4f}][DisplayUnitEditorStartingStatusEdit] Could not add starting status: No prefab in work. Hit the \"New\" button Sherlock!", APP_RUN_TIME);
				LOG_FILE_ERROR("[{:.4f}][DisplayUnitEditorStartingStatusEdit] Could not add starting status: No prefab in work. Hit the \"New\" button Sherlock!", APP_RUN_TIME);
			}
		}
		ImGui::PopID();
	}
}

void GameEditor::DisplayUnitEditorAbilitiesEdit()
{
	ImGuiID input_text_id = g_idUnitEditorElementID + 400;
	ImGuiID input_text_ok_id = g_idUnitEditorElementID + 401;
	ImGuiID remove_button_id = g_idUnitEditorElementID + 9001;
	std::string to_be_removed;

	if (ImGui::CollapsingHeader("Abilities"))
	{
		// Show all already added abilities.
		if (g_pCurrentEditedPrefab)
		{
			// Assume there are not duplicate abilities.
			int i = 0;
			for (auto& abl : g_pCurrentEditedPrefab->abilities_vec)
			{
				ImGui::BulletText(abl.c_str());
				ImGui::SameLine();
				ImGui::PushID(remove_button_id + i);
				if(ImGui::SmallButton("x"))
				{
					to_be_removed = abl;
				}
				ImGui::PopID();
				i++;
			}
		}

		if(to_be_removed.size() > 0)
		{
			auto it = std::find(g_pCurrentEditedPrefab->abilities_vec.begin(), g_pCurrentEditedPrefab->abilities_vec.end(), to_be_removed);
			g_pCurrentEditedPrefab->abilities_vec.erase(it);
		}


		ImGui::PushID(input_text_id);
		ImGui::InputText("|", g_cPrefabAbilityName, sizeof(g_cPrefabAbilityName));
		ImGui::PopID();
		HelpMarkerWithoutQuestion("Add an ability to the unit. The unit will be able to execute the ability and it has to match the name of the ability in-game. An ability can be e.g. \"Heal\" or \"AttackMeele\"");
		ImGui::SameLine();
		ImGui::PushID(input_text_ok_id);
		if (ImGui::SmallButton("OK"))
		{
			// Check for sanity.
			std::string name = std::string(g_cPrefabAbilityName);

			auto length = name.size() > 0;
			auto duplicate = false;

			if (length && g_pCurrentEditedPrefab != nullptr)
			{
				duplicate = std::find(g_pCurrentEditedPrefab->abilities_vec.begin(), g_pCurrentEditedPrefab->abilities_vec.end(), name) != g_pCurrentEditedPrefab->abilities_vec.end();

				if(!duplicate)
				{
					LOG_DBG_INFO("[{:.4f}][DisplayUnitEditorAbilitiesEdit] Add ability \"{}\"!", APP_RUN_TIME, name);
					LOG_FILE_INFO("[{:.4f}][DisplayUnitEditorAbilitiesEdit] Add ability \"{}\"!", APP_RUN_TIME, name);

					// Set the layout template name for the prefab.
					g_pCurrentEditedPrefab->abilities_vec.push_back(name);
				}

				memset(&g_cPrefabAbilityName, 0, sizeof(g_cPrefabAbilityName));
			}
			
			// Error messages.
			if (!length)
			{
				LOG_DBG_ERROR("[{:.4f}][DisplayUnitEditorAbilitiesEdit] Could not add ability: Empty name!", APP_RUN_TIME);
				LOG_FILE_ERROR("[{:.4f}][DisplayUnitEditorAbilitiesEdit] Could not add ability: Empty name!", APP_RUN_TIME);
			}
			else if (duplicate)
			{
				LOG_DBG_ERROR("[{:.4f}][DisplayUnitEditorAbilitiesEdit] Could not add starting status: Duplicate!", APP_RUN_TIME);
				LOG_FILE_ERROR("[{:.4f}][DisplayUnitEditorAbilitiesEdit] Could not add starting status: Duplicate!", APP_RUN_TIME);
			}
			else if (!g_pCurrentEditedPrefab)
			{
				LOG_DBG_ERROR("[{:.4f}][DisplayUnitEditorAbilitiesEdit] Could not add ability: No prefab in work. Hit the \"New\" button Sherlock!", APP_RUN_TIME);
				LOG_FILE_ERROR("[{:.4f}][DisplayUnitEditorAbilitiesEdit] Could not add ability: No prefab in work. Hit the \"New\" button Sherlock!", APP_RUN_TIME);
			}
		}
		ImGui::PopID();
	}
}

void GameEditor::DisplayUnitEditorUnitSpriteEdit()
{
	// Display Drop-Down button to select a sprite.
	std::vector< std::string > unit_sprite_vec; // TODO Doing this every time is costly...

	// Find our current item index in the vector.
	int current_item_index = 0;
	int index = 0;
	bool changed = false;
	for (auto& pair : m_unitDecalDatabase)
	{
		unit_sprite_vec.push_back(pair.first);
	}
	for (int i = 0; i < unit_sprite_vec.size(); i++)
	{
		if (unit_sprite_vec[i].compare(g_sUnitEditorCurrentUnitSprite) == 0)
		{
			current_item_index = i;
			break;
		}
	}

	// Create the Combo window.
	if (ImGui::BeginCombo("Unit Sprite", g_sUnitEditorCurrentUnitSprite.c_str()))
	{
		for (int i = 0; i < unit_sprite_vec.size(); i++)
		{
			const bool is_selected = (current_item_index == i);

			if (ImGui::Selectable(unit_sprite_vec[i].c_str(), is_selected))
			{
				current_item_index = i;
				changed = true;
			}

			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	HelpMarkerWithoutQuestion("Change the sprite of the unit. This is the main representation of the unit in-game, without additional indicators and banner");

	// Apply the change.
	if (changed)
	{
		if(g_pCurrentEditedPrefab)
		{
			g_sUnitEditorCurrentUnitSprite = unit_sprite_vec[current_item_index];
			g_pCurrentEditedPrefab->sprite = unit_sprite_vec[current_item_index];

			LOG_DBG_INFO("[{:.4f}][DisplayUnitEditorUnitSpriteEdit] Unit Sprite changed to \"{}\"", APP_RUN_TIME, unit_sprite_vec[current_item_index]);
			LOG_FILE_INFO("[{:.4f}][DisplayUnitEditorUnitSpriteEdit] Unit Sprite changed to \"{}\"", APP_RUN_TIME, unit_sprite_vec[current_item_index]);
		}
		else
		{
			LOG_DBG_ERROR("[{:.4f}][DisplayUnitEditorUnitSpriteEdit] Could not change sprite: No prefab in work. Hit the \"New\" button Sherlock!", APP_RUN_TIME);
			LOG_FILE_ERROR("[{:.4f}][DisplayUnitEditorUnitSpriteEdit] Could not change sprite: No prefab in work. Hit the \"New\" button Sherlock!", APP_RUN_TIME);
		}
	}
}

void GameEditor::DisplayPrefabEditorBuildingSpriteEdit()
{
	// Display Drop-Down button to select a sprite.
	std::vector< std::string > building_sprite_vec; // TODO Doing this every time is costly...

	// Find our current item index in the vector.
	int current_item_index = 0;
	int index = 0;
	bool changed = false;
	for (auto& pair : m_structureDecalDatabase)
	{
		building_sprite_vec.push_back(pair.first);
	}
	for (int i = 0; i < building_sprite_vec.size(); i++)
	{
		if (building_sprite_vec[i].compare(g_sUnitEditorCurrentUnitSprite) == 0)
		{
			current_item_index = i;
			break;
		}
	}

	// Create the Combo window.
	if (ImGui::BeginCombo("Building Sprite", g_sUnitEditorCurrentUnitSprite.c_str()))
	{
		for (int i = 0; i < building_sprite_vec.size(); i++)
		{
			const bool is_selected = (current_item_index == i);

			if (ImGui::Selectable(building_sprite_vec[i].c_str(), is_selected))
			{
				current_item_index = i;
				changed = true;
			}

			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	HelpMarkerWithoutQuestion("Change the sprite of the building. This is the main representation of the building in-game, without additional indicators etc.");

	// Apply the change.
	if (changed)
	{
		if (g_pCurrentEditedPrefab)
		{
			g_sUnitEditorCurrentUnitSprite = building_sprite_vec[current_item_index];
			g_pCurrentEditedPrefab->sprite = building_sprite_vec[current_item_index];

			LOG_DBG_INFO("[{:.4f}][DisplayUnitEditorUnitSpriteEdit] Unit Sprite changed to \"{}\"", APP_RUN_TIME, building_sprite_vec[current_item_index]);
			LOG_FILE_INFO("[{:.4f}][DisplayUnitEditorUnitSpriteEdit] Unit Sprite changed to \"{}\"", APP_RUN_TIME, building_sprite_vec[current_item_index]);
		}
		else
		{
			LOG_DBG_ERROR("[{:.4f}][DisplayUnitEditorUnitSpriteEdit] Could not change sprite: No prefab in work. Hit the \"New\" button Sherlock!", APP_RUN_TIME);
			LOG_FILE_ERROR("[{:.4f}][DisplayUnitEditorUnitSpriteEdit] Could not change sprite: No prefab in work. Hit the \"New\" button Sherlock!", APP_RUN_TIME);
		}
	}
}

void GameEditor::DisplayStatusEffectEditor()
{
	std::string name = "Status Effect Editor";
	auto scene_edit_window_width = 600.f;
	auto status_effect_preview_window_width = 700.0f;
	auto status_effect_element_window_width = 300.f;
	auto main_window_height = DEFAULT_WINDOW_HEIGHT - 50.0f;

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration;

	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(ImVec2(scene_edit_window_width + status_effect_preview_window_width + status_effect_element_window_width + 35.0f, main_window_height), ImGuiCond_Appearing);
	ImGui::Begin(name.c_str(), &g_bStatusEffectEditorOpen, flags);

	// Display all editing windows.
	ImGui::BeginChild("Stats", ImVec2(scene_edit_window_width, main_window_height - 1.0f), true);

	DisplayStatusEffectEditorMainMenu();
	
	if(g_pCurrentEditedStatusEffect)
	{
		g_pCurrentEditedStatusEffect->sprite = g_sStatusEffectEditorCurrentIconSprite;

		DisplayEditFieldString(g_cStatusEffectName, sizeof(g_cStatusEffectName), g_pCurrentEditedStatusEffect->name, "Name", "Change the internal name of the Status Effect. Follow the convention \"SE_Default_Poison\" or \"SE_Default_Creature_Summoned\"; where \"Default\" indicates the status to be race agnostic and the standard accessible to a class of units", 10000, 10001);
		DisplayEditFieldString(g_cStatusEffectDisplayName, sizeof(g_cStatusEffectDisplayName), g_pCurrentEditedStatusEffect->displayName, "Display Name", "Change the Status Effect name that the player will see in-game", 10002, 10003);
		DisplayEditFieldString(g_cStatusEffectDescription, sizeof(g_cStatusEffectDescription), g_pCurrentEditedStatusEffect->description, "Description", "Change the Status Effect description that the player will read in-game to understand what it is", 10004, 10005);
		DisplayEditFieldString(g_cStatusEffectBehaviorTreeImplName, sizeof(g_cStatusEffectBehaviorTreeImplName), g_pCurrentEditedStatusEffect->behaviorTreeImplName, "Behavior Tree Impl Name", "Set the BT for the Status Effect. Follow the convention \"BT_Impl_\" and the implementation chosen from: \"Testing_Tree\", \"Turn_Based\", \"Persistent\" or \"Reoccurring_[YOUR IMPL NAME]\"", 10006, 10007);
		DisplayEditFieldString(g_cStatusEffectTimerType, sizeof(g_cStatusEffectTimerType), g_pCurrentEditedStatusEffect->timerType, "Timer Type", "Change the type of the timer. Choose from \"Turn\", \"Persistent\", \"Reoccurring\"", 10008, 10009);
		DisplayEditFieldNumber(g_pCurrentEditedStatusEffect->timerValue, 1, 100, "Timer Value", "Change the value of the timer. Based on context it means: \"Turn\"=How many turns the Effect stays applied until it removes itself. \"Persistent\"=Ignore the value. \"Reoccurring\"=If effect was removed, specify how many turns the condition for the effect has to be invalid, before it will be re-applied again", 10018, 10019);
		DisplayEditFieldNumber(g_pCurrentEditedStatusEffect->applicableTo, 0, 3, "Applicable To", "Change on which Entity Type in-game the Status Effect is applicable. 0=Maptile, 1=Mapobject, 2=Unit, 3=Building", 10010, 10011);
		DisplayEditFieldNumber(g_pCurrentEditedStatusEffect->applicationProbability, 1, 100, "Application Prob", "Change the probability with which the Status Effect will be applied on casting", 10012, 10013);
		DisplayEditFieldNumber(g_pCurrentEditedStatusEffect->minValue, 1, 100, "Min Value", "Change the minimal value that is applied by this Status Effect. Based on context this can be healing or damaging etc.", 10014, 10015);
		DisplayEditFieldNumber(g_pCurrentEditedStatusEffect->maxValue, 1, 100, "Max Value", "Change the maximal value that is applied by this Status Effect. Based on context this can be healing or damaging etc.", 10016, 10017);
		DisplayEditFieldIcon("Icon Field", "Change the Status Effect in-game icon. It will be displayed on the side of a unit and potentially as stand-alone image", m_statusEffectDecalDatabase, g_sStatusEffectEditorCurrentIconSprite);
	}
	ImGui::EndChild();

	DisplayStatusEffectEditorIconSprite();

	ImGui::End();
}

void GameEditor::DisplayStatusEffectEditorMainMenu()
{
	auto quick_save = g_sCurrentEditedStatusEffectFilePath.compare("none") != 0 && g_pCurrentEditedStatusEffect != nullptr;

	if (ImGui::SmallButton("New"))
	{
		if(g_pCurrentEditedStatusEffect)
		{
			delete g_pCurrentEditedStatusEffect;
			g_pCurrentEditedStatusEffect = nullptr;
		}

		g_pCurrentEditedStatusEffect = new SStatusEffect();

		g_sStatusEffectEditorCurrentIconSprite = "none";
		g_sCurrentEditedStatusEffectFilePath = "none";
	}
	if (quick_save)
	{
		ImGui::SameLine();
		if (ImGui::SmallButton("Quick Save..."))
		{
			ExportStatusEffect(g_sCurrentEditedStatusEffectFilePath, g_pCurrentEditedStatusEffect);
		}
	}
	ImGui::SameLine();
	if (ImGui::SmallButton("Save As..."))
	{
		g_bExportingStatusEffect = true;
		g_pExportedStatusEffect = g_pCurrentEditedStatusEffect;
	}
	ImGui::SameLine();
	if (ImGui::SmallButton("Load From..."))
	{
		g_bImportingStatusEffect = true;
	}



	ImGui::SameLine();
	ImGui::SetCursorPosX(ImGui::GetWindowSize().x - 25.0f);
	if (ImGui::SmallButton("X"))
	{
		ToggleMenuItem(g_bStatusEffectEditorOpen);
	}
	ImGui::Separator();
	ImGui::Separator();
}

void GameEditor::DisplayStatusEffectEditorIconSprite()
{
	// Do not render if we have not selected still.
	if (g_sStatusEffectEditorCurrentIconSprite.compare("none") == 0 ||
		g_sStatusEffectEditorCurrentIconSprite.size() == 0)
	{
		return;
	}

	float sprite_width = m_decalSizeDatabase[g_sStatusEffectEditorCurrentIconSprite].first;
	float sprite_height = m_decalSizeDatabase[g_sStatusEffectEditorCurrentIconSprite].second;

	auto wnd_size = ImGui::GetWindowSize();
	auto position = ImVec2((wnd_size.x - sprite_width) * 0.5f, (wnd_size.y - sprite_height) * 0.25f);
	ImGui::SetCursorPos(position);
	ImGui::Image((ImTextureID)m_decalDatabase[g_sStatusEffectEditorCurrentIconSprite]->id, ImVec2(sprite_width, sprite_height));
}

void GameEditor::DisplayStatusEffectExportWindow(SStatusEffect* se)
{
	static char se_filepath_name[64] = "";
	if (se)
	{
		ImGui::SetNextWindowPos(ImVec2(ScreenWidth() / 2.0f - ScreenWidth() / 4.0f, ScreenHeight() / 2.0f - ScreenHeight() / 4.0f), ImGuiCond_Appearing);
		ImGui::SetNextWindowSize(ImVec2(500, 250), ImGuiCond_Appearing);

		ImGui::Begin("Status Effect Export", &g_bExportingStatusEffect);
		ImGui::InputText("|", se_filepath_name, 64);
		HelpMarkerWithoutQuestion("Name of the file that will be created. The file will be saved in \"assets/TilesetData/StatusEffect\". It is not necessary to add the XML extension");
		ImGui::SameLine();
		if (ImGui::SmallButton("OK"))
		{
			// Check for sanity.
			std::string name = std::string(se_filepath_name);

			bool length = name.length() > 0;
			bool result = false;
			if (length)
			{
				result = ExportStatusEffect(name, se);
			}

			if (!length)
			{
				LOG_DBG_ERROR("[{:.4f}][DisplayStatusEffectExportWindow] Error exporting Status Effect \"{}\": Filepath has 0 length!", APP_RUN_TIME, se->name);
				LOG_FILE_ERROR("[{:.4f}][DisplayStatusEffectExportWindow]  Error exporting Status Effect \"{}\": Filepath has 0 length!", APP_RUN_TIME, se->name);
			}
			if (result)
			{
				LOG_DBG_INFO("[{:.4f}][DisplayStatusEffectExportWindow] Success exporting Status Effect \"{}\"!", APP_RUN_TIME, se->name);
				LOG_FILE_INFO("[{:.4f}][DisplayStatusEffectExportWindow]  Success exporting Status Effect \"{}\"!", APP_RUN_TIME, se->name);

				ImportStatusEffectCache(g_sStatusEffectCacheFilepath);
			}
			else
			{
				LOG_DBG_ERROR("[{:.4f}][DisplayUnitPrefabExportWindow] Error exporting Status Effect \"{}\"!", APP_RUN_TIME, se->name);
				LOG_FILE_ERROR("[{:.4f}][DisplayUnitPrefabExportWindow]  Error exporting Status Effect \"{}\"!", APP_RUN_TIME, se->name);
			}

			memset(&se_filepath_name, 0, sizeof(se_filepath_name));
			g_bExportingStatusEffect = false;
			g_pExportedStatusEffect = nullptr;
		}

		ImGui::End();
	}
}

void GameEditor::DisplayStatusEffectImportWindow()
{
	static char se_import_filepath_name[256] = "";

	ImGui::SetNextWindowPos(ImVec2(ScreenWidth() / 2.0f - ScreenWidth() / 4.0f, ScreenHeight() / 2.0f - ScreenHeight() / 4.0f), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(ImVec2(500, 250), ImGuiCond_Appearing);

	ImGui::Begin("Status Effect Import", &g_bImportingStatusEffect);
	ImGui::InputText("|", se_import_filepath_name, 64);
	HelpMarkerWithoutQuestion("Name of the file that will be imported. The file will be loaded from \"assets/TilesetData/StatusEffect\". It is not necessary to add the XML extension");
	ImGui::SameLine();
	if (ImGui::SmallButton("OK"))
	{
		// Check for sanity.
		std::string name = std::string(se_import_filepath_name);

		bool length = name.length() > 0;
		bool result = false;
		if (length)
		{
			g_pCurrentEditedStatusEffect = ImportStatusEffect(name);
			result = (g_pCurrentEditedStatusEffect != nullptr);
		}

		if (!length)
		{
			LOG_DBG_ERROR("[{:.4f}][DisplayStatusEffectImportWindow] Error importing Status Effect: Filepath has 0 length!", APP_RUN_TIME);
			LOG_FILE_ERROR("[{:.4f}][DisplayStatusEffectImportWindow]  Error importing Status Effect: Filepath has 0 length!", APP_RUN_TIME);
		}
		if (result)
		{
			g_sStatusEffectEditorCurrentIconSprite = g_pCurrentEditedStatusEffect->sprite;
			g_sCurrentEditedStatusEffectFilePath = name;

			LOG_DBG_INFO("[{:.4f}][DisplayStatusEffectImportWindow] Success importing Status Effect \"{}\"!", APP_RUN_TIME, g_pCurrentEditedStatusEffect->name);
			LOG_FILE_INFO("[{:.4f}][DisplayStatusEffectImportWindow]  Success importing Status Effect \"{}\"!", APP_RUN_TIME, g_pCurrentEditedStatusEffect->name);
		}
		else
		{
			LOG_DBG_ERROR("[{:.4f}][DisplayStatusEffectImportWindow] Error importing Status Effect \"{}\"!", APP_RUN_TIME, name);
			LOG_FILE_ERROR("[{:.4f}][DisplayStatusEffectImportWindow]  Error importing Status Effect \"{}\"!", APP_RUN_TIME, name);
		}

		memset(&se_import_filepath_name, 0, sizeof(se_import_filepath_name));
		g_bImportingStatusEffect = false;
	}

	DisplayStatusEffectQuickLoadDropDown();

	ImGui::End();
}

void GameEditor::DisplayStatusEffectQuickLoadDropDown()
{
	auto open = ImGui::CollapsingHeader("Quick Load");
	HelpMarkerWithoutQuestion("Select a Status Effect to load from all available in the Status Effect Cache. The Status Effect Cache consists of all saved Status Effects");

	if (open)
	{
		for (auto& se : m_statusEffectCacheMap)
		{
			if (ImGui::Button(se.first.c_str()))
			{
				g_pCurrentEditedStatusEffect = ImportStatusEffect(se.first);
				if (g_pCurrentEditedStatusEffect)
				{
					g_sStatusEffectEditorCurrentIconSprite = g_pCurrentEditedStatusEffect->sprite;
					g_bImportingStatusEffect = false;
					g_sCurrentEditedStatusEffectFilePath = se.first;
				}
				return;
			}
		}
	}
}

bool GameEditor::ImportStatusEffectCache(const std::string& filepath)
{
	tinyxml2::XMLDocument cache;
	if (cache.LoadFile(filepath.c_str()) != tinyxml2::XMLError::XML_SUCCESS)
	{
		LOG_DBG_ERROR("[{:.4f}][ImportStatusEffectCache] Could not load Status Effect Cache: \"{}\"!", APP_RUN_TIME, filepath);
		LOG_FILE_ERROR("[{:.4f}][ImportStatusEffectCache] Could not load Status Effect Cache: \"{}\"!", APP_RUN_TIME, filepath);
		cache.Clear();
		return false;
	}

	auto root = cache.RootElement();
	auto se = root->FirstChildElement("StatusEffect");
	while (se)
	{
		m_statusEffectCacheMap.emplace(se->Attribute("path"), se->Attribute("name"));

		se = se->NextSiblingElement("StatusEffect");
	}



	LOG_DBG_INFO("[{:.4f}][ImportStatusEffectCache] Status Effect Cache loaded: \"{}\" with size: \"{}\"!", APP_RUN_TIME, filepath, m_statusEffectCacheMap.size());
	LOG_FILE_INFO("[{:.4f}][ImportStatusEffectCache] Status Effect Cache loaded: \"{}\" with size: \"{}\"!", APP_RUN_TIME, filepath, m_statusEffectCacheMap.size());

	m_statusEffectCacheLoaded = true;

	return true;
}

bool GameEditor::ExportStatusEffect(const std::string& filepath, SStatusEffect* se)
{
	tinyxml2::XMLDocument doc;
	auto xmlRoot = doc.NewElement("StatusEffect");
	doc.InsertEndChild(xmlRoot);

	auto data = xmlRoot->InsertNewChildElement("Data");

	// Export default data.
	data->SetAttribute("name", se->name.c_str());
	data->SetAttribute("displayName", se->displayName.c_str());
	data->SetAttribute("description", se->description.c_str());
	data->SetAttribute("behaviorTreeImplName", se->behaviorTreeImplName.c_str());
	data->SetAttribute("timerType", se->timerType.c_str());
	data->SetAttribute("timerValue", se->timerValue);
	data->SetAttribute("sprite", se->sprite.c_str());
	data->SetAttribute("applicableTo", se->applicableTo); 
	data->SetAttribute("applicationProbability", se->applicationProbability); 
	data->SetAttribute("minValue", se->minValue); 
	data->SetAttribute("maxValue", se->maxValue);


	std::string path = "assets/TilesetData/StatusEffect/" + filepath + ".xml";
	if (doc.SaveFile(path.c_str()) != tinyxml2::XMLError::XML_SUCCESS)
	{
		LOG_DBG_ERROR("[{:.4f}][ExportStatusEffect] Could not save Status Effect: \"{}\"!", APP_RUN_TIME, path);
		LOG_FILE_ERROR("[{:.4f}][ExportStatusEffect] Could not save Status Effect: \"{}\"!", APP_RUN_TIME, path);
		doc.Clear();
		return false;
	}

	LOG_DBG_INFO("[{:.4f}][ExportStatusEffect] Exported Status Effect: \"{}\" as \"{}\"!", APP_RUN_TIME, se->name, path);
	LOG_FILE_INFO("[{:.4f}][ExportStatusEffect] Exported Status Effect: \"{}\" as \"{}\"!", APP_RUN_TIME, se->name, path);



	// Store the SE in the SE Cache
	// Load.
	std::string cache_path = "assets/TilesetData/StatusEffect/StrategyEditor_StatusEffectCache.xml";
	tinyxml2::XMLDocument cache;
	if (cache.LoadFile(cache_path.c_str()) != tinyxml2::XMLError::XML_SUCCESS)
	{
		LOG_DBG_ERROR("[{:.4f}][ExportStatusEffect] Could not load Prefab Cache: \"{}\"!", APP_RUN_TIME, cache_path);
		LOG_FILE_ERROR("[{:.4f}][ExportStatusEffect] Could not load Prefab Cache: \"{}\"!", APP_RUN_TIME, cache_path);
		cache.Clear();
		return false;
	}

	// Add the new Prefab Element.
	auto cache_root = cache.RootElement();
	auto prefab_element = cache_root->InsertNewChildElement("StatusEffect");
	prefab_element->SetAttribute("name", se->name.c_str());
	prefab_element->SetAttribute("path", filepath.c_str());

	// Unload.
	if (cache.SaveFile(cache_path.c_str()) != tinyxml2::XMLError::XML_SUCCESS)
	{
		LOG_DBG_ERROR("[{:.4f}][ExportStatusEffect] Could not save Status Effect in Prefab Cache: \"{}\" as \"{}\"!", APP_RUN_TIME, se->name, filepath);
		LOG_FILE_ERROR("[{:.4f}][ExportStatusEffect] Could not save Status Effect in Prefab Cache: \"{}\" as \"{}\"!", APP_RUN_TIME, se->name, filepath);
		cache.Clear();
		return false;
	}

	LOG_DBG_INFO("[{:.4f}][ExportStatusEffect] Saved Status Effect in Prefab Cache: \"{}\" as \"{}\"!", APP_RUN_TIME, se->name, filepath);
	LOG_FILE_INFO("[{:.4f}][ExportStatusEffect] Saved Status Effect in Prefab Cache: \"{}\" as \"{}\"!", APP_RUN_TIME, se->name, filepath);


	return true;
}

SStatusEffect* GameEditor::ImportStatusEffect(const std::string& filepath)
{
	std::string path = "assets/TilesetData/StatusEffect/" + filepath + ".xml";

	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(path.c_str()) != tinyxml2::XMLError::XML_SUCCESS)
	{
		LOG_DBG_ERROR("[{:.4f}][ImportStatusEffect] Could not load file: \"{}\"!", APP_RUN_TIME, path);
		LOG_FILE_ERROR("[{:.4f}][ImportStatusEffect] Could not load file: \"{}\"!", APP_RUN_TIME, path);
		doc.Clear();
		return nullptr;
	}

	SStatusEffect* se = new SStatusEffect();

	auto xmlRoot = doc.RootElement();
	auto data = xmlRoot->FirstChildElement("Data");

	if (se)
	{
		se->name = data->Attribute("name");
		se->displayName = data->Attribute("displayName");
		se->description = data->Attribute("description");
		se->behaviorTreeImplName = data->Attribute("behaviorTreeImplName");
		se->timerType = data->Attribute("timerType");
		se->timerValue = data->IntAttribute("timerValue");
		se->applicableTo = data->IntAttribute("applicableTo");
		se->applicationProbability = data->IntAttribute("applicationProbability");
		se->minValue = data->IntAttribute("minValue");
		se->maxValue = data->IntAttribute("maxValue");
		se->sprite = data->Attribute("sprite");

		LOG_DBG_INFO("[{:.4f}][ImportStatusEffect] Loaded Status Effect: \"{}\"!", APP_RUN_TIME, path);
		LOG_FILE_INFO("[{:.4f}][ImportStatusEffect] Loaded Status Effect: \"{}\"!", APP_RUN_TIME, path);
	}
	else
	{
		LOG_DBG_ERROR("[{:.4f}][ImportStatusEffect] Could not load Status Effect: \"{}\"!", APP_RUN_TIME, path);
		LOG_FILE_ERROR("[{:.4f}][ImportStatusEffect] Could not load Status Effect: \"{}\"!", APP_RUN_TIME, path);

		delete se;
		se = nullptr;
	}

	return se;
}

void GameEditor::DisplayAbilityEditor()
{
	std::string name = "Ability Editor";
	auto scene_edit_window_width = 600.f;
	auto ability_preview_window_width = 700.0f;
	auto ability_element_window_width = 300.f;
	auto main_window_height = DEFAULT_WINDOW_HEIGHT - 50.0f;

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration;

	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(ImVec2(scene_edit_window_width + ability_preview_window_width + ability_element_window_width + 35.0f, main_window_height), ImGuiCond_Appearing);
	ImGui::Begin(name.c_str(), &g_bAbilityEditorOpen, flags);

	// Display all editing windows.
	ImGui::BeginChild("Stats", ImVec2(scene_edit_window_width, main_window_height - 1.0f), true);

	DisplayAbilityEditorMainMenu();

	if (g_pCurrentEditedAbility)
	{
		DisplayEditFieldString(g_cAbilityName, sizeof(g_cAbilityName), g_pCurrentEditedAbility->name, "Name", "Change the internal name of the Ability; must be unique", 10000, 10001);
		DisplayEditFieldString(g_cAbilityDisplayName, sizeof(g_cAbilityDisplayName), g_pCurrentEditedAbility->displayName, "Display Name", "Change the in-game name, that will be displayed to the player", 10002, 10003);
		DisplayEditFieldString(g_cAbilityDescription, sizeof(g_cAbilityDescription), g_pCurrentEditedAbility->description, "Description", "Change the in-game description of the ability; the player will read it to know what the ability is doing and what it is for", 10004, 10005);
		DisplayEditFieldNumber(g_pCurrentEditedAbility->applicableTo, 0, 3, "Applicable To", "Change the entity type on which the ability can be used: 0=Maptile, 1=Map Object, 2=Unit, 3=Building", 1006, 1007);
		DisplayEditFieldNumber(g_pCurrentEditedAbility->minRange, 0, 100, "Min Range", "The minimal range of the distance that the Ability can be used, where 0 means the position where the caster is standing", 70001, 70002);
		DisplayEditFieldNumber(g_pCurrentEditedAbility->maxRange, 0, 100, "Max Range", "The maximum range of the distance that the Ability can be used, where 0 means the position where the caster is standing", 70003, 70004);
		DisplayEditFieldBoolean(g_pCurrentEditedAbility->triviallyRemovable, "Trivially Removable", "Change whether the ability can be trivially removed by using the \"Resting\" ability", 70007);
		DisplayEditFieldBoolean(g_pCurrentEditedAbility->usableOnSelf, "Usable on self", "Change whether the ability is usable on the unit that is casting the ability itself", 10008);
		DisplayEditFieldBoolean(g_pCurrentEditedAbility->usableOnFriendlies, "Usable on friendlies", "Change whether the ability is usable on the allies of the casting unit", 10009);
		DisplayEditFieldBoolean(g_pCurrentEditedAbility->usableOnEnemies, "Usable on enemies", "Change whether the ability is usable on enemies of the casting unit", 10010);
		DisplayAbilityAppliedStatusEffectsEditorWindow();
	}
	ImGui::EndChild();

	DisplayStatusEffectEditorIconSprite();

	ImGui::End();
}

void GameEditor::DisplayAbilityEditorMainMenu()
{
	auto quick_save = g_sCurrentEditedAbilityFilePath.compare("none") != 0 && g_pCurrentEditedAbility != nullptr;

	if (ImGui::SmallButton("New"))
	{
		if (g_pCurrentEditedAbility)
		{
			delete g_pCurrentEditedAbility;
			g_pCurrentEditedAbility = nullptr;
		}

		g_pCurrentEditedAbility = new SAbility();

		g_sCurrentEditedAbilityFilePath = "none";
	}
	if (quick_save)
	{
		ImGui::SameLine();
		if (ImGui::SmallButton("Quick Save..."))
		{
			ExportAbility(g_sCurrentEditedAbilityFilePath, g_pCurrentEditedAbility);
		}
	}
	ImGui::SameLine();
	if (ImGui::SmallButton("Save As..."))
	{
		g_bExportingAbility = true;
		g_pExportedAbility = g_pCurrentEditedAbility;
	}
	ImGui::SameLine();
	if (ImGui::SmallButton("Load From..."))
	{
		g_bImportingAbility = true;
	}



	ImGui::SameLine();
	ImGui::SetCursorPosX(ImGui::GetWindowSize().x - 25.0f);
	if (ImGui::SmallButton("X"))
	{
		ToggleMenuItem(g_bAbilityEditorOpen);
	}
	ImGui::Separator();
	ImGui::Separator();
}

void GameEditor::DisplayAbilityEditorExportWindow(SAbility* abl)
{
	static char abl_filepath_name[64] = "";
	if (abl)
	{
		ImGui::SetNextWindowPos(ImVec2(ScreenWidth() / 2.0f - ScreenWidth() / 4.0f, ScreenHeight() / 2.0f - ScreenHeight() / 4.0f), ImGuiCond_Appearing);
		ImGui::SetNextWindowSize(ImVec2(500, 250), ImGuiCond_Appearing);

		ImGui::Begin("Ability Export", &g_bExportingAbility);
		ImGui::InputText("|", abl_filepath_name, 64);
		HelpMarkerWithoutQuestion("Name of the file that will be created. The file will be saved in \"assets/TilesetData/Ability\". It is not necessary to add the XML extension");
		ImGui::SameLine();
		if (ImGui::SmallButton("OK"))
		{
			// Check for sanity.
			std::string name = std::string(abl_filepath_name);

			bool length = name.length() > 0;
			bool result = false;
			if (length)
			{
				result = ExportAbility(name, abl);
			}

			if (!length)
			{
				LOG_DBG_ERROR("[{:.4f}][DisplayAbilityEditorExportWindow] Error exporting Status Effect \"{}\": Filepath has 0 length!", APP_RUN_TIME, abl->name);
				LOG_FILE_ERROR("[{:.4f}][DisplayAbilityEditorExportWindow]  Error exporting Status Effect \"{}\": Filepath has 0 length!", APP_RUN_TIME, abl->name);
			}
			if (result)
			{
				LOG_DBG_INFO("[{:.4f}][DisplayAbilityEditorExportWindow] Success exporting Status Effect \"{}\"!", APP_RUN_TIME, abl->name);
				LOG_FILE_INFO("[{:.4f}][DisplayAbilityEditorExportWindow]  Success exporting Status Effect \"{}\"!", APP_RUN_TIME, abl->name);

				ImportAbilityCache(g_sAbilityCacheFilepath);
			}
			else
			{
				LOG_DBG_ERROR("[{:.4f}][DisplayAbilityEditorExportWindow] Error exporting Status Effect \"{}\"!", APP_RUN_TIME, abl->name);
				LOG_FILE_ERROR("[{:.4f}][DisplayAbilityEditorExportWindow]  Error exporting Status Effect \"{}\"!", APP_RUN_TIME, abl->name);
			}

			memset(&abl_filepath_name, 0, sizeof(abl_filepath_name));
			g_bExportingAbility = false;
			g_pExportedAbility = nullptr;
		}

		ImGui::End();
	}
}

void GameEditor::DisplayAbilityEditorImportWindow()
{
	static char abl_import_filepath_name[256] = "";

	ImGui::SetNextWindowPos(ImVec2(ScreenWidth() / 2.0f - ScreenWidth() / 4.0f, ScreenHeight() / 2.0f - ScreenHeight() / 4.0f), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(ImVec2(500, 250), ImGuiCond_Appearing);

	ImGui::Begin("Ability Import", &g_bImportingAbility);
	ImGui::InputText("|", abl_import_filepath_name, 64);
	HelpMarkerWithoutQuestion("Name of the file that will be imported. The file will be loaded from \"assets/TilesetData/Ability\". It is not necessary to add the XML extension");
	ImGui::SameLine();
	if (ImGui::SmallButton("OK"))
	{
		// Check for sanity.
		std::string name = std::string(abl_import_filepath_name);

		bool length = name.length() > 0;
		bool result = false;
		if (length)
		{
			g_pCurrentEditedAbility = ImportAbility(name);
			result = (g_pCurrentEditedAbility != nullptr);
		}

		if (!length)
		{
			LOG_DBG_ERROR("[{:.4f}][DisplayAbilityEditorImportWindow] Error importing Ability: Filepath has 0 length!", APP_RUN_TIME);
			LOG_FILE_ERROR("[{:.4f}][DisplayAbilityEditorImportWindow]  Error importing Ability: Filepath has 0 length!", APP_RUN_TIME);
		}
		if (result)
		{
			g_sCurrentEditedAbilityFilePath = name;

			LOG_DBG_INFO("[{:.4f}][DisplayAbilityEditorImportWindow] Success importing Ability \"{}\"!", APP_RUN_TIME, g_pCurrentEditedStatusEffect->name);
			LOG_FILE_INFO("[{:.4f}][DisplayAbilityEditorImportWindow]  Success importing Ability \"{}\"!", APP_RUN_TIME, g_pCurrentEditedStatusEffect->name);
		}
		else
		{
			LOG_DBG_ERROR("[{:.4f}][DisplayAbilityEditorImportWindow] Error importing Ability \"{}\"!", APP_RUN_TIME, name);
			LOG_FILE_ERROR("[{:.4f}][DisplayAbilityEditorImportWindow]  Error importing Ability \"{}\"!", APP_RUN_TIME, name);
		}

		memset(&abl_import_filepath_name, 0, sizeof(abl_import_filepath_name));
		g_bImportingAbility = false;
	}

	DisplayAbilityEditorQuickLoadDropDown();

	ImGui::End();
}

void GameEditor::DisplayAbilityAppliedStatusEffectsEditorWindow()
{
	ImGuiID input_text_id = g_idUnitEditorElementID + 20000;
	ImGuiID input_text_ok_id = g_idUnitEditorElementID + 20001;
	ImGuiID remove_button_id = g_idUnitEditorElementID + 20002;
	std::string to_be_removed;

	if (ImGui::CollapsingHeader("Applied Status Effects on use"))
	{
		if (g_pCurrentEditedAbility)
		{
			int i = 0;
			for (auto& status : g_pCurrentEditedAbility->appliedStatusEffectsOnUse)
			{
				ImGui::BulletText(status.c_str());
				ImGui::SameLine();
				ImGui::PushID(remove_button_id + i);
				if (ImGui::SmallButton("x"))
				{
					to_be_removed = status;
				}
				ImGui::PopID();
				i++;
			}
		}

		if (to_be_removed.size() > 0)
		{
			auto it = std::find(g_pCurrentEditedAbility->appliedStatusEffectsOnUse.begin(), g_pCurrentEditedAbility->appliedStatusEffectsOnUse.end(), to_be_removed);
			g_pCurrentEditedAbility->appliedStatusEffectsOnUse.erase(it);
		}

		ImGui::PushID(input_text_id);
		ImGui::InputText("|", g_cAbilityAppliedStatusEffectName, sizeof(g_cAbilityAppliedStatusEffectName));
		ImGui::PopID();
		HelpMarkerWithoutQuestion("");
		ImGui::SameLine();
		ImGui::PushID(input_text_ok_id);
		if (ImGui::SmallButton("OK"))
		{
			// Check for sanity.
			std::string name = std::string(g_cAbilityAppliedStatusEffectName);

			auto length = name.size() > 0;
			auto duplicate = false;

			if (length && g_pCurrentEditedAbility != nullptr)
			{

				duplicate = std::find(g_pCurrentEditedAbility->appliedStatusEffectsOnUse.begin(), g_pCurrentEditedAbility->appliedStatusEffectsOnUse.end(), name) != g_pCurrentEditedAbility->appliedStatusEffectsOnUse.end();

				if (!duplicate)
				{
					LOG_DBG_INFO("[{:.4f}][DisplayAbilityAppliedStatusEffectsEditorWindow] Add applied starting status \"{}\"!", APP_RUN_TIME, name);
					LOG_FILE_INFO("[{:.4f}][DisplayAbilityAppliedStatusEffectsEditorWindow] Add applied starting status \"{}\"!", APP_RUN_TIME, name);

					// Set the layout template name for the prefab.
					g_pCurrentEditedAbility->appliedStatusEffectsOnUse.push_back(name);
				}

				memset(&g_cAbilityAppliedStatusEffectName, 0, sizeof(g_cAbilityAppliedStatusEffectName));
			}

			// Error messages.
			if (!length)
			{
				LOG_DBG_ERROR("[{:.4f}][DisplayAbilityAppliedStatusEffectsEditorWindow] Could not add applied starting status: Empty name!", APP_RUN_TIME);
				LOG_FILE_ERROR("[{:.4f}][DisplayAbilityAppliedStatusEffectsEditorWindow] Could not add applied starting status: Empty name!", APP_RUN_TIME);
			}
			else if (duplicate)
			{
				LOG_DBG_ERROR("[{:.4f}][DisplayAbilityAppliedStatusEffectsEditorWindow] Could not add applied starting status: Duplicate!", APP_RUN_TIME);
				LOG_FILE_ERROR("[{:.4f}][DisplayAbilityAppliedStatusEffectsEditorWindow] Could not add applied starting status: Duplicate!", APP_RUN_TIME);
			}
			else if (!g_pCurrentEditedAbility)
			{
				LOG_DBG_ERROR("[{:.4f}][DisplayAbilityAppliedStatusEffectsEditorWindow] Could not add applied starting status: No Ability in work. Hit the \"New\" button Sherlock!", APP_RUN_TIME);
				LOG_FILE_ERROR("[{:.4f}][DisplayAbilityAppliedStatusEffectsEditorWindow] Could not add applied starting status: No Ability in work. Hit the \"New\" button Sherlock!", APP_RUN_TIME);
			}
		}
		ImGui::PopID();
	}
}

void GameEditor::DisplayAbilityEditorQuickLoadDropDown()
{
	auto open = ImGui::CollapsingHeader("Quick Load");
	HelpMarkerWithoutQuestion("Select an Ability to load from all available in the Ability Cache. The Ability Cache consists of all saved Ability");

	if (open)
	{
		for (auto& abl : m_abilityCacheMap)
		{
			if (ImGui::Button(abl.first.c_str()))
			{
				g_pCurrentEditedAbility = ImportAbility(abl.first);
				if (g_pCurrentEditedAbility)
				{
					g_bImportingAbility = false;
					g_sCurrentEditedAbilityFilePath = abl.first;
				}
				return;
			}
		}
	}
}

bool GameEditor::ImportAbilityCache(const std::string& filepath)
{
	tinyxml2::XMLDocument cache;
	if (cache.LoadFile(filepath.c_str()) != tinyxml2::XMLError::XML_SUCCESS)
	{
		LOG_DBG_ERROR("[{:.4f}][ImportAbilityCache] Could not load Ability Cache: \"{}\"!", APP_RUN_TIME, filepath);
		LOG_FILE_ERROR("[{:.4f}][ImportAbilityCache] Could not load Ability Cache: \"{}\"!", APP_RUN_TIME, filepath);
		cache.Clear();
		return false;
	}

	auto root = cache.RootElement();
	auto se = root->FirstChildElement("Ability");
	while (se)
	{
		m_abilityCacheMap.emplace(se->Attribute("path"), se->Attribute("name"));

		se = se->NextSiblingElement("Ability");
	}



	LOG_DBG_INFO("[{:.4f}][ImportAbilityCache] Ability Cache loaded: \"{}\" with size: \"{}\"!", APP_RUN_TIME, filepath, m_abilityCacheMap.size());
	LOG_FILE_INFO("[{:.4f}][ImportAbilityCache] Ability Cache loaded: \"{}\" with size: \"{}\"!", APP_RUN_TIME, filepath, m_abilityCacheMap.size());

	m_abilityCacheLoaded = true;

	return true;
}

bool GameEditor::ExportAbility(const std::string& filepath, SAbility* abl)
{
	tinyxml2::XMLDocument doc;
	auto xmlRoot = doc.NewElement("Ability");
	doc.InsertEndChild(xmlRoot);

	auto data = xmlRoot->InsertNewChildElement("Data");

	// Export default data.
	data->SetAttribute("name", abl->name.c_str());
	data->SetAttribute("displayName", abl->displayName.c_str());
	data->SetAttribute("description", abl->description.c_str());
	data->SetAttribute("applicableTo", abl->applicableTo);
	data->SetAttribute("minRange", abl->minRange);
	data->SetAttribute("maxRange", abl->maxRange);
	data->SetAttribute("triviallyRemovable", abl->triviallyRemovable);
	data->SetAttribute("usableOnSelf", abl->usableOnSelf);
	data->SetAttribute("usableOnFriendlies", abl->usableOnFriendlies);
	data->SetAttribute("usableOnEnemies", abl->usableOnEnemies);

	// Export all status effects that will be applied on use.
	auto applied_se = data->InsertNewChildElement("AppliedStatusEffectsOnUse");
	for(auto& se: abl->appliedStatusEffectsOnUse)
	{
		auto seXml = applied_se->InsertNewChildElement("StatusEffect");
		seXml->SetAttribute("name", se.c_str());
	}


	std::string path = "assets/TilesetData/Ability/" + filepath + ".xml";
	if (doc.SaveFile(path.c_str()) != tinyxml2::XMLError::XML_SUCCESS)
	{
		LOG_DBG_ERROR("[{:.4f}][ExportAbility] Could not save Ability: \"{}\"!", APP_RUN_TIME, path);
		LOG_FILE_ERROR("[{:.4f}][ExportAbility] Could not save Ability: \"{}\"!", APP_RUN_TIME, path);
		doc.Clear();
		return false;
	}

	LOG_DBG_INFO("[{:.4f}][ExportAbility] Exported Ability: \"{}\" as \"{}\"!", APP_RUN_TIME, abl->name, path);
	LOG_FILE_INFO("[{:.4f}][ExportAbility] Exported Ability: \"{}\" as \"{}\"!", APP_RUN_TIME, abl->name, path);



	// Store the Ability in the Ability Cache
	// Load.
	std::string cache_path = "assets/TilesetData/Ability/StrategyEditor_AbilityCache.xml";
	tinyxml2::XMLDocument cache;
	if (cache.LoadFile(cache_path.c_str()) != tinyxml2::XMLError::XML_SUCCESS)
	{
		LOG_DBG_ERROR("[{:.4f}][ExportAbility] Could not load Ability Cache: \"{}\"!", APP_RUN_TIME, cache_path);
		LOG_FILE_ERROR("[{:.4f}][ExportAbility] Could not load Ability Cache: \"{}\"!", APP_RUN_TIME, cache_path);
		cache.Clear();
		return false;
	}

	// Add the new Prefab Element.
	auto cache_root = cache.RootElement();
	auto abl_element = cache_root->InsertNewChildElement("Ability");
	abl_element->SetAttribute("name", abl->name.c_str());
	abl_element->SetAttribute("path", filepath.c_str());

	// Unload.
	if (cache.SaveFile(cache_path.c_str()) != tinyxml2::XMLError::XML_SUCCESS)
	{
		LOG_DBG_ERROR("[{:.4f}][ExportAbility] Could not save Ability in Ability Cache: \"{}\" as \"{}\"!", APP_RUN_TIME, abl->name, filepath);
		LOG_FILE_ERROR("[{:.4f}][ExportAbility] Could not save Status Effect in Prefab Cache: \"{}\" as \"{}\"!", APP_RUN_TIME, abl->name, filepath);
		cache.Clear();
		return false;
	}

	LOG_DBG_INFO("[{:.4f}][ExportAbility] Saved Ability in Ability Cache: \"{}\" as \"{}\"!", APP_RUN_TIME, abl->name, filepath);
	LOG_FILE_INFO("[{:.4f}][ExportAbility] Saved Ability in Ability Cache: \"{}\" as \"{}\"!", APP_RUN_TIME, abl->name, filepath);


	return true;
}

SAbility* GameEditor::ImportAbility(const std::string& filepath)
{
	std::string path = "assets/TilesetData/Ability/" + filepath + ".xml";

	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(path.c_str()) != tinyxml2::XMLError::XML_SUCCESS)
	{
		LOG_DBG_ERROR("[{:.4f}][ImportAbility] Could not load file: \"{}\"!", APP_RUN_TIME, path);
		LOG_FILE_ERROR("[{:.4f}][ImportAbility] Could not load file: \"{}\"!", APP_RUN_TIME, path);
		doc.Clear();
		return nullptr;
	}

	SAbility* abl = new SAbility();

	auto xmlRoot = doc.RootElement();
	auto data = xmlRoot->FirstChildElement("Data");

	if (abl)
	{
		abl->name = data->Attribute("name");
		abl->displayName = data->Attribute("displayName");
		abl->description = data->Attribute("description");
		abl->applicableTo = data->IntAttribute("applicableTo");
		abl->minRange = data->IntAttribute("minRange");
		abl->maxRange = data->IntAttribute("maxRange");
		abl->triviallyRemovable = data->BoolAttribute("triviallyRemovable");
		abl->usableOnSelf = data->BoolAttribute("usableOnSelf");
		abl->usableOnFriendlies = data->BoolAttribute("usableOnFriendlies");
		abl->usableOnEnemies = data->BoolAttribute("usableOnEnemies");

		auto applied_se_xml = data->FirstChildElement("AppliedStatusEffectsOnUse");
		auto seXml = applied_se_xml->FirstChildElement("StatusEffect");
		while(seXml)
		{
			abl->appliedStatusEffectsOnUse.push_back(seXml->Attribute("name"));
			seXml = seXml->NextSiblingElement("StatusEffect");
		}

		LOG_DBG_INFO("[{:.4f}][ImportAbility] Loaded Ability: \"{}\"!", APP_RUN_TIME, path);
		LOG_FILE_INFO("[{:.4f}][ImportAbility] Loaded Ability: \"{}\"!", APP_RUN_TIME, path);
	}
	else
	{
		LOG_DBG_ERROR("[{:.4f}][ImportAbility] Could not load Ability: \"{}\"!", APP_RUN_TIME, path);
		LOG_FILE_ERROR("[{:.4f}][ImportAbility] Could not load Ability: \"{}\"!", APP_RUN_TIME, path);

		delete abl;
		abl = nullptr;
	}

	return abl;
}

void GameEditor::DisplayBuildingEditorNameEdit()
{
	DisplayUnitEditorNameEdit();
}

void GameEditor::DisplayBuildingEditorLayoutTemplateEdit()
{
	DisplayUnitEditorLayoutTemplateNameEdit();
}

void GameEditor::DisplayBuildingEditorDefenseEdit()
{
	// Health.
	DisplayUnitEditorHealthEdit();
	// Defense Bonus for occupier.
	DisplayUnitEditorDefenseEdit();
}

void GameEditor::DisplayBuildingEditorProducerEdit()
{
	// Gold Production.
	ImGuiID button_id = g_idUnitEditorElementID + 30000;
	ImGuiID button_scalar_id = g_idUnitEditorElementID + 30001;

	if (ImGui::CollapsingHeader("Gold Production"))
	{
		if (g_pCurrentEditedPrefab)
		{
			int gold_prod = g_pCurrentEditedPrefab->gold_production;
			ImGui::PushID(button_id);
			ImGui::SliderInt("Int", &gold_prod, 1, 1000, "%d");
			ImGui::PopID();
			HelpMarkerWithoutQuestion("The Gold production of the building in-game");
			ImGui::SameLine();
			ImGui::PushID(button_scalar_id);
			ImGui::InputScalar("ScalarInt", ImGuiDataType_U32, &gold_prod, &u32_one);
			ImGui::PopID();
			HelpMarkerWithoutQuestion("The Gold production of the building in-game");

			g_pCurrentEditedPrefab->gold_production = gold_prod;
		}
	}

	// RP Production.
	if (ImGui::CollapsingHeader("RP Production"))
	{
		if (g_pCurrentEditedPrefab)
		{
			int rp = g_pCurrentEditedPrefab->research_points_production;
			ImGui::PushID(button_id * 2);
			ImGui::SliderInt("Int", &rp, 1, 1000, "%d");
			ImGui::PopID();
			HelpMarkerWithoutQuestion("The Research Points production of the building in-game");
			ImGui::SameLine();
			ImGui::PushID(button_scalar_id * 2);
			ImGui::InputScalar("ScalarInt", ImGuiDataType_U32, &rp, &u32_one);
			ImGui::PopID();
			HelpMarkerWithoutQuestion("The Research Points production of the building in-game");

			g_pCurrentEditedPrefab->research_points_production = rp;
		}
	}
}

void GameEditor::DisplayBuildingEditorVisibilityEdit()
{
	// Tile Range.
	ImGuiID button_id = g_idUnitEditorElementID + 40000;
	ImGuiID button_scalar_id = g_idUnitEditorElementID + 40001;

	if (ImGui::CollapsingHeader("Visibility Distance"))
	{
		if (g_pCurrentEditedPrefab)
		{
			int range = g_pCurrentEditedPrefab->visibility_distance_in_tiles;
			ImGui::PushID(button_id);
			ImGui::SliderInt("Int", &range, 1, 10, "%d");
			ImGui::PopID();
			HelpMarkerWithoutQuestion("The buildings visibility range in tiles");
			ImGui::SameLine();
			ImGui::PushID(button_scalar_id);
			ImGui::InputScalar("ScalarInt", ImGuiDataType_U32, &range, &u32_one);
			ImGui::PopID();
			HelpMarkerWithoutQuestion("The buildings visibility range in tiles");

			g_pCurrentEditedPrefab->visibility_distance_in_tiles = range;
		}
	}
}

void GameEditor::DisplayBuildingEditorRequirementsEdit()
{
	// Gold Cost.
	DisplayUnitEditorGoldCostEdit();


	// Predecessor Building. Required only of level > 1.
	ImGuiID input_text_id = g_idUnitEditorElementID + 2000;
	ImGuiID input_text_ok_id = g_idUnitEditorElementID + 2001;
	if (ImGui::CollapsingHeader("Predecessor Building Name"))
	{
		// Quick exit.
		if (g_pCurrentEditedPrefab)
		{
			if (g_pCurrentEditedPrefab->level <= 1) return;
		}

		ImGui::PushID(input_text_id);
		ImGui::InputText("|", g_cPrefabBuildingPredecessorName, sizeof(g_cPrefabBuildingPredecessorName));
		if (g_pCurrentEditedPrefab)
		{
			HelpMarkerWithoutQuestion(g_pCurrentEditedPrefab->predecessor_building_for_upgrade.c_str());
		}
		ImGui::PopID();

		ImGui::SameLine();
		ImGui::PushID(input_text_ok_id);
		if (ImGui::SmallButton("OK"))
		{
			// Check for sanity.
			std::string name = std::string(g_cPrefabBuildingPredecessorName);

			auto length = name.size() > 0;

			if (length && g_pCurrentEditedPrefab != nullptr)
			{
				LOG_DBG_INFO("[{:.4f}][DisplayBuildingEditorRequirementsEdit] Set Predecessor Building Name to \"{}\"!", APP_RUN_TIME, name);
				LOG_FILE_INFO("[{:.4f}][DisplayBuildingEditorRequirementsEdit] Set Predecessor Building Name to \"{}\"!", APP_RUN_TIME, name);

				// Set the layout template name for the prefab.
				g_pCurrentEditedPrefab->predecessor_building_for_upgrade = name;

				// Reset the Editing char array.
				memset(&g_cPrefabBuildingPredecessorName, 0, sizeof(g_cPrefabBuildingPredecessorName));
			}
			else if (!length)
			{
				LOG_DBG_ERROR("[{:.4f}][DisplayBuildingEditorRequirementsEdit] Could not set Predecessor Building Name: Empty name!", APP_RUN_TIME);
				LOG_FILE_ERROR("[{:.4f}][DisplayBuildingEditorRequirementsEdit] Could not set Predecessor Building Name: Empty name!", APP_RUN_TIME);
			}
			else if (!g_pCurrentEditedPrefab)
			{
				LOG_DBG_ERROR("[{:.4f}][DisplayBuildingEditorRequirementsEdit] Could not set Predecessor Building Name: No prefab in work. Hit the \"New\" button Sherlock!", APP_RUN_TIME);
				LOG_FILE_ERROR("[{:.4f}][DisplayBuildingEditorRequirementsEdit] Could not set Predecessor Building Name: No prefab in work. Hit the \"New\" button Sherlock!", APP_RUN_TIME);
			}
		}
		ImGui::PopID();
		HelpMarkerWithoutQuestion("Set the building (Predecessor Building), which upgraded to the next tier, will become this one. Required only if this buildings level is greater than 1");
	}
}



void GameEditor::DisplayUnitEditorUnitSprite()
{
	// Do not render if we have not selected still.
	if (g_sUnitEditorCurrentUnitSprite.compare("none") == 0)
	{
		return;
	}

	float spite_width = m_decalSizeDatabase[g_sUnitEditorCurrentUnitSprite].first;
	float spite_height = m_decalSizeDatabase[g_sUnitEditorCurrentUnitSprite].second;
	auto wnd_size = ImGui::GetWindowSize();
	auto position = ImVec2( (wnd_size.x - spite_width) * 0.5f, (wnd_size.y - spite_height) * 0.25f);
	ImGui::SetCursorPos(position);
	ImGui::Image((ImTextureID)m_decalDatabase[g_sUnitEditorCurrentUnitSprite]->id, ImVec2(spite_width * 2, spite_height * 2));
}


void GameEditor::DisplayShapesWindow()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	ImGui::SetNextWindowPos(ImVec2(0.0f, 40.0f), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(ImVec2(230.0f, ScreenHeight() - 100.0f), ImGuiCond_Appearing);
	ImGui::Begin("Shapes", &g_bShapeWindowOpen, flags);

	// Show a small menu for Layout Editor.
	if(ImGui::SmallButton("Save As..."))
	{
		g_bSavingPrefabLayoutTemplate = true;
	}
	if(ImGui::SmallButton("Load From..."))
	{
		g_bLoadPrefabLayoutTemplate = true;
	}




	ImGui::RadioButton("None", &g_iCurrentSelectedShape, -1);
	HelpMarkerWithoutQuestion("De-Select any shapes from being placed");

	ImGui::RadioButton("Rectangle", &g_iCurrentSelectedShape, 1);
	HelpMarkerWithoutQuestion("Select a rectangle to be placed as layout element");

	ImGui::End();
}

void GameEditor::DisplayShapePropertiesEditor(SShape* shape)
{
	if (!shape)
	{
		g_bShapePropertyWindowOpen = false;
		return;
	}

	SRectangle* rect = nullptr;


	ImGui::SetNextWindowPos(ImVec2(ScreenWidth() - 600, 25.0f), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(ImVec2(500, 250), ImGuiCond_Appearing);

	ImGui::Begin("Shape Properties", &g_bShapePropertyWindowOpen);

	bool root = shape->IsRootElement();
	ImGui::Checkbox("Root Element", &root);

	if (shape->Type() == 0)
	{
		// Circle Removed.
	}
	else if (shape->Type() == 1)
	{
		rect = reinterpret_cast<SRectangle*>(shape);
		rect->SetAsRoot(root);

		// Display Position
		DisplayShapePosition(rect);
		// Display Dimension
		DisplayShapeDimension(rect);
		// Display Type
		DisplayShapeElementType(rect);
	}
	ImGui::End();
}



void GameEditor::DisplayShapePosition(SRectangle* rect)
{
	int x = rect->x, y = rect->y;
	ImGui::SliderInt("X", &x, 1.0f, 256.0f, "%d", ImGuiSliderFlags_Logarithmic);
	ImGui::SliderInt("Y", &y, 1.0f, 256.0f, "%d", ImGuiSliderFlags_Logarithmic);
	rect->x = x;
	rect->y = y;
}



void GameEditor::DisplayShapeDimension(SRectangle* rect)
{
	int w = rect->w, h = rect->h;
	ImGui::SliderInt("W", &w, 1.0f, 256.0f, "%d", ImGuiSliderFlags_Logarithmic);
	ImGui::SliderInt("H", &h, 1.0f, 256.0f, "%d", ImGuiSliderFlags_Logarithmic);
	rect->w = w;
	rect->h = h;
}

void GameEditor::DisplayShapeElementType(SRectangle* rect)
{
	if (g_PrefabElementTypeVec.size() == 0) g_PrefabElementTypeVec.push_back("none");


	int current_item_index = 0;
	bool changed = false;
	for (int i = 0; i < g_PrefabElementTypeVec.size(); i++)
	{
		if (g_PrefabElementTypeVec[i].compare(rect->ElementType()) == 0)
		{
			current_item_index = i;
			break;
		}
	}

	// Create the Combo window.
	if (ImGui::BeginCombo("Element Type", rect->ElementType().c_str()))
	{
		for (int i = 0; i < g_PrefabElementTypeVec.size(); i++)
		{
			const bool is_selected = (current_item_index == i);

			if (ImGui::Selectable(g_PrefabElementTypeVec[i].c_str(), is_selected))
			{
				current_item_index = i;
				LOG_DBG_INFO("[{:.4f}][DisplayShapeElementType] Changed Prefab Element Type to \"{}\"", APP_RUN_TIME, g_PrefabElementTypeVec[current_item_index]);
				changed = true;
			}

			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	HelpMarkerWithoutQuestion("Change the Element Type of the Prefab Layout Element");

	ImGui::SameLine();
	if (ImGui::SmallButton("+"))
	{
		g_bAddingPrefabElementType = true;
	}
	HelpMarkerWithoutQuestion("Add a new Prefab Element Type");


	// Apply the change.
	if (changed)  rect->element_type = g_PrefabElementTypeVec[current_item_index];
}

SShape* GameEditor::GetShapeAtMousePosition(float x, float y)
{
	for (auto& shape : m_currentLayoutTemplateVec)
	{
		if (shape->Type() == 0)
		{
			// Circle. Removed.
		}
		else if (shape->Type() == 1)
		{
			// Rectangle.
			auto rect = reinterpret_cast<SRectangle*>(shape);
			if (PointRectangleCollision(x, y, rect->x, rect->y, rect->w, rect->h))
			{
				return shape;
			}
		}
	}
	return nullptr;
}

void GameEditor::CreateShapeAtMousePosition(int shape_index, float x, float y)
{
	SShape* shape = nullptr;
	switch (shape_index)
	{
	case 0:
	{
		// Circle removed.
		break;
	}
	case 1:
	{
		shape = new SRectangle(x, y, 1, 1);
		break;
	}
	default:
		return;
	}

	m_currentLayoutTemplateVec.push_back(shape);
	g_iCurrentSelectedShape = -1;
}

void GameEditor::DeleteShapeAtMousePosition(float x, float y)
{
	// Find Any shape at position.
	int index = 0;
	for(auto& shape: m_currentLayoutTemplateVec)
	{
		if(shape->Type() == 0)
		{
			// Circle. Removed.
		}
		else if(shape->Type() == 1)
		{
			// Rectangle.
			auto rect = reinterpret_cast<SRectangle*>(shape);
			if (PointRectangleCollision(x, y, rect->x, rect->y, rect->w, rect->h))
			{
				// Delete Rect.
				delete shape;
				rect = nullptr;
				m_currentLayoutTemplateVec.erase(m_currentLayoutTemplateVec.begin() + index);
				return;
			}
		}


		index++;
	}
}

bool GameEditor::PointRectangleCollision(float p1, float p2, float x, float y, float w, float h)
{
	 if(p1 > x && p1 < x + w &&
		p2 > y && p2 < y + h)
	 {
		 return true;
	 }

	 return false;
}

bool GameEditor::PointCircleCollision(float p1, float p2, float x, float y, float r)
{	
	// Adjust circle position
	float xx = x + r;
	float yy = y + r;

	// Compute Euclidian distance.
	float dist = std::sqrt((xx - p1) * (xx - p1) + (yy - p2) * (yy - p2));

	// If distance is smaller than radius, then point inside circle.
	if(dist <= r)
	{
		return true;
	}
	return false;
}

void GameEditor::DisplayAddingPrefabElementType()
{
	ImGui::SetNextWindowPos(ImVec2(ScreenWidth() / 2.0f - ScreenWidth() / 4.0f, ScreenHeight() / 2.0f - ScreenWidth() / 4.0f), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(ImVec2(500, 250), ImGuiCond_Appearing);
	ImGui::Begin("Adding Prefab Element Type", &g_bAddingPrefabElementType);

	static char new_prefab_element_type_name[64] = "";
	ImGui::InputText("|", new_prefab_element_type_name, 64);
	ImGui::SameLine();
	if (ImGui::SmallButton("OK"))
	{
		// Check for sanity.
		std::string name = std::string(new_prefab_element_type_name);

		if(TryAddingPrefabElementType(name))
		{
			memset(&new_prefab_element_type_name, 0, sizeof(new_prefab_element_type_name));
			g_bAddingPrefabElementType = false;
		}
	}

	ImGui::End();
}

bool GameEditor::TryAddingPrefabElementType(const std::string& name)
{
	bool length = name.length() > 0;
	bool duplicate = std::find(g_PrefabElementTypeVec.begin(), g_PrefabElementTypeVec.end(), name) != g_PrefabElementTypeVec.end();

	if (length && !duplicate)
	{
		g_PrefabElementTypeVec.push_back(name);
	}

	if (!length)
	{
		LOG_DBG_ERROR("[{:.4f}][TryAddingPrefabElementType] Error adding Prefab Element Type \"{}\": Name has 0 length!", APP_RUN_TIME, name);
		LOG_FILE_ERROR("[{:.4f}][TryAddingPrefabElementType]  Error adding Prefab Element Type \"{}\": Name has 0 length!", APP_RUN_TIME, name);
	}
	if (duplicate)
	{
		LOG_DBG_ERROR("[{:.4f}][TryAddingPrefabElementType] Error adding Prefab Element Type \"{}\": Name is duplicate!", APP_RUN_TIME, name);
		LOG_FILE_ERROR("[{:.4f}][TryAddingPrefabElementType]  Error adding Prefab Element Type \"{}\": Name is duplicate!", APP_RUN_TIME, name);
	}


	if (duplicate || !length) return false;


	LOG_DBG_INFO("[{:.4f}][TryAddingPrefabElementType] Successfully added Prefab Element Type \"{}\"!", APP_RUN_TIME, name);
	LOG_FILE_INFO("[{:.4f}][TryAddingPrefabElementType]  Successfully added Prefab Element Type \"{}\"!", APP_RUN_TIME, name);
	return true;
}

bool GameEditor::DisplaySavingPrefabLayoutElement()
{
	ImGui::SetNextWindowPos(ImVec2(ScreenWidth() / 2.0f - ScreenWidth() / 4.0f, ScreenHeight() / 2.0f - ScreenHeight() / 4.0f), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(ImVec2(500, 250), ImGuiCond_Appearing);

	bool success = false;

	ImGui::Begin("Save As...", &g_bSavingPrefabLayoutTemplate);

	static char prefab_layout_template_name[64] = "";
	ImGui::InputText("|", prefab_layout_template_name, 64);
	ImGui::SameLine();
	if (ImGui::SmallButton("OK"))
	{
		// Check for sanity.
		std::string name = std::string(prefab_layout_template_name);

		bool length = name.length() > 0;
		if (length)
		{
			success = ExportPrefabLayoutTemplate(name);
		}
		memset(&prefab_layout_template_name, 0, sizeof(prefab_layout_template_name));
		g_bSavingPrefabLayoutTemplate = false;
	}

	ImGui::End();

	return success;
}

bool GameEditor::ExportPrefabLayoutTemplate(const std::string& filepath)
{
	tinyxml2::XMLDocument doc;
	auto root = doc.NewElement("PrefabLayoutTemplate");
	doc.InsertEndChild(root);

	// Find the Root.
	SShape* root_element = nullptr;
	for(auto & s: m_currentLayoutTemplateVec)
	{
		if(s->IsRootElement())
		{
			root_element = s;
			break;
		}
	}

	// Add Root to XML.
	auto root_element_xml = root->InsertNewChildElement("Root");
	root_element_xml->SetAttribute("name", root_element->ElementType().c_str());
	root_element_xml->SetAttribute("x", root_element->PositionX());
	root_element_xml->SetAttribute("y", root_element->PositionY());
	root_element_xml->SetAttribute("w", root_element->WidthX());
	root_element_xml->SetAttribute("h", root_element->WidthY());


	// Add all other elements to XML.
	for(auto& s: m_currentLayoutTemplateVec)
	{
		if(!s->IsRootElement())
		{
			float offsetx = s->PositionX() - root_element->PositionX();
			float offsety = s->PositionY() - root_element->PositionY();

			auto element = root_element_xml->InsertNewChildElement("LayoutElement");

			element->SetAttribute("name", s->ElementType().c_str());
			element->SetAttribute("offsetx", offsetx);
			element->SetAttribute("offsety", offsety);
			element->SetAttribute("w", s->WidthX());
			element->SetAttribute("h", s->WidthY());

		}
	}



	std::string path = "assets/TilesetData/" + filepath + ".xml";
	if(doc.SaveFile(path.c_str()) != tinyxml2::XML_SUCCESS)
	{
		return false;
	}

	return true;
}

bool GameEditor::DisplayLoadingPrefabLayoutElement()
{
	ImGui::SetNextWindowPos(ImVec2(ScreenWidth() / 2.0f - ScreenWidth() / 4.0f, ScreenHeight() / 2.0f - ScreenHeight() / 4.0f), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(ImVec2(500, 250), ImGuiCond_Appearing);

	bool success = false;

	ImGui::Begin("Load From...", &g_bLoadPrefabLayoutTemplate);

	static char load_prefab_layout_template_name[64] = "";
	ImGui::InputText("|", load_prefab_layout_template_name, 64);
	ImGui::SameLine();
	std::string name;
	if (ImGui::SmallButton("OK"))
	{
		// Check for sanity.
		std::string name = std::string(load_prefab_layout_template_name);

		bool length = name.length() > 0;
		if (length)
		{
			success = ImportPrefabLayoutTemplate(name);
		}
		else
		{
			LOG_DBG_ERROR("[{:.4f}][ImportPrefabLayoutTemplate] Invalid Filepath to import PrefabLayoutTemplate: \"{}\"!", APP_RUN_TIME, name);
			LOG_FILE_ERROR("[{:.4f}][ImportPrefabLayoutTemplate] Invalid Filepath to import PrefabLayoutTemplate: \"{}\"!", APP_RUN_TIME, name);
		}

		if (!success)
		{
			LOG_DBG_ERROR("[{:.4f}][ImportPrefabLayoutTemplate] Could not import PrefabLayoutTemplate \"{}\"!", APP_RUN_TIME, name);
			LOG_FILE_ERROR("[{:.4f}][ImportPrefabLayoutTemplate] Could not import PrefabLayoutTemplate \"{}\"!", APP_RUN_TIME, name);
		}
		else
		{
			LOG_DBG_INFO("[{:.4f}][ImportPrefabLayoutTemplate] Successfully imported PrefabLayoutTemplate \"{}\"!", APP_RUN_TIME, name);
			LOG_FILE_INFO("[{:.4f}][ImportPrefabLayoutTemplate] Successfully imported PrefabLayoutTemplate \"{}\"!", APP_RUN_TIME, name);
		}
		memset(&load_prefab_layout_template_name, 0, sizeof(load_prefab_layout_template_name));
		g_bLoadPrefabLayoutTemplate = false;
	}

	ImGui::End();

	return success;
}

bool GameEditor::ImportPrefabLayoutTemplate(const std::string& filepath)
{
	std::string path = "assets/TilesetData/" + filepath + ".xml";


	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(path.c_str()) != tinyxml2::XMLError::XML_SUCCESS)
	{
		doc.Clear();
		return false;
	}

	auto root = doc.RootElement();

	auto root_element = root->FirstChildElement("Root");
	auto x = root_element->FloatAttribute("x");
	auto y = root_element->FloatAttribute("y");
	auto w = root_element->FloatAttribute("w");
	auto h = root_element->FloatAttribute("h");
	auto type = root_element->Attribute("name");


	// Add Root element.
	auto root_shape = new SRectangle(x, y, w, h);
	root_shape->SetAsRoot(true);
	root_shape->SetElementType(type);

	m_currentLayoutTemplateVec.push_back(root_shape);
	g_PrefabElementTypeVec.push_back(std::string(type));

	auto layout_element = root_element->FirstChildElement("LayoutElement");
	while(layout_element)
	{
		// Add all other elements.
		auto offsetx = layout_element->FloatAttribute("offsetx") + x;
		auto offsety = layout_element->FloatAttribute("offsety") + y;
		auto w = layout_element->FloatAttribute("w");
		auto h = layout_element->FloatAttribute("h");
		auto type = layout_element->Attribute("name");

		auto shape = new SRectangle(offsetx, offsety, w, h);
		shape->SetAsRoot(false);
		shape->SetElementType(std::string(type));

		m_currentLayoutTemplateVec.push_back(shape);
		g_PrefabElementTypeVec.push_back(std::string(type));

		layout_element = layout_element->NextSiblingElement("LayoutElement");
	}


	return true;
}



void SRectangle::Draw(GameEditor* editor)
{
	editor->tv.DrawDecal(olc::vf2d(x, y), editor->m_editorDecalDatabase["Rect"], olc::vf2d(w, h));
}



bool GameEditor::OnUserDestroy()
{
	delete g_pCurrentEditedPrefab;
	g_pCurrentEditedPrefab = nullptr;
	g_PrefabElementTypeVec.clear();

	sound::SoundSystem::get()->Terminate();
	Logger::Terminate();
	return true;
}

int main()
{
	if (editor.Construct(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, 1, 1, true, true, false))
		editor.Start();

	return 0;
}
