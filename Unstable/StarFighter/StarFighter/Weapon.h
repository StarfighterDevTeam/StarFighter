#ifndef WEAPON_H_INCLUDED
#define WEAPON_H_INCLUDED

#include "Game.h"

class WeaponAttack
{
public:
	WeaponAttack();
	int m_speed;
	int m_damage;
	int m_energy_cost;
	int m_chance_of_hit;
	int m_chance_of_fire;
	int m_chance_of_electricity;
	int m_chance_of_stun;
	int m_balance;
	CrewType m_crew_required;
};

class Weapon
{
public:
	Weapon(WeaponType type);

	WeaponType m_type;

	vector<WeaponAttack> m_attacks;

	int m_weight;
	bool m_energetic;
	bool m_ranged;
};


#endif //WEAPON_H_INCLUDED