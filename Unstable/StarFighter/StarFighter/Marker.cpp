#include "Marker.h"

extern Game* CurrentGame;

using namespace sf;

Marker::Marker(GameObject* target) : GameObject(sf::Vector2f(0, 0), sf::Vector2f(0, 0), "2D/marker.png", sf::Vector2f(20, 40))
{
	m_target = target;
	m_visible = false;
}

Marker::~Marker()
{

}

void Marker::Update(sf::Time deltaTime)
{
	m_visible = IsInsideArea(m_target->m_size, m_target->getPosition(), sf::Vector2f(REF_WINDOW_RESOLUTION_X, REF_WINDOW_RESOLUTION_Y)) == false;
	
	if (m_visible == true)
	{
		float angle = GetAngleRadBetweenPositions(m_target->getPosition(), sf::Vector2f(REF_WINDOW_RESOLUTION_X * 0.5, REF_WINDOW_RESOLUTION_Y * 0.5));

		const float offset = m_size.y * 0.5 + 10;
		sf::Vector2f position;
		if (angle < M_PI_4)//right border (1/2)
		{
			position.x = REF_WINDOW_RESOLUTION_X - offset;
			position.y = CosInterpolation(angle, 0, M_PI_4, 0, 1) * REF_WINDOW_RESOLUTION_Y * 0.5;//
		}
		else if (angle < 3 * M_PI_4)//up border
		{
			position.x = REF_WINDOW_RESOLUTION_X - offset;//
			position.y = offset;
		}
		else if (angle < 5 * M_PI_4)//left border
		{
			position.x = offset;
			position.y = 10;//
		}
		else if (angle < 7 * M_PI_4)//down border
		{
			position.x = offset;//
			position.y = REF_WINDOW_RESOLUTION_Y - offset;
		}
		else//right border (2/2)
		{
			position.x = offset;//
			position.y = REF_WINDOW_RESOLUTION_Y - offset;
		}

		setPosition(position);
	}

	if (m_frameNumber > 1)
	{
		AnimatedSprite::Update(deltaTime);
	}
}