#ifndef SHIP_H_INCLUDED
#define SHIP_H_INCLUDED

#include "Globals.h"
#include "InputGuy.h"
#include "Game.h"
#include "GameObject.h"
#include "Discoball.h"

#define SHIP_START_X								990
#define SHIP_START_Y								540

#define SHIP_ACCELERATION							100.f
#define SHIP_DECELERATION_COEF						5000.f
#define SHIP_MAX_SPEED								400.f
#define SHIP_MIN_SPEED								50.f

#define SHIP_MIN_SPEED_FOR_TACKLE					200.f
#define SHIP_TACKLE_ACCELERATION_RATIO				0.3f
#define SHIP_MAX_TACKLE_SPEED_BONUS_RATIO			3.f
#define SHIP_SPEED_PERCENTAGE_ON_HOLDING_TACKLE		0.6f
#define SHIP_TACKLE_MIN_LASTING_TIME				0.f
#define SHIP_TACKLE_MAX_HOLD_TIME					0.2f
#define SHIP_TACKLE_HOLDDECELERATION_COFF			0.1f
#define SHIP_TACKLE_DECELERATION_COFF				0.4f
#define SHIP_MIN_SPEED_AFTER_TACKLE					200.f

#define CARRY_AGAIN_COOLDOWN						0.5f
#define TACKLE_AGAIN_COOLDOWN						0.5f

enum TacklingStatus
{
	NOT_TACKLING,
	INITIATE_TACLKE,
	MAX_SPEED_TACKLE,
	HOLDING_TACKLE,
	ENDING_TACKLE,
};

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
	bool ScreenBorderContraints();

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
	void ManageTackle();
	void ManageSwitchRotation();
	void ManageKeyReleases();
	bool isFiringButtonReleased;
	bool wasFiringButtonReleased;
	bool isSwitchingButtonReleased;
	void DiscoballSpeedConstraints();
	sf::Clock carrier_clock;
	sf::Clock carry_again_clock;
	sf::Clock tackle_again_clock;

	TacklingStatus isTackling;
	sf::Clock tackle_max_hold_clock;
	sf::Clock tackle_min_clock;
	sf::Vector2f speed_on_tackling;
	float tackle_curSpeedBonus;

private:
	bool moving;
	bool movingX;
	bool movingY;
};

#endif // SHIP_H_INCLUDED
