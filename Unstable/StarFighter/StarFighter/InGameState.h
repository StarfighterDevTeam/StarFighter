#ifndef METAGAME_H_INCLUDED
#define METAGAME_H_INCLUDED

#include "SFInventoryPanel.h"
#include "SFMenuPanel.h"
#include "Scene.h"
#include "GameState.h"
#include <fstream>
#include <iostream>
#include <sstream>

class InGameState : public GameState
{
public:
	Ship* m_playerShip;
	Scene* m_currentScene;
	Scene* m_nextScene;
	map<string, int> m_knownScenes;
	string m_currentSceneSave;
	InGameStateMachine m_IG_State;

	void Initialize(Player player);
	void Update(Time deltaTime);
	void Draw();
	void Release();
	int SavePlayer(string file);
	string LoadPlayerSave(string file);
	bool AddToKnownScenes(string scene_name);
	void SaveSceneHazardLevelUnlocked(string scene_name, int hazard_level);
	int GetSceneHazardLevelUnlocked(string scene_name);
	void InGameStateMachineCheck(sf::Time deltaTime);

	sf::Clock m_clockHubExit;
	sf::Clock m_bossSpawnCountdown;
	bool m_hasDisplayedDestructionRatio;

	void UpdatePortalsMaxUnlockedHazardLevel(Scene* scene_);
	void RespawnInLastHub();

	static void CreateSFPanel(SFPanelTypes panel_type, Ship* playerShip);
	static void DestroySFPanel(Ship* playerShip);

private:
	sf::RenderWindow* mainWindow;
	sf::Text* framerate;
};

#endif //METAGAME_H_INCLUDED