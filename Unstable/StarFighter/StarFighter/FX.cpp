#include "FX.h"

extern Game* CurrentGame;

FX::FX(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int frameNumber, sf::Time duration, int animationNumber) : GameObject(position, speed, textureName, size, sf::Vector2f(size.x / 2, size.y / 2), frameNumber, animationNumber)
{
	//deltaClockExploding.restart();
	m_duration = duration;
	m_visible = true;
	m_isOnScene = true;
	m_collider_type = Neutral;
}

void FX::update(sf::Time deltaTime)
{
	static sf::Vector2f newposition, offset, newspeed;
	newspeed = m_speed;

	//Basic movement (initial vector)
	newposition.x = this->getPosition().x + (newspeed.x)*deltaTime.asSeconds();
	newposition.y = this->getPosition().y + (newspeed.y)*deltaTime.asSeconds();

	this->setPosition(newposition.x, newposition.y);

	if (m_deltaClockExploding.getElapsedTime() > sf::seconds(TIME_BETWEEN_ANIMATION_FRAMES*m_frameNumber)) 
	{
		m_visible = false;
		m_GarbageMe = true;
	}

	AnimatedSprite::update(deltaTime);
}


FX* FX::Clone()
{
	FX* new_FX = new FX(this->getPosition(), this->m_speed, this->m_textureName, this->m_size, this->m_frameNumber, this->m_duration);
	return new_FX;
}

Aura::Aura(GameObject* target, std::string textureName, sf::Vector2f size, int frameNumber, int animationNumber) : FX(target->getPosition(), sf::Vector2f(0, 0), textureName, size, frameNumber, sf::seconds(0), animationNumber)
{
	m_target = target;
	m_visible = true;
	m_isOnScene = true;
	m_offset = sf::Vector2f(0, 0);
}

void Aura::Init(std::string textureName, sf::Vector2f size, int frameNumber)
{
	m_textureName = textureName;
	m_size = size;
	m_frameNumber = frameNumber;
}

void Aura::update(sf::Time deltaTime)
{
	static sf::Vector2f newposition, newoffset;
	newoffset = m_offset;
	newposition.x = m_target->getPosition().x + newoffset.x;
	newposition.y = m_target->getPosition().y + newoffset.y;
	setPosition(newposition.x,newposition.y);

	AnimatedSprite::update(deltaTime);
	
	//if (m_visible)
	//{
	//	AnimatedSprite::update(deltaTime);
	//}
	//else
	//{
	//	//if the Aura is not visible, we don't display it and reset its animation to frame 0
	//	m_currentFrame = 0;
	//}
}

Aura* Aura::Clone()
{
	Aura* new_aura = new Aura(this->m_target, this->m_textureName, this->m_size, this->m_frameNumber);

	return new_aura;
}

FakeShip::FakeShip(GameObject* m_target, std::string textureName, sf::Vector2f size, int m_frameNumber, int m_animationNumber) : Aura(m_target, textureName, size, m_frameNumber, m_animationNumber)
{

}

void FakeShip::update(sf::Time deltaTime)
{
	assert(m_target != NULL);

	if (m_target->m_currentAnimationIndex != m_currentAnimationIndex)
	{
		setAnimationLine(m_target->m_currentAnimationIndex, true);
	}

	Aura::update(deltaTime);
}

void FakeShip::PlayStroboscopicEffect(Time effect_duration, Time time_between_poses)
{
	if (m_stroboscopic_effect_clock.getElapsedTime().asSeconds() > time_between_poses.asSeconds())
	{
		Stroboscopic* strobo = new Stroboscopic(effect_duration, this);
		(*CurrentGame).addToScene(strobo, PlayerStroboscopicLayer, BackgroundObject);

		m_stroboscopic_effect_clock.restart();
	}
}