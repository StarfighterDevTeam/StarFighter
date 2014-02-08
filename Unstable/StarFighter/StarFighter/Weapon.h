#ifndef WEAPON_H_INCLUDED
#define WEAPON_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "Globals.h"
#include "Assets.h"
#include "FX.h"

#define LASER_RATE_OF_FIRE			0.4f
#define LASER_DAMAGE				100

#define LASERFAST_RATE_OF_FIRE			0.2f
#define LASERFAST_DAMAGE				50

enum WeaponType {
	Laser,
	LaserFast,
	NBVAL_Weapon
};

struct Weapon : public sf::Sprite
{
	sf::Vector2f speed;
	int damage;
	Weapon(int m_weapon_type = 0);
	bool firing_ready;
	void Fire(IndependantType m_collider_type);
	sf::Vector2i fire_direction;
	float rate_of_fire;
	sf::Clock deltaClock;
	int weapon_type;
	int ammo_type;
	Ammo* ammunition;

	Weapon* Clone();
};


#endif // WEAPON_H_INCLUDED