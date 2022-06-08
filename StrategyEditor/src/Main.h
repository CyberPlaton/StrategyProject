/*
* Change Log:
* 03.04.2022 - Version 0.1 complete:
*			 o Rendering Decal Database loaded from JSON in Gameengine format.
*			 o Layer menu. Add, Remove, Toggle visibility and Edit layers you place objects on.
*			 o Coloring Layer names in a intuitive way to hint at their current status (active, visible etc.).
*			 o Render Decals in Tabs according to their type (e.g. Forest, Water etc).
* 
* 
* 28.05.2022 - Version 0.2 complete:
*			 o Creating special Game Entities (Cities and Forts).
*			 o Adding Territory and Building slots for Cities and Forts.
*			 o Ambient Audio "Editor". Playing loaded and chosen Audio Track.
*
* 30.05.2022 - Version 0.2.1 "complete":
*			 o Adding sound sources to the map.
*			 o Serializing and Deserializing entities with Sound Components.
*			 o Displaying collision boxes of sound sources.
*			 o Sound Source editor: Changing name, so that another sound file is played.
*				Play once, looped and stop a sound file.
*			 o Adding a Proof of concept scene in the Jungle Forest playing ambient sounds.
* 
* 06.06.2022 - Version 0.3.0 complete:
*			 o Adding SoundChannel Editor for creation a Sound Channel tree to be submitted to FMOD.
*			 o Now embedding SoundChannel data into the Map. Supporting exporting and importing the Tree structure.
*			 o Reworked Audio Editor:
*				- Displaying all loaded and available Audio Assets.
*				- Displaying all placed on the world map Sound Sources (by clicking on them you can edit them).
*			 o Reworked Sound Source Editor:
*				- Changing the name is now safer and a MUST (however not really enforced).
*				- Adding convenient way to set the desired ChannelGroup for the Sound Source
*				- Adding manipulation of the collision box dimensions.
*				- Adding convenient way to alter collision box color (Color Picker).
* 
* 09.06.2022 - Version 0.3.1 Patch complete:
*			 o Adding SoundChannel child nodes to SoundChannelTree structure.
*			 o Removing SoundChannel nodes from SoundChannelTree structure.
* 
* TODO: Ambient Audio "Editor" has to be update to be based on FMOD,
*		as soon as the Audio Engine with FMOD is completed.
*		After this the "Editor" can be extended based on FMOD to utilize 
*		".bank" files, FMOD events, triggers etc.
*/

// OLC INLCUDE
#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"

#define OLC_GFX_OPENGL33
#define OLC_PGE_APPLICATION
#include "olc/olcPixelGameEngine.h"
#define OLC_PGEX_TRANSFORMEDVIEW
#include "olc/olcPGEX_TransformedView.h"
#define OLC_PGEX_SOUND
#include "olc/olcPGEX_Sound.h"
#define OLC_GFX_OPENGL33
#define OLC_PGEX_DEAR_IMGUI_IMPLEMENTATION
#include "olc/imgui_impl_pge.h"

#include "SoundSystem.h"

#define EDITOR_MAJOR_VERSION 0
#define EDITOR_MINOR_VERSION 3
#define EDITOR_PATCH_VERSION 1


#define STRING(text) #text
#define EDITOR_TITLE_STRING StrategyEditor v
#ifdef DEBUG
#define TITLE(title, major, minor, review) \
STRING(title) \
STRING(major) \
"." STRING(minor) \
"." STRING(review) \
"_" STRING(DEBUG)
#endif
#ifdef RELEASE
#define TITLE(title, major, minor, review) \
STRING(title) \
STRING(major) \
"." STRING(minor) \
"." STRING(review) \
"_" STRING(RELEASE)
#endif
#ifdef DISTR
#define TITLE(title, major, minor, review) \
STRING(title) \
STRING(major) \
"." STRING(minor) \
"." STRING(review)
#endif


// SPDLOG
#include "Logging.h"
#define APP_RUN_TIME Logger::AppRunningTime()
// XML
#include "tinyxml2/tinyxml2.h"
// JSON
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
// STL
#include <map>
#include <vector>


