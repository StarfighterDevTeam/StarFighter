#ifndef SHIP_H_INCLUDED
#define SHIP_H_INCLUDED

#include "Globals.h"
#include "GameObject.h"
#include "InputGuy.h"
#include "Game.h"
#include "SFTextPop.h"
#include "Block.h"

#define SHIP_START_X                990
#define SHIP_START_Y                540
#define SHIP_ACCELERATION	        2000.0f
#define SHIP_DECCELERATION_COEF		5000.0f
#define SHIP_MAX_SPEED_HORIZONTAL	400.0f
#define SHIP_MIN_SPEED				50.0f
#define SHIP_SPRITE_RATE_SEC        0.2f
#define SHIP_JUMP_ACCELERATION	    800.f
#define SHIP_GRAVITY				30.0f
#define SHIP_MAX_SPEED_VERTICAL		3000.f

enum PlayerActions
{
	Action_Idle,
	Action_Jumping,
	Action_Muting,
	Action_Pausing,
	Action_EditorMode,
	Action_EditorFast,
	Action_Respawn,
	NBVAL_PlayerActions,
};

enum PlayerInputStates
{
	Input_Release,//0
	Input_Tap,//1
	Input_Hold,//2
};

enum PlayerAnimations
{
	Animation_Idle,
	Animation_WalkRight,
	Animation_WalkLeft,
};

enum PlayerJumpState
{
	PlayerJump_Idle,
	PlayerJump_Jumping,
	PlayerJump_Falling,
	PlayerJump_Hanging,
	PlayerJump_PullingUp,
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
	
	void ManageHudControls(sf::Vector2f inputs_directions);
	void ManageAcceleration(sf::Vector2f inputs_direction);
	void ManageJump();
	void IdleDecelleration(sf::Time deltaTime);
	bool ScreenBorderContraints();
	void MaxSpeedConstraints();
	void UpdateRotation();
	int ChooseAnimation();

	bool m_disable_inputs;
	ControlerType m_controllerType;
	PlayerInputStates m_inputs_states[NBVAL_PlayerActions];
	bool m_actions_states[NBVAL_PlayerActions];

	void SetControllerType(ControlerType contoller);
	void GetInputState(bool input_guy_boolean, PlayerActions action);
	void UpdateInputStates();
	bool UpdateAction(PlayerActions action, PlayerInputStates state_required, bool condition);
	void PlayStroboscopicEffect(Time effect_duration, sf::Time time_between_poses);

	static int Ship::SaveShip(Ship* ship);
	static bool Ship::LoadShip(Ship* ship);

	sf::Clock m_stroboscopic_effect_clock;

	SFPanel* m_SFTargetPanel;
	SFPanelTypes m_is_asking_SFPanel;

	PlayerJumpState m_state;
	bool Land(float coordinate) override;
	bool Fall() override;
	bool HitWallFromLeft(float coordinate) override;
	bool HitWallFromRight(float coordinate) override;
	bool HitCeiling(float coordinate) override;
	bool HangToWallFromLeft(float coordinate_x, float edge_height) override;
	bool HangToWallFromRight(float coordinate_x, float edge_height) override;

	void Respawn();

private:
	bool m_moving;
	bool m_movingX;
	bool m_movingY;
};

#endif // SHIP_H_INCLUDED
