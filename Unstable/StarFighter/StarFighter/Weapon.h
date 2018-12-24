#ifndef WEAPON_H_INCLUDED
#define WEAPON_H_INCLUDED

#include "Game.h"

class Weapon
{
public:
	Weapon(WeaponType type);
	~Weapon();

	WeaponType m_type;

	vector<WeaponAttack*> m_attacks;

	int m_weight;
	bool m_energetic;
	bool m_ranged;

	WeaponAttack* m_attack_selected;
};


#endif //WEAPON_H_INCLUDED