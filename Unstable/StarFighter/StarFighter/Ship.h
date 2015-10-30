#ifndef SHIP_H_INCLUDED
#define SHIP_H_INCLUDED

#include "Globals.h"
#include "InputGuy.h"
#include "Game.h"
#include "GameObject.h"
#include "Discoball.h"
#include "LevelPortal.h"
#include "Bumper.h"

enum TacklingStatus
{
	NOT_TACKLING,
	INITIATE_TACLKE,
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

enum PlayableCharacters
{
	Natalia,
	Quorra,
	Katyusha,
	Savannah,
	NB_PLAYABLE_CHARACTERS,
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
	float GetMaxSpeed();
	void PlayerBumper(GameObject* bumper, Time deltaTime) override;
	
	void IdleDecelleration(sf::Time deltaTime);
	bool ScreenBorderContraints();

	bool disable_inputs;

	ControlerType m_controllerType;
	void SetControllerType(ControlerType contoller);

	//IA
	sf::Vector2f GetInputsToGetPosition(sf::Vector2f position, sf::Time deltaTime);
	bool arrived_at_destination;
	sf::Vector2f m_destination;

	//TRON SPECIFIC
	Discoball* m_discoball;
	float discoball_curAngle;
	bool discoball_clockwise;
	void GetDiscoball(GameObject* discoball, float angle_collision) override;
	void PlayerContact(GameObject* player, float angle_collision) override;
	void ManageDiscoball(sf::Time deltaTime);
	void ThrowDiscoball();
	void SwitchRotation();
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
	float throw_curSpeedBonus;

	Teams m_team;
	void GetPortal(GameObject* portal) override;
	PlayableCharacters m_character;
	IngameScript m_script;
	bool isLaunchingScript;
	void LoadPlayerShipWithScript(IngameScript script) override;

	void PlayStroboscopicEffect(Time effect_duration, sf::Time time_between_poses);
	sf::Clock stroboscopic_effect_clock;

protected:
	bool moving;
	bool movingX;
	bool movingY;
	sf::Vector2f m_input_direction;
};

#endif // SHIP_H_INCLUDED
