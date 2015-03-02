#ifndef PORTAL_H_INCLUDED
#define PORTAL_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "Independant.h"

enum PortalState
{
	PortalInvisible,//0
	PortalClose,//1
	PortalGhost,//2
	PortalOpen,//3
};

class Portal : public Independant
{
public:
	Portal(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber);
	void update(sf::Time deltaTime) override;
	std::string destination_name;
	sf::Vector2f offset;//respect to the background
	PortalState state;
	Directions direction;
};

#endif //PORTAL_H_INCLUDED