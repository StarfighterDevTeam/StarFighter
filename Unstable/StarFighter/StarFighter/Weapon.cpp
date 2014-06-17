#include "Weapon.h"

extern Game* CurrentGame;

Weapon::Weapon(Ammo* Ammunition)
{
	speed.x = 0;
	speed.y = 0;
	firing_ready = true;
	//rate_of_fire = LASER_RATE_OF_FIRE;
	//damage = LASER_DAMAGE;
	//fire_direction = sf::Vector2i(0, 1);
	this->ammunition = Ammunition;

	/*if (m_weapon_type == WeaponType::Laser)
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
	}*/
}

void Weapon::Fire(IndependantType m_collider_type)
{
	if (deltaClock.getElapsedTime() > sf::seconds(rate_of_fire))
	{
		firing_ready = true;
	}

	if (firing_ready)
	{
		Ammo* bullet = this->ammunition->Clone();
		bullet->setPosition(getPosition().x,getPosition().y + ((bullet->m_size.y/2)*(fire_direction.y)));
		bullet->speed = sf::Vector2f(bullet->speed.x*(fire_direction.x),bullet->speed.y*(fire_direction.y));
		bullet->setVisible(true);
		bullet->collider_type = m_collider_type;
		bullet->isOnScene = true;

		(*CurrentGame).addToScene(bullet,PlayerShipLayer, m_collider_type);

		deltaClock.restart();
		firing_ready = false;		
	}
}

Weapon* Weapon::Clone()
{
	Weapon* weapon = new Weapon(this->ammunition->Clone());
	weapon->fire_direction = this->fire_direction;
	weapon->rate_of_fire = this->rate_of_fire;
	return weapon;
}


