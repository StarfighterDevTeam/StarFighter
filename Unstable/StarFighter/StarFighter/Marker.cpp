#include "Marker.h"

extern Game* CurrentGame;

using namespace sf;

Marker::Marker(MarkerType marker_type, GameObject* target)
{
	m_marker_type = marker_type;
	m_target = target;
	m_visible = false;

	m_distance_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_distance_text.setCharacterSize(18);
	m_distance_text.setStyle(sf::Text::Bold);

	string textureName;

	switch (marker_type)
	{
		case Marker_Enemy:
		{
			textureName = "2D/marker.png";
			m_distance_text.setColor(sf::Color::Red);
		}
	}

	Init(sf::Vector2f(0, 0), sf::Vector2f(0, 0), textureName, sf::Vector2f(20, 40), 1, 1);
}

Marker::~Marker()
{

}

void Marker::Update(sf::Time deltaTime)
{
	//is target on screen? if yes, marker doesn't need to be visible
	m_visible = IsInsideArea(m_target->m_size, m_target->getPosition(), sf::Vector2f(REF_WINDOW_RESOLUTION_X, REF_WINDOW_RESOLUTION_Y)) == false;

	if (m_visible == true)
	{	
		//orientation of the marker
		const float angle = GetAngleRadBetweenPositions(m_target->getPosition(), sf::Vector2f(REF_WINDOW_RESOLUTION_X * 0.5, REF_WINDOW_RESOLUTION_Y * 0.5));
		setRotation((angle - M_PI) * 180 / M_PI);

		//text

		//position of the marker
		sf::Vector2f position;
		GameObject* player = (GameObject*)(*CurrentGame).m_playerShip;
		const float offset = m_size.y * 0.5 + 10;
		
		//update target's position respect to the player, as it may not be updated anymore because of the far distance (m_removeMe == true)
		m_target->setPosition(sf::Vector2f(m_target->m_position.x - player->m_position.x + REF_WINDOW_RESOLUTION_X * 0.5, -(m_target->m_position.y - player->m_position.y) + REF_WINDOW_RESOLUTION_Y * 0.5));
		
		//compute the line equation to the target and stich it to the adequate screen border
		const float a = REF_WINDOW_RESOLUTION_Y / REF_WINDOW_RESOLUTION_X;//as in y=ax+b
		const float dx = m_target->getPosition().x - (REF_WINDOW_RESOLUTION_X * 0.5);
		const float dy = m_target->getPosition().y - (REF_WINDOW_RESOLUTION_Y * 0.5);
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
			position.x = (position.y - b2) / a2;

			m_distance_text.setPosition(sf::Vector2f(position.x - text_width * 0.5, position.y - offset - m_distance_text.getCharacterSize()));
		}
		else//up
		{
			position.y = offset;
			position.x = (position.y - b2) / a2;

			m_distance_text.setPosition(sf::Vector2f(position.x - m_distance_text.getGlobalBounds().width * 0.5, position.y + offset));
		}
		
		setPosition(position);
	}

	if (m_frameNumber > 1)
	{
		AnimatedSprite::Update(deltaTime);
	}
}

void Marker::Draw(RenderTarget& screen)
{
	if (m_visible)
		screen.draw(m_distance_text);

	GameObject::Draw(screen);
}