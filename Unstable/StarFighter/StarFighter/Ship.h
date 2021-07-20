#ifndef SHIP_H_INCLUDED
#define SHIP_H_INCLUDED

#include "Globals.h"
#include "GameObject.h"
#include "InputGuy.h"
#include "Game.h"
#include "SFTextPop.h"

#define SHIP_START_X					990
#define SHIP_START_Y					540
#define SHIP_ACCELERATION_FORWARD		200.0f
#define SHIP_BRAKE_SPEED				400.0f
#define SHIP_ACCELERATION_BACKWARD		100.0f
#define SHIP_DECELERATION				50.0f
#define SHIP_MAX_SPEED_FORWARD			350.0f
#define SHIP_MAX_SPEED_BACKWARD			150.0f
#define SHIP_MIN_SPEED					20.0f
#define SHIP_TURN_RATE					100.0f
#define SHIP_SPRITE_RATE_SEC			0.2f

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

class Ship : public GameObject
{
public :
	Ship();
	Ship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	Ship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size);
	void Init();
	virtual ~Ship();
	void update(sf::Time deltaTime) override;
	bool ScreenBorderContraints();
	void UpdateRotation();
	void Draw(sf::RenderTexture* screen) override;
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

	//MICRO BOATS
	float m_acceleration;
	bool m_reverse_move;

	sf::Text m_debug_text;

private:
	bool m_moving;
	bool m_movingX;
	bool m_movingY;
};

#endif // SHIP_H_INCLUDED
