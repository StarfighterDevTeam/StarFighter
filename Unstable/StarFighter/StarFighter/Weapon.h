#ifndef WEAPON_H_INCLUDED
#define WEAPON_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "Globals.h"
#include "Assets.h"

#define LASER_RATE_OF_FIRE			0.3f
#define LASER_MAX_AMMO_PER_STAGE	10
#define LASER_DAMAGE				10

struct Weapon : public sf::Sprite
{
	sf::Vector2f speed;
	int damage;
	Weapon();
	bool firing_ready;
	void Fire(IndependantType m_collider_type);
	sf::Vector2i fire_direction;

	sf::Clock deltaClock;
};


#endif // WEAPON_H_INCLUDED