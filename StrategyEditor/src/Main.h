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

		// Called once at the start, so create things here
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		SetDrawTarget((uint8_t)m_GameLayer);
		Clear(olc::BLANK);

		// called once per frame
		for (int x = 0; x < ScreenWidth(); x++)
			for (int y = 0; y < ScreenHeight(); y++)
				Draw(x, y, olc::Pixel(rand() % 256, rand() % 256, rand() % 256));


		DrawStringDecal(olc::vf2d(1200, 50), "FPS: " + std::to_string(GetFPS()));


		return true;
	}

	void DrawUI(void)
	{
		SetDrawTarget((uint8_t)m_GUILayer);
		Clear(olc::BLANK);


		static bool open = true;
		ImGui::ShowDemoWindow(&open);


		//This finishes the Dear ImGui and renders it to the screen
		pge_imgui.ImGui_ImplPGE_Render();
	}

private:
	olc::TileTransformedView tv;
	olc::imgui::PGE_ImGUI pge_imgui;
	int m_GUILayer;
	int m_GameLayer;
};
