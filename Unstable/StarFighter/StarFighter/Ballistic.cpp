#include "Ballistic.h"

extern Game* CurrentGame;

using namespace sf;

Ballistic::Ballistic(BallisticType ballistic_type, sf::Vector2f position, AllianceType alliance, float heading, float range) : L16Entity(position, alliance, 4, Circle_L16Ballistic_MultiDomain)
{
	m_L16_type = L16Entity_Ballistic;
	m_ballistic_type = ballistic_type;

	switch (m_ballistic_type)
	{
		case Ballistic_AAM:
		{
			m_circle_type = Circle_L16Ballistic_Air;
			m_speed_min = 100;
			m_speed_max = 300;
			m_acceleration = 800;
			m_roll_rate_min = 100;
			m_roll_rate_max = 500;
			break;
		}
	}

	m_manoeuvrability = 0.05 * (getRadius() * 2);//0.03;

	m_heading = heading;
	m_roll = 0;
	m_speed = GetVectorFromLengthAndAngle(m_speed_min, heading * M_PI / 180);
	m_lifespan = range / m_speed_max;
}

Ballistic::~Ballistic()
{

}

void Ballistic::update(sf::Time deltaTime)
{
	m_lifespan -= deltaTime.asSeconds();

	if (m_lifespan <= 0)
	{
		m_garbageMe = true;
		m_visible = false;
	}
	else
	{
		BoundAngle(m_heading, 360);

		sf::Vector2f acceleration_vector = GetVectorFromLengthAndAngle(m_acceleration * deltaTime.asSeconds(), m_heading * M_PI / 180);
		m_speed += acceleration_vector;

		float speed_max = CosInterpolation(abs(m_roll), 0, 90, m_speed_max, m_speed_min);
		NormalizeVector(&m_speed, speed_max);

		m_radar_heading = m_heading;

		L16Entity::update(deltaTime);
	}
}