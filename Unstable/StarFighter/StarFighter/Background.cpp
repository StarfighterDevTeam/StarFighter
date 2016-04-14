#include "Background.h"

extern Game* CurrentGame;

Background::Background(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, Directions direction, float first_screen_offset) : GameObject(position, speed, textureName, size, sf::Vector2f(size.x / 2, size.y / 2))
{
	m_shop = NULL;
	m_visible = true;
	
	sf::Vector2f size_ = GameObject::getSize_for_Direction(direction, size);
	this->setPosition_Y_for_Direction(direction, sf::Vector2f(size_.x / 2, (-size_.y / 2) + first_screen_offset), true);

	for (int i = 0; i < NO_DIRECTION; i++)
	{
		m_portals[(Directions)i] = NULL;
	}
}

Background::~Background()
{
	if (m_shop)
	{
		m_shop->m_GarbageMe = true;
		m_shop->m_visible = false;
		m_shop = NULL;
	}

	for (int i = 0; i < NO_DIRECTION; i++)
	{
		if (m_portals[(Directions)i])
		{
			m_portals[(Directions)i]->m_GarbageMe = true;
			m_portals[(Directions)i]->m_visible = false;
			m_portals[(Directions)i] = NULL;
		}
	}
}

void Background::update(sf::Time deltaTime, float hyperspeedMultiplier)
{
	static sf::Vector2f newposition, newspeed;
	newspeed = m_speed;

	if (hyperspeedMultiplier > 1)
	{
		newspeed.x += GameObject::getSpeed_for_Scrolling((*CurrentGame).m_direction, (hyperspeedMultiplier - 1) * (*CurrentGame).m_vspeed).x;
		newspeed.y += GameObject::getSpeed_for_Scrolling((*CurrentGame).m_direction, (hyperspeedMultiplier - 1) * (*CurrentGame).m_vspeed).y;
	}
	else if (hyperspeedMultiplier < 1)
	{
		newspeed.x = m_speed.x * hyperspeedMultiplier;
		newspeed.y = m_speed.y * hyperspeedMultiplier;
	}

	setGhost(hyperspeedMultiplier > 1.0f);

	newposition.x = getPosition().x + (newspeed.x)*deltaTime.asSeconds();
	newposition.y = getPosition().y + (newspeed.y)*deltaTime.asSeconds();

	setPosition(newposition.x, newposition.y);

	//portals follow the background
	for (int i = 0; i < NO_DIRECTION; i++)
	{
		if (m_portals[(Directions)i] != NULL)
		{
			m_portals[(Directions)i]->setPosition(newposition.x + m_portals[(Directions)i]->m_offset.x, newposition.y + m_portals[(Directions)i]->m_offset.y);
		}
	}

	if (this->m_shop != NULL)
	{
		this->m_shop->setPosition(newposition);
	}

	AnimatedSprite::update(deltaTime);
}

void Background::SetPortalsState(PortalState m_state)
{
	for (int i = 0; i < NO_DIRECTION; i++)
	{
		if (m_portals[(Directions)i] != NULL)
		{
			switch (m_state)
			{
				case PortalGhost:
				{
					m_portals[(Directions)i]->m_state = PortalGhost;
					m_portals[(Directions)i]->m_visible = true;
					m_portals[(Directions)i]->setGhost(true);
					break;
				}

				case PortalClose:
				{
					m_portals[(Directions)i]->m_state = PortalClose;
					m_portals[(Directions)i]->m_visible = true;
					m_portals[(Directions)i]->setGhost(false);
					m_portals[(Directions)i]->Close();
					break;
				}

				case PortalOpen:
				{
					m_portals[(Directions)i]->m_state = PortalOpen;
					m_portals[(Directions)i]->m_visible = true;
					m_portals[(Directions)i]->setGhost(false);
					m_portals[(Directions)i]->Open();
					break;
				}

				case PortalInvisible:
				{
					m_portals[(Directions)i]->m_state = PortalInvisible;
					m_portals[(Directions)i]->setGhost(false);
					m_portals[(Directions)i]->Close();
					break;
				}
			}
		}
	}
}