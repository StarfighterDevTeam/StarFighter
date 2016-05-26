#ifndef STARSHIP_H_INCLUDED
#define STARSHIP_H_INCLUDED

#include "Location.h"
#include "Ore.h"

class Starship : public GameObject
{
public :
	Starship();
	Starship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	virtual ~Starship();
	void update(sf::Time deltaTime) override;
};

#endif // STARSHIP_H_INCLUDED
