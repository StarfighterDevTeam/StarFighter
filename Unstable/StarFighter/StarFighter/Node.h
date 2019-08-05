#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

#include "Game.h"

class Node : public CircleObject
{
public :
	Node(sf::Vector2f position, AllianceType alliance);
	Node(sf::Vector2f position, AllianceType alliance, float radius);

	void update(sf::Time deltaTime) override;
	bool IsHoveredByMouse();

	bool m_hovered;
	bool m_selected;
};

#endif // NODE_H_INCLUDED