#define MAX_MAPSIZE_X 256
#define MAX_MAPSIZE_Y 256
#define DEFAULT_DECAL_SIZE_X 128
#define DEFAULT_DECAL_SIZE_Y 128
#define DEFAULT_UNIT_DECAL_OFFSET_X -0.2f
#define DEFAULT_UNIT_DECAL_OFFSET_Y -0.4f


#define DEFAULT_WIDGET_IMAGE_SIZE_X 16
#define DEFAULT_WIDGET_IMAGE_SIZE_Y 16


#define DEFAULT_MAP_HEIGHT_STEP 10.0f
#define DEFAULT_MIN_ZOOM_DISTANCE 1.0f
#define DEFAULT_MAX_ZOOM_DISTANCE 50.0f

// COMMON
#include "Mapobject.h"
using LayeredGameworld = std::map< std::string, std::vector< std::vector< Entity* > > >;

// COMMON USED DATASTRUCTURES
struct Tree
{
	Tree(const std::string& name) : m_name(name) {};
	Tree() : m_name("None") {};

	Tree& operator[](const std::string& name)
	{
		if (m_name.compare(name) == 0)
		{
			return *this;
		}

		for (auto& kid : m_children)
		{
			if (kid->m_name.compare(name) == 0) return *kid;
		}

		auto node = new Tree(name);
		m_children.push_back(node);
		return *node;
	}

	Tree* Node(const std::string& name)
	{
		if (m_name.compare(name) == 0)
		{
			return this;
		}

		for (auto& kid : m_children)
		{
			if (kid->m_name.compare(name) == 0) return kid;
		}

		auto node = new Tree(name);
		m_children.push_back(node);
		return node;
	}

	bool Has(const std::string& name)
	{
		if (m_name.compare(name) == 0)
		{
			return true;
		}

		for (auto& kid : m_children)
		{
			if(kid->Has(name)) return true;
		}

		return false;
	}

	void RemoveNode(const std::string& name)
	{
		// If we are to be removed, clear everything.
		if (m_name.compare(name) == 0)
		{
			ClearTree();
		}

		// If one of the first grade children is to be removed,
		// remove it here.
		for (int i = 0; i < m_children.size(); i++)
		{
			if (m_children[i]->m_name.compare(name) == 0)
			{
				m_children.erase(m_children.begin() + i);
				return;
			}
		}

		// If one of the higher grade children is to be removed,
		// try to find the node in each one of them and to remove it.
		for (auto& kid : m_children)
		{
			if (RemoveChildFromTree(kid, name))
			{
				return;
			}
		}
	}

	void ClearTree()
	{
		while (m_children.size() > 0)
		{
			auto kid = m_children[m_children.size() - 1];
			delete kid;
			kid = nullptr;
			m_children.pop_back();
		}
	}

	std::vector< Tree* > m_children;
	std::string m_name;

private:

	bool RemoveChildFromTree(Tree* node, const std::string& name)
	{
		// No need to check whether we are searched.

		// Check for first grade children.
		for (int i = 0; i < node->m_children.size(); i++)
		{
			auto n = node->m_children[i];

			if (n->m_name.compare(name) == 0)
			{
				node->m_children.erase(node->m_children.begin() + i);
				return true;
			}
		}


		// Check for higher grade children.
		for (auto& kid : node->m_children)
		{
			if (RemoveChildFromTree(kid, name))
			{
				return true;
			}
		}

		// We couldnt find any node with provided name.
		return false;
	}
};



// Utility. Create a hook function to actually call the GameEditors OnUserUpdate function.
class GameEditor;
void MainRender(GameEditor* editor);

