#ifndef WING_H_INCLUDED
#define WING_H_INCLUDED

#include "L16Entity.h"

class Wing : public L16Entity
{
public:
	Wing(sf::Vector2f position, AllianceType alliance, float heading);
	~Wing();

	void update(sf::Time deltaTime) override;

	bool m_autopilot;
	float m_speed_min;
	float m_speed_max;
	float m_acceleration;
	float m_manoeuvrability;
	float m_heading;//in degrees
	float m_roll;//in degrees
	float m_roll_speed_min;
	float m_roll_speed_max;
};

#endif // WING_H_INCLUDED
