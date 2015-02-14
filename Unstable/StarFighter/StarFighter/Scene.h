#ifndef SCENE_H_INCLUDED
#define SCENE_H_INCLUDED

#include "FileLoader.h"

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
	void Scene::Update(Time deltaTime);
	void Scene::Draw(sf::RenderWindow* window);
	void Scene::GenerateEnemies(Time deltaTime);
	void Scene::GenerateBoss();
	void Scene::EndSceneAnimation(float transition_UP, float transition_DOWN);
	void Scene::ExitHubTransition(float transition_speed_UP, float transition_speed_DOWN);
	void Scene::LoadSceneFromFile(string name, int hazard_level, bool reverse_scene = false, bool first_scene = false);
	void Scene::SetLinkZone(Directions direction);
	std::string Scene::LoadSceneNameFromFile(string name);
	string m_name;
	float vspeed;
	bool m_hazardbreak_has_occurred;

	float Scene::getSceneBeastScore();
	
	Independant* bg;
	Directions direction;
	bool generating_enemies;
	bool generating_boss;

	int getSceneHazardBreakValue();
	int getSceneHazardLevelValue();
	std::string links[Directions::NO_DIRECTION];
	std::string links_displayname[Directions::NO_DIRECTION];
	HudElement link_zone[Directions::NO_DIRECTION];

private:
	Ship* playerShip;
	list<EnemyBase> enemies;
	list<EnemyBase*> boss_list;
	list<EnemyBase> enemies_ranked_by_class[NBVAL_EnemyClass];
	int total_class_probability[NBVAL_EnemyClass];
	list<EnemyBase*> sceneEnemyClassesAvailable[NBVAL_EnemyClass];
	sf::RenderWindow* mainWindow;
	
	int hazard_break_value;
	int hazard_level;

	sf::Clock clockHubExit;

	void HazardBreakEvent();
};

#endif // STARFIGHTER_H_INCLUDED