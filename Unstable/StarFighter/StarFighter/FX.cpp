#include "FX.h"

FX::FX(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int m_frameNumber, sf::Time m_duration) : Independant(position, speed, textureName, size, sf::Vector2f(size.x/2, size.y/2), m_frameNumber)
{
	this->exploding = true;
	this->deltaClockExploding.restart();
	this->duration = m_duration;
}

void FX::update(sf::Time deltaTime)
{
	Independant::update(deltaTime);

	if (deltaClockExploding.getElapsedTime() > duration)
	{
		exploding = false;
	}

	if (!exploding)
	{
		this->setVisible(false);
		this->isOnScene = false;
	}
}