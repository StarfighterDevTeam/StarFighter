#ifndef METAGAME_H_INCLUDED
#define METAGAME_H_INCLUDED

#include "Scene.h"
#include "GameState.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <list>

class InGameState : public GameState
{
public:
	Ship* playerShip;
	Scene* currentScene;
	Scene* nextScene;
	map<string, int> knownScenes;
	string currentSceneSave;

	InGameStateMachine IG_State;

	void Initialize(Player player);
	void Update(Time deltaTime);
	void Draw();
	void Release();
	int SavePlayer(string file);
	void InGameState::SaveShipEquipment(string file, list<Independant*>* equipped, list<Independant*>* stored);
	void InGameState::SetEquipementConfigLine(Equipment* actualEquipment, bool equipped, vector<string>* configLine);
	string LoadPlayerSave(string file);
	bool AddToKnownScenes(string scene_name);
	void SetSceneHazardLevel(string scene_name, int hazard_level);
	int GetSceneHazardLevel(string scene_name);
	void InGameStateMachineCheck(sf::Time deltaTime);
	sf::Clock clockHubExit;
	sf::Clock bossSpawnCountdown;
	bool hasDisplayedDestructionRatio;

private:
	sf::RenderWindow* mainWindow;
	sf::Text* framerate;
};

#endif //METAGAME_H_INCLUDED