#include "Background.h"

Portal::Portal(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber) : Independant(position, speed, textureName, size, origin, frameNumber)
{
	visible = true;
	DontGarbageMe = true;
}

Background::Background(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, Directions direction) : Independant(position, speed, textureName, size, sf::Vector2f(size.x / 2, size.y / 2))
{
	visible = true;
	for (int i = 0; i < Directions::NO_DIRECTION; i++)
	{
		this->portals[(Directions)i] = new Portal(position, speed, PORTAL_TEXTURE_NAME, sf::Vector2f(PORTAL_WIDTH, PORTAL_HEIGHT), sf::Vector2f(PORTAL_WIDTH / 2, PORTAL_HEIGHT / 2), 1);

		//applying offset respect to the center of the background, depending on the direction
		float bg_size = Independant::getSize_for_Direction(direction, size).y;
		this->portals[(Directions)i]->offset = Independant::getSpeed_for_Scrolling(direction, -bg_size / 2);
		this->portals[(Directions)i]->setPosition((this->portals[(Directions)i]->getPosition().x + this->portals[(Directions)i]->offset.x), 
			this->portals[(Directions)i]->getPosition().y + this->portals[(Directions)i]->offset.y);
	}
}

void Background::update(sf::Time deltaTime)
{
	//if not disappeared, move it
	if (!this->GarbageMe)
	{
		static sf::Vector2f newposition;

		newposition.x = this->getPosition().x + (this->speed.x)*deltaTime.asSeconds();
		newposition.y = this->getPosition().y + (this->speed.y)*deltaTime.asSeconds();

		this->setPosition(newposition.x, newposition.y);

		//portals follow the background
		for (int i = 0; i < Directions::NO_DIRECTION; i++)
		{
			this->portals[(Directions)i]->setPosition(newposition.x + this->portals[(Directions)i]->offset.x, newposition.y + this->portals[(Directions)i]->offset.y);
		}

		AnimatedSprite::update(deltaTime);
	}
}

void Background::Garbage()
{
	this->GarbageMe = true;
	for (int i = 0; i < Directions::NO_DIRECTION; i++)
	{
		this->portals[(Directions)i]->visible = false;
		this->portals[(Directions)i]->DontGarbageMe = false;
		this->portals[(Directions)i]->GarbageMe = true;
	}
}