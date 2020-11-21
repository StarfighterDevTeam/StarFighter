#ifndef GAMELOOP_H_INCLUDED
#define GAMELOOP_H_INCLUDED

#include "Gamestate.h"

extern Game* CurrentGame;

class Gameloop : public GameState
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
	int SavePlayer(Ship* playerShip = (*CurrentGame).m_playerShip);
	string LoadPlayerSave(Ship* playerShip = (*CurrentGame).m_playerShip);
	bool AddToKnownScenes(string scene_name, Ship* playerShip = (*CurrentGame).m_playerShip);
	void SaveSceneHazardLevelUnlocked(string scene_name, int hazard_level, Ship* playerShip = (*CurrentGame).m_playerShip);
	int GetSceneHazardLevelUnlocked(string scene_name, Ship* playerShip = (*CurrentGame).m_playerShip);
	void LoadAllScenes(string scenes_file);
	void LoadAllEnemies(string enemies_file);
	void LoadAllFX(string FX_file);
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

#endif //GAMELOOP_H_INCLUDED