#ifndef CIRCLEOBJECT_H_INCLUDED
#define CIRCLEOBJECT_H_INCLUDED

#include "GameObject.h"

enum AllianceType
{
	PlayerAlliance,
	NeutralAlliance,
	EnemyAlliance,
	NB_ALLIANCE_TYPES,
};

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

	AllianceType m_alliance;
	sf::Color m_color;

	sf::Vector2f m_previous_speed;
	sf::Vector2f m_speed;
	CircleType m_type;
};

#endif // CIRCLEOBJECT_H_INCLUDED