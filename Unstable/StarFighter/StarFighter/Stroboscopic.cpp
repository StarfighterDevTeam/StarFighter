#include "Stroboscopic.h"

extern Game* CurrentGame;

using namespace sf;

Stroboscopic::Stroboscopic(sf::Time decade_time, GameObject* parent) : GameObject(parent->getPosition(), sf::Vector2f(0, 0), parent->m_textureName, parent->m_size, sf::Vector2f(parent->m_size.x/2, parent->m_size.y/2), parent->m_frameNumber, parent->m_animationNumber)
{
	m_alpha = 255;
	m_decade_time = decade_time;

	//getting a one-framed animation (the current frame and animation)
	Animation* anim = new Animation();
	anim->setSpriteSheet(*this->defaultAnimation.getSpriteSheet());
	setAnimationLine(parent->currentAnimationIndex);
	anim->addFrame(currentAnimation->getFrame(parent->m_currentFrame));

	this->currentAnimation = anim;
	this->play(*currentAnimation);

	this->setRotation(parent->getRotation());
}

Stroboscopic::~Stroboscopic()
{
	m_alpha = 255;
}

void Stroboscopic::update(sf::Time deltaTime)
{
	Uint8 decade_ = (Uint8)(ceil(deltaTime.asSeconds() * 255 / m_decade_time.asSeconds()));
    if (m_alpha - decade_ > 0)
	{
		m_alpha -= decade_;
	}
	else
	{
		m_alpha = 0;
		GarbageMe = true;
	}

	setColor(Color(255, 255, 255, m_alpha));

	AnimatedSprite::update(deltaTime);
}