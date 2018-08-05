#include "Ammo.h"

extern Game* CurrentGame;

using namespace sf;

Ammo::Ammo(GameObject* owner, float speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(sf::Vector2f(0, 0), sf::Vector2f(0, 0), textureName, size, origin, frameNumber, animationNumber)
{
	m_ref_speed = speed;
	m_target = NULL;
	m_owner = owner;
	m_visible = false;
}

Ammo::Ammo(GameObject* owner, sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	m_target = NULL;
	m_owner = owner;
	m_visible = false;
}

Ammo::~Ammo()
{
	
}

void Ammo::update(sf::Time deltaTime)
{
	if (m_target)
	{
		SetSpeedForConstantSpeedToDestination(m_target->getPosition(), m_ref_speed);
	}

	GameObject::update(deltaTime);
}
