#include "Background.h"

extern Game* CurrentGame;

Background::Background(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size) : GameObject(position, speed, textureName, size, sf::Vector2f(size.x / 2, size.y / 2))
{
	m_collider_type = BackgroundObject;
	m_layer = BackgroundLayer;

	m_shop = NULL;
	m_visible = true;
	m_DontGarbageMe = true;

	for (int i = 0; i < NO_DIRECTION; i++)
		m_portals[i] = NULL;
}

Background::~Background()
{
	if (m_shop != NULL)
		m_shop->Death();

	for (int i = 0; i < NO_DIRECTION; i++)
		if (m_portals[i] != NULL)
			m_portals[i]->Death();
}

void Background::update(sf::Time deltaTime, float hyperspeedMultiplier)
{
	static sf::Vector2f newposition, newspeed;
	newspeed = m_speed;

	float l_hyperspeedMultiplier = hyperspeedMultiplier < 1 ? hyperspeedMultiplier : 1;

	//slowmotion
	newspeed.y += (l_hyperspeedMultiplier - 1) * (*CurrentGame).m_vspeed;

	if (m_ghost == false && l_hyperspeedMultiplier > 1)
		setGhost(true);
	else if (m_ghost == true)
		setGhost(false);

	newposition.x = getPosition().x + (newspeed.x)*deltaTime.asSeconds();
	newposition.y = getPosition().y + (newspeed.y)*deltaTime.asSeconds();

	setPosition(newposition.x, newposition.y);

	//portals follow the background
	for (int i = 0; i < NO_DIRECTION; i++)
		if (m_portals[(Directions)i] != NULL)
			m_portals[(Directions)i]->setPosition(newposition.x + m_portals[(Directions)i]->m_offset.x, newposition.y + m_portals[(Directions)i]->m_offset.y);

	if (m_shop != NULL)
		m_shop->setPosition(newposition);

	AnimatedSprite::update(deltaTime);
}

void Background::SetPortalsState(PortalState state)
{
	for (int i = 0; i < NO_DIRECTION; i++)
	{
		if (m_portals[(Directions)i] != NULL && m_portals[(Directions)i]->m_state != state)
		{
			m_portals[(Directions)i]->m_state = state;

			switch (state)
			{
				case PortalGhost:
				{
					m_portals[(Directions)i]->m_visible = true;
					m_portals[(Directions)i]->setGhost(true);
					break;
				}

				case PortalClose:
				{
					m_portals[(Directions)i]->m_visible = true;
					m_portals[(Directions)i]->setGhost(false);
					m_portals[(Directions)i]->Close();
					break;
				}

				case PortalOpen:
				{
					m_portals[(Directions)i]->m_visible = true;
					m_portals[(Directions)i]->setGhost(false);
					m_portals[(Directions)i]->Open();
					break;
				}

				case PortalInvisible:
				{
					m_portals[(Directions)i]->setGhost(false);
					m_portals[(Directions)i]->Close();
					break;
				}
			}
		}
	}
}