class GameEditor : public olc::PixelGameEngine
{
public:
	GameEditor() : pge_imgui(false)
	{
		sAppName = TITLE(EDITOR_TITLE_STRING, EDITOR_MAJOR_VERSION, EDITOR_MINOR_VERSION, EDITOR_PATCH_VERSION);
	}

public:
	bool OnUserCreate() override
	{
#ifdef DISTR
		// Remove console in distribution build (Windows only).
		ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif

		// Initialize Random.
		srand(time(0));

		// Initialize Logging.
		if (!Logger::Initialize()) return false;

		// Initialize Audio.
		olc::SOUND::InitialiseAudio(44100, 1, 8, 512);
		
		// FMOD
		SoundSystem::get()->Initialize();
		/*
		SoundSystem::get()->CreateChannelGroup("SFX");
		SoundSystem::get()->CreateChannelGroup("Music");

		auto tilex = 15.0f;
		auto tiley = 5.0f;
		SoundSystem::get()->CreateSoundOnChannel("assets/Audio/main_theme_battle.wav", "BattleTheme", "Music", false, { tilex, tiley, 0.0f});
		*/

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
		loaded &= LoadTilesetData("Ground","assets/Tileset/Ground", "assets/TilesetData/Ground.json");
		loaded &= LoadTilesetData("Mountain","assets/Tileset/Mountain", "assets/TilesetData/Mountain.json");
		loaded &= LoadTilesetData("Road","assets/Tileset/Road", "assets/TilesetData/Road.json");
		loaded &= LoadTilesetData("Sea","assets/Tileset/Sea", "assets/TilesetData/Sea.json");
		loaded &= LoadTilesetData("Bridge","assets/Tileset/Bridge", "assets/TilesetData/Bridge.json");
		loaded &= LoadTilesetData("River", "assets/Tileset/River", "assets/TilesetData/River.json");
		loaded &= LoadTilesetData("Structure", "assets/Tileset/Structure", "assets/TilesetData/Structure.json");
		loaded &= LoadTilesetData("Wall", "assets/Tileset/Wall", "assets/TilesetData/Wall.json");
		loaded &= LoadTilesetData("Hill", "assets/Tileset/Hill", "assets/TilesetData/Hill.json");
		loaded &= LoadTilesetData("Unit", "assets/Tileset/Unit", "assets/TilesetData/Unit.json");

		loaded &= LoadEditorGraphicalData();

		// Load special Asset data.
		auto sprite = new olc::Sprite("assets/Tileset/Structure/Fort.png");
		auto decal = new olc::Decal(sprite);
		m_structureDecalDatabase.emplace("Fort", decal);
		m_decalDatabase.emplace("Fort", decal);
		m_spriteDatabase.push_back(sprite);

		sprite = new olc::Sprite("assets/Tileset/Structure/Fort_2.png");
		decal = new olc::Decal(sprite);
		m_structureDecalDatabase.emplace("Fort_2", decal);
		m_decalDatabase.emplace("Fort_2", decal);
		m_spriteDatabase.push_back(sprite);

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

		return loaded;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::BLANK);
		SetDrawTarget((uint8_t)m_GUILayer);


		SoundSystem::get()->SetListenerPositionVector(m_camerax, m_cameray, -m_cameraHeigth);
		SoundSystem::get()->Update();


		HandleInput();
		UpdateVisibleRect();

		UpdateEntities();

		RenderMainFrame();

		DrawStringDecal(olc::vf2d(5, 25), "FPS: " + std::to_string(GetFPS()));

		return true;
	}

	bool OnUserDestroy() override final
	{
		SoundSystem::get()->Terminate();
		olc::SOUND::DestroyAudio();
		Logger::Terminate();
		return true;
	}

	void DrawUI(void)
	{		
		RenderGUI();

		pge_imgui.ImGui_ImplPGE_Render();
	}

