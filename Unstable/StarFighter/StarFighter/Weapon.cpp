#include "Weapon.h"

extern Game* CurrentGame;

Weapon::Weapon()
{
	speed.x = 0;
	speed.y = 0;
	damage = LASER_DAMAGE;
	firing_ready = true;
	fire_direction = sf::Vector2i(0, 1);
}
void Weapon::Fire(IndependantType collider_type)
{
	if (deltaClock.getElapsedTime() > sf::seconds(LASER_RATE_OF_FIRE))
	{
		firing_ready = true;
	}

	if (firing_ready)
	{
		Independant* laser = new Independant(sf::Vector2f(getPosition().x,getPosition().y- ((LASER_HEIGHT/2)*(fire_direction.y))),sf::Vector2f(0*(fire_direction.x),-500*(fire_direction.y)), LASER_FILENAME, sf::Vector2f(LASER_WIDTH,LASER_HEIGHT));
		laser->setVisible(true);
		laser->collider_type = collider_type;
		laser->isOnScene = true;
		(*CurrentGame).addToScene(laser,1);

		deltaClock.restart();
		firing_ready = false;		
	}
}


