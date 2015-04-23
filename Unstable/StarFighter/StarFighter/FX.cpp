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

void FX::update(sf::Time deltaTime, float hyperspeedMultiplier)
{
	static sf::Vector2f newposition, offset, newspeed;
	newspeed = this->speed;

	if (hyperspeedMultiplier > 1)
	{
		newspeed.x += Independant::getSpeed_for_Scrolling((*CurrentGame).direction, (hyperspeedMultiplier - 1) * (*CurrentGame).vspeed).x;
		newspeed.y += Independant::getSpeed_for_Scrolling((*CurrentGame).direction, (hyperspeedMultiplier - 1) * (*CurrentGame).vspeed).y;
	}
	else if (hyperspeedMultiplier < 1)
	{
		newspeed.x = this->speed.x * hyperspeedMultiplier;
		newspeed.y = this->speed.y * hyperspeedMultiplier;
	}

	this->setGhost(hyperspeedMultiplier > 1.0f);

	//Basic movement (initial vector)
	newposition.x = this->getPosition().x + (newspeed.x)*deltaTime.asSeconds();
	newposition.y = this->getPosition().y + (newspeed.y)*deltaTime.asSeconds();

	this->setPosition(newposition.x, newposition.y);

	if (deltaClockExploding.getElapsedTime() > sf::seconds(TIME_BETWEEN_ANIMATION_FRAMES*this->frameNumber)) 
	{
		this->visible = false;
		this->GarbageMe = true;
	}

	AnimatedSprite::update(deltaTime);
}


FX* FX::Clone()
{
	FX* new_FX = new FX(this->getPosition(), this->speed, this->textureName, this->m_size, this->frameNumber, this->duration);
	new_FX->display_name = this->display_name;
	return new_FX;
}

Aura::Aura(Independant* m_target, std::string textureName, sf::Vector2f size, int m_frameNumber, int m_animationNumber) : FX(m_target->getPosition(), sf::Vector2f(0, 0), textureName, size, m_frameNumber, sf::seconds(0), m_animationNumber)
{
	this->target = m_target;
	visible = true;
	isOnScene = true;
	offset = sf::Vector2f(0, 0);
}

void Aura::Init(std::string m_textureName, sf::Vector2f size, int m_frameNumber)
{
	this->textureName = m_textureName;
	this->m_size = size;
	this->frameNumber = m_frameNumber;
}

void Aura::update(sf::Time deltaTime, float hyperspeedMultiplier)
{
	static sf::Vector2f newposition, newoffset;
	newoffset = getSpeed_for_Direction((*CurrentGame).direction, this->offset);
	newposition.x = target->getPosition().x + newoffset.x;
	newposition.y = target->getPosition().y + newoffset.y;
	this->setPosition(newposition.x,newposition.y);

	if (visible)
	{
		AnimatedSprite::update(deltaTime);
	}
	else
	{
		//if the Aura is not visible, we don't display it and reset its animation to frame 0
		this->m_currentFrame = 0;
	}
	
}

Aura* Aura::Clone()
{
	Aura* new_aura = new Aura(this->target, this->textureName, this->m_size, this->frameNumber);
	new_aura->display_name = this->display_name;
	return new_aura;
}

FakeShip::FakeShip(Independant* m_target, std::string textureName, sf::Vector2f size, int m_frameNumber, int m_animationNumber) : Aura(m_target, textureName, size, m_frameNumber, m_animationNumber)
{

}

void FakeShip::update(sf::Time deltaTime, float hyperspeedMultiplier)
{
	if (this->target->currentAnimationIndex != this->currentAnimationIndex)
	{
		this->setAnimationLine(this->target->currentAnimationIndex, true);
	}

	//Ghost while hyperspeeding
	//this->setGhost(hyperspeedMultiplier > 1.0f);

	Aura::update(deltaTime, hyperspeedMultiplier);
}

bool FakeShip::GetLoot(Independant& independant)
{
	return this->target->GetLoot(independant);
}