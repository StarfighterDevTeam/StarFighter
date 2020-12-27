#include "FX.h"

extern Game* CurrentGame;

FX::FX(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, bool permanent, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, sf::Vector2f(size.x / 2, size.y / 2), frameNumber, animationNumber)
{
	//deltaClockExploding.restart();
	m_duration = permanent == true ? 0 : frameNumber * TIME_BETWEEN_ANIMATION_FRAMES;
	m_visible = true;
	m_isOnScene = true;
	m_collider_type = BackgroundObject;
	m_layer = ExplosionLayer;
}

void FX::update(sf::Time deltaTime, float hyperspeedMultiplier)
{
	static sf::Vector2f newposition, offset, newspeed;
	newspeed = m_speed;

	float l_hyperspeedMultiplier = hyperspeedMultiplier < 1 ? hyperspeedMultiplier : 1;

	//slowmotion
	newspeed.y += (l_hyperspeedMultiplier - 1) * (*CurrentGame).m_vspeed;

	setGhost(hyperspeedMultiplier > 1.0f);

	//Basic movement (initial vector)
	newposition.x = this->getPosition().x + (newspeed.x)*deltaTime.asSeconds();
	newposition.y = this->getPosition().y + (newspeed.y)*deltaTime.asSeconds();

	setPosition(newposition.x, newposition.y);

	if (m_duration > 0)
	{
		m_duration -= deltaTime.asSeconds() * hyperspeedMultiplier;
		if (m_duration <= 0)
		{
			m_visible = false;
			m_garbageMe = true;
		}
	}

	AnimatedSprite::update(deltaTime);
}


FX* FX::Clone()
{
	FX* new_FX = new FX(this->getPosition(), this->m_speed, this->m_textureName, this->m_size, bool(this->m_duration == 0), this->m_frameNumber, this->m_animationNumber);
	new_FX->m_display_name = this->m_display_name;
	return new_FX;
}

// AURA
Aura::Aura(GameObject* target, std::string textureName, sf::Vector2f size, int frameNumber, int animationNumber) : FX(target->getPosition(), sf::Vector2f(0, 0), textureName, size, true, frameNumber, animationNumber)
{
	m_target = target;
	m_visible = true;
	m_isOnScene = true;
	m_offset = sf::Vector2f(0, 0);
	m_collider_type = BackgroundObject;
	m_layer = FakeShipLayer;
}

void Aura::Init(std::string textureName, sf::Vector2f size, int frameNumber)
{
	m_textureName = textureName;
	m_size = size;
	m_frameNumber = frameNumber;
}

void Aura::update(sf::Time deltaTime, float hyperspeedMultiplier)
{
	static sf::Vector2f newposition;

	float angle = m_target->getRotation() / 180 * M_PI;
	float offset_x = m_offset.x * cos(angle) - m_offset.y * sin(angle);
	float offset_y = m_offset.x * sin(angle) + m_offset.y * cos(angle);

	newposition.x = m_target->getPosition().x + offset_x;
	newposition.y = m_target->getPosition().y + offset_y;
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
	new_aura->m_display_name = this->m_display_name;

	return new_aura;
}

// FAKE SHIP

FakeShip::FakeShip(GameObject* target, std::string textureName, sf::Vector2f size, int frameNumber, int animationNumber) : Aura(target, textureName, size, frameNumber, animationNumber)
{
	m_collider_type = FakePlayerShip;
	m_layer = FakeShipLayer;
}

void FakeShip::update(sf::Time deltaTime, float hyperspeedMultiplier)
{
	assert(m_target != NULL);

	if (m_target->m_currentAnimationIndex != m_currentAnimationIndex)
	{
		setAnimationLine(m_target->m_currentAnimationIndex, true);
	}

	//Ghost while hyperspeeding
	//this->setGhost(hyperspeedMultiplier > 1.0f);

	Aura::update(deltaTime, hyperspeedMultiplier);
}

bool FakeShip::GetLoot(GameObject& GameObject)
{
	return m_target->GetLoot(GameObject);
}

void FakeShip::PlayStroboscopicEffect(Time effect_duration, Time time_between_poses, int max_alpha)
{
	if (m_stroboscopic_effect_clock.getElapsedTime().asSeconds() > time_between_poses.asSeconds())
	{
		Stroboscopic* strobo = new Stroboscopic(effect_duration, this, max_alpha);
		(*CurrentGame).addToScene(strobo, true);

		m_stroboscopic_effect_clock.restart();
	}
}