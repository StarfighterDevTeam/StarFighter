#include "Marker.h"

extern Game* CurrentGame;

using namespace sf;

Marker::Marker(GameObject* target) : GameObject(sf::Vector2f(0, 0), sf::Vector2f(0, 0), "2D/marker.png", sf::Vector2f(20, 40))
{
	m_target = target;
	m_visible = false;
}

Marker::~Marker()
{

}

void Marker::Update(sf::Time deltaTime)
{
	setPosition(sf::Vector2f(200, m_size.y * 0.5 + 10));

	if (m_frameNumber > 1)
	{
		AnimatedSprite::Update(deltaTime);
	}
}