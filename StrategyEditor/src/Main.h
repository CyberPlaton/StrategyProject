
// OLC INLCUDE
#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"
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

		m_GameLayer = CreateLayer();
		EnableLayer(m_GameLayer, true);
		SetLayerCustomRenderFunction(1, std::bind(&MainRender, this));


		tv = olc::TileTransformedView({ ScreenWidth(), ScreenHeight() }, { DEFAULT_DECAL_SIZE_X, DEFAULT_DECAL_SIZE_Y });

		return LoadTilesetData("assets/Tileset", "assets/TilesetData/TilesetOverworld.json");
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		SetDrawTarget((uint8_t)m_GameLayer);
		Clear(olc::BLANK);

		HandleInput();
		UpdateVisibleRect();

		RenderMainFrame();

		DrawStringDecal(olc::vf2d(5, 25), "FPS: " + std::to_string(GetFPS()));

		return true;
	}

	void DrawUI(void)
	{
		SetDrawTarget((uint8_t)m_GUILayer);
		Clear(olc::BLANK);

		RenderGUI();

		pge_imgui.ImGui_ImplPGE_Render();
	}

private:
	olc::TileTransformedView tv;
	olc::imgui::PGE_ImGUI pge_imgui;
	int m_GUILayer;
	int m_GameLayer;
	
	float m_camerax = 0;
	float m_cameray = 0;

	olc::vi2d m_visiblePointLeftUp;
	olc::vi2d m_visiblePointDownRight;

	// Gameworld
	std::vector< std::vector< Mapobject* > > m_gameworld;

	// Gameeditor is responsible to cleanup the decal data.
	std::map< std::string, olc::Decal* > m_decalDatabase;

private:

	// GUI
	void RenderGUI();
	void RenderDecalDatabase(bool open);
	void RenderMainMenu();
	
	// GAMEWORLD
	void RenderMainFrame();

	// UTIL
	bool LoadTilesetData(const std::string& tilesetpath, const std::string& datapath);
	void ToggleMenuItem(bool& item);
	void HandleInput();
	void UpdateVisibleRect();
	void RenderMapobject(Mapobject* object);
};
