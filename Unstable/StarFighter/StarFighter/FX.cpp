#include "FX.h"

extern Game* CurrentGame;

Aura::Aura(GameObject* target, std::string textureName, sf::Vector2f size, int frameNumber, int animationNumber) : GameObject(sf::Vector2f(0, 0), sf::Vector2f(0, 0), textureName, size, sf::Vector2f(size.x/2, size.y/2), frameNumber, animationNumber)
{
	m_target = target;
	if (m_target)
		setPosition(m_target->getPosition());
}

void Aura::update(sf::Time deltaTime)
{
	if (m_target && visible)
	{
		setPosition(m_target->getPosition());
		AnimatedSprite::update(deltaTime);
	}
}