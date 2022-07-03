#pragma once

#include "olcPixelGameEngine.h"
#include "SoundSystem.h"

#include <vector>
#include <map>

struct Component
{
	Component(std::string name): m_name(name) {}
	virtual void Update(){}
	std::string m_name;
};

struct ComponentSprite : public Component
{
	ComponentSprite(std::string decal, std::string layer) : Component("Sprite")
	{
		m_decal = decal;
		m_layer = layer;
		m_width = 1;
		m_height = 1;
	}

	void Update() override{}

	std::string m_decal;
	std::string m_layer;
	int m_width, m_height;
};

struct ComponentCity : public Component
{
	ComponentCity(const std::string& name, int x, int y) : Component(name), m_centerx(x), m_centery(y) {}

	// Where Buildings belonging to this city can be build by a player
	// owning this city.
	void AddBuildingSlot(int x, int y)
	{ 
		if (HasBuildingSlot(x, y)) return;
		m_buildingSlots.push_back({ x, y }); 
	}
	
	// Territory belonging to this city.
	void AddTerritory(int x, int y)
	{ 
		if (HasTerritory(x, y)) return;
		m_territory.push_back({ x, y }); 
	}

	void RemoveBuildingSlot(int x, int y);
	void RemoveTerritory(int x, int y);

	int m_centerx;
	int m_centery;

	std::vector< std::pair< int, int > > m_buildingSlots;
	std::vector< std::pair< int, int > > m_territory;

	std::string m_prefabFilepath;

private:

	bool HasBuildingSlot(int x, int y);
	bool HasTerritory(int x, int y);
};

struct ComponentTownhall : public ComponentCity
{
	ComponentTownhall(int x, int y) : ComponentCity("Townhall", x, y){}
};
struct ComponentFort : public ComponentCity
{
	ComponentFort(int x, int y) : ComponentCity("Fort", x, y){}
};

struct ComponentUnit : public Component
{
	ComponentUnit(): Component("Unit") {}

	std::string m_prefabFilepath;

};

struct ComponentSound : public Component
{
	ComponentSound(float radius, const std::string& sound_name) : Component("Sound"), m_radius(radius)
	{
		m_soundName = sound_name;
	}
	ComponentSound(float radius, float r, float g, float b, float a, const std::string& sound_name) : Component("Sound"), m_radius(radius), r(r), g(g), b(b), a(a)
	{
		m_soundName = sound_name;
		m_soundChannelGroup = "Master";
	}
	ComponentSound(float radius, float r, float g, float b, float a, const std::string& sound_name, const std::string& sound_source_name, const std::string& channel_group) : Component("Sound"), m_radius(radius), r(r), g(g), b(b), a(a)
	{
		m_soundName = sound_name;
		m_soundSourceName = sound_source_name;
		m_soundChannelGroup = channel_group;
	}

	/// @brief The Sound Source max distance. Used in FMOD::Sound::set3DMinMaxDistance().
	float m_radius;

	/// @brief Name of the Sound File without extensions. E.g. atmosphere_1 or cicada_2. Non unique.
	std::string m_soundName;
	
	/// @brief Name of the SoundChannel object. Stored in SoundSystem and searched for in GetSound(), thus unique.
	std::string m_soundSourceName;
	
	/// @brief Name of the FMOD::ChannelGroup in which this sound is intended to play. Non unique.
	std::string m_soundChannelGroup;
	float r, g, b, a;
};

struct Entity
{
	Entity(const std::string& entity_name) : m_name(entity_name){}
	Entity() { m_name = "none"; }

	void Update();

	bool Dirty() { return m_dirty; }
	void Dirty(bool v) { m_dirty = v; }
	
	Component* Get(std::string component);
	template < typename T >
	T* Get(std::string component)
	{
		if(Has(component))
		{
			return reinterpret_cast<T*>(Get(component));
		}
		return nullptr;
	}
	
	bool Has(std::string component);
	
	void Add(Component* c, std::string name) { m_components.emplace(name, c); }
	void Add(Entity* e) { m_children.push_back(e); }

	bool m_dirty;

	std::string m_name;
	float m_positionx;
	float m_positiony;

	Entity* m_parent;
	std::vector< Entity* > m_children;

	std::map< std::string, Component* > m_components;
};