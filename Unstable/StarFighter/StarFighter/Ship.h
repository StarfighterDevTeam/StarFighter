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

class Ship : public Independant
{

public :
	Ship(Vector2f position, ShipConfig m_ship_config);
	void Init(int x, int y);
	void update(sf::Time deltaTime) override;
	Weapon weapon;
	void setShipConfig(ShipConfig m_ship_config);

private:
	bool moving;
	ShipConfig ship_config;

	//Independant* shipSprite;
	//Animation* currentAnimation;
	//Animation animation;
	//AnimatedSprite animatedSprite;
	/*virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(*shipSprite, states);
	}*/

};




#endif // SHIP_H_INCLUDED
