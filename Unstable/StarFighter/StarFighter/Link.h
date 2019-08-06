#ifndef LINK_H_INCLUDED
#define LINK_H_INCLUDED

#include "LineObject.h"
#include "Game.h"
#include "Node.h"

class Link : public LineObject
{
public :
	Link(Node* node_a, Node* node_b);
	void update(sf::Time deltaTime) override;
	void ResetColor();

	Node* m_node_a;
	Node* m_node_b;

	bool m_update_needed;
};

#endif // LINK_H_INCLUDED
