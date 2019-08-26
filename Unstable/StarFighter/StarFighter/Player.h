#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include "AIShip.h"

enum PlayerActions
{
	Action_Idle,
	Action_Speeding,
	Action_Braking,
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

class Player : public Ship
{
public :
	Player(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	virtual ~Player();
	void Update(sf::Time deltaTime) override;
	void Draw(RenderTarget& screen) override;
	void UpdateInputStates();
	bool UpdateAction(PlayerActions action, PlayerInputStates state_required, bool condition);
	void GetInputState(bool input_guy_boolean, PlayerActions action);
	int SaveShip(Player* ship);
	bool LoadShip(Player* ship);
	PlayerInputStates m_inputs_states[NBVAL_PlayerActions];
	void SetControllerType(ControlerType contoller);

	bool m_actions_states[NBVAL_PlayerActions];
	bool m_disable_inputs;
	ControlerType m_controllerType;

	//Star Hunter
	void MarkThis(AIShip* target) override;
	AIShip* GetTargetableEnemyShip(const GameObject* ref_object, const float dist_max, const float angle_delta_max) override;
	vector<AIShip*> m_marked_ships;
};

#endif // PLAYER_H_INCLUDED
