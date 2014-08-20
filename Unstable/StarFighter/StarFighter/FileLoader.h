#ifndef FILELOADER_H_INCLUDED
#define FILELOADER_H_INCLUDED

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include "Globals.h"
#include "Enemy.h"
#include "Ship.h"
#include "EnemyPool.h"
#include "FileLoadUtils.h"

enum HazardLevels
{
	HAZARD_LEVEL_1,//0
	HAZARD_LEVEL_2,//1
	HAZARD_LEVEL_3,//2
	HAZARD_LEVEL_4,//3
	HAZARD_LEVEL_5,//4
	NB_HAZARD_LEVELS,//5
};

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
	BACKGROUND_VSPEED,//4
	BACKGROUND_VERTICAL,//5
};

enum ShipConfigData
{
	SHIPCONFIG_NAME,//0
	SHIPCONFIG_AIRBRAKE,//1
	SHIPCONFIG_ENGINE,//2
	SHIPCONFIG_ARMOR,//3
	SHIPCONFIG_SHIELD,//4
	SHIPCONFIG_MODULE,//5
	SHIPCONFIG_WEAPON,//6
	SHIPCONFIG_AMMO,//7
	SHIPCONFIG_SHIPMODEL,//8
	SHIPCONFIG_DEATH_FX,//9

};

enum EquipmentData
{
	EQUIPMENT_COMPARE,//0
	EQUIPMENT_NAME,//1
	EQUIPMENT_IMAGE_NAME,//2
	EQUIPMENT_WIDTH,//3
	EQUIPMENT_HEIGHT,//4
	EQUIPMENT_FRAMES,//5
	EQUIPMENT_MAXSPEED_X,//6
	EQUIPMENT_MAXSPEED_Y,//7
	EQUIPMENT_DECCELERATION,//8
	EQUIPMENT_ACCELERATION_X,//9
	EQUIPMENT_ACCELERATION_Y,//10
	EQUIPMENT_ARMOR,//11
	EQUIPMENT_SHIELD,//12
	EQUIPMENT_SHIELD_REGEN,//13
	EQUIPMENT_BOT,//14
	EQUIPMENT_FAKE_TEXTURE,//15
	EQUIPMENT_FAKE_WIDTH,//16
	EQUIPMENT_FAKE_HEIGHT,//17
	EQUIPMENT_FAKE_FRAMES,//18
};

enum EnemyData
{
	ENEMY_NAME,//0
	ENEMY_ARMOR,//1
	ENEMY_SHIELD,//2
	ENEMY_SHIELD_REGEN,//3
	ENEMY_DAMAGE,//4
	ENEMY_WEAPON,//5
	ENEMY_AMMO,//6
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
	WEAPON_RATE_OF_FIRE,//1
	WEAPON_IMAGE_NAME,//2
	WEAPON_WIDTH,//3
	WEAPON_HEIGHT,//4
	WEAPON_FRAMES,//5
	WEAPON_SOUND,//6
	WEAPON_MULTISHOT,//7
	WEAPON_XSPREAD,//8
	WEAPON_ALTERNATE,//9
	WEAPON_DISPERSION,//10
	WEAPON_RAFALE,//11
	WEAPON_RAFALE_COOLDOWN,//12
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

enum BotData
{
	BOT_NAME,//0
	BOT_ARMOR,//1
	BOT_SHIELD,//2
	BOT_SHIELD_REGEN,//3
	BOT_DAMAGE,//4
	BOT_WEAPON,//5
	BOT_AMMO,//6
	BOT_IMAGE_NAME,//7
	BOT_WIDTH,//8
	BOT_HEIGHT,//9
	BOT_FRAMES,//10
	BOT_FX_DEATH,//11
	BOT_PATTERN,//12
	BOT_SPEED,//13
	BOT_RADIUS,//14
	BOT_XSPREAD,//15
	BOT_YSPREAD,//16
};

class FileLoader
{
public:
	static ShipConfig* FileLoader::LoadShipConfig(string name);
	static ShipModel* FileLoader::LoadShipModel(string name);
	static EnemyBase*  FileLoader::LoadEnemy(string name, int probability, int poolSize, int enemyClass);
	static EnemyPool*  FileLoader::LoadEnemyPool(string name);
	static Weapon* FileLoader::LoadWeapon(string name, int fire_direction, Ammo* ammo);
	static Ammo* FileLoader::LoadAmmo(string name);
	static FX* FileLoader::LoadFX(string name);
	static Equipment* FileLoader::LoadEquipment(string name);
	static Bot* FileLoader::LoadBot(string name);
};

#endif //FILELOADER_H_INCLUDED