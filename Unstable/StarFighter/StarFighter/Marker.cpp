#include "Marker.h"

extern Game* CurrentGame;

using namespace sf;

Marker::Marker(MarkerType marker_type, SpatialObject* target)
{
	m_target = target;

	m_distance_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_distance_text.setCharacterSize(18);
	m_distance_text.setStyle(sf::Text::Bold);

	string textureName = "2D/marker.png";

	Init(sf::Vector2f(0, 0), sf::Vector2f(0, 0), textureName, sf::Vector2f(20, 40), 1, (int)NB_MARKER_TYPES);

	const float a = 10;//length
	const float b = 4;//thickness
	for (int i = 0; i < 8; i++)
	{
		m_targeting_rect[i].setSize(i % 2 == 0 ? sf::Vector2f(a, b) : sf::Vector2f(b, a));
		m_targeting_rect[i].setOrigin(sf::Vector2f(m_targeting_rect[i].getSize().x * 0.5, m_targeting_rect[i].getSize().y * 0.5));
		m_targeting_rect[i].setOutlineThickness(0);
	}

	const float size = MaxBetweenValues(target->m_size.x, target->m_size.y);
	m_mission_rect.setSize(sf::Vector2f(size, size));
	m_mission_rect.setOrigin(sf::Vector2f(m_mission_rect.getSize().x * 0.5, m_mission_rect.getSize().y * 0.5));
	m_mission_rect.setOutlineThickness(4);
	m_mission_rect.setOutlineColor(sf::Color::Blue);
	m_mission_rect.setFillColor(sf::Color::Transparent);
	m_mission_rect.setRotation(45);

	SetMarkerType(marker_type);
}

void Marker::SetMarkerType(MarkerType marker_type)
{
	m_marker_type = marker_type;

	//distance marker
	sf::Color text_color;
	sf::Color targeting_rect_color;
	switch (marker_type)
	{
		case Marker_Enemy:
		{
			text_color = sf::Color::Red;
			targeting_rect_color = sf::Color::Red;
			break;
		}
		case Marker_Ally:
		{
			text_color = sf::Color::Green;
			targeting_rect_color = sf::Color::Green;
			break;
		}
		case Marker_Mission:
		{
			text_color = sf::Color::Blue;
			targeting_rect_color = sf::Color::Blue;
			break;
		}
	}

	m_distance_text.setColor(text_color);
	for (int i = 0; i < 8; i++)
		m_targeting_rect[i].setFillColor(targeting_rect_color);

	SetAnimationLine((int)marker_type);
}

Marker::~Marker()
{

}

