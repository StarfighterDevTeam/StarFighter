#ifndef SCENE_H_INCLUDED
#define SCENE_H_INCLUDED

#include "FileLoader.h"
#include "EnemyGenerator.h"

using namespace std;

enum InGameStateMachine
{
	LOADING,
	SCROLLING,
	LAST_SCREEN,
	BOSS_FIGHT,
	TRANSITION_PHASE1_2,
	TRANSITION_PHASE2_2,
	HUB_ROAMING,
};

enum ScenesData
{
	SCENE_NAME,
	SCENE_FILENAME,
	SCENE_DISPLAYNAME,
	SCENE_LINK_UP,
	SCENE_LINK_DOWN,
	SCENE_LINK_RIGHT,
	SCENE_LINK_LEFT,
	SCENE_HAZARD_BREAK,
};

class Scene
{
public:
	//Scene(string name, ShipConfig* shipConf);
	Scene(string name, int hazard_level, bool reverse_scene=false, bool first_scene=false);
	Scene(string name);
	~Scene();
	void GenerateEnemies(Time deltaTime);
	void GenerateBoss();
	void LoadSceneFromFile(string name, int hazard_level, bool reverse_scene = false, bool first_scene = false);

	string m_name;
	float m_vspeed;
	bool m_hazardbreak_has_occurred;

	float getSceneBeastScore();

	Background* m_bg;
	Directions m_direction;
	bool m_generating_enemies;
	bool m_generating_boss;
	sf::Clock m_spawnClock;

	int getSceneHazardLevelUnlockedValue();
	int getSceneHazardLevelValue();

	std::string m_links[Directions::NO_DIRECTION];

	vector<EnemyGenerator*> m_sceneEnemyGenerators;

	void GenerateEnemiesv2(Time deltaTime);
	void SpawnEnemy(int enemy_class);
	void CollateralSpawnCost(float collateral_cost, float collateral_multiplier = 0, int below_enemy_class = (int)EnemyClass::NBVAL_EnemyClass);
	void ApplyHazardLevelModifiers(int hazard_level, Enemy& enemy);

	void HazardBreak();

	int m_hazard_level_unlocked;
	bool m_canHazardBreak;
	vector <string> m_scenesLinkedToUpdate;
	sf::Text m_textHazardBreak;
	sf::Font* m_fontHazardBreak;

	void DisplayDestructions(bool hazard_break = false);
	bool IsLastSceneBeforeHub();

private:
	vector<EnemyBase*> m_boss_list;
	vector<EnemyBase*> m_enemies_ranked_by_class[NBVAL_EnemyClass];
	int m_total_class_probability[NBVAL_EnemyClass];
	int m_hazard_level;
};

#endif // STARFIGHTER_H_INCLUDED