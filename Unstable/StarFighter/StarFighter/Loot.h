#ifndef LOOT_H_INCLUDED
#define LOOT_H_INCLUDED

#include "Globals.h"
#include "Game.h"
#include "GameObject.h"
#include "Weapon.h"

enum LootType
{
	Loot_WeaponKatana,
	Loot_WeaponSpear,
	Loot_WeaponShuriken,
	Loot_BonusMeleeRange,
	Loot_DashAmmo,
	NBVAL_LOOT_TYPES,
};

class Loot : public GameObject
{
public :
	Loot(LootType type, sf::Vector2f position);
	void update(sf::Time deltaTime) override;

	LootType m_type;

	sf::Vector2f m_range_bonus;
	size_t m_dash_ammo;
	WeaponType m_weapon_type;

	float m_flotation_timer;
	float m_attraction_radius;
	GameObject* m_attraction_target;

	float m_expiration_timer;
	float m_expiration_duration;
};

#endif // LOOT_H_INCLUDED
