#ifndef LOCATION_H_INCLUDED
#define LOCATION_H_INCLUDED

#include "Ore.h"

class Location : public GameObject
{
public :
	Location();
	Location(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	virtual ~Location();
	void update(sf::Time deltaTime) override;
};

#endif // LOCATION_H_INCLUDED
