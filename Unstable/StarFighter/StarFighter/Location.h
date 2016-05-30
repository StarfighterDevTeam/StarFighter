#ifndef LOCATION_H_INCLUDED
#define LOCATION_H_INCLUDED

#include "Ore.h"
#include "Building.h"

enum LocationType
{
	LocationType_None,
	LocationType_OreField,
	LocationType_Planet,
};

class Location : public GameObject
{
public :
	Location() {};
	Location(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	virtual ~Location();
	void update(sf::Time deltaTime) override;

	string GetBestPropulsionAvailable();
	virtual bool CanSupplyFuel();

	string m_display_name;
	LocationType m_type;

	size_t m_stock;
	size_t m_stock_max;
	map<string, size_t> m_ores_stored;
	bool m_fuel_refill;
};

class Planet : public Location
{
public:
	Planet() {};
	Planet(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	virtual ~Planet();
	void update(sf::Time deltaTime) override;

	bool Build(string name, bool ignore_cost);
	size_t GetNbSlotsTaken();

	Ore* GetRandomOre();
	void Harvest();
	bool Stock(Ore* ore);
	virtual bool CanSupplyFuel();

	map<string, float> m_drill_sucess_rates;
	size_t m_building_slots;
	vector<Building*> m_buildings;
};

class OreField : public Location
{
public:
	OreField() {};
	OreField(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	virtual ~OreField();
	void update(sf::Time deltaTime) override;

	size_t GetLightestOreWeight();

	size_t m_drill_slots;
	map<string, float> m_drill_sucess_rates;
	size_t m_min_ore_weight;
};

#endif // LOCATION_H_INCLUDED
