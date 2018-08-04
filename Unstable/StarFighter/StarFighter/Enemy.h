#ifndef ENEMY_H_INCLUDED
#define ENEMY_H_INCLUDED

#include "Globals.h"
#include "Game.h"
#include "GameObject.h"
#include "Weapon.h"
#include "FX.h"

class Ship;

class Enemy : public GameObject
{
public :
	Enemy(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	~Enemy();
	void update(sf::Time deltaTime) override;

	bool DealDamage(int dmg) override;
	void Death() override;

	int m_hp;
	int m_hp_max;
	int m_dmg;
	float m_ref_speed;
	int GetRating() override;

	//melee
	Weapon* m_melee_weapon;
	sf::Clock m_melee_cooldown_clock;
	bool m_is_attacking;
	float m_melee_cooldown;
	//void UpdateMeleeWeaponPosition();
};

#endif // ENEMY_H_INCLUDED
