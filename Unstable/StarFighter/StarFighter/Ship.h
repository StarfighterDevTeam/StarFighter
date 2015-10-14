#ifndef SHIP_H_INCLUDED
#define SHIP_H_INCLUDED

#include "Globals.h"
#include "InputGuy.h"
#include "Game.h"
#include "GameObject.h"
#include "Discoball.h"

#define SHIP_START_X								990
#define SHIP_START_Y								540
#define SHIP_ACCELERATION							2000.0f
#define SHIP_DECCELERATION_COEF						5000.0f
#define SHIP_MAX_SPEED								400.0f
#define SHIP_MIN_SPEED								50.0f

#define CARRY_AGAIN_COOLDOWN						0.5f

class Ship : public GameObject
{
public :
	Ship();
	Ship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	Ship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size);
	void Init();
	virtual ~Ship();
	void update(sf::Time deltaTime) override;
	
	void ManageAcceleration(sf::Vector2f inputs_direction);
	void IdleDecelleration(sf::Time deltaTime);
	void ScreenBorderContraints();

	bool disable_inputs;

	//TRON SPECIFIC
	Discoball* m_discoball;
	float discoball_curAngle;
	float discoball_curAngularSpeed;
	float discoball_clockwise;
	void GetDiscoball(GameObject* discoball, float angle_collision) override;
	void ManageDiscoball(sf::Time deltaTime);
	void ReleaseDiscoball();
	void ManageFire();
	void ManageSwitchRotation();
	void ManageKeyReleases();
	bool isFiringButtonReleased;
	bool isSwitchingButtonReleased;
	void DiscoballSpeedConstraints();
	sf::Clock carrier_clock;
	sf::Clock carry_again_clock;

private:
	bool moving;
	bool movingX;
	bool movingY;
};

#endif // SHIP_H_INCLUDED
