#ifndef SCENE_H_INCLUDED
#define SCENE_H_INCLUDED

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include "Globals.h"
#include "Enemy.h"
#include "Ship.h"
#include "Exceptions.h"

using namespace std;

struct EnemyBase
{
	Enemy* enemy;
	float probability;
	int poolsize;
};

class Scene
{

public:
	Scene(string name, ShipConfig* shipConf);
	void Scene::StartGame(sf::RenderWindow*	window);
	void Scene::Update(Time deltaTime);
	Ship* Scene::GetPlayerShip();
	void Scene::GenerateEnemies(Time deltaTime);

private:
	EnemyBase*  Scene::LoadEnemy(string name, float probability, int poolSize);
	Weapon* Scene::LoadWeapon(string name, int fire_direction, Ammo* ammo);
	Ammo* Scene::LoadAmmo(string name);

	list<vector<string>>* Scene::FileLoader(string name);

	Ship* playerShip;
	list<EnemyBase> enemies;
	list<vector<string>> config;
	list<vector<string>> enemyConfig;
	list<vector<string>> weaponConfig;
	list<vector<string>> ammoConfig;
	Independant* bg;
	sf::RenderWindow* mainWindow;
};

#endif // STARFIGHTER_H_INCLUDED