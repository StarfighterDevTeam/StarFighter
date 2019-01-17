#ifndef METAGAME_H_INCLUDED
#define METAGAME_H_INCLUDED

#include "GameState.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "Game.h"
#include "Ship.h"
#include "SFPanelSpecific.h"
#include "FileLoadUtils.h"

#include "Robot.h"

class GameObject;

class InGameState : public GameState
{
public:
	GameObject* playerShip;

	void Initialize(Player player);
	void Update(sf::Time deltaTime);
	void UpdateUI(sf::Time deltaTime);
	void Draw();
	void Release();
	void UpdateCamera(sf::Time deltaTime);
	static void CreateSFPanel(SFPanelTypes panel_type, Ship* playerShip);
	static void DestroySFPanel(Ship* playerShip);
	static void LoadCSVFile(string scenes_file);

	//ROBOT
	Robot m_robots[2];

	void InitRobots();
	void InitUI();

	void AttackResolution();
	void EffectsResolution();
	void GrabResolution();
	void GuardResolution();

	bool ResolveAttack(WeaponAttack* attack, SlotIndex target_index, bool is_execution, bool is_counter_attack, bool &range_weapon_used, bool &triggers_execution, bool &triggers_counterattack);
	void ClearAttacksListResolved();
	int GetRobotIndexOfLastAttackResolved();

	int m_nb_attacks_becore_contextual;

	//UI
	void UI_GetAction(int robot_index);
	void UI_SyncSml(int crew_index, int robot_index);
	SFText m_UI_turn;
	SFText m_UI_phase;
	float m_guard_timer;

private:
	sf::RenderWindow* mainWindow;
	sf::Text* framerate;
};

#endif //METAGAME_H_INCLUDED