#ifndef ENEMY_H_INCLUDED
#define ENEMY_H_INCLUDED

#include "Globals.h"
#include "Game.h"
#include "GameObject.h"
#include "Loot.h"// > Weapon > Ammo > FX

class Ship;

enum EnemyType
{
	Enemy_Wufeng_Katana,
	Enemy_Wufeng_Spear,
	Enemy_Wufeng_Shuriken,
	Enemy_Wufeng_Summoner,
	Enemy_Ghost,
	NBVAL_ENEMYTYPES,
};

enum EnemyState
{
	EnemyState_Idle,
	EnemyState_FollowTarget,
	EnemyState_Summoning,
	EnemyState_Dash,
};

class Enemy : public GameObject
{
public :
	//Enemy(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	Enemy(EnemyType type, sf::Vector2f position);
	~Enemy();
	void update(sf::Time deltaTime) override;
	void Draw(sf::RenderTexture& screen) override;

	bool DealDamage(int dmg) override;
	void Death() override;

	EnemyType m_type;
	EnemyState m_state;

	int m_hp;
	int m_hp_max;
	int m_dmg;
	float m_ref_speed;
	int GetRating() override;
	sf::Vector2f Flocking();
	sf::Vector2f FlockingGetAveragePosition();
	sf::Vector2f FlockingGetAverageSpeed();
	sf::Vector2f FlockingSeparate();

	//states
	void SetEnemyState(EnemyState state);

	//attack
	Weapon* m_weapon;
	bool m_is_attacking;
	float m_attack_cooldown;
	float m_attack_cooldown_timer;
	GameObject* CanParry();
	float RangeToInterceptTarget(GameObject* target);
	float RangeToDodgeTarget(GameObject* target);

	//lateral dash - TODO
	//float m_dash_cooldown_clock;

	//roaming
	float m_roaming_timer;

	//dash
	float m_dash_range;
	float m_dash_cooldown;
	float m_dash_cooldown_timer;
	GameObject* CanDodge();
	bool DashAwayFromTarget(GameObject* target);
	sf::Vector2f m_dash_target;
	void PlayStroboscopicEffect(Time effect_duration, Time time_between_poses);

	//summoner
	EnemyType m_enemy_summoned;
	sf::Vector2f m_summoning_cooldown_range;
	float m_summoning_cooldown;
	float m_summoning_cooldown_timer;
	float m_summoning_timer;
	float m_summoning_duration;
	sf::Vector2f* m_invocation_coordinates;
	bool TrySummoning();
	bool Summon();

	//AI
	void UpdateAI(sf::Time deltaTime);
	bool FollowTarget(GameObject* target);
	bool AttackTarget(GameObject* target);
	bool Roam(sf::Time deltaTime);
	float m_roam_duration;
	sf::Vector2f m_roam_destination;
	bool m_arrived_at_roam_destination;
	float m_aggro_radius;

	//feedback
	float m_hit_feedback_timer;

	//debug
	CircleShape m_aggro_radius_feedback;
};

#endif // ENEMY_H_INCLUDED
