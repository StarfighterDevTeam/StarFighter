#include "Shop.h"

extern Game* CurrentGame;

Shop::Shop(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int m_frameNumber, int m_animationNumber) : GameObject(position, speed, textureName, size, origin, m_frameNumber, m_animationNumber)
{
	m_collider_type = ShopObject;
	m_layer = PortalLayer;

	m_DontGarbageMe = true;
	m_level = 1;
}

Shop::~Shop()
{
	for (GameObject* item : m_items)
		if (item != NULL)
			delete item;
}

void Shop::update(sf::Time deltaTime, float hyperspeedMultiplier)
{
	AnimatedSprite::update(deltaTime);
}