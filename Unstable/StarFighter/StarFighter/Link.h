#ifndef LINK_H_INCLUDED
#define LINK_H_INCLUDED

#include "LineObject.h"
#include "Game.h"
#include "Node.h"

class Link : public LineObject
{
public :
	Link(Node* node_a, Node* node_b);
	~Link();
	void update(sf::Time deltaTime) override;
	void UpdateQuadPointsPosition() override;

	Node* m_node_a;
	Node* m_node_b;
};

#endif // LINK_H_INCLUDED
