#ifndef LOOT_H_INCLUDED
#define LOOT_H_INCLUDED

#include "Globals.h"
#include "Game.h"
#include "GameObject.h"
#include "Weapon.h"

enum LootTypes
{
	Loot_WeaponKatana,
	Loot_WeaponSpear,
	Loot_WeaponShuriken,
	Loot_BonusMeleeRange,
	NBVAL_LOOT_TYPES,
};

class Loot : public GameObject
{
public :
	Loot(LootTypes type, sf::Vector2f position);
	void update(sf::Time deltaTime) override;

	LootTypes m_type;

	sf::Vector2f m_range_bonus;
	WeaponTypes m_weapon_type;
};

#endif // LOOT_H_INCLUDED
