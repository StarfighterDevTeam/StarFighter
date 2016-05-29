#include "Ore.h"

extern Game* CurrentGame;

using namespace sf;

Ore::Ore(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	
}

Ore::~Ore()
{

}

void Ore::update(sf::Time deltaTime)
{
	GameObject::update(deltaTime);
}

Ore* Ore::CreateOre(string name)
{
	string textureName = (*CurrentGame).m_oreConfig[name][OreData_TextureName];

	Ore* new_ore = new Ore(sf::Vector2f(0, 0), sf::Vector2f(0, 0), textureName, sf::Vector2f(100, 100), sf::Vector2f(50, 50));

	new_ore->m_display_name = (*CurrentGame).m_oreConfig[name][OreData_Name];
	new_ore->m_weight = (size_t)stoi((*CurrentGame).m_oreConfig[name][OreData_Weight]);
	new_ore->m_price = (size_t)stoi((*CurrentGame).m_oreConfig[name][OreData_Price]);
	new_ore->m_extraction_duration = atof((*CurrentGame).m_oreConfig[name][OreData_Extraction].c_str());

	return new_ore;
}