#ifndef SCENE_H_INCLUDED
#define SCENE_H_INCLUDED

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include "Globals.h"
#include "Enemy.h"

using namespace std;

class Scene
{

public:
	Scene(string name);

private:
	Enemy*  Scene::LoadEnemy(string name, float probability);
	Weapon* Scene::LoadWeapon(string name, int fire_direction);
	Ammo* Scene::LoadAmmo(string name);

	list<vector<string>>* Scene::FileLoader(string name);


	list<Enemy*> enemies;
	list<vector<string>> config;
	list<vector<string>> enemyConfig;
	list<vector<string>> weaponConfig;
	list<vector<string>> ammoConfig;
	Independant* bg;
};

#endif // STARFIGHTER_H_INCLUDED