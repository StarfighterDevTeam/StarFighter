#include "Shop.h"

Shop::Shop(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int m_frameNumber, int m_animationNumber) : Independant(position, speed, textureName, size, origin, m_frameNumber, m_animationNumber)
{
	DontGarbageMe = true;
}

void Shop::update(sf::Time deltaTime, float hyperspeedMultiplier)
{
	AnimatedSprite::update(deltaTime);
}