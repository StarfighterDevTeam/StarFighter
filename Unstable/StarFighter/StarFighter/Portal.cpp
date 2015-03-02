#include "Portal.h"

Portal::Portal(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber) : Independant(position, speed, textureName, size, origin, frameNumber)
{
	visible = false;
	DontGarbageMe = true;
	state = PortalState::PortalInvisible;
	setGhost(true);
	display_name = "0";
	destination_name = "0";
}

void Portal::update(sf::Time deltaTime)
{
	if (!this->GarbageMe)
	{
		AnimatedSprite::update(deltaTime);
	}
}