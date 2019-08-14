#ifndef WING_H_INCLUDED
#define WING_H_INCLUDED

#include "L16Entity.h"

class Wing : public L16Entity
{
public:
	Wing(sf::Vector2f position, AllianceType alliance, float heading);
	~Wing();

	void update(sf::Time deltaTime) override;
	void Draw(RenderTarget& screen) override;
	void UpdateWingsPositionToNewHeading();

	bool m_autopilot;
	float m_speed_min;
	float m_speed_max;
	float m_acceleration;
	float m_manoeuvrability;
	float m_heading;//in degrees
	float m_roll;//in degrees
	float m_roll_rate_min;//"taux de roulis"
	float m_roll_rate_max;
	float m_wingspan;//includes both wings
	float m_chord_length;//"corde", wing width

	LineObject* m_wings;
};

#endif // WING_H_INCLUDED
