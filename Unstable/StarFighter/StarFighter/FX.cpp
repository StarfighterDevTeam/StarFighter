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

void FX::update(sf::Time deltaTime)
{
	Independant::update(deltaTime);

	//if (deltaClockExploding.getElapsedTime() > duration)
	if (deltaClockExploding.getElapsedTime() > sf::seconds(TIME_BETWEEN_ANIMATION_FRAMES*this->frameNumber)) 
		// la duration ne sert � rien pour les explosions : on veut toujours 0,2sec x le nombre de frames. �a �vite donc d'avoir � renseigner une duration correcte dans FX.csv
	{
		exploding = false;
	}

	if (!exploding)
	{
		this->setVisible(false);
		this->isOnScene = false;
		this->GarbageMe = true;
	}
}


FX* FX::Clone()
{
	return new FX(this->getPosition(), this->speed, this->textureName, this->m_size, this->frameNumber, this->duration);
}