#ifndef DISCOBALL_H_INCLUDED
#define DISCOBALL_INCLUDED

#include "Globals.h"
#include "Game.h"

class Discoball : public GameObject
{
public :
	Discoball();
	Discoball(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	Discoball(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size);
	void Init();
	virtual ~Discoball();
	void update(sf::Time deltaTime) override;
};

#endif // DISCOBALL_H_INCLUDED
