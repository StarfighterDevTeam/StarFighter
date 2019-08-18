#ifndef BALLISTIC_H_INCLUDED
#define BALLISTIC_H_INCLUDED

#include "L16Entity.h"

enum BallisticType
{
	Ballistic_AAM,
};

class Ballistic : public L16Entity
{
public:
	Ballistic(BallisticType ballistic_type, sf::Vector2f position, AllianceType alliance, float heading, float range, CircleType collision_domain, L16Entity* locked_target);
	~Ballistic();

	BallisticType m_ballistic_type;

	float m_speed_min;
	float m_speed_max;
	float m_acceleration;
	float m_manoeuvrability;
	float m_heading;//in degrees
	float m_roll;//in degrees
	float m_roll_rate_min;//"taux de roulis"
	float m_roll_rate_max;
	float m_lifespan;
	L16Entity* m_locked_target;

	void update(sf::Time deltaTime) override;
};

#endif // BALLISTIC_H_INCLUDED
