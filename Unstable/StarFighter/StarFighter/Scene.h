#ifndef SCENE_H_INCLUDED
#define SCENE_H_INCLUDED

#include "FileLoader.h"

using namespace std;

class Scene
{
public:
	//Scene(string name, ShipConfig* shipConf);
	Scene(string name);
	void Scene::StartGame(sf::RenderWindow*	window);
	void Scene::Update(Time deltaTime);
	Ship* Scene::GetPlayerShip();
	void Scene::GenerateEnemies(Time deltaTime);
	void Scene::EndSceneAnimation(float transition_UP, float transition_DOWN);
	void Scene::ExitHubTransition(float transition_speed_UP, float transition_speed_DOWN);
	void Scene::LoadSceneFromFile(string name);

	bool getPhaseShifter(int index);
	void setPhaseShifter(int index, bool b);

	float vspeed;
	bool sceneIsOver;

	float Scene::getSceneBeastScore();
	
	
	Independant* bg;
	Independant* hub;

	int getSceneHazardBreakValue();
	int getSceneHazardLevelValue();

private:
	Ship* playerShip;
	list<EnemyBase> enemies;
	list<EnemyBase> enemies_ranked_by_class[NBVAL_EnemyClass];
	int total_class_probability[NBVAL_EnemyClass];
	list<vector<string>> config;
	list<EnemyBase*> sceneIndependantsLayered[NBVAL_EnemyClass];
	//Independant* hubClone;
	sf::RenderWindow* mainWindow;
	
	int hazard_break_value;
	int hazard_level;

	bool phaseShifter[SceneBooleans::NBVAL_SceneBooleans];

	void hubRoaming();
	sf::Clock clockHubExit;
	int transitionDestination;

	sf::Vector2i scrolling_direction;
	sf::Vector2f ApplyScrollingDirectionOnPosition(sf::Vector2f position);
	sf::Vector2f ApplyScrollingDirectionOnSpeed(float vspeed);
	//float ApplyScrollingDirectionOnRotation();

	void HazardBreakEvent();
};

#endif // STARFIGHTER_H_INCLUDED