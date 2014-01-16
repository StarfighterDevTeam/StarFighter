#include "Weapon.h"

extern Game* CurrentGame;

Weapon::Weapon()
{
	speed.x = 0;
	speed.y = 0;
	firing_ready = true;
}
/*
void Weapon::Init(int x, int y)
{
TextureLoader *loader;
loader = TextureLoader::getInstance ();
sf::Texture* texture = loader->loadTexture(LASER_FILENAME,LASER_WIDTH,LASER_HEIGHT);

this->setTexture(*texture);
setOrigin(LASER_WIDTH/2, LASER_HEIGHT/2);
}
*/
void Weapon::Fire()
{
	if (deltaClock.getElapsedTime() > sf::seconds(LASER_RATE_OF_FIRE))
	{
		firing_ready = true;
	}

	if (firing_ready)
	{
		Independant* laser = new Independant(sf::Vector2f(getPosition().x,getPosition().y- (LASER_HEIGHT/2)),sf::Vector2f(0,-500), LASER_FILENAME, sf::Vector2f(LASER_WIDTH,LASER_HEIGHT));
		(*CurrentGame).addToScene(laser);

		printf("Tir de laser \n");

		deltaClock.restart();
		firing_ready = false;		
	}
}


