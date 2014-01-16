#ifndef AMMO_H_INCLUDED
#define AMMO_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "Globals.h"

#define LASER_FILENAME				"Assets/2D/laser.png"
#define LASER_WIDTH					4
#define LASER_HEIGHT				12
#define LASER_RATE_OF_FIRE			0.3f
#define LASER_MAX_AMMO_PER_STAGE	10

class Ammo: public sf::Sprite
{

public :
	Ammo();
	void Init(int x, int y);
	void Update(sf::Time deltaTime);


private:
	sf::Vector2f speed;

};

#endif // AMMO_H_INCLUDED