void Marker::Update(sf::Time deltaTime)
{
	GameObject* player = (GameObject*)(*CurrentGame).m_playerShip;

	//update target's position respect to the player, as it may not be set because of the far distance (m_removeMe == true)
	m_target->setPosition(sf::Vector2f(m_target->m_position.x - player->m_position.x + REF_WINDOW_RESOLUTION_X * 0.5, -(m_target->m_position.y - player->m_position.y) + REF_WINDOW_RESOLUTION_Y * 0.5));

	//is target on screen? if yes, marker doesn't need to be visible
	m_onScreen = IsInsideArea(m_target->m_size, m_target->getPosition(), sf::Vector2f(REF_WINDOW_RESOLUTION_X, REF_WINDOW_RESOLUTION_Y)) == true;

	if (m_onScreen == false)
	{
		//orientation of the marker
		const float dx = m_target->getPosition().x - REF_WINDOW_RESOLUTION_X * 0.5;
		const float dy = m_target->getPosition().y - REF_WINDOW_RESOLUTION_Y * 0.5;
		const float angle = GetAngleRadFromVector(sf::Vector2f(dx, -dy));
		setRotation(angle * 180 / M_PI);

		//position of the marker
		sf::Vector2f position;
		const float offset = m_size.y * 0.5 + 10;
		
		//compute the line equation to the target and stich it to the adequate screen border
		const float a = REF_WINDOW_RESOLUTION_Y / REF_WINDOW_RESOLUTION_X;//as in y=ax+b
		const float a2 = dx != 0 ? dy / dx : 0;
		const float b2 = m_target->getPosition().y - (a2 * m_target->getPosition().x);

		//text
		m_distance_text.setString(to_string((int)GetVectorLength(sf::Vector2f(dx, dy))) + "m");
		const float text_width = m_distance_text.getGlobalBounds().width;

		//white screen border?
		if (dx > 0 && abs(dy) < a * dx)//right
		{
			position.x = REF_WINDOW_RESOLUTION_X - offset;
			position.y = a2 * position.x + b2;

			m_distance_text.setPosition(sf::Vector2f(position.x - offset - text_width, position.y - m_distance_text.getCharacterSize()));
		}
		else if (abs(dy) < -a * dx)//left
		{
			position.x = offset;
			position.y = a2 * position.x + b2;

			m_distance_text.setPosition(sf::Vector2f(position.x + offset, position.y - m_distance_text.getCharacterSize() * 0.5));
		}
		else if (dy > 0)//down
		{
			position.y = REF_WINDOW_RESOLUTION_Y - offset;
			position.x = a2 != 0 ? (position.y - b2) / a2 : REF_WINDOW_RESOLUTION_X * 0.5;

			m_distance_text.setPosition(sf::Vector2f(position.x - text_width * 0.5, position.y - offset - m_distance_text.getCharacterSize()));
		}
		else//up
		{
			position.y = offset;
			position.x = a2 != 0 ? (position.y - b2) / a2 : REF_WINDOW_RESOLUTION_X * 0.5;

			m_distance_text.setPosition(sf::Vector2f(position.x - m_distance_text.getGlobalBounds().width * 0.5, position.y + offset));
		}
		
		setPosition(position);
	}
	else
	{
		//targeting marker
		const float L = m_targeting_rect[0].getSize().x - m_targeting_rect[0].getSize().y;
		const float size = MaxBetweenValues(m_target->m_size.x, m_target->m_size.y);

		m_targeting_rect[0].setPosition(sf::Vector2f(m_target->getPosition().x - size * 0.5 + (L * 0.5), m_target->getPosition().y - m_target->m_size.y * 0.5));
		m_targeting_rect[1].setPosition(sf::Vector2f(m_target->getPosition().x - size * 0.5, m_target->getPosition().y - size * 0.5 + (L * 0.5)));

		m_targeting_rect[2].setPosition(sf::Vector2f(m_target->getPosition().x + size * 0.5 - (L * 0.5), m_target->getPosition().y - m_target->m_size.y * 0.5));
		m_targeting_rect[3].setPosition(sf::Vector2f(m_target->getPosition().x + size * 0.5, m_target->getPosition().y - size * 0.5 + (L * 0.5)));

		m_targeting_rect[4].setPosition(sf::Vector2f(m_target->getPosition().x - size * 0.5 + (L * 0.5), m_target->getPosition().y + m_target->m_size.y * 0.5));
		m_targeting_rect[5].setPosition(sf::Vector2f(m_target->getPosition().x - size * 0.5, m_target->getPosition().y + size * 0.5 - (L * 0.5)));

		m_targeting_rect[6].setPosition(sf::Vector2f(m_target->getPosition().x + size * 0.5 - (L * 0.5), m_target->getPosition().y + m_target->m_size.y * 0.5));
		m_targeting_rect[7].setPosition(sf::Vector2f(m_target->getPosition().x + size * 0.5, m_target->getPosition().y + size * 0.5 - (L * 0.5)));

		//mission marker
		m_mission_rect.setPosition(m_target->getPosition());
	}

	if (m_frameNumber > 1)
		AnimatedSprite::Update(deltaTime);
}

void Marker::Draw(RenderTarget& screen)
{
	if (m_visible == false)
		return;

	//bool display_hostility = (m_target->m_hostility == Hostility_Ally || m_target->m_roe == ROE_FireAtWill);

	if (m_onScreen == true)
	{
		if (m_marker_type != Marker_Mission)
			for (int i = 0; i < 8; i++)
				screen.draw(m_targeting_rect[i]);
		else
			screen.draw(m_mission_rect);
	}
	else
	{
		screen.draw(m_distance_text);
		GameObject::Draw(screen);
	}
}