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

enum BrawlingStatus
{
	NOT_BRAWLING,
	INITIATE_BRAWL,
	ENDING_BRAWL,
};

enum DodingStatus
{
	NOT_DODGING,
	INITIATE_DODGING,
	ENDING_DODGE,
};

enum HitRecoveryStatus
{
	NOT_HIT,
	RECOVERING_FROM_BRAWL,
	RECOVERING_FROM_TACKLE,
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
	bool discoball_clockwise;
	void GetDiscoball(GameObject* discoball, float angle_collision) override;
	void PlayerContact(GameObject* player, float angle_collision) override;
	void ManageDiscoball(sf::Time deltaTime);
	void ThrowDiscoball();
	void ReleaseDiscoball(float angularSpeedBonus = 0.f);
	void UpdateRotation();
	void ManageFire();
	void ManageTackle();
	void ManageBrawl();
	void ManageDodge();
	void ManageSwitchRotation();
	void ManageKeyReleases();
	void ManageFeedbacks();
	void ManageHitRecovery();
	void ResetStatus();
	bool isFiringButtonReleased;
	bool wasFiringButtonReleased;
	bool isSwitchingButtonReleased;
	bool isDodgingButtonReleased;
	bool wasDodgingButtonReleased;
	void DiscoballSpeedConstraints();
	void ApplyGameRules();
	sf::Clock carrier_clock;
	sf::Clock carry_again_clock;
	sf::Clock tackle_again_clock;
	sf::Clock brawl_duration_clock;
	sf::Clock brawl_again_clock;
	sf::Clock throw_bonus_speed_clock;
	sf::Clock hit_recovery_clock;
	sf::Clock dodge_duration_clock;
	sf::Clock dodge_again_clock;

	TacklingStatus isTackling;
	ThrowingStatus isThrowing;
	BrawlingStatus isBrawling;
	HitRecoveryStatus isRecovering;
	DodingStatus isDodging;
	sf::Clock tackle_max_hold_clock;
	sf::Clock tackle_min_clock;
	sf::Vector2f speed_on_tackling;
	float tackle_curSpeedBonus;
	float throw_curSpeedBonus;

	Teams team;

private:
	bool moving;
	bool movingX;
	bool movingY;
};

#endif // SHIP_H_INCLUDED
