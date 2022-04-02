#include "Main.h"

// IMGUI STATICS
static bool m_bDecalDatabaseOpen = true;

void MainRender(GameEditor* editor)
{
	editor->OnUserUpdate((float)1/ 60);
}
void GameEditor::RenderGUI()
{
	ImGui::Begin("DecalDatabase", &m_bDecalDatabaseOpen);
	for (auto& pair : m_decalDatabase)
	{
		ImGui::ImageButton((ImTextureID)pair.second->id, { 128, 128 });
	}
	ImGui::End();
}
void GameEditor::RenderMainFrame()
{

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
			printf("Load: \"%s\" / \"%s\" .png \n", tilesetpath.c_str(), name.c_str());
			
			olc::Sprite sprite(tilesetpath + "/" + name + ".png");
			auto decal = new olc::Decal(&sprite);
			
			m_decalDatabase.try_emplace(name, decal);
		}
	}
	printf("Loaded tilecount: %zu\n", m_decalDatabase.size());
	return true;
}


int main()
{
	GameEditor demo;
	if (demo.Construct(1360, 768, 1, 1))
		demo.Start();

	return 0;
}