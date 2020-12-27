#include "Stroboscopic.h"

extern Game* CurrentGame;

using namespace sf;

Stroboscopic::Stroboscopic(sf::Time decade_time, GameObject* parent, int max_alpha) : GameObject(parent->getPosition(), sf::Vector2f(0, 0), parent->m_textureName, parent->m_size, sf::Vector2f(parent->m_size.x/2, parent->m_size.y/2), parent->m_frameNumber, parent->m_animationNumber)
{
	m_collider_type = BackgroundObject;
	m_layer = PlayerStroboscopicLayer;

	m_alpha = max_alpha;
	m_decay_time = decade_time;

	//getting a one-framed animation (the current frame and animation)
	Animation* anim = new Animation();
	anim->setSpriteSheet(*this->m_defaultAnimation.getSpriteSheet());
	setAnimationLine(parent->m_currentAnimationIndex);
	anim->addFrame(m_currentAnimation->getFrame(parent->m_currentFrame));

	m_currentAnimation = anim;
	play(*m_currentAnimation);

	setRotation(parent->getRotation());
}

Stroboscopic::~Stroboscopic()
{

}

void Stroboscopic::update(sf::Time deltaTime, float hyperspeedMultiplier)
{
	Uint8 decay = (Uint8)(ceil(deltaTime.asSeconds() * 255 / m_decay_time.asSeconds()));
	if (m_alpha - decay > 0)
	{
		m_alpha -= decay;
	}
	else
	{
		m_alpha = 0;
		m_garbageMe = true;
	}

	setColor(Color(255, 255, 255, m_alpha));

	AnimatedSprite::update(deltaTime);
}