#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include "Mission.h"

#define NB_MISSIONS_MAX		5

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

class Player : public HumanShip
{
public :
	Player(sf::Vector2i sector_index);
	~Player();

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
	void MarkThis(SpatialObject* target, bool isMission) override;
	void UnmarkThis(SpatialObject* target, bool isMission) override;
	void UpdateMarkers(sf::Time deltaTime) override;
	int GetMarkedObjectsCount() override;
	bool AcceptMission(Mission* mission);
	void SetCurrentMission(Mission* mission);
	void RemoveMission(Mission* mission);
	void EndMission(Mission* mission, MissionStatus status);
	bool CycleMission();
	void UpdateMissions();
	void Death() override;
	void GetHitByLoot(GameObject* loot) override;
	bool CheckMarkingConditions() override;
	void GetHitByAmmo(GameObject* ammo, bool send_network_packet = true) override;

	void DebugDrawMissions();
	void DebugDrawMoney();

	vector<SpatialObject*> m_marked_objects;
	vector<Mission*> m_missions;
	int m_money;

	//Network
	void UpdateNetwork();
	bool SendNetworkPacket(NetworkPacketType type, Ammo* ammo = NULL, bool is_local_player = false, Asteroid* asteroid = NULL, AsteroidField* field = NULL, Planet* planet = NULL);
	bool SendNetworkPacket(Ammo* ammo, bool is_local_player) override;
	bool SendNetworkPacket(Ammo* ammo);
	bool SendNetworkPacket(Asteroid* asteroid) override;
	bool SendNetworkPacket(AsteroidField* field);
	bool SendNetworkPacket(Planet* planet);
	bool SendNetworkPacket(sf::Vector2i sector_index);
	bool ReceiveNetworkPacket();
	GameObject* GetOnlinePlayer();
};

#endif // PLAYER_H_INCLUDED
