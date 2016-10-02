#pragma once
#ifndef SHIP_H_INCLUDED
#define SHIP_H_INCLUDED

#include "SEShip.h"

class Ship : public SEShip
{
public:
	Ship();
	Ship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	Ship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size);

	virtual void UpdateInputStates() override;
	virtual void update(sf::Time deltaTime) override;
};

#endif // SHIP_H_INCLUDED
