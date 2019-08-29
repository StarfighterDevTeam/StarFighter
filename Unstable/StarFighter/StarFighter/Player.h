#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include "Mission.h"

#define NB_MISSIONS_ACCEPTED_MAX		5

enum PlayerActions
{
	Action_Idle,
	Action_Speeding,
	Action_Braking,
	Action_Firing,
	Action_CyclingMission,
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
	void SetControllerType(ControlerType contoller);
	PlayerInputStates m_inputs_states[NBVAL_PlayerActions];

	bool m_actions_states[NBVAL_PlayerActions];
	bool m_disable_inputs;
	ControlerType m_controllerType;

	//Star Hunter
	void MarkThis(SpatialObject* target) override;
	SpatialObject* GetTargetableEnemyShip(const GameObject* ref_object, const float dist_max, const float angle_delta_max) override;
	bool AcceptMission(Mission* mission);
	void SetCurrentMission(Mission* mission);
	void RemoveMission(Mission* mission);
	void CancelMission(Mission* mission);
	bool CycleMission();
	void AddMissionMarker(SpatialObject* target);
	void RemoveMissionMarker(SpatialObject* target);

	vector<SpatialObject*> m_marked_objects;
	vector<Mission*> m_missions;
};

#endif // PLAYER_H_INCLUDED
