#ifndef LOCATION_H_INCLUDED
#define LOCATION_H_INCLUDED

#include "Ore.h"
#include "Building.h"
#include "StockEntity.h"
#include "Starship.h"

class Location : public StockEntity
{
public :
	Location() {};
	Location(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	virtual ~Location();
	void update(sf::Time deltaTime) override;

	virtual bool CanSupplyFuel();
	string GetBestPropulsionAvailable() override;

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
	bool Produce(string name, bool ignore_cost);
	size_t GetNbSlotsTaken();
	void Harvest();
	bool CanSupplyFuel() override;

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

	size_t m_drill_slots;
};

#endif // LOCATION_H_INCLUDED
