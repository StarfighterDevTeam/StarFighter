#ifndef ENEMY_H_INCLUDED
#define ENEMY_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "TextureLoader.h"
#include "GameObject.h"
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

const float LootTable_DroppingSomething[NBVAL_EnemyClass] = { 0.0, 0.25, 0.25, 0.5, 0.5, 0.8, 0.8, 1.0, 0.0 };
const float LootTable_DropIsEquipment[NBVAL_EnemyClass] = {0.0, 0.1, 0.1, 0.3, 0.3, 0.6, 0.6, 1.0, 0.0};
const sf::Vector2f LootTable_BeastScale_Base[NBVAL_EnemyClass] = { sf::Vector2f(0.0, 0.0), sf::Vector2f(0.7, 1.3), sf::Vector2f(0.7, 1.3), sf::Vector2f(0.9, 1.5), sf::Vector2f(0.9, 1.5), sf::Vector2f(1.1, 1.7), sf::Vector2f(1.1, 1.7), sf::Vector2f(1.5, 2.2), sf::Vector2f(0.0, 0.0) };
const float LootTable_BeastScaleThreshold[NUMBER_OF_BEASTSCALE_THRESHOLD] =  {0.0, 1.5, 3.0, 4.5, 6.0};

const int XPTable_PerEnemyClass[NBVAL_EnemyClass] = { 0, 10, 10, 30, 30, 100, 100, 500, 0 };

class Enemy : public GameObject
{
public:
	Enemy(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, FX* FX_death, int frameNumber = 1, int animationNumber = 1);
	~Enemy();
	void DeletePhases();
	void update(sf::Time deltaTime, float hyperspeedMultiplier) override;
	void UpdateHealthBars();
	Enemy* Clone(); void Death() override;

	vector<Weapon*> m_weapons_list;
	FX* m_FX_death;

	float m_radius;
	float m_angspeed;

	void GenerateLoot() override;
	bool CreateRandomLootv2(EnemyClass loot_class, float BeastScaleBonus = 0, bool force_BeastScale = false, float BeastScale_min = 0.0f, float BeastScale_max = 6.0f);
	static bool AssignRandomEquipment(EquipmentType equipment_type, int level, GameObject* object, float beastScore);
	void GetDamageFrom(GameObject& object) override;

	EnemyClass m_enemy_class;

	//phases
	void setPhase(Phase* phase);
	Phase* getPhase(string phaseName);
	Phase* m_currentPhase;

	vector <Phase*> m_phases;

	bool CheckCondition();

	sf::Time m_phaseTimer;
	sf::Time m_enemyTimer;

	static Phase* LoadPhase(string name);
	static Dialog* LoadDialog(string name);
	static Weapon* LoadWeapon(string name, int fire_direction, Ammo* ammo);
	static Ammo* LoadAmmo(string name);
	static FX* LoadFX(string name);

	bool m_face_target;
	bool m_reset_facing;
	BouncingType m_bouncing;
	int m_shots_fired;
	bool m_input_blocker;

	sf::RectangleShape m_armorBar;
	sf::RectangleShape m_armorBarContainer;
	sf::RectangleShape m_shieldBar;
	sf::RectangleShape m_shieldBarContainer;
	sf::Text m_enemyLevel;
	sf::Font* m_font;
	float m_armorBar_offsetY;
	float m_shieldBar_offsetY;

	//float offsetBetweenHealthBars;
	void RotateFeedbacks(float angle);

	sf::Time m_feedbackTimer;

	int m_level;

	void ApplyLevelModifiers();

private:
	int GetChosenProperty(vector<int> *properties_roll_table, int properties_to_choose_from, int p);
};

struct EnemyBase
{
public:
	Enemy* m_enemy;
	int m_probability;
	int m_enemyclass;
	float m_spawnCost;
	float m_repeatChance;
	float m_missChance;
	int m_proba_min;//interval of dice roll values for a hit
	int m_proba_max;
};

#endif // INDEPENDANT_H_INCLUDED