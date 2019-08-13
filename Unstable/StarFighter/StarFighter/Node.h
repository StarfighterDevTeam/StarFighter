#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

#include "Wing.h"

class Node : public L16Entity
{
public :
	Node(sf::Vector2f position, AllianceType alliance, float radius = 16);
	~Node();

	void update(sf::Time deltaTime) override;
	Wing* CreateWing();

	bool m_is_terminal_node;
	vector<L16Entity*> m_linked_entities;
};

#endif // NODE_H_INCLUDED
