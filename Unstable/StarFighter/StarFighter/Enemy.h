#ifndef ENEMY_H_INCLUDED
#define ENEMY_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "TextureLoader.h"
#include "Independant.h"
#include "Weapon.h"
#include "Loot.h"
#include "Ship.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "MovePattern.h"
#include "FileLoadUtils.h"


#define	ENEMYX_DAMAGE			5
#define	ENEMYX_ARMOR			100
#define	ENEMYX_SHIELD			100
#define	ENEMYX_SHIELD_REGEN		1

#define	ENEMYY_DAMAGE			5
#define	ENEMYY_ARMOR			200
#define	ENEMYY_SHIELD			200
#define	ENEMYY_SHIELD_REGEN		1

const float LootTable_DroppingSomething[EnemyClass::NBVAL_EnemyClass] =  {0.0, 0.25, 0.5, 0.8, 1.0, 0.0};
const float LootTable_DropIsEquipment[EnemyClass::NBVAL_EnemyClass] = {0.0, 0.1, 0.3, 0.6, 1.0, 0.0};
const sf::Vector2f LootTable_BeastScale_Base[EnemyClass::NBVAL_EnemyClass] =  {sf::Vector2f(0.0, 0.0), sf::Vector2f(0.7,1.3), sf::Vector2f(0.9,1.5), sf::Vector2f(1.1, 1.7),sf::Vector2f (1.5,2.2), sf::Vector2f(0.0, 0.0)};
const float LootTable_BeastScaleThreshold[NUMBER_OF_BEASTSCALE_THRESHOLD] =  {0.0, 1.5, 3.0, 4.5, 6.0};
const int LootTable_MaxPropertiesPerEquipmentType[EquipmentType::NBVAL_EQUIPMENT+1] =  {1, 2, 1, 2, 4, 4};

enum EnemyPhaseData
{
	PHASE_NAME,//0
	PHASE_VSPEED,//1
	PHASE_WEAPON,//2
	PHASE_AMMO,//3
	PHASE_WEAPON_2,//4
	PHASE_AMMO_2,//5
	PHASE_WEAPON_3,//6
	PHASE_AMMO_3,//7
	PHASE_PATTERN,//8
	PHASE_ANGSPEED,//9
	PHASE_RADIUS,//10
	PHASE_TRANSITION,//11
	PHASE_CONDITION,//12
	PHASE_OPERATOR,//13
	PHASE_VALUE,//14
	PHASE_TRANSITION_2,//15
	PHASE_CONDITION_2,//16
	PHASE_OPERATOR_2,//17
	PHASE_VALUE_2,//18
};

enum ConditionType
{
	NoCondition,
	VerticalPosition,
	Clock,
};

class ConditionTransition;
class Phase
{
public:
	Phase();
	std::string display_name;
	list<Weapon*> weapons_list;
	PatternType pattern;
	float angspeed;
	float radius;
	float vspeed;
	bool hasTransition;
	list<ConditionTransition*> transitions_list;
};

class ConditionTransition
{
public:
	ConditionTransition(ConditionType m_condition, FloatCompare m_op, float m_value, std::string m_nextPhase_name);
	ConditionType condition;
	FloatCompare op;
	float value;
	Phase* nextPhase;
	std::string nextPhase_name;
};

class Enemy : public Independant
{
public:
	Enemy::Enemy(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, FX* m_FX_death);
	void update(sf::Time deltaTime) override;
	list<Weapon*> weapons_list;
	Enemy* Clone();
	FX* FX_death;
	void Death() override;
	float radius;
	float angspeed;
	bool hasWeapon;
	void GenerateLoot() override;
	void CreateRandomLoot(float BeastScaleBonus=0) override;
	EnemyClass enemy_class;

	//phases
	void setPhase(string phase_name);
	list<Phase*> phases_list;
	Phase* currentPhase;
	bool hasPhases;
	bool CheckCondition();
	sf::Clock phaseClock;
	Phase* LoadPhase(string name);
	Weapon* LoadWeapon(string name, int fire_direction, Ammo* ammo);
	Ammo* LoadAmmo(string name);
	FX* LoadFX(string name);

private:
	int GetChosenProperty(vector<int> *properties_roll_table, int properties_to_choose_from, int p);	
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
	WEAPON_TARGET_SEAKING,//13
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
	AMMO_PATTERN,//8
	AMMO_ANGSPEED,//9
	AMMO_RADIUS,//10
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

#endif // INDEPENDANT_H_INCLUDED