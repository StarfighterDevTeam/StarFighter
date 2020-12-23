#ifndef GAMELOOP_H_INCLUDED
#define GAMELOOP_H_INCLUDED

#include "Gamestate.h"

extern Game* CurrentGame;

class Gameloop : public GameState
{
public:
	Gameloop();
	
	Ship* m_playership;
	Scene* m_currentScene;
	Scene* m_nextScene;

	void Initialize(Player player);
	void Update(Time deltaTime);
	void Draw();
	void Release();
	bool AddToKnownScenes(string scene_name, Ship* playership = (*CurrentGame).m_playership);
	void SaveSceneHazardLevelUnlocked(string scene_name, int hazard_level, Ship* playership = (*CurrentGame).m_playership);
	int GetSceneHazardLevelUnlocked(string scene_name, Ship* playership = (*CurrentGame).m_playership);
	void LoadAllScenes(string scenes_file);
	void LoadAllEnemies(string enemies_file);
	void LoadAllFX(string FX_file);
	void LoadAllUpgrades(string upgrades_file);
	void UpdateShipConfig(Ship* ship, string config_name);

	void GameloopStateMachineCheck(sf::Time deltaTime);
	void SpawnInScene(string scene_name, Ship* playership, bool display_scene_name);

	void PlayerTakesExit();

	sf::Clock m_clockHubExit;
	sf::Clock m_bossSpawnCountdown;
	bool m_hasDisplayedDestructionRatio;

	void UpdatePortalsMaxUnlockedHazardLevel(Scene* scene, Ship* playership = (*CurrentGame).m_playership);
	void CheckScriptedDialogs();

	static void CreateSFPanel(SFPanelTypes panel_type, Ship* playership);
	static void DestroySFPanel(Ship* playership);

private:
	sf::RenderWindow* mainWindow;
	sf::Text* framerate;
};

#endif //GAMELOOP_H_INCLUDED