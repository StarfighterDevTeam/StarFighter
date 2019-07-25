#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

#include "Game.h"

class Node : public GameObject
{
public :
	Node(sf::Vector2f position, bool player);
	Node(sf::Vector2f position, bool player, float radius);

	void update(sf::Time deltaTime) override;

	bool IsHoveredByMouse();

	bool m_hovered;
	bool m_selected;
};

#endif // NODE_H_INCLUDED
