#ifndef CIRCLEOBJECT_H_INCLUDED
#define CIRCLEOBJECT_H_INCLUDED

#include "GameObject.h"

class Node;
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

	virtual Wave* CreateWaveBounce(sf::Vector2f position, float radius, sf::Vector2f vector, Node* bounced_node);
	virtual void WaveReception(Wave* wave);
	virtual AllianceType GetOriginAlliance();

	AllianceType m_alliance;
	sf::Color m_color;

	sf::Vector2f m_previous_speed;
	sf::Vector2f m_speed;
	CircleType m_type;

	bool m_garbageMe;
	bool m_visible;
};

#endif // CIRCLEOBJECT_H_INCLUDED