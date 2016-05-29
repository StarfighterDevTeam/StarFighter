#include "Building.h"

extern Game* CurrentGame;

using namespace sf;

//BUILDING
Building::Building(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	m_stock = 0;
	m_stock_max = 0;
	m_slots = 1;
}

Building::~Building()
{

}

Building* Building::CreateBuilding(string name)
{
	string textureName = (*CurrentGame).m_buildingConfig[name][BuildingData_TextureName];

	Building* new_building = new Building(sf::Vector2f(0, 0), sf::Vector2f(0, 0), textureName, sf::Vector2f(64, 64), sf::Vector2f(32, 32));

	new_building->m_display_name = (*CurrentGame).m_buildingConfig[name][BuildingData_Name];
	new_building->m_slots = (size_t)stoi((*CurrentGame).m_buildingConfig[name][BuildingData_Slots]);
	new_building->m_stock_max = (size_t)stoi((*CurrentGame).m_buildingConfig[name][BuildingData_Stock]);
	new_building->m_ore_exploitable = (*CurrentGame).m_buildingConfig[name][BuildingData_OreExploited];

	return new_building;
}

void Building::update(sf::Time deltaTime)
{
	GameObject::update(deltaTime);
}