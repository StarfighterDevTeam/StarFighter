#ifndef BACKGROUND_H_INCLUDED
#define BACKGROUND_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "Independant.h"

class Portal : public Independant
{
public:
	Portal(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber);
	std::string destination_name;
	sf::Vector2f offset;//respect to the background
};

struct Background : public Independant
{
public:
	Background(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, Directions direction);
	Portal* portals[Directions::NO_DIRECTION];
	void Garbage();
	void update(sf::Time deltaTime) override;
};

#endif // BACKGROUND_H_INCLUDED
