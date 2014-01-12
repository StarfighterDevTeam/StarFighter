#ifndef SHIP_H_INCLUDED
#define SHIP_H_INCLUDED

#include <SFML/Graphics.hpp>

#define SHIP_FILENAME				"Assets/2D/player.png"
#define SHIP_START_X                0
#define SHIP_START_Y                0
#define SHIP_ACCELERATION_X         10.0f
#define SHIP_ACCELERATION_Y         10.0f
#define SHIP_DECCELERATION_COEF		200.0f
#define SHIP_MAX_SPEED_X            1000.0f
#define SHIP_MAX_SPEED_Y            1000.0f
#define SHIP_MIN_SPEED_X            50.0f
#define SHIP_MIN_SPEED_Y            50.0f

struct Ship : sf::Sprite
{
	sf::Vector2f speed;
	bool moving;

    Ship();
	void Init(int x, int y);
    void Update(float seconds);
};


#endif // SHIP_H_INCLUDED
