#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

#include "Game.h"

class Node : public GameObject
{
public :
	Node(sf::Vector2f position, bool player);
	Node(sf::Vector2f position, bool player, float radius);
};

#endif // NODE_H_INCLUDED
