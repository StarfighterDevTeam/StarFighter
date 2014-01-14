#ifndef WEAPON_H_INCLUDED
#define WEAPON_H_INCLUDED

#include <SFML/Graphics.hpp>

#define LASER_FILENAME				"Assets/2D/laser.png"
#define LASER_WIDTH					4
#define LASER_HEIGHT				12
#define LASER_RATE_OF_FIRE			0.3f
#define LASER_MAX_AMMO_PER_STAGE	10

struct Weapon : sf::Sprite

{
	sf::Vector2f speed;
    Weapon();
	void Init(int x, int y);
    void Update(float seconds);
};


#endif // WEAPON_H_INCLUDED