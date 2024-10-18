#ifndef PLAYERSHIP_H_INCLUDED
#define PLAYERSHIP_H_INCLUDED

#include "Globals.h"
#include "GameObject.h"
#include "InputGuy.h"
#include "Game.h"
#include "SFTextPop.h"
#include "MapTile.h"

#define SHIP_START_X                990
#define SHIP_START_Y                540
#define SHIP_ACCELERATION	        2000.0f
#define SHIP_DECCELERATION_COEF		5000.0f
#define SHIP_MAX_SPEED				400.0f
#define SHIP_MIN_SPEED				50.0f
#define SHIP_SPRITE_RATE_SEC        0.2f

enum PlayerActions
{
	Action_Idle,
	Action_Firing,
	Action_Muting,
	Action_Pausing,
	NBVAL_PlayerActions,
};

enum PlayerInputStates
{
	Input_Release,//0
	Input_Tap,//1
	Input_Hold,//2
};

class PlayerShip : public GameObject
{
public :
	PlayerShip();
	PlayerShip(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2u size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	PlayerShip(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2u size);
	void Init();
	virtual ~PlayerShip();
	void update(const float DTIME) override;

	bool m_disable_inputs;
	ControlerType m_controllerType;
	PlayerInputStates m_inputs_states[NBVAL_PlayerActions];
	bool m_actions_states[NBVAL_PlayerActions];

	void SetControllerType(ControlerType contoller);
	void GetInputState(bool input_guy_boolean, PlayerActions action);
	void UpdateInputStates();
	bool UpdateAction(PlayerActions action, PlayerInputStates state_required, bool condition);
	void PlayStroboscopicEffect(Time effect_duration, sf::Time time_between_poses);

	static int PlayerShip::SaveShip(Ship* ship);
	static bool PlayerShip::LoadShip(Ship* ship);

	sf::Clock m_stroboscopic_effect_clock;

	SFPanel* m_SFTargetPanel;
	SFPanelTypes m_is_asking_SFPanel;

	//PIRATE
	float m_curForwardSpeed = 0.f;
	float m_targetForwardSpeed = 0.f;
	float m_turnSpeed;
	float m_curHeadingDegrees = 0.f;//in degrees
	float m_targetHeadingDegrees = 0.f;//in degrees

	MapTile* m_curTile = nullptr;
	SFText* m_textCurTileDebug = nullptr;

private:
	bool m_moving;
	bool m_movingX;
	bool m_movingY;
};

#endif // PLAYERSHIP_H_INCLUDED
