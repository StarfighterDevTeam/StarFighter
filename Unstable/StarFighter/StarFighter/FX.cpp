#include "FX.h"

FX::FX(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int m_frameNumber, sf::Time m_duration) : Independant(position, speed, textureName, size, sf::Vector2f(size.x/2, size.y/2), m_frameNumber)
{
	exploding = true;
	deltaClockExploding.restart();
	duration = m_duration;
	visible = true;
	isOnScene = true;
	collider_type = IndependantType::Neutral;
	frameNumber = m_frameNumber;
}

void FX::update(sf::Time deltaTime, sf::Vector2f polarOffset[MovePatternType::NBVAL_MovePattern])
{
	FX::update(deltaTime);
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


FX* FX::Clone()
{
	return new FX(this->getPosition(), this->speed, this->textureName, this->m_size, this->frameNumber, this->duration);
}