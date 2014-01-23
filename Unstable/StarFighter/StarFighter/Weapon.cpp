#include "Weapon.h"

extern Game* CurrentGame;

Weapon::Weapon()
{
	this->speed.x = 0;
	this->speed.y = 0;
	this->damage = LASER_DAMAGE;
	this->firing_ready = true;
	this->fire_direction = sf::Vector2i(0, 1);
}

void Weapon::Fire(IndependantType m_collider_type)
{
	if (deltaClock.getElapsedTime() > sf::seconds(LASER_RATE_OF_FIRE))
	{
		firing_ready = true;
	}

	if (firing_ready)
	{
		Ammo* laser = new Ammo(sf::Vector2f(getPosition().x,getPosition().y- ((LASER_HEIGHT/2)*(fire_direction.y))),sf::Vector2f(0*(fire_direction.x),-500*(fire_direction.y)), LASER_FILENAME, sf::Vector2f(LASER_WIDTH,LASER_HEIGHT)/*, sf::Vector2f(LASER_WIDTH/2,LASER_HEIGHT/2), LASER_FRAME_NUMBER*/ ,LASER_DAMAGE);
		
		laser->setVisible(true);
		laser->collider_type = m_collider_type;
		laser->isOnScene = true;
		(*CurrentGame).addToScene(laser,PlayerShipLayer, m_collider_type);

		FX* explosion = new FX (sf::Vector2f(150, 500),sf::Vector2f(0,0), FX_EXPLOSION_FILENAME, sf::Vector2f(FX_EXPLOSION_WIDTH, FX_EXPLOSION_HEIGHT), FX_EXPLOSION_FRAME_NUMBER, sf::seconds(1));
		
		explosion->setVisible(true);
		explosion->collider_type = IndependantType::Background;
		explosion->isOnScene = true;
		(*CurrentGame).addToScene(explosion, LayerType::ExplosionLayer, IndependantType::Background);

		deltaClock.restart();
		firing_ready = false;		
	}
}


