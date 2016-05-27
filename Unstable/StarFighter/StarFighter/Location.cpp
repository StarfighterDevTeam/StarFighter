#include "Location.h"

extern Game* CurrentGame;

using namespace sf;

//LOCATION
Location::Location(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	m_type = LocationType_None;
}

Location::~Location()
{

}

void Location::update(sf::Time deltaTime)
{
	GameObject::update(deltaTime);
}

//ORE FIELD
OreField::OreField(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : Location(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	m_type = LocationType_OreField;

	for (size_t i = 0; i < NBVAL_OreTypes; i++)
	{
		m_drill_sucess_rates[i] = 0;
	}

	m_min_ore_weight = 0;
}

OreField::~OreField()
{

}

void OreField::update(sf::Time deltaTime)
{
	Location::update(deltaTime);
}

size_t OreField::GetLightestOreWeight()
{
	int min = -1;
	for (size_t i = 0; i < NBVAL_OreTypes; i++)
	{
		if (m_drill_sucess_rates[i] > 0)
		{
			if (stoi((*CurrentGame).m_oreConfig[i][OreData_Weight]) < min || min < 0)
			{
				min = stoi((*CurrentGame).m_oreConfig[i][OreData_Weight]);
			}
		}
	}

	return min < 0 ? 0 : (size_t)min;
}