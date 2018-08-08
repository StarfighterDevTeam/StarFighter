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

class Enemy : public GameObject
{
public :
	Enemy(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	Enemy(sf::Vector2f position, EnemyType type);
	~Enemy();
	void update(sf::Time deltaTime) override;

	bool DealDamage(int dmg) override;
	void Death() override;

	EnemyType m_type;

	int m_hp;
	int m_hp_max;
	int m_dmg;
	float m_ref_speed;
	int GetRating() override;
	sf::Vector2f Flocking();
	sf::Vector2f FlockingGetAveragePosition();
	sf::Vector2f FlockingGetAverageSpeed();
	sf::Vector2f FlockingSeparate();

	//melee
	Weapon* m_weapon;
	sf::Clock m_melee_cooldown_clock;
	bool m_is_attacking;
	float m_melee_cooldown;
	
};

#endif // ENEMY_H_INCLUDED
