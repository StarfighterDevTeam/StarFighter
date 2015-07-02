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
	void GenerateEnemies(Time deltaTime);
	void GenerateBoss();
	void LoadSceneFromFile(string name, int hazard_level, bool reverse_scene = false, bool first_scene = false);
	string m_name;
	float vspeed;
	bool m_hazardbreak_has_occurred;

	float getSceneBeastScore();
	
	void DestroyScene();

	Background* bg;
	Directions direction;
	bool generating_enemies;
	bool generating_boss;
	sf::Clock spawnClock;

	int getSceneHazardLevelUnlockedValue();
	int getSceneHazardLevelValue();
	std::string links[Directions::NO_DIRECTION];

	vector<EnemyGenerator*> sceneEnemyGenerators;
	void GenerateEnemiesv2(Time deltaTime);
	void SpawnEnemy(int enemy_class);
	void CollateralSpawnCost(float collateral_cost, float collateral_multiplier = 0, int below_enemy_class = (int)EnemyClass::NBVAL_EnemyClass);
	void ApplyHazardLevelModifiers(int hazard_level_, Enemy& enemy_);

	void HazardBreak();
	int m_hazard_level_unlocked;
	bool canHazardBreak;
	vector <string> scenesLinkedToUpdate;

	sf::Text m_textHazardBreak;
	sf::Font* m_fontHazardBreak;
	void DisplayDestructions(bool hazard_break = false);
	bool IsLastSceneBeforeHub();

private:
	vector<EnemyBase*> boss_list;
	vector<EnemyBase*> enemies_ranked_by_class[NBVAL_EnemyClass];
	int total_class_probability[NBVAL_EnemyClass];
	int m_hazard_level;

};

#endif // STARFIGHTER_H_INCLUDED