#include "Ballistic.h"

extern Game* CurrentGame;

using namespace sf;

Ballistic::Ballistic(BallisticType ballistic_type, sf::Vector2f position, AllianceType alliance, float heading) : L16Entity(position, alliance, 4)
{
	m_ballistic_type = ballistic_type;
	
	m_type = L16Entity_Ballistic;

	m_speed_min = 100;
	m_speed_max = 300;
	m_acceleration = 800;
	m_roll_rate_min = 100;
	m_roll_rate_max = 500;
	m_manoeuvrability = 0.05 * (getRadius() * 2);//0.03;

	m_heading = heading;
	m_roll = 0;
	m_speed = GetVectorFromLengthAndAngle(m_speed_min, heading);
	printf("");
}

Ballistic::~Ballistic()
{

}