/*
* Change Log:
* 03.04.2022 - Version 0.1 complete:
*			 o Rendering Decal Database loaded from JSON in Gameengine format.
*			 o Layer menu. Add, Remove, Toggle visibility and Edit layers you place objects on.
*			 o Coloring Layer names in a intuitive way to hint at their current status (active, visible etc.)
*			 o Render Decals in Tabs according to their type (e.g. Forest, Water etc)
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
#define OLC_GFX_OPENGL33
#define OLC_PGEX_DEAR_IMGUI_IMPLEMENTATION
#include "olc/imgui_impl_pge.h"


#define EDITOR_MAJOR_VERSION 0
#define EDITOR_MINOR_VERSION 1
#define EDITOR_PATCH_VERSION 0


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


#define DEFAULT_WIDGET_IMAGE_SIZE_X 16
#define DEFAULT_WIDGET_IMAGE_SIZE_Y 16


// COMMON
#include "Mapobject.h"
using LayeredGameworld = std::map< std::string, std::vector< std::vector< Entity* > > >;


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
		m_GUILayer = CreateLayer();
		EnableLayer(m_GUILayer, true);
		SetLayerCustomRenderFunction(0, std::bind(&GameEditor::DrawUI, this));

		tv = olc::TileTransformedView({ ScreenWidth(), ScreenHeight() }, { DEFAULT_DECAL_SIZE_X, DEFAULT_DECAL_SIZE_Y });

		m_visibleLayers.resize(256);
		CreateRenderingLayer("Default", 0);
		m_currentLayer = "Default";
		m_visibleLayers[0] = 1;
		UpdateLayerSorting();

		//return LoadTilesetData("assets/Tileset", "assets/TilesetData/TilesetOverworld.json") && LoadEditorGraphicalData();
		bool loaded = LoadTilesetData("Forest", "assets/Tileset/Forest", "assets/TilesetData/Forest.json");
		loaded &= LoadTilesetData("Ground","assets/Tileset/Ground", "assets/TilesetData/Ground.json");
		loaded &= LoadTilesetData("Mountain","assets/Tileset/Mountain", "assets/TilesetData/Mountain.json");
		loaded &= LoadTilesetData("Road","assets/Tileset/Road", "assets/TilesetData/Road.json");
		loaded &= LoadTilesetData("Water","assets/Tileset/Water", "assets/TilesetData/Water.json");
		loaded &= LoadTilesetData("Structure","assets/Tileset/Structure", "assets/TilesetData/Structure.json");
		loaded &= LoadEditorGraphicalData();


		auto sprite = new olc::Sprite("assets/Townhall_I.png");
		auto decal = new olc::Decal(sprite);
		m_structureDecalDatabase.emplace("Townhall", decal);
		m_decalDatabase.emplace("Townhall", decal);
		m_spriteDatabase.push_back(sprite);

		sprite = new olc::Sprite("assets/Castle_I.png");
		decal = new olc::Decal(sprite);
		m_structureDecalDatabase.emplace("Castle", decal);
		m_decalDatabase.emplace("Castle", decal);
		m_spriteDatabase.push_back(sprite);

		sprite = new olc::Sprite("assets/Watchtower.png");
		decal = new olc::Decal(sprite);
		m_structureDecalDatabase.emplace("Watchtower", decal);
		m_decalDatabase.emplace("Watchtower", decal);
		m_spriteDatabase.push_back(sprite);


		return loaded;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::BLANK);
		SetDrawTarget((uint8_t)m_GUILayer);

		HandleInput();
		UpdateVisibleRect();

		UpdateEntities();

		RenderMainFrame();

		DrawStringDecal(olc::vf2d(5, 25), "FPS: " + std::to_string(GetFPS()));

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
	std::map< std::string, olc::Decal* > m_decalDatabase;
	std::vector< olc::Sprite* > m_spriteDatabase;

	// Editor specific decal datatabase.
	std::map< std::string, olc::Decal* > m_editorDecalDatabase;
	std::vector< olc::Sprite* > m_editorSpriteDatabase;
private:

	// GUI
	void RenderGUI();
	void RenderDecalDatabase();
	void RenderMainMenu();
	void RenderLayerUI();
	void RenderDecalDatabase(const std::map< std::string, olc::Decal* >& db);
	void RenderEntityDatabase();
	void DisplayEntityEditor(Entity* e);

	// GAMEWORLD
	void RenderMainFrame();
	void UpdateEntities();

	// UTIL
	bool LoadTilesetData(const std::string& database, const std::string& tilesetpath, const std::string& datapath);
	bool LoadEditorGraphicalData();
	void ToggleMenuItem(bool& item);
	void HandleInput();
	void UpdateVisibleRect();
	void RenderMapobject(Entity* object);
	void CreateMapobject(uint64_t x, uint64_t y, std::string decal, std::string name = "none");
	std::string CreateMapobjectName();
	bool IsMapobjectNameUsed(const std::string& name);
	void DeleteMapobject(Entity* object);
	

	// Note: Layer 0 is by Default the first created.
	// Layers are drawn from 0 to n.
	void CreateRenderingLayer(std::string layer_name, int order);
	void ChangeLayerOrder(std::string layer_name, int order);
	void ChangeLayerName(std::string layer_name, std::string new_name);
	void DeleteRenderingLayer(std::string layer_name);
	void InitializeMatrix(std::vector< std::vector< Entity* > >& matrix);
	void UpdateLayerSorting();
	void ToggleLayerVisibility(int layer);
	bool LayerVisible(int layer);

	// UTIL ALGORITHM
	std::multimap< int, std::string, std::greater<int> > SortDescending(std::map< std::string, int >& map);
	std::map< int, std::string > SortAscending(std::map< std::string, int >& map);


	// UTIL IMGUI
	void BeginTooltip(const char* help_text);
};