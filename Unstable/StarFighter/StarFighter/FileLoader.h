#ifndef FILELOADER_H_INCLUDED
#define FILELOADER_H_INCLUDED

#include "Ship.h"

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
	ENEMY_CLASS,//3
	ENEMY_CLASS_LEVEL,//4
	ENEMY_CLASS_SPAWNCOST,//5
	ENEMY_CLASS_REPEAT_CHANCE,//6
	ENEMY_CLASS_MISS_CHANCE,//7
};

enum SceneDataShop
{
	SCENE_COMPARE_SHOP,//0
	SHOP_TEXTURE_NAME,//1
	SHOP_WIDTH,//2
	SHOP_HEIGHT,//3
	SHOP_LEVEL,//4
};

enum SceneDataBoss
{
	SCENE_COMPARE_BOSS,//0
	BOSS,//1
	BOSS_CLASS,//2
	BOSS_SPAWN_X,//3
	BOSS_SPAWN_Y,//4
	BOSS_SPAWN_ROTATION,//4
	BOSS_LEVEL,//5 obsolete
};

enum ShipConfigData
{
	SHIPCONFIG_NAME,//0
	SHIPCONFIG_ENGINE,//1
	SHIPCONFIG_ARMOR,//2
	SHIPCONFIG_SHIELD,//3
	SHIPCONFIG_MODULE,//4
	SHIPCONFIG_WEAPON,//5
	SHIPCONFIG_SHIPMODEL,//6
	SHIPCONFIG_DEATH_FX,//7
};

enum EnemyData
{
	ENEMY_NAME,//0
	ENEMY_ARMOR,//1
	ENEMY_SHIELD,//2
	ENEMY_SHIELD_REGEN,//3
	ENEMY_DAMAGE,//4
	ENEMY_WEAPON,//5
	ENEMY_WEAPON_2,//6
	ENEMY_WEAPON_3,//7
	ENEMY_IMAGE_NAME,//8
	ENEMY_WIDTH,//9
	ENEMY_HEIGHT,//10
	ENEMY_FRAMES,//11
	ENEMY_SKIN,//12
	ENEMY_NB_SKINS,//13
	ENEMY_SPEED,//14
	ENEMY_FX_DEATH,//15
	ENEMY_VALUE,//16
	ENEMY_ROTATION_SPEED,//17
	ENEMY_PHASE,//18
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

class FileLoader
{
public:
	static Ship* LoadShipConfig(string name);
	static ShipModel* LoadShipModel(string name);
	static EnemyBase* LoadEnemyBase(string name, int probability, int enemyClass);
	static EnemyPool* LoadEnemyPool(string name);
	static Weapon* LoadWeapon(string name, int fire_direction);
	static Ammo* LoadAmmo(string name);
	static FX* LoadFX(string name);
	static Equipment* LoadEquipment(string name);
	static Bot* LoadBot(string name);
};

#endif //FILELOADER_H_INCLUDED