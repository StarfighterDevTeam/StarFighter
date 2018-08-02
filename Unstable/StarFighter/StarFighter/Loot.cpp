#include "Loot.h"

extern Game* CurrentGame;

using namespace sf;

Loot::Loot(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	m_melee_range_bonus = sf::Vector2f(50, 0);
}

void Loot::update(sf::Time deltaTime)
{
	AnimatedSprite::update(deltaTime);
}