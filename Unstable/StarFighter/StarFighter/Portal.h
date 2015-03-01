#ifndef PORTAL_H_INCLUDED
#define PORTAL_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "Independant.h"

enum PortalState
{
	PortalClose,//0
	PortalOpen,//1
	PortalGhost,//2
};

class Portal : public Independant
{
public:
	Portal(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber);
	std::string destination_name;
	sf::Vector2f offset;//respect to the background
	PortalState state;
};

#endif //PORTAL_H_INCLUDED