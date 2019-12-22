#ifndef SHIP_H_INCLUDED
#define SHIP_H_INCLUDED

#include "Globals.h"
#include "GameObject.h"
#include "InputGuy.h"
#include "Game.h"
#include "SFTextPop.h"
#include "Door.h"

#define SHIP_SPRITE_RATE_SEC        0.2f

enum PlayerActions
{
	Action_Idle,
	Action_Firing,
	Action_Muting,
	Action_Pausing,
	Action_Editor,
	Action_TurnEditor,
	Action_Erase,
	Action_Offset,
	Action_Add1,
	Action_Add2,
	Action_Add3,
	Action_Add4,
	Action_Add5,
	Action_Add6,
	Action_Add7,
	Action_Add8,
	Action_Left,
	Action_Right,
	Action_Up,
	Action_Down,
	NBVAL_PlayerActions,
};

enum PlayerInputStates
{
	Input_Release,//0
	Input_Tap,//1
	Input_Hold,//2
};

enum MoveState
{
	Move_Idle,
	Move_Moving,
};

class Ship : public GameObject
{
public :
	Ship();
	Ship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	Ship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size);
	Ship(sf::Vector2f position, sf::Vector2f speed, sf::Color color, float radius, float stroke_size = 0);
	void Init();
	virtual ~Ship();
	void update(sf::Time deltaTime) override;
	
	void ManageHudControls(sf::Vector2f inputs_directions);
	bool ScreenBorderContraints();
	void UpdateRotation();

	bool m_disable_inputs;
	ControlerType m_controllerType;
	PlayerInputStates m_inputs_states[NBVAL_PlayerActions];
	bool m_actions_states[NBVAL_PlayerActions];

	void SetControllerType(ControlerType contoller);
	void GetInputState(bool input_guy_boolean, PlayerActions action);
	void UpdateInputStates();
	void PlayStroboscopicEffect(Time effect_duration, sf::Time time_between_poses);

	static int Ship::SaveShip(Ship* ship);
	static bool Ship::LoadShip(Ship* ship);

	sf::Clock m_stroboscopic_effect_clock;

	SFPanel* m_SFTargetPanel;
	SFPanelTypes m_is_asking_SFPanel;

	//Music Doors
	bool Move(PlayerActions action);
	bool MoveEditor(PlayerActions action);

	static bool IsMovementPossible(pair<int, int> tileA, pair<int, int> tileB);
	
	bool m_editor_mode;
	Door* m_editor_door;
	float m_speed_max;
	MoveState m_move_state;
	

private:
	bool m_moving;
	bool m_movingX;
	bool m_movingY;
};

#endif // SHIP_H_INCLUDED
