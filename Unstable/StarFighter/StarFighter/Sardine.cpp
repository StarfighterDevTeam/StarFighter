#include "Sardine.h"

extern Game* CurrentGame;

using namespace sf;

Sardine::Sardine(SardineType type, GameObject* lane) : GameObject(sf::Vector2f(0, 0), sf::Vector2f(0, 0), "2D/sardine.png", sf::Vector2f(32, 32))
{
	m_type = type;
	m_lane = lane;
	m_position_offset = sf::Vector2f(0.f, 0.f);
}

void Sardine::update(sf::Time deltaTime)
{
	if (m_lane)
	{
		setPosition(sf::Vector2f(m_lane->getPosition().x + m_position_offset.x, getPosition().y + m_position_offset.y));
	}

	GameObject::update(deltaTime);
}