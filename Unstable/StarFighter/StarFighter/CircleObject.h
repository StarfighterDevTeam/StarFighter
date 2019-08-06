#ifndef CIRCLEOBJECT_H_INCLUDED
#define CIRCLEOBJECT_H_INCLUDED

#include "GameObject.h"
class Wave;

enum CircleType
{
	NodeType,
	WaveType,
	NB_CIRCLE_TYPES,
};

class CircleObject : public sf::CircleShape
{
public:
	CircleObject(AllianceType alliance);
	~CircleObject();

	virtual void update(sf::Time deltaTime);

	virtual Wave* CreateWaveBounce(sf::Vector2f position, float radius, sf::Vector2f vector);
	virtual void WaveReception();

	AllianceType m_alliance;
	sf::Color m_color;

	sf::Vector2f m_previous_speed;
	sf::Vector2f m_speed;
	CircleType m_type;

	bool m_garbageMe;
};

#endif // CIRCLEOBJECT_H_INCLUDED