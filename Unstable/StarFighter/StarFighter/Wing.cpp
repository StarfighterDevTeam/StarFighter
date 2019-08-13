#include "Wing.h"

extern Game* CurrentGame;

using namespace sf;

Wing::Wing(sf::Vector2f position, AllianceType alliance, float heading) : L16Entity(position, alliance, 8)
{
	m_type = L16Entity_Wing;

	m_speed_min = 70;
	m_speed_max = 100;
	m_acceleration = 500;
	m_manoeuvrability = 0.03;
	m_roll_speed_min = 100;
	m_roll_speed_max = 500;
	m_heading = heading;
	m_roll = 0;

	m_speed = GetVectorFromLengthAndAngle(m_speed_min, heading);
	m_autopilot = false;

	m_radar_activated = true;
	m_radar_frequency = 0.5;
	m_radar_range = 500;
	m_radar_wavespeed = 200;
	m_radar_heading = m_heading;
	m_radar_coverage = 30;
}

Wing::~Wing()
{

}

void Wing::update(sf::Time deltaTime)
{
	float previous_speed = GetVectorLength(m_speed);

	//manual pilot
	sf::Vector2f inputs_direction = sf::Vector2f(0, 0);
	//float roll_speed = Lerp(previous_speed, m_speed_min, m_speed_max, m_roll_speed_max, m_roll_speed_min);
	float speed_ratio = (previous_speed - m_speed_min) / (m_speed_max - m_speed_min);
	float roll_speed = CosInterpolation(speed_ratio, 0, 1, m_roll_speed_max, m_roll_speed_min);
	
	if (m_autopilot == false)
	{
		if ((*CurrentGame).m_window_has_focus == true)
		{
			inputs_direction = InputGuy::getDirections();
		}

		if (inputs_direction.x < 0)
		{
			m_roll += roll_speed * deltaTime.asSeconds();
		}
		else if (inputs_direction.x > 0)
		{
			m_roll -= roll_speed * deltaTime.asSeconds();
		}
		else
		{
			if (m_roll > roll_speed * deltaTime.asSeconds())
			{
				m_roll -= roll_speed * deltaTime.asSeconds();
			}
			else if (m_roll < - roll_speed * deltaTime.asSeconds())
			{
				m_roll += roll_speed * deltaTime.asSeconds();
			}
			else
			{
				m_roll = 0;
			}
		}
	}

	//apply speed & direction
	Bound(m_roll, -90, 90);
	if (inputs_direction != sf::Vector2f(0, 0))
	{
		m_heading += m_roll * m_manoeuvrability;
	}
	
	BoundAngle(m_heading, 360);
	sf::Vector2f acceleration_vector = GetVectorFromLengthAndAngle(m_acceleration * deltaTime.asSeconds(), m_heading * M_PI / 180);
	m_speed += acceleration_vector;

	//float speed_max = Lerp(-abs(m_roll), -90, 0, m_speed_min, m_speed_max);
	float roll_ratio = abs(m_roll) / 90;
	float speed_max = CosInterpolation(roll_ratio, 0, 1, m_speed_max, m_speed_min);
	NormalizeVector(&m_speed, speed_max);

	printf("roll: %f, roll speed: %f, heading: %f, speed: %f\n", m_roll, roll_speed, m_heading, GetVectorLength(m_speed));

	m_radar_heading = m_heading;

	L16Entity::update(deltaTime);
}