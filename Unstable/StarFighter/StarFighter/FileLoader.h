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
#include "Background.h"

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
	ENEMY_CLASS_SPAWNCOST,//4
	ENEMY_CLASS_REPEAT_CHANCE,//5
	ENEMY_CLASS_MISS_CHANCE,//6
};

enum SceneDataBoss
{
	SCENE_COMPARE_BOSS,//0
	BOSS,//1
	BOSS_CLASS,//2
	BOSS_SPAWN_X,//3
	BOSS_SPAWN_Y,//4
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
	EQUIPMENT_COMPARE,
	EQUIPMENT_NAME,
	EQUIPMENT_DISPLAY_NAME,
	EQUIPMENT_IMAGE_NAME,
	EQUIPMENT_WIDTH,
	EQUIPMENT_HEIGHT,
	EQUIPMENT_FRAMES,
	EQUIPMENT_MAXSPEED,
	EQUIPMENT_DECCELERATION,
	EQUIPMENT_ACCELERATION,
	EQUIPMENT_HYPERSPEED,
	EQUIPMENT_ARMOR,
	EQUIPMENT_SHIELD,
	EQUIPMENT_SHIELD_REGEN,
	EQUIPMENT_DAMAGE,
	EQUIPMENT_BOT,
	EQUIPMENT_FAKE_TEXTURE,
	EQUIPMENT_FAKE_WIDTH,
	EQUIPMENT_FAKE_HEIGHT,
	EQUIPMENT_FAKE_FRAMES,
};

enum SavedEquipmentData
{
	EQUIPMENTSAVE_EQUIPPED,
	EQUIPMENTSAVE_TYPE,
	EQUIPMENTSAVE_NAME,
	EQUIPMENTSAVE_DISPLAY_NAME,
	EQUIPMENTSAVE_ARMOR,
	EQUIPMENTSAVE_SHIELD,
	EQUIPMENTSAVE_SHIELD_REGEN,
	EQUIPMENTSAVE_DAMAGE,
	EQUIPMENTSAVE_BOT,
	EQUIPMENTSAVE_NBCOLS,
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
	ENEMY_WEAPON_2,//7
	ENEMY_AMMO_2,//8
	ENEMY_WEAPON_3,//9
	ENEMY_AMMO_3,//10
	ENEMY_IMAGE_NAME,//11
	ENEMY_WIDTH,//12
	ENEMY_HEIGHT,//13
	ENEMY_FRAMES,//14
	ENEMY_SPEED,//15
	ENEMY_FX_DEATH,//16
	ENEMY_VALUE,//17
	ENEMY_PATTERN,//18
	ENEMY_ANGSPEED,//19
	ENEMY_RADIUS,//20
	ENEMY_CLOCKWISE,//21
	ENEMY_ROTATION_SPEED,//22
	ENEMY_PHASE,//23
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
	BOT_ANGSPEED,//13
	BOT_RADIUS,//14
	BOT_CLOCKWISE,//15
	BOT_ROTATION_SPEED,//16
	BOT_XSPREAD,//17
	BOT_YSPREAD,//18
};

class FileLoader
{
public:
	static ShipConfig* LoadShipConfig(string name);
	static ShipModel* LoadShipModel(string name);
	static EnemyBase*  LoadEnemyBase(string m_name, int m_probability, int m_enemyClass);
	static EnemyPool*  LoadEnemyPool(string name);
	static Weapon* LoadWeapon(string name, int fire_direction, Ammo* ammo);
	static Ammo* LoadAmmo(string name);
	static FX* LoadFX(string name);
	static Equipment* LoadEquipment(string name);
	static Bot* LoadBot(string name);
};

#endif //FILELOADER_H_INCLUDED