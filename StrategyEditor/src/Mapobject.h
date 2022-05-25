#pragma once

#include "olcPixelGameEngine.h"

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
};


struct Entity
{

	void Update();
	bool Dirty() { return m_dirty; }
	void Dirty(bool v) { m_dirty = v; }
	Component* Get(std::string component);
	template < typename T >
	T* Get(std::string component)
	{
		return reinterpret_cast<T*>(Get(component));
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