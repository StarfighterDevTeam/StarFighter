#ifndef METAGAME_H_INCLUDED
#define METAGAME_H_INCLUDED

#include "GameState.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "Game.h"
#include "Ship.h"
#include "ShipIA.h"
#include "Discoball.h"
#include "Goal.h"
#include "Bumper.h"
#include "LevelPortal.h"

class GameObject;

enum ScreenBorder
{
	BorderUp,
	BorderDown,
	BorderRight,
	BorderLeft,
};

class InGameState : public GameState
{
public:
	GameObject* playerShip;

	void Initialize(Player player);
	void Update(sf::Time deltaTime);
	void Draw();
	void Release();
	void UpdateCamera(sf::Time deltaTime);

	void StartMainMenu();
	void StartMultiGame(bool reset_scores = true);
	void StartMultiGameBig(bool reset_scores = true);

	void InitializeSoloCharacter();
	void InitializeMapDesign();

	void StartTuto01();
	void StartTuto02();
	void StartTuto03();
	void StartTuto04();
	void ShootingTrainingTuto01();

	void SetIngameScript(IngameScript script, bool reset_scores = true);

	IngameScript m_script;
	IngameScript m_next_script;

	static Ship* CreateCharacter(sf::Vector2f position = sf::Vector2f(REF_WINDOW_RESOLUTION_X / 2, REF_WINDOW_RESOLUTION_Y / 2), PlayableCharacters character = Natalia, Teams team = BlueTeam, bool IA = false);
	static ShipIA* CreateIACharacter(sf::Vector2f position = sf::Vector2f(REF_WINDOW_RESOLUTION_X / 2, REF_WINDOW_RESOLUTION_Y / 2), PlayableCharacters character = Natalia, Teams team = BlueTeam, IADifficultyLevel IA_level = IAEasy, bool activated = true);

	static Discoball* CreateDiscoball(sf::Vector2f position = sf::Vector2f(REF_WINDOW_RESOLUTION_X / 2, REF_WINDOW_RESOLUTION_Y / 2));
	static Bumper* CreateBumper(BumperType type, sf::Vector2f position, bool vertical, float size);
	static LevelPortal* CreateLevelPortal(IngameScript script, sf::Vector2f position);
	static GameObject* CreateGoal(Teams team, sf::Vector2f position, sf::Vector2f size);

	static void AutoFillGoalBumpers(ScreenBorder border, float goal_size, float goal_pos);

	PlayableCharacters chosen_character;

	//HUD
	sf::Text score_blue_text;
	sf::Text score_red_text;

private:
	sf::RenderWindow* mainWindow;
	sf::Text* framerate;
};

#endif //METAGAME_H_INCLUDED