#ifndef SCENE_H_INCLUDED
#define SCENE_H_INCLUDED

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include "Globals.h"
#include "Enemy.h"
#include "Ship.h"
#include "MovePattern.h"

using namespace std;

enum SceneDataEnemy
{
	SCENE_COMPARE_ENEMY,//0
	ENEMY,//1
	ENEMY_PROBABILITY,//2
	ENEMY_POOLSIZE,//3
	ENEMY_CLASS,//4
};

enum SceneDataBackground
{
	SCENE_COMPARE_BACKGROUND,//0
	BACKGROUND_NAME,//1
	BACGKROUND_WIDTH,//2
	BACKGROUND_HEIGHT,//3
};

enum EnemyData
{
	ENEMY_NAME,//0
	ENEMY_ARMOR,//1
	ENEMY_SHIELD,//2
	ENEMY_SHIELD_REGEN,//3
	ENEMY_DAMAGE,//4
	WEAPON,//5
	AMMO,//6
	ENEMY_IMAGE_NAME,//7
	ENEMY_WIDTH,//8
	ENEMY_HEIGHT,//9
	ENEMY_FRAMES,//10
	ENEMY_SPEED,//11
};

enum WeaponData
{
	WEAPON_NAME,//0
	RATE_OF_FIRE,//1
	WEAPON_IMAGE_NAME,//2
	WEAPON_WIDTH,//3
	WEAPON_HEIGHT,//4
	WEAPON_SOUND,//5
};

enum AmmoData
{
	AMMO_NAME,//0
	AMMO_DAMAGE,//1
	AMMO_SPEED,//2
	AMMO_IMAGE_NAME,//3
	AMMO_WIDTH,//4
	AMMO_HEIGHT,//5
	AMMO_FRAMES,//6
	AMMO_FX,//7
};

enum MovePatternData
{
	MOVEPATTERN_NAME,//0
	MOVEPATTERN_RADIUS,//1
	MOVEPATTERN_TRIGGERY,//2
};

enum FXData
{
	FX_TYPE,//0
	FX_NAME,//1
	FX_FILENAME,//2
	FX_WIDTH,//3
	FX_HEIGHT,//4
	FX_FRAMES,//5
	FX_DURATION,//6
};

struct EnemyBase
{
	Enemy* enemy;
	float probability;
	int poolsize;
	int enemyclass;
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
	EnemyBase*  Scene::LoadEnemy(string name, float probability, int poolSize, int enemyClass);
	Weapon* Scene::LoadWeapon(string name, int fire_direction, Ammo* ammo);
	Ammo* Scene::LoadAmmo(string name);
	FX* Scene::LoadFX(string name);
	MovePattern* Scene::LoadMovePattern(string name, float radius, float triggerY);
	list<vector<string>>* Scene::FileLoader(string name);

	Ship* playerShip;
	list<EnemyBase> enemies;
	list<MovePattern> mpatterns;
	list<vector<string>> config;
	list<vector<string>> enemyConfig;
	list<vector<string>> weaponConfig;
	list<vector<string>> ammoConfig;
	list<vector<string>> FXConfig;
	list<vector<string>> movepatternConfig;
	Independant* bg;
	sf::RenderWindow* mainWindow;
	sf::Text* framerate;
};

#endif // STARFIGHTER_H_INCLUDED