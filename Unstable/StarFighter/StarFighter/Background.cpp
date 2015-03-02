#include "Background.h"

Background::Background(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, Directions direction, float first_screen_offset) : Independant(position, speed, textureName, size, sf::Vector2f(size.x / 2, size.y / 2))
{
	visible = true;
	isOnScene = true;

	sf::Vector2f size_ = Independant::getSize_for_Direction(direction, size);
	this->setPosition_Y_for_Direction(direction, sf::Vector2f(size_.x / 2, (-size_.y / 2) + first_screen_offset), true);

	for (int i = 0; i < Directions::NO_DIRECTION; i++)
	{
		this->portals[(Directions)i] = new Portal(position, speed, PORTAL_TEXTURE_NAME, sf::Vector2f(PORTAL_WIDTH, PORTAL_HEIGHT), sf::Vector2f(PORTAL_WIDTH / 2, PORTAL_HEIGHT / 2), 1);
		sf::Vector2f bg_size = Independant::getSize_for_Direction((Directions)i, size);
		//applying offset respect to the center of the background, depending on the direction
		this->portals[(Directions)i]->offset = Independant::getSpeed_for_Scrolling((Directions)i, (-bg_size.y / 2) + (PORTAL_HEIGHT/2));
		this->portals[(Directions)i]->setPosition(this->getPosition().x + this->portals[(Directions)i]->offset.x, this->getPosition().y + this->portals[(Directions)i]->offset.y);

		//rotation
		this->portals[(Directions)i]->setRotation(Independant::getRotation_for_Direction((Directions)i));

		//direction
		this->portals[(Directions)i]->direction = (Directions)i;
	}

	this->SetPortalsState(PortalState::PortalInvisible);
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

void Background::SetPortalsState(PortalState m_state)
{
	for (int i = 0; i < Directions::NO_DIRECTION; i++)
	{
		switch (m_state)
		{
			case PortalState::PortalGhost:
			{
				this->portals[(Directions)i]->state = PortalState::PortalGhost;
				this->portals[(Directions)i]->visible = true;
				this->portals[(Directions)i]->setGhost(true);
				break;
			}

			case PortalState::PortalOpen:
			{
				this->portals[(Directions)i]->state = PortalState::PortalOpen;
				this->portals[(Directions)i]->visible = true;
				this->portals[(Directions)i]->setGhost(false);
				break;
			}

			case PortalState::PortalInvisible:
			{
				this->portals[(Directions)i]->state = PortalState::PortalInvisible;
				this->portals[(Directions)i]->visible = false;
				break;
			}
		}
	}
}