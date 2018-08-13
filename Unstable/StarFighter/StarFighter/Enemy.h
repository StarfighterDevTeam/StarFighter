#ifndef ENEMY_H_INCLUDED
#define ENEMY_H_INCLUDED

#include "Globals.h"
#include "Game.h"
#include "GameObject.h"
#include "Loot.h"
#include "FX.h"

class Ship;

enum EnemyType
{
	Enemy_Wufeng_Katana,
	Enemy_Wufeng_Spear,
	Enemy_Wufeng_Shuriken,
	NBVAL_ENEMYTYPES,
};

enum EnemyPhase
{
	EnemyPhase_Idle,
	EnemyPhase_FollowTarget,
};

class Enemy : public GameObject
{
public :
	//Enemy(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	Enemy(sf::Vector2f position, EnemyType type);
	~Enemy();
	void update(sf::Time deltaTime) override;
	void Draw(sf::RenderTexture& screen) override;

	bool DealDamage(int dmg) override;
	void Death() override;

	EnemyType m_type;
	EnemyPhase m_phase;

	int m_hp;
	int m_hp_max;
	int m_dmg;
	float m_ref_speed;
	int GetRating() override;
	sf::Vector2f Flocking();
	sf::Vector2f FlockingGetAveragePosition();
	sf::Vector2f FlockingGetAverageSpeed();
	sf::Vector2f FlockingSeparate();

	//attack
	Weapon* m_weapon;
	bool m_is_attacking;
	float m_attack_cooldown;
	float m_attack_cooldown_timer;
	GameObject* CanParry();
	float RangeToInterceptTarget(GameObject* target);

	//lateral dash
	sf::Clock m_dash_cooldown_clock;

	//roaming
	sf::Clock m_roaming_clock;

	//AI
	void UpdateAI(sf::Time deltaTime);
	bool FollowTarget(GameObject* target);
	bool AttackTarget(GameObject* target);
	bool Dash();
	bool Roam(sf::Time deltaTime);
	float m_roam_duration;
	sf::Vector2f m_destination;
	bool m_arrived_at_destination;
	float m_aggro_radius;

	//debug
	CircleShape m_aggro_radius_feedback;
};

#endif // ENEMY_H_INCLUDED
