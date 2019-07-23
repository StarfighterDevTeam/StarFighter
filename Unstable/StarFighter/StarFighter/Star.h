#ifndef STAR_H_INCLUDED
#define STAR_H_INCLUDED

#include "Globals.h"
#include "Game.h"
#include "GameObject.h"

class Star : public GameObject
{
public :
	Star();
	void update(sf::Time deltaTime) override;
};

#endif // STAR_H_INCLUDED
