#ifndef TERMINAL_H_INCLUDED
#define TERMINAL_H_INCLUDED

#include "Node.h"

class Terminal : public Node
{
public :
	Terminal(sf::Vector2f position, AllianceType alliance);
};

#endif // TERMINAL_H_INCLUDED