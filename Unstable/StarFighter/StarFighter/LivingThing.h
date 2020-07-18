#ifndef LIVINGTHING_H_INCLUDED
#define LIVINGTHING_H_INCLUDED

#include "Globals.h"
#include "Game.h"
#include "GameObject.h"

class LivingThing : public GameObject
{
public :
	LivingThing();

	virtual ~LivingThing();
	void update(sf::Time deltaTime) override;
};

#endif // LIVINGTHING_H_INCLUDED
