#ifndef SHIP_H_INCLUDED
#define SHIP_H_INCLUDED

#include "Globals.h"
#include "InputGuy.h"
#include "Game.h"
#include "GameObject.h"
#include "Discoball.h"

enum TacklingStatus
{
	NOT_TACKLING,
	INITIATE_TACLKE,
	MAX_SPEED_TACKLE,
	HOLDING_TACKLE,
	ENDING_TACKLE,
};

enum ThrowingStatus
{
	NOT_THROWING,
	AFTER_THROW,
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
	
	void GetDirectionInputs(sf::Vector2f inputs_direction);
	void MaxSpeedConstraints();
	void IdleDecelleration(sf::Time deltaTime);
	bool ScreenBorderContraints();

	bool disable_inputs;

	ControlerType m_controllerType;
	void SetControllerType(ControlerType contoller);

	//TRON SPECIFIC
	Discoball* m_discoball;
	float discoball_curAngle;
	float discoball_curAngularSpeed;
	float discoball_clockwise;
	void GetDiscoball(GameObject* discoball, float angle_collision) override;
	void ManageDiscoball(sf::Time deltaTime);
	void ThrowDiscoball();
	void ManageFire();
	void ManageTackle();
	void ManageSwitchRotation();
	void ManageKeyReleases();
	void ManageFeedbacks();
	bool isFiringButtonReleased;
	bool wasFiringButtonReleased;
	bool isSwitchingButtonReleased;
	void DiscoballSpeedConstraints();
	sf::Clock carrier_clock;
	sf::Clock carry_again_clock;
	sf::Clock tackle_again_clock;
	sf::Clock throw_bonus_speed_clock;

	TacklingStatus isTackling;
	ThrowingStatus isThrowing;
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
