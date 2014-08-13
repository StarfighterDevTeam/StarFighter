#ifndef SCENE_H_INCLUDED
#define SCENE_H_INCLUDED

#include "FileLoader.h"

using namespace std;

enum InGameStateMachine
{
	LOADING,
	SCROLLING,
	LAST_SCREEN,
	TRANSITION_PHASE1_2,
	TRANSITION_PHASE2_2,
	HUB_ROAMING,
};

enum ScenesData
{
	SCENE_NAME,
	SCENE_FILENAME,
	SCENE_LINK_UP,
	SCENE_LINK_DOWN,
	SCENE_LINK_RIGHT,
	SCENE_LINK_LEFT,
};

enum Directions
{
	DIRECTION_UP,
	DIRECTION_DOWN,
	DIRECTION_RIGHT,
	DIRECTION_LEFT,
	NB_DIRECTIONS
};

class Scene
{
public:
	//Scene(string name, ShipConfig* shipConf);
	Scene(string name, bool reverse_scene=false, bool first_scene=false);
	void Scene::Update(Time deltaTime);
	Ship* Scene::GetPlayerShip();
	void Scene::GenerateEnemies(Time deltaTime);
	void Scene::EndSceneAnimation(float transition_UP, float transition_DOWN);
	void Scene::ExitHubTransition(float transition_speed_UP, float transition_speed_DOWN);
	void Scene::LoadSceneFromFile(string name, bool reverse_scene=false, bool first_scene=false);
	void Scene::SetScenePositionAndSpeed(sf::Vector2i direction, float vspeed, float w, float h);

	bool getPhaseShifter(int index);
	void setPhaseShifter(int index, bool b);

	float vspeed;
	bool sceneIsOver;

	float Scene::getSceneBeastScore();
	
	Independant* bg;
	sf::Vector2i direction;
	bool generating_enemies;

	int getSceneHazardBreakValue();
	int getSceneHazardLevelValue();
	std::string links[Directions::NB_DIRECTIONS];

private:
	Ship* playerShip;
	list<EnemyBase> enemies;
	list<EnemyBase> enemies_ranked_by_class[NBVAL_EnemyClass];
	int total_class_probability[NBVAL_EnemyClass];
	list<EnemyBase*> sceneIndependantsLayered[NBVAL_EnemyClass];
	sf::RenderWindow* mainWindow;
	
	int hazard_break_value;
	int hazard_level;

	bool phaseShifter[SceneBooleans::NBVAL_SceneBooleans];

	void hubRoaming();
	sf::Clock clockHubExit;
	sf::Vector2f ApplyScrollingDirectionOnPosition(sf::Vector2f position);
	sf::Vector2f ApplyScrollingDirectionOnSpeed(float vspeed);

	void HazardBreakEvent();
};

#endif // STARFIGHTER_H_INCLUDED