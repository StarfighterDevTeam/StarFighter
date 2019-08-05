#ifndef LINEOBJECT_H_INCLUDED
#define LINEOBJECT_H_INCLUDED

#include "CircleObject.h"

class LineObject
{
public:
	LineObject(sf::Vector2f position_a, sf::Vector2f position_b, AllianceType alliance);
	~LineObject();

	sf::Vertex m_points[2];

	virtual void update(sf::Time deltaTime);

	AllianceType m_alliance;
	sf::Color m_color;
};

#endif // CIRCLEOBJECT_H_INCLUDED