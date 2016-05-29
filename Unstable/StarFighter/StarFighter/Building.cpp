#include "Building.h"

extern Game* CurrentGame;

using namespace sf;

//BUILDING
Building::Building(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	m_stock = 0;
	m_stock_max = 0;
	m_slots = 1;
	m_can_extract_ore = false;
	m_extraction_duration_bonus = 0;
	m_current_extraction = NULL;
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
	new_building->m_can_extract_ore = stoi((*CurrentGame).m_buildingConfig[name][BuildingData_CanExtractOre]) == 1;
	new_building->m_extraction_duration_bonus = stof((*CurrentGame).m_buildingConfig[name][BuildingData_ExtractionDurationBonus]);

	return new_building;
}

void Building::update(sf::Time deltaTime)
{
	GameObject::update(deltaTime);
}

bool Building::Extract(Ore* ore)
{
	if (!ore)
	{
		return false;
	}

	if (!m_current_extraction)
	{
		m_current_extraction = ore;
	}

	//Extract
	return m_extraction_clock.getElapsedTime().asSeconds() > MaxBetweenValues(sf::Vector2f(m_current_extraction->m_extraction_duration - m_extraction_duration_bonus, 0));
}