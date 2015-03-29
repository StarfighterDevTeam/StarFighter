#include "Background.h"

extern Game* CurrentGame;

Background::Background(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, Directions direction, float first_screen_offset) : Independant(position, speed, textureName, size, sf::Vector2f(size.x / 2, size.y / 2))
{
	
	visible = true;
	
	sf::Vector2f size_ = Independant::getSize_for_Direction(direction, size);
	this->setPosition_Y_for_Direction(direction, sf::Vector2f(size_.x / 2, (-size_.y / 2) + first_screen_offset), true);

	for (int i = 0; i < Directions::NO_DIRECTION; i++)
	{
		this->portals[(Directions)i] = new Portal(position, speed, PORTAL_TEXTURE_NAME, sf::Vector2f(PORTAL_WIDTH, PORTAL_HEIGHT), sf::Vector2f(PORTAL_WIDTH / 2, PORTAL_HEIGHT / 2), PORTAL_FRAMES, PORTAL_ANIMATIONS);
		sf::Vector2f bg_size = Independant::getSize_for_Direction((Directions)i, size);
		//applying offset respect to the center of the background, depending on the direction
		this->portals[(Directions)i]->offset = Independant::getSpeed_for_Scrolling((Directions)i, (-bg_size.y / 2) + (PORTAL_HEIGHT/2));
		this->portals[(Directions)i]->setPosition(this->getPosition().x + this->portals[(Directions)i]->offset.x, this->getPosition().y + this->portals[(Directions)i]->offset.y);

		//rotation
		this->portals[(Directions)i]->setRotation(Independant::getRotation_for_Direction((Directions)i));

		//direction
		this->portals[(Directions)i]->direction = (Directions)i;
	}
}

void Background::update(sf::Time deltaTime, float hyperspeedMultiplier)
{
	//if not disappeared, move it
	if (!this->GarbageMe)
	{
		static sf::Vector2f newposition, newspeed;
		newspeed = this->speed;

		if (hyperspeedMultiplier > 1)
		{
			newspeed = this->getSpeedYMultiplier_for_Direction((*CurrentGame).direction, hyperspeedMultiplier);
		}
		else if (hyperspeedMultiplier < 1)
		{
			newspeed.x = this->speed.x * hyperspeedMultiplier;
			newspeed.y = this->speed.y * hyperspeedMultiplier;
		}

		newposition.x = this->getPosition().x + (newspeed.x)*deltaTime.asSeconds();
		newposition.y = this->getPosition().y + (newspeed.y)*deltaTime.asSeconds();

		this->setPosition(newposition.x, newposition.y);

		//portals follow the background
		for (int i = 0; i < Directions::NO_DIRECTION; i++)
		{
			this->portals[(Directions)i]->setPosition(newposition.x + this->portals[(Directions)i]->offset.x, newposition.y + this->portals[(Directions)i]->offset.y);
		}

		AnimatedSprite::update(deltaTime);
	}
	else
	{
		for (int i = 0; i < Directions::NO_DIRECTION; i++)
		{
			this->portals[(Directions)i]->GarbageMe = true;
		}
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

			case PortalState::PortalClose:
			{
				this->portals[(Directions)i]->state = PortalState::PortalClose;
				this->portals[(Directions)i]->visible = true;
				this->portals[(Directions)i]->setGhost(false);
				this->portals[(Directions)i]->Close();
				break;
			}

			case PortalState::PortalOpen:
			{
				this->portals[(Directions)i]->state = PortalState::PortalOpen;
				this->portals[(Directions)i]->visible = true;
				this->portals[(Directions)i]->setGhost(false);
				this->portals[(Directions)i]->Open();
				break;
			}

			case PortalState::PortalInvisible:
			{
				this->portals[(Directions)i]->state = PortalState::PortalInvisible;
				this->portals[(Directions)i]->setGhost(false);
				this->portals[(Directions)i]->Close();
				break;
			}
		}
	}
}