#include "Stroboscopic.h"

extern Game* CurrentGame;

using namespace sf;

Stroboscopic::Stroboscopic(sf::Time decade_time, GameObject* parent) : GameObject(parent->getPosition(), sf::Vector2f(0, 0), parent->m_textureName, parent->m_size, sf::Vector2f(parent->m_size.x/2, parent->m_size.y/2), parent->m_frameNumber, parent->m_animationNumber)
{
	m_alpha = 255;
	m_decay_time = decade_time;

	//getting a one-framed animation (the current frame and animation)
	Animation* anim = new Animation();
	anim->setSpriteSheet(*this->m_defaultAnimation.getSpriteSheet());
	setAnimationLine(parent->m_currentAnimationIndex);
	anim->addFrame(m_currentAnimation->getFrame(parent->m_currentFrame));

	this->m_currentAnimation = anim;
	this->play(*m_currentAnimation);

	this->setRotation(parent->getRotation());

	setColor(parent->m_color, true);
}

Stroboscopic::~Stroboscopic()
{
	m_alpha = 255;
}

void Stroboscopic::update(sf::Time deltaTime)
{
	Uint8 decay = (Uint8)(ceil(deltaTime.asSeconds() * 255 / m_decay_time.asSeconds()));
	if (m_alpha - decay > 0)
	{
		m_alpha -= decay;
	}
	else
	{
		m_alpha = 0;
		m_GarbageMe = true;
	}

	setColor(Color(m_color.r, m_color.g, m_color.b, m_alpha));

	AnimatedSprite::update(deltaTime);
}