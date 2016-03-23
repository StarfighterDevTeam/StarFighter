#include "Portal.h"

extern Game* CurrentGame;

Portal::Portal(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	m_DontGarbageMe = true;
	m_state = PortalOpen;
}

void Portal::update(sf::Time deltaTime, float hyperspeedMultiplier)
{
	if (!m_GarbageMe)
	{
		//update animations (transitioning frop opening to open and from closing to close)
		if (m_currentAnimationIndex == PortalAnimation::PortalOpening && m_currentFrame + 2 > m_currentAnimation->getSize())
		{
			setAnimationLine(PortalAnimation::PortalOpenIdle, false);
			
		}
		else if (m_currentAnimationIndex == PortalAnimation::PortalClosing && m_currentFrame + 2 > m_currentAnimation->getSize())
		{
			setAnimationLine(PortalAnimation::PortalCloseIdle, false);

		}
		else if (m_currentAnimationIndex == PortalAnimation::PortalCloseIdle && m_state == PortalInvisible)
		{
			m_visible = false;
		}

		AnimatedSprite::update(deltaTime);
	}
}

void Portal::Open()
{
	if (m_currentAnimationIndex != PortalAnimation::PortalOpening && m_currentAnimationIndex != PortalAnimation::PortalOpenIdle)
	{
		setAnimationLine(PortalAnimation::PortalOpening, false);

		(*CurrentGame).PlaySFX(SFX_PortalOpening);
	}
}

void Portal::Close()
{
	if (m_currentAnimationIndex != PortalAnimation::PortalClosing && m_currentAnimationIndex != PortalAnimation::PortalCloseIdle)
	{
		setAnimationLine(PortalAnimation::PortalClosing, false);
	}
}