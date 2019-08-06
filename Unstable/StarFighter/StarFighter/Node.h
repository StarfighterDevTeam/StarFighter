#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

#include "Game.h"

class Link;

class Node : public CircleObject
{
public :
	Node(sf::Vector2f position, AllianceType alliance);
	Node(sf::Vector2f position, AllianceType alliance, float radius);
	~Node();

	void update(sf::Time deltaTime) override;
	bool IsHoveredByMouse();
	void ResetColor();

	bool m_hovered;
	bool m_selected;

	vector<Node*> m_linked_nodes;
};

#endif // NODE_H_INCLUDED
