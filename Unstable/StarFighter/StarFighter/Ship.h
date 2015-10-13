#ifndef SHIP_H_INCLUDED
#define SHIP_H_INCLUDED

#include "Globals.h"
#include "Independant.h"
#include "InputGuy.h"
#include "Game.h"

#define SHIP_START_X                0
#define SHIP_START_Y                0
#define SHIP_ACCELERATION	        20.0f
#define SHIP_DECCELERATION_COEF		1000.0f
#define SHIP_MAX_SPEED				500.0f
#define SHIP_MIN_SPEED				50.0f
#define SHIP_SPRITE_RATE_SEC        0.2f

class Ship : public Independant
{
public :
	Ship();
	Ship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	virtual ~Ship();
	void update(sf::Time deltaTime) override;
	
	void ManageHudControls(sf::Vector2f inputs_directions);
	void ManageAcceleration(sf::Vector2f inputs_direction);
	void IdleDecelleration(sf::Time deltaTime);
	void ScreenBorderContraints();

	bool disable_inputs;
	
private:
	bool moving;
	bool movingX;
	bool movingY;
};

#endif // SHIP_H_INCLUDED
