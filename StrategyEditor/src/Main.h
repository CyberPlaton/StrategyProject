
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


#define MAX_MAPSIZE_X 1024
#define MAX_MAPSIZE_Y 1024
#define DEFAULT_DECAL_SIZE_X 128
#define DEFAULT_DECAL_SIZE_Y 128

// COMMON
#include "Mapobject.h"

using LayeredGameworld = std::map< std::string, std::vector< std::vector< Mapobject* > > >;


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


		CreateRenderingLayer("Default", 0);

		return LoadTilesetData("assets/Tileset", "assets/TilesetData/TilesetOverworld.json") && LoadEditorGraphicalData();
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::BLANK);
		SetDrawTarget((uint8_t)m_GUILayer);

		HandleInput();
		UpdateVisibleRect();

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
	std::multimap< int, std::string > m_sortedLayers;

	uint64_t m_mapobjectCount = 0;
	std::string m_currentLayer = "Default";

	// Gameeditor is responsible to cleanup the decal and sprite data.
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
	
	// GAMEWORLD
	void RenderMainFrame();

	// UTIL
	bool LoadTilesetData(const std::string& tilesetpath, const std::string& datapath);
	bool LoadEditorGraphicalData();
	void ToggleMenuItem(bool& item);
	void HandleInput();
	void UpdateVisibleRect();
	void RenderMapobject(Mapobject* object);
	void CreateMapobject(uint64_t x, uint64_t y, std::string decal, std::string name = "none");
	std::string CreateMapobjectName();

	// Note: Layer 0 is by Default the first created.
	// Layers are drawn from 0 to n.
	void CreateRenderingLayer(std::string layer_name, int order);
	void ChangeLayerOrder(std::string layer_name, int order);
	void ChangeLayerName(std::string layer_name, std::string new_name);
	void DeleteRenderingLayer(std::string layer_name);
	void InitializeMatrix(std::vector< std::vector< Mapobject* > >& matrix);

	// UTIL ALGORITHM
	std::multimap< int, std::string, std::greater<int> > SortDescending(std::map< std::string, int >& map);
	std::multimap< int, std::string > SortAscending(std::map< std::string, int >& map);
};