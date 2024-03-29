/*
* Milestones:
*			 Version 0.1.x & 0.2.x complete: (Bootstrapping)
*				 o Basic map creation, import and export.
*				 o Editor setup.
* 
*			 Version 0.3.x complete: (Audio Editor)
*				 o Defining audio properties intended for FMOD inside the map data.
*				 o Live editing and checking of audio in the map (the feel and mood, different settings etc)
* 
*			 Version 0.4.x complete: (Unit Editor)
*				 - Prerequisites: Add Animated Sprites to OLC.
*				 o Create and define a Unit-"Scene" for a unit type.
*				 o Define out of which elements the scene consists.
*				 o Define the positions and other properties of those elements.
*				 o Place those Unit-"Scene"-Entities as units into the map and preview them in real time.
*					 (These units are the units which can be recruited by a player and commanded by him.
*					  Thus we create here generally units which we can save and export as an independent unit
*					  not bound by any specific map)
* 
*			 Version 0.5.x complete: (AI Editor)
*				 o Creation and editing of different Navigation Meshes ready to be imported by the engine.
*				 o Creation and editing of "Script"-Entities (they will not be executed inside the editor),
*					written and LUA and loaded into the client.
*					(think this through for security, having a LUA file in clients reach sounds not secure,
*					however adding script entities to the map is awesome)
* 
*					
* 
* 
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
* 12.06.2022 - Version 0.3.2 Patch complete:
*			 o SoundChannel now playing if their hearable radius encompasses the camera (listener) position.
*			 o Integration of FMOD with the Editor:
*				 - Fixing the positioning of the sound in 3D space relative to the camera (listener).
*				 - Defining the 3D max distance of a sound source.
*			 o Quality of life improvements:
*				 - Better UI visibility whether a sound source is played and in which mode.
*				 - Increased the draw distance by 10 map tiles.
*			 o ChannelGroup Control now creates an FMOD representation and allows for volume customization.
* 
* 28.06.2022 - Version 0.4.0 complete:
*			 o Layout Template Editor for unit layout template creation
*			 o Unit Prefab Editor:
*				 - Editing stats of a unit like Attack values and Gold cost
*				 - Adding starting statuses and abilities
*			 o Exporting / Importing allows for quick editing and creation
* 
* 04.07.2022 - Version 0.4.1 Patch complete:
*			 o Prefab Caching: Quick load and re-edit
*			 o Create Unit and Building Prefab and distinguish them (Prefab self tells what kind it is).
*			 o Export ComponentUnit and  export ComponentTownhall/Fort re-factored, it stores now what it is (Unit or Building) and path to prefab.
*			 o Placing Prefabs on the map by selecting them from the PrefabDatabase (otherwise the objects are just sprites basically).
*
* 04.07.2022 - Version 0.4.2 Patch complete:
*			 o Building Prefab editing and exporting.
*			 o Map Data Cache: Quick load and re-edit already created maps.
* 
* 05.07.2022 - Version 0.4.3 Patch complete:
*			 o Fixing miscellaneous bugs.
*			 o Removing already added Starting Status Effects and Abilities of Unit Prefabs.
*			 o For Unit Prefabs:
*				 - Whether its a ranged unit field.
*				 - Ranged Attack min and max values.
*				 - Ranged Attack Range min and max values.
*			 o Import/Export of Unit Prefabs updated.
*			 o Quick Save functionality for current edited Prefab.
* 
* 
* 09.07.2022 - Version 0.4.4 Patch complete:
*			 o Remove Sound System due to bad implementation and general issues.
* 
* 20.07.2022 - Version 0.4.5 Patch complete:
*			 o Add newly created Audio System.
* 
* 
* 24.07.2022 - version 0.5.0 complete:
*			 o Status Effect Editor:
*				 - Fast and easy creation, exporting and importing of Status Effects to be used in-game.
*				 - Caching, to quickly load and refactor already existing Status Effects.
*				 - Quick Save button; to save you from retyping the names every time.
*			 o Ability Editor:
*				 - Fast and easy creation, exporting and import of Abilities to be used in-game.
*				 - Caching, to quickly load and refactor already existing Abilities.
*				 - Quick Save button; to save you from retyping the names every time.
* 
* 
* 
* 
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
#define OLC_GFX_OPENGL33
#define OLC_PGEX_DEAR_IMGUI_IMPLEMENTATION
#include "olc/imgui_impl_pge.h"


#define EDITOR_MAJOR_VERSION 0
#define EDITOR_MINOR_VERSION 5
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
// AUDIO
#include "SoundSystem.h"
// STL
#include <map>
#include <vector>


#define MAX_MAPSIZE_X 256
#define MAX_MAPSIZE_Y 256
#define DEFAULT_DECAL_SIZE_X 128
#define DEFAULT_DECAL_SIZE_Y 128
#define DEFAULT_UNIT_DECAL_OFFSET_X -0.2f
#define DEFAULT_UNIT_DECAL_OFFSET_Y -0.4f
#define BIGGER_UNIT_DECAL_OFFSET_X -0.2f
#define BIGGER_UNIT_DECAL_OFFSET_Y -1.0f


#define DEFAULT_WIDGET_IMAGE_SIZE_X 16
#define DEFAULT_WIDGET_IMAGE_SIZE_Y 16


#define DEFAULT_MAP_HEIGHT_STEP 10.0f
#define DEFAULT_MIN_ZOOM_DISTANCE 1.0f
#define DEFAULT_MAX_ZOOM_DISTANCE 50.0f

#define DEFAULT_WINDOW_WIDTH 1600
#define DEFAULT_WINDOW_HEIGHT 900

// COMMON
#include "Mapobject.h"
using LayeredGameworld = std::map< std::string, std::vector< std::vector< Entity* > > >;
class GameEditor;




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

	Tree* GetNode(const std::string& name)
	{
		// Search in self.
		if (m_name.compare(name) == 0)
		{
			return this;
		}

		// Search in first grade children.
		for (auto& kid : m_children)
		{
			if (kid->m_name.compare(name) == 0) return kid;
		}

		// Search in deeper children.
		for(auto& kid: m_children)
		{
			auto tree = kid->GetNode(name);
			if (tree) return tree;
		}

		// Could not find it.
		return nullptr;
	}

	virtual Tree* Node(const std::string& name)
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
				LOG_DBG_INFO("[{:.4f}][RemoveNode] Remove First-Grade child node \"{}\" from \"{}\"!", APP_RUN_TIME, name, m_name);
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
		// Check for first grade children.
		for (int i = 0; i < node->m_children.size(); i++)
		{
			auto n = node->m_children[i];

			if (n->m_name.compare(name) == 0)
			{
				node->m_children.erase(node->m_children.begin() + i);
				LOG_DBG_INFO("[{:.4f}][RemoveChildFromTree] Remove Higher-Grade child node \"{}\" from \"{}\"!", APP_RUN_TIME, name, node->m_name);
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

		// We could not find any node with provided name.
		LOG_DBG_WARN("[{:.4f}][RemoveChildFromTree] Could not remove Higher-Grade child node \"{}\" from \"{}\"!", APP_RUN_TIME, name, node->m_name);
		return false;
	}
};


/// @brief Commonly used shapes in the Unit Layout Template editor.
struct SShape
{
	virtual int Type() = 0;
	virtual void Draw(GameEditor* editor) = 0;
	virtual std::string ElementType() = 0;
	virtual bool IsRootElement() = 0;

	virtual float PositionX() = 0;
	virtual float PositionY() = 0;
	virtual float WidthX() = 0;
	virtual float WidthY() = 0;
	virtual void SetElementType(const std::string&) = 0;

	virtual void SetAsRoot(bool) = 0;
};


struct SRectangle : public SShape
{
	SRectangle(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {}

	void Draw(GameEditor* editor) override final;
	int Type() override final { return type; }
	std::string ElementType() override final { return element_type; }
	bool IsRootElement() override final { return root_element; }
	float PositionX() { return x; }
	float PositionY() { return y; }
	void SetAsRoot(bool v) { root_element = v; }
	float WidthX() {return w;}
	float WidthY() { return h; }
	void SetElementType(const std::string& name) { element_type = name;}


	int type = 1;
	float x, y;
	float w, h;

	bool root_element = false;
	std::string element_type = "none";
};


/// @brief Datastructure holding information required to export a Prefab.
struct SPrefab
{
	std::string layout_template_name;
	std::string prefab_name;
	bool unit_prefab;
	bool building_prefab;
	int sight_range;
	bool can_detect_hidden_units;

	// For both sides.
	uint64_t health;
	uint64_t defense;
	uint64_t level;
	uint64_t gold_cost;
	std::string sprite;


	// Building Stats.
	// Exported only for Building Prefabs.
	uint64_t gold_production;
	uint64_t research_points_production;
	uint64_t visibility_distance_in_tiles;
	std::string predecessor_building_for_upgrade; // The same building but one level below. If level=1, then this is empty.
	

	// Unit Stats.
	// Exported only for Unit Prefabs.
	uint64_t action_points;
	uint64_t armor;
	uint64_t attack_min;
	uint64_t attack_max;
	bool can_attack_ranged;
	uint64_t ranged_attack_min;
	uint64_t ranged_attack_max;
	uint64_t ranged_range_min;
	uint64_t ranged_range_max;
	uint64_t movement_type;
	uint64_t race;
	int unit_progression_type;
	std::string building_name; // Requirement for producing.
	uint64_t building_level;   // Requirement for producing.
	std::vector< std::string > starting_status_vec;
	std::vector< std::string > abilities_vec;

	// If a units weapon is armor piercing, it ignores the enemies armor value.
	bool armor_piercing_weapon;

	// If a unit is anti cavalry it does extra damage against mounted units.
	bool anti_cavalry;
	int anti_cavalry_bonus = 0;

	// If a unit attacks from the side, it deals additional flanking bonus more damage.
	int flanking_bonus = 0;

	// If a unit attacks from behind, it deals additional backstab bonus more damage.
	int backstab_bonus = 0;
};


struct SStatusEffect
{
	std::string name;
	std::string displayName;
	std::string behaviorTreeImplName;
	std::string description;
	std::string timerType;

	int applicableTo;
	int applicationProbability;
	int minValue;
	int maxValue;

	int timerValue;

	std::string sprite;
};


struct SAbility
{
	std::string name;
	std::string displayName;
	std::string description;

	int minRange;
	int maxRange;
	bool triviallyRemovable;

	int applicableTo;
	bool usableOnSelf;
	bool usableOnFriendlies;
	bool usableOnEnemies;

	std::vector< std::string > appliedStatusEffectsOnUse;
};



// Utility. Create a hook function to actually call the GameEditors OnUserUpdate function.
void MainRender(GameEditor* editor);

class GameEditor : public olc::PixelGameEngine
{
	friend class SRectangle;
public:
	GameEditor() : pge_imgui(false)
	{
		sAppName = TITLE(EDITOR_TITLE_STRING, EDITOR_MAJOR_VERSION, EDITOR_MINOR_VERSION, EDITOR_PATCH_VERSION);
	}

public:
	bool OnUserCreate() override;
	bool OnUserUpdate(float fElapsedTime) override;
	bool OnUserDestroy() override final;

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
	std::map< std::string, olc::Decal* > m_statusEffectDecalDatabase;
	std::map< std::string, olc::Decal* > m_decalDatabase;
	std::vector< olc::Sprite* > m_spriteDatabase;
	std::map< std::string, std::pair< uint64_t, uint64_t > > m_decalSizeDatabase;

	// Editor specific decal database.
	std::map< std::string, olc::Decal* > m_editorDecalDatabase;
	std::vector< olc::Sprite* > m_editorSpriteDatabase;

	// Audio
	std::map< std::string, std::string > m_soundPathMap;


	// Permanent Editor specific layers which cannot be altered.
	std::vector< int > m_PermanentLayersVec;


	// Unit Layout Template Editor.
	/// @brief Current in work layout template. On creating a new one this one is deleted.
	std::vector< SShape* > m_currentLayoutTemplateVec;

	std::map< std::string, std::string > m_prefabDecalDatabase;
	std::map< std::string, std::string > m_prefabCacheMap;
	bool m_prefabCacheLoaded = false;

	// Status Effect Editor.
	std::map< std::string, std::string > m_statusEffectCacheMap;
	bool m_statusEffectCacheLoaded = false;

	// Abilities Editor
	std::map< std::string, std::string > m_abilityCacheMap;
	bool m_abilityCacheLoaded = false;


	// Map Data.
	std::map< std::string, std::string > m_mapDataCacheMap;
	bool m_mapDataCacheLoaded = false;


private:

	// GUI
	void RenderGUI();
	void RenderDecalDatabase();
	void RenderPrefabDatabase();
	void RenderMainMenu();
	void RenderLayerUI();
	void RenderDecalDatabase(const std::map< std::string, olc::Decal* >& db);
	void RenderEntityDatabase();
	void DisplayEntityEditor(Entity* e);
	

	// GUI UNIT EDITOR UTIL
	bool ExportUnitPrefab(const std::string& filepath, SPrefab* prefab);
	SPrefab* ImportUnitPrefab(const std::string& filepath);
	void DisplayUnitPrefabExportWindow(SPrefab* prefab);
	void DisplayUnitPrefabImportWindow();
	bool ImportUnitPrefabCache(const std::string& filepath);

	// Prefab Editor.
	void DisplayUnitEditor();
	void DisplayUnitEditorMainMenu();
	// Prefab Editor. Unit Section.
	void DisplayUnitEditorNameEdit();
	void DisplayUnitEditorLayoutTemplateNameEdit();
	void DisplayUnitEditorHealthEdit();
	void DisplayUnitEditorActionPointsEdit();
	void DisplayUnitEditorLevelEdit();
	void DisplayUnitEditorArmorEdit();
	void DisplayUnitEditorAttackEdit();
	void DisplayUnitEditorDefenseEdit();
	void DisplayUnitEditorMovementTypeEdit();
	void DisplayUnitEditorRaceEdit();
	void DisplayUnitEditorBuildingRequirementsEdit();
	void DisplayUnitEditorGoldCostEdit();
	void DisplayUnitEditorStartingStatusEdit();
	void DisplayUnitEditorAbilitiesEdit();
	void DisplayUnitEditorUnitSpriteEdit();
	void DisplayUnitEditorUnitSprite();
	void DisplayUnitEditorPrefabQuickLoadDropDown();
	// Prefab Editor. Building Section.
	void DisplayBuildingEditorNameEdit();
	void DisplayBuildingEditorLayoutTemplateEdit();
	void DisplayBuildingEditorDefenseEdit();
	void DisplayBuildingEditorProducerEdit();
	void DisplayBuildingEditorVisibilityEdit();
	void DisplayBuildingEditorRequirementsEdit();
	void DisplayPrefabEditorBuildingSpriteEdit();
	// Status Effect Editor.
	void DisplayStatusEffectEditor();
	void DisplayStatusEffectEditorMainMenu();
	void DisplayStatusEffectEditorIconSprite();
	void DisplayStatusEffectExportWindow(SStatusEffect* se);
	void DisplayStatusEffectImportWindow();
	void DisplayStatusEffectQuickLoadDropDown();
	bool ImportStatusEffectCache(const std::string& filepath);

	bool ExportStatusEffect(const std::string& filepath, SStatusEffect* se);
	SStatusEffect* ImportStatusEffect(const std::string& filepath);
	
	// Ability Editor
	void DisplayAbilityEditor();
	void DisplayAbilityEditorMainMenu();
	void DisplayAbilityEditorExportWindow(SAbility* abl);
	void DisplayAbilityEditorImportWindow();
	void DisplayAbilityAppliedStatusEffectsEditorWindow();
	void DisplayAbilityEditorQuickLoadDropDown();
	bool ImportAbilityCache(const std::string& filepath);
	bool ExportAbility(const std::string& filepath, SAbility* abl);
	SAbility* ImportAbility(const std::string& filepath);

	// General Editor Utilities.
	void DisplayEditFieldNumber(int& value, int min, int max, const char* field_name, const char* field_desc, ImGuiID slider_id, ImGuiID scalar_id);
	void DisplayEditFieldBoolean(bool& value, const char* field_name, const char* field_desc, ImGuiID checkbox_id);
	bool DisplayEditFieldString(char* string, int string_size, std::string& current_string, const char* field_name, const char* field_desc, ImGuiID text_input_id, ImGuiID text_input_ok_id);
	void DisplayEditFieldIcon(const char* field_name, const char* field_desc, std::map< std::string, olc::Decal* >& data_map, std::string& current_icon);




	// GUI UNIT TEMPLATE LAYOUT EDITOR
	void DisplayShapesWindow();
	void DisplayShapePropertiesEditor(SShape* shape);
	void DisplayShapePosition(SRectangle* rect);
	void DisplayShapeDimension(SRectangle* rect);
	void DisplayShapeElementType(SRectangle* rect);
	SShape* GetShapeAtMousePosition(float x, float y);
	void CreateShapeAtMousePosition(int shape_index, float x, float y);
	void DeleteShapeAtMousePosition(float x, float y);
	bool PointRectangleCollision(float p1, float p2, float x, float y, float w, float h);
	bool PointCircleCollision(float p1, float p2, float x, float y, float r);
	void DisplayAddingPrefabElementType();
	bool TryAddingPrefabElementType(const std::string& name);
	bool DisplaySavingPrefabLayoutElement();
	bool ExportPrefabLayoutTemplate(const std::string& filepath);
	bool DisplayLoadingPrefabLayoutElement();
	bool ImportPrefabLayoutTemplate(const std::string& filepath);

	void RenderMainFrameForUnitEditor();
	void RenderMainFrame();
	void UpdateEntities();

	// UTIL
	bool LoadTilesetData(const std::string& database, const std::string& tilesetpath, const std::string& datapath, bool verbose = false);
	bool LoadEditorGraphicalData();
	bool LoadAudioData(const std::string& filepath);
	void ToggleMenuItem(bool& item);
	void HandleInput();
	void UpdateVisibleRect();
	void RenderMapobject(Entity* object);
	Entity* CreateMapobject(uint64_t x, uint64_t y, std::string decal, bool unit, std::string name = "none");
	Entity* CreateMapobjectEx(uint64_t x, uint64_t y, std::string layer, std::string decal, std::string name = "none");
	Entity* CreatePrefabedMapobject(uint64_t x, uint64_t y, std::string layer, std::string decal, std::string name = "none");
	std::string CreateMapobjectName();
	bool IsMapobjectNameUsed(const std::string& name);
	void DeleteMapobject(Entity* object);
	std::string GetMapobjectNameAt(int x, int y, std::string layer);
	Entity* GetMapobjectAt(int x, int y, std::string layer);
	void SetDecalSize(const std::string& name, uint64_t w, uint64_t h);
	uint64_t GetDecalWidth(const std::string& name);
	uint64_t GetDecalHeight(const std::string& name);
	olc::Pixel GetRandomColor(uint64_t alpha = 255);
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
	bool DoesLayerExist(const std::string& layer_name);

	// UTIL ALGORITHM
	std::multimap< int, std::string, std::greater<int> > SortDescending(std::map< std::string, int >& map);
	std::map< int, std::string > SortAscending(std::map< std::string, int >& map);
	void CopyStringToCharArray(const std::string& s, char arr[], uint64_t arr_size);
	

	// UTIL SERIALIZATION/DESERIALIZATION
	// Game World and Map.
	bool ExportMapData(const std::string& filepath, const std::string& map_name);
	bool ImportMapData(const std::string& filepath);
	bool ImportMapDataCache(const std::string& filepath);
	void FreeMapData();
	void DisplayMapExportDialog();
	void DisplayMapImportDialog();

	// Import
	Entity* ImportEntity(tinyxml2::XMLElement* xml, const std::string& layer);
	void ImportEntityComponentFort(tinyxml2::XMLElement* xml, Entity* entity);
	void ImportEntityComponentTownhall(tinyxml2::XMLElement* xml, Entity* entity);
	void ImportEntityComponentUnit(tinyxml2::XMLElement* xml, Entity* entity);


	// Export
	void ExportEntity(tinyxml2::XMLElement* xml, Entity* entity);
	void ExportEntityComponentFort(tinyxml2::XMLElement* xml, Entity* entity);
	void ExportEntityComponentTownhall(tinyxml2::XMLElement* xml, Entity* entity);
	void ExportEntityComponentUnit(tinyxml2::XMLElement* xml, Entity* entity);


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