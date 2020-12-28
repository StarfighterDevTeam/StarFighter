#ifndef ENEMY_H_INCLUDED
#define ENEMY_H_INCLUDED

#include "Upgrade.h"

#define	ENEMYX_DAMAGE			5
#define	ENEMYX_ARMOR				100
#define	ENEMYX_SHIELD			100
#define	ENEMYX_SHIELD_REGEN		1

#define	ENEMYY_DAMAGE			5
#define	ENEMYY_ARMOR				200
#define	ENEMYY_SHIELD			200
#define	ENEMYY_SHIELD_REGEN		1

const float LootTable_DroppingSomething[NBVAL_EnemyClass] = { 0.0, 0.25, 0.25, 0.5, 0.5, 0.8, 0.8, 1.0, 0.0 };
const float LootTable_DropIsEquipment[NBVAL_EnemyClass] = {0.0, 0.1, 0.1, 0.3, 0.3, 0.6, 0.6, 1.0, 0.0};
const sf::Vector2f LootTable_BeastScale_Base[NBVAL_EnemyClass] = { sf::Vector2f(0.0, 0.0), sf::Vector2f(0.7, 1.3), sf::Vector2f(0.7, 1.3), sf::Vector2f(0.9, 1.5), sf::Vector2f(0.9, 1.5), sf::Vector2f(1.1, 1.7), sf::Vector2f(1.1, 1.7), sf::Vector2f(1.5, 2.2), sf::Vector2f(0.0, 0.0) };

const int XPTable_PerEnemyClass[NBVAL_EnemyClass] = { 0, 10, 10, 30, 30, 100, 100, 500, 0 };

class Enemy : public GameObject
{
public:
	Enemy(){};
	Enemy(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, FX* FX_death, int frameNumber = 1, int animationNumber = 1);
	~Enemy();
	void update(sf::Time deltaTime, float hyperspeedMultiplier) override;
	void Enemy::Draw(sf::RenderTexture& screen) override;//DEBUG
	void UpdateHealthBars();
	Enemy* Clone();
	void Death(bool give_money) override;

	vector<Weapon*> m_weapons_list;
	FX* m_FX_death;

	float m_radius;
	float m_angspeed;

	void GetDamageFrom(GameObject& object) override;
	void GetDamage(int damage) override;
	void GenerateLoot() override;
	bool IsBoss() override;

	EnemyClass m_enemy_class;

	//phases
	void setPhase(Phase* phase);
	Phase* getPhase(string phaseName);
	Phase* m_currentPhase;

	vector <Phase*> m_phases;

	bool CheckCondition() override;

	float m_phaseTimer;
	float m_enemyTimer;

	static Phase* LoadPhase(string name);
	static Dialog* LoadDialog(string name);
	static Weapon* LoadWeapon(string name, int fire_direction);
	static Ammo* LoadAmmo(string name);
	static FX* LoadFX(string name);
	static Bot* LoadBot(string name);
	
	bool m_face_target;
	bool m_reset_facing;
	bool m_shoot_when_aligned;
	BouncingType m_bouncing;
	int m_shots_fired;
	bool m_input_blocker;

	SFRectangle m_armorBar;
	SFRectangle m_armorBarContainer;
	SFRectangle m_shieldBar;
	SFRectangle m_shieldBarContainer;
	SFText m_enemyLevel;
	sf::Font* m_font;
	float m_armorBar_offsetY;
	float m_shieldBar_offsetY;

	float m_health_feedbackTimer;
	int m_level;

	void ApplyLevelModifiers();
};

struct EnemyBase
{
public:
	EnemyBase(){};
	~EnemyBase(){};
	Enemy* m_enemy;
	int m_probability;
	int m_enemyclass;
	float m_spawnCost;
	float m_repeatChance;
	float m_missChance;
	int m_proba_min;//interval of dice roll values for a hit
	int m_proba_max;
};

#endif // ENEMY_H_INCLUDED