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
	void StartShooting01();

	void SetIngameScript(IngameScript script, bool reset_scores = true);

	IngameScript m_script;
	IngameScript m_next_script;

	Ship* CreateCharacter(sf::Vector2f position = sf::Vector2f(REF_WINDOW_RESOLUTION_X / 2, REF_WINDOW_RESOLUTION_Y / 2), PlayableCharacters character = Natalia, Teams team = BlueTeam, bool IA = false);
	ShipIA* CreateIACharacter(sf::Vector2f position = sf::Vector2f(REF_WINDOW_RESOLUTION_X / 2, REF_WINDOW_RESOLUTION_Y / 2), PlayableCharacters character = Natalia, Teams team = BlueTeam, IADifficultyLevel IA_level = IAEasy, bool activated = true);

	Discoball* CreateDiscoball(sf::Vector2f position = sf::Vector2f(REF_WINDOW_RESOLUTION_X / 2, REF_WINDOW_RESOLUTION_Y / 2));
	Bumper* CreateBumper(BumperType type, sf::Vector2f position, bool vertical, float size);
	LevelPortal* CreateLevelPortal(IngameScript script, sf::Vector2f position);
	GameObject* CreateGoal(Teams team, sf::Vector2f position, sf::Vector2f size);

	PlayableCharacters chosen_character;

	//HUD
	sf::Text score_blue_text;
	sf::Text score_red_text;

private:
	sf::RenderWindow* mainWindow;
	sf::Text* framerate;
};

#endif //METAGAME_H_INCLUDED