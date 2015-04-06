#include "Portal.h"

Portal::Portal(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : Independant(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	DontGarbageMe = true;
	state = PortalState::PortalOpen;
}

void Portal::update(sf::Time deltaTime, float hyperspeedMultiplier)
{
	if (!this->GarbageMe)
	{
		//update animations (transitioning frop opening to open and from closing to close)
		if (this->currentAnimationIndex == PortalAnimation::PortalOpening && this->m_currentFrame + 2 > this->currentAnimation->getSize())
		{
			this->setAnimationLine(PortalAnimation::PortalOpenIdle, false);
			
		}
		else if (this->currentAnimationIndex == PortalAnimation::PortalClosing && this->m_currentFrame + 2 > this->currentAnimation->getSize())
		{
			this->setAnimationLine(PortalAnimation::PortalCloseIdle, false);

		}
		else if (this->currentAnimationIndex == PortalAnimation::PortalCloseIdle && this->state == PortalState::PortalInvisible)
		{
			this->visible = false;
		}

		AnimatedSprite::update(deltaTime);
	}
}

void Portal::Open()
{
	if (this->currentAnimationIndex != PortalAnimation::PortalOpening && this->currentAnimationIndex != PortalAnimation::PortalOpenIdle)
	{
		this->setAnimationLine(PortalAnimation::PortalOpening, false);
	}
}

void Portal::Close()
{
	if (this->currentAnimationIndex != PortalAnimation::PortalClosing && this->currentAnimationIndex != PortalAnimation::PortalCloseIdle)
	{
		this->setAnimationLine(PortalAnimation::PortalClosing, false);
	}
}