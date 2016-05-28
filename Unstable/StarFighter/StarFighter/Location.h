#ifndef LOCATION_H_INCLUDED
#define LOCATION_H_INCLUDED

#include "Ore.h"

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

	string m_display_name;
	LocationType m_type;

	size_t m_stock;
	size_t m_stock_max;
	vector<Ore*> m_ores_stored;
};

class Planet : public Location
{
public:
	Planet() {};
	Planet(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	virtual ~Planet();
	void update(sf::Time deltaTime) override;
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
	float m_drill_sucess_rates[NBVAL_OreTypes];
	size_t m_min_ore_weight;
};

#endif // LOCATION_H_INCLUDED
