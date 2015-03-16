#include "FX.h"

extern Game* CurrentGame;

FX::FX(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int m_frameNumber, sf::Time m_duration, int m_animationNumber) : Independant(position, speed, textureName, size, sf::Vector2f(size.x / 2, size.y / 2), m_frameNumber, m_animationNumber)
{
	//deltaClockExploding.restart();
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
		// la duration ne sert à rien pour les explosions : on veut toujours 0,2sec x le nombre de frames. ça évite donc d'avoir à renseigner une duration correcte dans FX.csv
	{
		this->setVisible(false);
		this->isOnScene = false;
		this->GarbageMe = true;
	}
}


FX* FX::Clone()
{
	FX* new_FX = new FX(this->getPosition(), this->speed, this->textureName, this->m_size, this->frameNumber, this->duration);
	new_FX->display_name = this->display_name;
	return new_FX;
}

Aura::Aura(Independant* m_target, std::string textureName, sf::Vector2f size, int m_frameNumber, int m_animationNumber) : FX(m_target->getPosition(), sf::Vector2f(0, 0), textureName, size, m_frameNumber, sf::seconds(0), m_animationNumber)
{
	assert(m_target != NULL);
	this->target = m_target;
	visible = true;
	isOnScene = true;
}

void Aura::Init(std::string m_textureName, sf::Vector2f size, int m_frameNumber)
{
	this->textureName = m_textureName;
	this->m_size = size;
	this->frameNumber = m_frameNumber;
}

void Aura::update(sf::Time deltaTime)
{
	static sf::Vector2f newposition;
	newposition.x = target->getPosition().x;
	newposition.y = target->getPosition().y;
	this->setPosition(newposition.x,newposition.y);

	AnimatedSprite::update(deltaTime);
}

Aura* Aura::Clone()
{
	Aura* new_aura = new Aura(this->target, this->textureName, this->m_size, this->frameNumber);
	new_aura->display_name = this->display_name;
	return new_aura;
}

FakeShip::FakeShip(Independant* m_target, std::string textureName, sf::Vector2f size, int m_frameNumber, int m_animationNumber) : Aura(m_target, textureName, size, m_frameNumber, m_animationNumber)
{
	assert(m_target != NULL);
}

void FakeShip::update(sf::Time deltaTime)
{
	if (this->target->currentAnimationIndex != this->currentAnimationIndex)
	{
		this->setAnimationLine(this->target->currentAnimationIndex, true);
	}

	Aura::update(deltaTime);
}
