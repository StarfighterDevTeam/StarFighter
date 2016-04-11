#ifndef METAGAME_H_INCLUDED
#define METAGAME_H_INCLUDED

#include "SFMapPanel.h"
#include "SFDialogPanel.h"
#include "SFInventoryPanel.h"
#include "SFMenuPanel.h"
#include "Scene.h"
#include "GameState.h"
#include <fstream>
#include <iostream>
#include <sstream>

extern Game* CurrentGame;

class InGameState : public GameState
{
public:
	Ship* m_playerShip;
	Scene* m_currentScene;
	Scene* m_nextScene;
	InGameStateMachine m_IG_State;

	void Initialize(Player player);
	void Update(Time deltaTime);
	void Draw();
	void Release();
	int SavePlayer(string file, Ship* playerShip = (*CurrentGame).m_playerShip);
	string LoadPlayerSave(string file, Ship* playerShip = (*CurrentGame).m_playerShip);
	bool AddToKnownScenes(string scene_name, Ship* playerShip = (*CurrentGame).m_playerShip);
	void SaveSceneHazardLevelUnlocked(string scene_name, int hazard_level, Ship* playerShip = (*CurrentGame).m_playerShip);
	int GetSceneHazardLevelUnlocked(string scene_name, Ship* playerShip = (*CurrentGame).m_playerShip);
	void LoadAllScenes(string scenes_file);
	void UpdateShipConfig(Ship* ship, string config_name);

	void InGameStateMachineCheck(sf::Time deltaTime);
	void SpawnInScene(string scene_name, Ship* playerShip = (*CurrentGame).m_playerShip);

	void PlayerTakesExit();

	sf::Clock m_clockHubExit;
	sf::Clock m_bossSpawnCountdown;
	bool m_hasDisplayedDestructionRatio;

	void UpdatePortalsMaxUnlockedHazardLevel(Scene* scene, Ship* playerShip = (*CurrentGame).m_playerShip);
	void RespawnInLastSafePoint();
	void CheckScriptedDialogs();

	static void CreateSFPanel(SFPanelTypes panel_type, Ship* playerShip);
	static void DestroySFPanel(Ship* playerShip);

private:
	sf::RenderWindow* mainWindow;
	sf::Text* framerate;
};

#endif //METAGAME_H_INCLUDED