private:
	olc::TileTransformedView tv;
	olc::imgui::PGE_ImGUI pge_imgui;
	int m_GUILayer;
	
	float m_camerax = 0;
	float m_cameray = 0;
	uint64_t m_tileUnderMouseX = 0;
	uint64_t m_tileUnderMouseY = 0;
	float m_cameraHeigth = 1.0f;

	olc::vi2d m_visiblePointLeftUp;
	olc::vi2d m_visiblePointDownRight;

	// Gameworld
	LayeredGameworld m_gameworld;
	std::map< std::string, int > m_layerOrder;
	std::multimap< int, std::string, std::greater< int > > m_sortedLayers;
	std::map< int, std::string > m_sortedLayersAscending;
	std::vector< uint32_t > m_visibleLayers;
	uint64_t m_mapobjectCount = 0;
	std::string m_currentLayer = "Default";
	std::vector< Entity* > m_entities;

	// Gameeditor is responsible to cleanup the decal and sprite data.
	std::map< std::string, olc::Decal* > m_forestDecalDatabase;
	std::map< std::string, olc::Decal* > m_roadDecalDatabase;
	std::map< std::string, olc::Decal* > m_groundDecalDatabase;
	std::map< std::string, olc::Decal* > m_waterDecalDatabase;
	std::map< std::string, olc::Decal* > m_mountainDecalDatabase;
	std::map< std::string, olc::Decal* > m_structureDecalDatabase;
	std::map< std::string, olc::Decal* > m_wallDecalDatabase;
	std::map< std::string, olc::Decal* > m_bridgeDecalDatabase;
	std::map< std::string, olc::Decal* > m_riverDecalDatabase;
	std::map< std::string, olc::Decal* > m_hillDecalDatabase;
	std::map< std::string, olc::Decal* > m_unitDecalDatabase;
	std::map< std::string, olc::Decal* > m_decalDatabase;
	std::vector< olc::Sprite* > m_spriteDatabase;
	std::map< std::string, std::pair< uint64_t, uint64_t > > m_decalSizeDatabase;

	// Editor specific decal database.
	std::map< std::string, olc::Decal* > m_editorDecalDatabase;
	std::vector< olc::Sprite* > m_editorSpriteDatabase;

	// Audio
	std::map< std::string, std::pair< bool, int > > m_soundMap;

	// Permanent Editor specific layers which cannot be altered.
	std::vector< int > m_PermanentLayersVec;

