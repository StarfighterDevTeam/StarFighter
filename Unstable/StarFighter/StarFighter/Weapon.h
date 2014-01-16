#ifndef WEAPON_H_INCLUDED
#define WEAPON_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "Globals.h"
#include "Ammo.h"

#define LASER_FILENAME				"Assets/2D/laser.png"
#define LASER_WIDTH					4
#define LASER_HEIGHT				12
#define LASER_RATE_OF_FIRE			0.3f
#define LASER_MAX_AMMO_PER_STAGE	10

struct Weapon : public sf::Sprite
{
	sf::Vector2f speed;
    Weapon();
	//void Init(int x, int y);
    virtual void Update(sf::Time deltaTime);
	bool firing_ready;
	void Fire();
	Ammo ammo[LASER_MAX_AMMO_PER_STAGE];
	int index;

	sf::Clock deltaClock;
};


#endif // WEAPON_H_INCLUDED