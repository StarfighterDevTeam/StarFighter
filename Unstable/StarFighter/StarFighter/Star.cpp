#include "Star.h"

extern Game* CurrentGame;

using namespace sf;

Star::Star()
{
	
}

void Star::update(sf::Time deltaTime)
{
	AnimatedSprite::update(deltaTime);
}