#ifndef WING_H_INCLUDED
#define WING_H_INCLUDED

#include "L16Entity.h"

class Wing : public L16Entity
{
public:
	Wing(sf::Vector2f position, AllianceType alliance);
	~Wing();

	void update(sf::Time deltaTime) override;

	bool m_autopilot;
	float m_min_speed;
	float m_max_speed;
	float m_acceleration;
	float m_angular_speed;
	float m_direction;//in degrees
};

#endif // WING_H_INCLUDED
