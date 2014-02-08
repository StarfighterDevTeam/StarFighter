#include "Weapon.h"

extern Game* CurrentGame;

Weapon::Weapon(int m_weapon_type)
{
	speed.x = 0;
	speed.y = 0;
	firing_ready = true;
	//rate_of_fire = LASER_RATE_OF_FIRE;
	//damage = LASER_DAMAGE;
	//fire_direction = sf::Vector2i(0, 1);

	if (m_weapon_type == WeaponType::Laser)
	{
		rate_of_fire = LASER_RATE_OF_FIRE;
		damage = LASER_DAMAGE;
		fire_direction = sf::Vector2i(0, 1);
		ammo_type = AmmoType::LaserBeam;
	}

	if (m_weapon_type == WeaponType::LaserFast)
	{
		rate_of_fire = LASERFAST_RATE_OF_FIRE;
		damage = LASERFAST_DAMAGE;
		fire_direction = sf::Vector2i(0, 1);
		ammo_type = AmmoType::LaserBeamBlue;
	}
}

void Weapon::Fire(IndependantType m_collider_type)
{
	if (deltaClock.getElapsedTime() > sf::seconds(rate_of_fire))
	{
		firing_ready = true;
	}

	if (firing_ready)
	{
		Ammo* laser = new Ammo(sf::Vector2f(getPosition().x,getPosition().y- ((LASER_HEIGHT/2)*(fire_direction.y))),sf::Vector2f(0*(fire_direction.x),-500*(fire_direction.y)), LASER_FILENAME, sf::Vector2f(LASER_WIDTH,LASER_HEIGHT), LASER_DAMAGE);
		//Ammo* laser = new Ammo(sf::Vector2f(getPosition().x,getPosition().y- ((LASER_HEIGHT/2)*(fire_direction.y))),sf::Vector2f(0*(fire_direction.x),-500*(fire_direction.y)), LASER_DAMAGE);
		laser->setVisible(true);
		laser->collider_type = m_collider_type;
		laser->isOnScene = true;
		(*CurrentGame).addToScene(laser,PlayerShipLayer, m_collider_type);

		deltaClock.restart();
		firing_ready = false;		
	}
}

Weapon* Weapon::Clone()
{
	Weapon* weapon = new Weapon();
	weapon->fire_direction = this->fire_direction;
	weapon->rate_of_fire = this->rate_of_fire;
	weapon->damage = this->damage;
	weapon->ammunition = this->ammunition->Clone();
	return weapon;
}


