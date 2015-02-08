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
};

class Enemy : public Independant
{
public:
	Enemy::Enemy(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, FX* m_FX_death);
	void update(sf::Time deltaTime) override;
	//Weapon* weapon;
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
	void setPhase(Phase* phase);
	list<Phase*> phases_list;
	bool hasPhases;

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

#endif // INDEPENDANT_H_INCLUDED