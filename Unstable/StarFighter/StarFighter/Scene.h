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
};



class Scene
{
public:
	//Scene(string name, ShipConfig* shipConf);
	Scene(string name, int hazard_level, bool reverse_scene=false, bool first_scene=false);
	Scene(string name);
	void Update(Time deltaTime);
	void Draw(sf::RenderWindow* window);
	void GenerateEnemies(Time deltaTime);
	void GenerateBoss();
	void EndSceneAnimation(float transition_UP, float transition_DOWN);
	void ExitHubTransition(float transition_speed_UP, float transition_speed_DOWN);
	void LoadSceneFromFile(string name, int hazard_level, bool reverse_scene = false, bool first_scene = false);
	void SetLinkZone(Directions direction);
	std::string LoadSceneNameFromFile(string name);
	string m_name;
	float vspeed;
	bool m_hazardbreak_has_occurred;

	float getSceneBeastScore();
	
	Background* bg;
	Directions direction;
	bool generating_enemies;
	bool generating_boss;
	sf::Clock spawnClock;

	int getSceneHazardLevelValue();
	std::string links[Directions::NO_DIRECTION];

	vector <EnemyGenerator*> sceneEnemyGenerators[NBVAL_EnemyClass];
	void GenerateEnemiesv2(Time deltaTime);
	void SpawnEnemy(int enemy_class);
	void CollateralSpawnCost(float collateral_cost, float collateral_multiplier = 0, int below_enemy_class = (int)EnemyClass::NBVAL_EnemyClass);

	void HazardBreak();

private:
	list<EnemyBase*> boss_list;
	vector<EnemyBase*> enemies_ranked_by_class[NBVAL_EnemyClass];
	int total_class_probability[NBVAL_EnemyClass];
	sf::RenderWindow* mainWindow;
	int hazard_level;

	sf::Clock clockHubExit;
};

#endif // STARFIGHTER_H_INCLUDED