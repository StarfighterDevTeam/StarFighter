#include "Weapon.h"

extern Game* CurrentGame;

Weapon::Weapon()
{
	speed.x = 0;
	speed.y = 0;
	firing_ready = true;
	fire_direction = sf::Vector2u(0, 1);
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
		Independant* laser = new Independant(sf::Vector2f(getPosition().x,getPosition().y- ((LASER_HEIGHT/2)*fire_direction.y)),sf::Vector2f(0*fire_direction.x,-500*fire_direction.y), LASER_FILENAME, sf::Vector2f(LASER_WIDTH,LASER_HEIGHT));
		laser->setVisible(true);
		laser->collider_type = 2; // 0=bg; 1=ship ; 2=friendly fire ; 3= enemy fire ; 4=enemy;
		(*CurrentGame).addToScene(laser);

		printf("Tir de laser \n");

		deltaClock.restart();
		firing_ready = false;		
	}
}