private:

	// GUI
	void RenderGUI();
	void RenderDecalDatabase();
	void RenderMainMenu();
	void RenderLayerUI();
	void RenderDecalDatabase(const std::map< std::string, olc::Decal* >& db);
	void RenderEntityDatabase();
	void DisplayEntityEditor(Entity* e);
	void DisplayBackgroundAudioEditor();
	void DisplaySoundChannelEditor();
	void DisplaySoundChannelNode(Tree* tree);
	void DisplaySoundSourceEditor(Entity* e);
	void DisplaySoundChannelAddRemoveOptions(Tree* node);
	void DisplayAddingChildNodeToSoundChannel(Tree* node);
	void RemoveNodeFromSoundChannelTree(Tree* tree, Tree* node);

	// GUI SOUND EDITOR UTIL
	void DisplayChannelGroupChanger(Entity* e);
	void AddSoundChannelGroupToVec(Tree* tree, std::vector< std::string >& vec);
	void DisplayDimensionChanger(Entity* e);
	void DisplayCollisionBoxColorPicker(Entity* e);

	// GAMEWORLD
	void RenderMainFrame();
	void UpdateEntities();

	// UTIL
	bool LoadTilesetData(const std::string& database, const std::string& tilesetpath, const std::string& datapath);
	bool LoadEditorGraphicalData();
	bool LoadAudioData(const std::string& filepath);
	void ToggleMenuItem(bool& item);
	void HandleInput();
	void UpdateVisibleRect();
	void RenderMapobject(Entity* object);
	Entity* CreateMapobject(uint64_t x, uint64_t y, std::string decal, bool unit, std::string name = "none");
	Entity* CreateMapobjectEx(uint64_t x, uint64_t y, std::string layer, std::string decal, std::string name = "none");
	Entity* CreateMapobjectAudioSource(uint64_t x, uint64_t y, uint64_t w, uint64_t h, const std::string& soundname);
	std::string CreateMapobjectName();
	bool IsMapobjectNameUsed(const std::string& name);
	void DeleteMapobject(Entity* object);
	void DeleteMapobjectAudioSource(Entity* object);
	std::string GetMapobjectNameAt(int x, int y, std::string layer);
	Entity* GetMapobjectAt(int x, int y, std::string layer);
	void SetDecalSize(const std::string& name, uint64_t w, uint64_t h);
	uint64_t GetDecalWidth(const std::string& name);
	uint64_t GetDecalHeight(const std::string& name);
	olc::Pixel GetRandomColor(uint64_t alpha = 255);
	bool CreateAndSubmitSoundChannelTree(Tree* tree);
	bool CreateAndSubmitSoundChannelNode(Tree* tree, const std::string& parent);
	void UpdateInGameSoundSourcesMap(std::map< std::string, Entity* >& map);
	uint32_t ConvertColorToPixel(ImVec4 color);

	void MakeMapobjectTownhall(int x, int y, std::string layer);
	void MakeMapobjectFort(int x, int y, std::string layer);
	void AddTerritoryToCity(Entity* e, int slotx, int sloty);
	void AddBuildingSlotToCity(Entity* e, int slotx, int sloty);
	void RemoveTerritoryFromCity(Entity* e, int slotx, int sloty);
	void RemoveBuildingSlotFromCity(Entity* e, int slotx, int sloty);


	// Note: Layer 0 is by Default the first created.
	// Layers are drawn from 0 to n.
	void CreateRenderingLayer(std::string layer_name, int order);
	void ChangeLayerOrder(std::string layer_name, int order);
	void ChangeLayerName(std::string layer_name, std::string new_name);
	void DeleteRenderingLayer(std::string layer_name);
	void InitializeMatrix(std::vector< std::vector< Entity* > >& matrix);
	void UpdateLayerSorting();
	void SetAllLayersVisible();
	void ToggleLayerVisibility(int layer);
	bool LayerVisible(int layer);

	// UTIL ALGORITHM
	std::multimap< int, std::string, std::greater<int> > SortDescending(std::map< std::string, int >& map);
	std::map< int, std::string > SortAscending(std::map< std::string, int >& map);
	

	// UTIL SERIALIZATION/DESERIALIZATION
	bool ExportMapData(const std::string& filepath);
	bool ImportMapData(const std::string& filepath);
	// Import
	Entity* ImportEntity(tinyxml2::XMLElement* xml, const std::string& layer);
	void ImportEntityComponentSound(tinyxml2::XMLElement* xml, Entity* entity);
	void ImportEntityComponentFort(tinyxml2::XMLElement* xml, Entity* entity);
	void ImportEntityComponentTownhall(tinyxml2::XMLElement* xml, Entity* entity);
	void ImportEntityComponentUnit(tinyxml2::XMLElement* xml, Entity* entity);
	// Import Sound Channel Tree Data
	bool LoadSoundChannelTreeMapData(tinyxml2::XMLElement* xml, Tree* tree);
	bool LoadSoundChannelTreeStandalone(const std::string& filepath, Tree* tree);
	void LoadSoundChannelNode(tinyxml2::XMLElement* xml_node, Tree* tree);

	// Export
	void ExportEntity(tinyxml2::XMLElement* xml, Entity* entity);
	void ExportEntityComponentSound(tinyxml2::XMLElement* xml, Entity* entity);
	void ExportEntityComponentFort(tinyxml2::XMLElement* xml, Entity* entity);
	void ExportEntityComponentTownhall(tinyxml2::XMLElement* xml, Entity* entity);
	void ExportEntityComponentUnit(tinyxml2::XMLElement* xml, Entity* entity);
	// Export Sound Channel Tree Data
	void ExportSoundChannelTree(tinyxml2::XMLElement* xml, Tree* tree);
	void ExportSoundChannelTreeNode(tinyxml2::XMLElement* xml, Tree* tree);

	// UTIL IMGUI
	void BeginTooltip(const char* help_text);
	static void HelpMarker(const char* desc)
	{
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}
	static void HelpMarkerWithoutQuestion(const char* desc)
	{
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}

};