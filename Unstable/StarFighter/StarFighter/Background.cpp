#include "Background.h"

extern Game* CurrentGame;

Background::Background(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, Directions direction, float first_screen_offset) : Independant(position, speed, textureName, size, sf::Vector2f(size.x / 2, size.y / 2))
{
	
	visible = true;
	
	sf::Vector2f size_ = Independant::getSize_for_Direction(direction, size);
	this->setPosition_Y_for_Direction(direction, sf::Vector2f(size_.x / 2, (-size_.y / 2) + first_screen_offset), true);

	for (int i = 0; i < Directions::NO_DIRECTION; i++)
	{
		this->portals[(Directions)i] = NULL;
	}
}

void Background::update(sf::Time deltaTime, float hyperspeedMultiplier)
{
	static sf::Vector2f newposition, newspeed;
	newspeed = this->speed;

	if (hyperspeedMultiplier > 1)
	{
		newspeed.x += Independant::getSpeed_for_Scrolling((*CurrentGame).direction, (hyperspeedMultiplier - 1) * (*CurrentGame).vspeed).x;
		newspeed.y += Independant::getSpeed_for_Scrolling((*CurrentGame).direction, (hyperspeedMultiplier - 1) * (*CurrentGame).vspeed).y;
	}
	else if (hyperspeedMultiplier < 1)
	{
		newspeed.x = this->speed.x * hyperspeedMultiplier;
		newspeed.y = this->speed.y * hyperspeedMultiplier;
	}

	this->setGhost(hyperspeedMultiplier > 1.0f);

	newposition.x = this->getPosition().x + (newspeed.x)*deltaTime.asSeconds();
	newposition.y = this->getPosition().y + (newspeed.y)*deltaTime.asSeconds();

	this->setPosition(newposition.x, newposition.y);

	//portals follow the background
	for (int i = 0; i < Directions::NO_DIRECTION; i++)
	{
		if (this->portals[(Directions)i] != NULL)
		{
			this->portals[(Directions)i]->setPosition(newposition.x + this->portals[(Directions)i]->offset.x, newposition.y + this->portals[(Directions)i]->offset.y);
		}
	}

	AnimatedSprite::update(deltaTime);
}

void Background::SetPortalsState(PortalState m_state)
{
	for (int i = 0; i < Directions::NO_DIRECTION; i++)
	{
		if (this->portals[(Directions)i] != NULL)
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
}