#include "Loot.h"

Loot (sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size) : Independant(position, speed, textureName, size)
{
	 this->visible = true;
}


void Loot::update(sf::Time deltaTime)
{
	Independant::update(deltaTime);
}

