#ifndef SHIP_H_INCLUDED
#define SHIP_H_INCLUDED

#include "Globals.h"
#include <SFML/Graphics.hpp>
#include "Weapon.h"
#include "AnimatedSprite.hpp"
#include "TextureLoader.h"
#include "Assets.h"

#define SHIP_START_X                0
#define SHIP_START_Y                0
#define SHIP_ACCELERATION_X         10.0f
#define SHIP_ACCELERATION_Y         10.0f
#define SHIP_DECCELERATION_COEF		500.0f
#define SHIP_MAX_SPEED_X            1000.0f
#define SHIP_MAX_SPEED_Y            1000.0f
#define SHIP_MIN_SPEED_X            50.0f
#define SHIP_MIN_SPEED_Y            50.0f

#define SHIP_SPRITE_RATE_SEC        0.2f

class ShipConfig
{
public:
	void Init(sf::Vector2f m_max_speed, std::string m_textureName);
	ShipConfig();
	std::string texturename;
	sf::Vector2f max_speed;
};

class Ship : public sf::Drawable
{

public :
	Ship();
	void Init(int x, int y);
	void Update(sf::Time deltaTime);
	void Display();
	Weapon weapon;
	ShipConfig ship_config;
	void getShipConfig(ShipConfig m_ship_config);

private:
	sf::Vector2f speed;
	sf::Vector2f max_speed;
	bool moving;
	Animation* currentAnimation;
	Animation animation;
	AnimatedSprite animatedSprite;
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
		target.draw(animatedSprite, states);
	}

};




#endif // SHIP_H_INCLUDED
