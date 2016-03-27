#include "Shop.h"

Shop::Shop(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int m_frameNumber, int m_animationNumber) : GameObject(position, speed, textureName, size, origin, m_frameNumber, m_animationNumber)
{
	m_DontGarbageMe = true;
	m_level = 1;
}

Shop::~Shop()
{
	size_t itemsVectorSize = m_items.size();
	for (size_t i = 0; i < itemsVectorSize; i++)
	{
		if (m_items[i])
		{
			delete m_items[i];
			m_items[i] = NULL;
		}
	}
	m_items.clear();
}

void Shop::update(sf::Time deltaTime, float hyperspeedMultiplier)
{
	AnimatedSprite::update(deltaTime);
}