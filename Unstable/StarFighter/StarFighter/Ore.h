#ifndef ORE_H_INCLUDED
#define ORE_H_INCLUDED

#include "Globals.h"
#include "GameObject.h"
#include "InputGuy.h"
#include "Game.h"
#include "SFTextPop.h"

class Ore : public GameObject
{
public :
	Ore();
	Ore(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	virtual ~Ore();
	void update(sf::Time deltaTime) override;
};

#endif // ORE_H_INCLUDED
