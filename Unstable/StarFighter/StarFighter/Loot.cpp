#include "Loot.h"

Loot::Loot (sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size) : Independant(position, speed, textureName, size)
{
	 visible = true;
	 isOnScene = true;
	 collider_type = IndependantType::LootObject;
}


void Loot::update(sf::Time deltaTime, float angle_rad)
{
	Independant::update(deltaTime, angle_rad);
}

