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

#include "FileLoadUtils.h"
#include "Phase.h"

#define	ENEMYX_DAMAGE			5
#define	ENEMYX_ARMOR			100
#define	ENEMYX_SHIELD			100
#define	ENEMYX_SHIELD_REGEN		1

#define	ENEMYY_DAMAGE			5
#define	ENEMYY_ARMOR			200
#define	ENEMYY_SHIELD			200
#define	ENEMYY_SHIELD_REGEN		1

const float LootTable_DroppingSomething[EnemyClass::NBVAL_EnemyClass] = { 0.0, 0.25, 0.25, 0.5, 0.5, 0.8, 0.8, 1.0, 0.0 };
const float LootTable_DropIsEquipment[EnemyClass::NBVAL_EnemyClass] = {0.0, 0.1, 0.1, 0.3, 0.3, 0.6, 0.6, 1.0, 0.0};
const sf::Vector2f LootTable_BeastScale_Base[EnemyClass::NBVAL_EnemyClass] = { sf::Vector2f(0.0, 0.0), sf::Vector2f(0.7, 1.3), sf::Vector2f(0.7, 1.3), sf::Vector2f(0.9, 1.5), sf::Vector2f(0.9, 1.5), sf::Vector2f(1.1, 1.7), sf::Vector2f(1.1, 1.7), sf::Vector2f(1.5, 2.2), sf::Vector2f(0.0, 0.0) };
const float LootTable_BeastScaleThreshold[NUMBER_OF_BEASTSCALE_THRESHOLD] =  {0.0, 1.5, 3.0, 4.5, 6.0};
const int LootTable_MaxPropertiesPerEquipmentType[EquipmentType::NBVAL_Equipment+1] =  {2, 1, 2, 2, 2};

const int XPTable_PerEnemyClass[EnemyClass::NBVAL_EnemyClass] = { 0, 10, 10, 30, 30, 100, 100, 500, 0 };

class Enemy : public Independant
{
public:
	Enemy(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, FX* m_FX_death, int m_frameNumber = 1, int m_animationNumber = 1);
	void update(sf::Time deltaTime, float hyperspeedMultiplier) override;
	void UpdateHealthBars(sf::Time deltaTime);
	vector<Weapon*> weapons_list;
	Enemy* Clone();
	FX* FX_death;
	void Death() override;
	void Destroy() override;
	float radius;
	float angspeed;
	void GenerateLoot() override;
	void CreateRandomLoot(float BeastScaleBonus=0) override;
	void CreateRandomLootv2(EnemyClass loot_class, float BeastScaleBonus = 0, bool force_BeastScale = false, float BeastScale_min = 0.0f, float BeastScale_max = 6.0f);
	void damage_from(Independant& independant) override;
	EnemyClass enemy_class;

	//phases
	void setPhase(Phase* phase);
	Phase* getPhase(string phaseName);
	Phase* currentPhase;
	vector <Phase*> phases;
	bool hasPhases;
	bool CheckCondition();
	sf::Time phaseTimer;
	sf::Time enemyTimer;
	static Phase* LoadPhase(string name);
	static Weapon* LoadWeapon(string name, int fire_direction, Ammo* ammo);
	static Ammo* LoadAmmo(string name);
	static FX* LoadFX(string name);
	bool face_target;
	bool reset_facing;
	BouncingType bouncing;
	int m_shots_fired;

	sf::RectangleShape* armorBar;
	sf::RectangleShape* armorBarContainer;
	sf::RectangleShape* shieldBar;
	sf::RectangleShape* shieldBarContainer;
	float armorBar_offsetY;
	float shieldBar_offsetY;
	//float offsetBetweenHealthBars;
	void RotateFeedbacks(float angle);
	sf::Time feedbackTimer;

	void ApplyLevelModifiers() override;

private:
	int GetChosenProperty(vector<int> *properties_roll_table, int properties_to_choose_from, int p);
};

struct EnemyBase
{
public:
	Enemy* enemy;
	int probability;
	int enemyclass;
	float spawnCost;
	float repeatChance;
	float missChance;
	int proba_min;//interval of dice roll values for a hit
	int proba_max;
};

#endif // INDEPENDANT_H_INCLUDED