#include "Wing.h"

extern Game* CurrentGame;

using namespace sf;

Wing::Wing(sf::Vector2f position, AllianceType alliance) : L16Entity(position, alliance, 8)
{
	m_type = L16Entity_Wing;

	m_speed = sf::Vector2f(0, 0);
	m_min_speed = 50;
	m_max_speed = 100;
	m_acceleration = 300;
	m_angular_speed = 200;
	m_direction = 0;

	m_autopilot = false;

	m_radar_activated = true;
	m_radar_frequency = 0.5;
	m_radar_range = 500;
	m_radar_wavespeed = 200;
	m_radar_direction = m_direction;
	m_radar_coverage = 30;
}

Wing::~Wing()
{

}

void Wing::update(sf::Time deltaTime)
{
	//manual pilot
	if (m_autopilot == false)
	{
		sf::Vector2f inputs_direction = sf::Vector2f(0, 0);
		if ((*CurrentGame).m_window_has_focus == true)
		{
			inputs_direction = InputGuy::getDirections();
		}

		if (inputs_direction.x < 0)
		{
			m_direction += m_angular_speed * deltaTime.asSeconds();
		}
		else if (inputs_direction.x > 0)
		{
			m_direction -= m_angular_speed * deltaTime.asSeconds();
		}
	}

	//apply speed & direction
	BoundAngle(m_direction, 360);
	sf::Vector2f acceleration_vector = GetVectorFromLengthAndAngle(m_acceleration * deltaTime.asSeconds(), m_direction * M_PI / 180);
	m_speed += acceleration_vector;
	BoundVector(&m_speed, m_min_speed, m_max_speed);

	m_radar_direction = m_direction;

	L16Entity::update(deltaTime);
}