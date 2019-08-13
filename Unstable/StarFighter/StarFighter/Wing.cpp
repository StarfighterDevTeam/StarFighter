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
	sf::Vector2f inputs_direction = sf::Vector2f(0, 0);

	m_autopilot = !m_selected;

	//manual pilot
	if (m_autopilot == false)
	{
		if ((*CurrentGame).m_window_has_focus == true)
		{
			inputs_direction = InputGuy::getDirections();
		}
	}
	else
	{
		//auto pilot
		if (m_mission == L16Mission_Idle)
		{
			//circle patrol
			inputs_direction.x = 1;
		}
	}

	//apply speed & direction
	float previous_speed = GetVectorLength(m_speed);
	//float roll_speed = Lerp(previous_speed, m_speed_min, m_speed_max, m_roll_speed_max, m_roll_speed_min);
	float roll_speed = CosInterpolation(previous_speed, m_speed_min, m_speed_max, m_roll_speed_max, m_roll_speed_min);
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
		else if (m_roll < -roll_speed * deltaTime.asSeconds())
		{
			m_roll += roll_speed * deltaTime.asSeconds();
		}
		else
		{
			m_roll = 0;
		}
	}

	Bound(m_roll, -90, 90);
	if (inputs_direction != sf::Vector2f(0, 0))
	{
		m_heading += m_roll * m_manoeuvrability;
	}
	
	BoundAngle(m_heading, 360);
	sf::Vector2f acceleration_vector = GetVectorFromLengthAndAngle(m_acceleration * deltaTime.asSeconds(), m_heading * M_PI / 180);
	m_speed += acceleration_vector;

	//float speed_max = Lerp(-abs(m_roll), -90, 0, m_speed_min, m_speed_max);
	float speed_max = CosInterpolation(abs(m_roll), 0, 90, m_speed_max, m_speed_min);
	NormalizeVector(&m_speed, speed_max);

	//printf("roll: %f, roll speed: %f, heading: %f, speed: %f\n", m_roll, roll_speed, m_heading, GetVectorLength(m_speed));

	m_radar_heading = m_heading;

	L16Entity::update(deltaTime);
}