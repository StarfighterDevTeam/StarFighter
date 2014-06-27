#ifndef SCENE_H_INCLUDED
#define SCENE_H_INCLUDED

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include "Globals.h"
#include "Enemy.h"
#include "Ship.h"
#include "EnemyPool.h"
#include "FileLoadUtils.h"

using namespace std;

enum SceneBooleans
{
	ENDSCENE_PHASE1,
	ENDSCENE_PHASE2,
	ENDSCENE_PHASE3,
	ENDSCENE_PHASE4,

	EXITHUB_PHASE1,
	EXITHUB_PHASE2,
	EXITHUB_PHASE3,

	NBVAL_SceneBooleans,
};

enum TransitionList
{
	NO_TRANSITION,//0
	TRANSITION_UP,//1
	TRANSITION_DOWN,//2
	TRANSITION_LEFT,//3
	TRANSITION_RIGHT,//4
};

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

enum ShipModelData
{
	SHIPMODEL_COMPARE,//0
	SHIPMODEL_NAME,//1
	SHIPMODEL_IMAGE_NAME,//2
	SHIPMODEL_WIDTH,//3
	SHIPMODEL_HEIGHT,//4
	SHIPMODEL_FRAMES,//5
	SHIPMODEL_MAXSPEED_X,//6
	SHIPMODEL_MAXSPEED_Y,//7
	SHIPMODEL_DECCELERATION,//8
	SHIPMODEL_ACCELERATION_X,//9
	SHIPMODEL_ACCELERATION_Y,//10
	SHIPMODEL_ARMOR,//11
	SHIPMODEL_SHIELD,//12
	SHIPMODEL_SHIELD_REGEN,//13
	SHIPMODEL_WEAPON,//14
	SHIPMODEL_AMMO,//15
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
	ENEMY_FX_DEATH,//12
	ENEMY_VALUE,//13
};

enum WeaponData
{
	WEAPON_NAME,//0
	RATE_OF_FIRE,//1
	WEAPON_IMAGE_NAME,//2
	WEAPON_WIDTH,//3
	WEAPON_HEIGHT,//4
	WEAPON_SOUND,//5
	WEAPON_MULTISHOT,//6
	WEAPON_XSPREAD,//7
	WEAPON_ALTERNATE,//8
	WEAPON_DISPERSION,//9
	WEAPON_RAFALE,//10
	WEAPON_RAFALE_COOLDOWN,//11
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

enum EnemyPoolData
{
	ENEMYPOOL_NAME,//0
	ENEMYPOOL_NB_LINES,//1
	ENEMYPOOL_NB_ROWS,//2
	ENEMYPOOL_RADIUSX,//3
	ENEMYPOOL_RADIUSY,//4
	ENEMYPOOL_VSPEED,//5
	ENEMYPOOL_TRIGGERY_PATTERN,//6
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
	int probability;
	int poolsize;
	int enemyclass;
	int proba_min;//interval of dice roll values for a hit
	int proba_max;
};

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

private:
	ShipConfig* Scene::LoadShipConfig(string name);
	EnemyBase*  Scene::LoadEnemy(string name, int probability, int poolSize, int enemyClass);
	EnemyPool*  Scene::LoadEnemyPool(string name);
	Weapon* Scene::LoadWeapon(string name, int fire_direction, Ammo* ammo);
	Ammo* Scene::LoadAmmo(string name);
	FX* Scene::LoadFX(string name);

	Ship* playerShip;
	list<EnemyBase> enemies;
	list<EnemyBase> enemies_ranked_by_class[NBVAL_EnemyClass];
	int total_class_probability[NBVAL_EnemyClass];
	list<vector<string>> config;
	list<vector<string>> enemyConfig;
	list<vector<string>> weaponConfig;
	list<vector<string>> ammoConfig;
	list<vector<string>> FXConfig;
	list<vector<string>> enemypoolConfig;
	list<vector<string>> shipConfig;
	list<EnemyBase*> sceneIndependantsLayered[NBVAL_EnemyClass];
	Independant* bg;
	Independant* hub;
	//Independant* hubClone;
	sf::RenderWindow* mainWindow;
	sf::Text* framerate;
	int hazard_break_value;
	int hazard_level;
	sf::RectangleShape hazardBar;
	sf::RectangleShape hazardBarMax;
	sf::Text* hazardBreakText;
	sf::Text* hazardBreakScore;

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