#ifndef WEAPON_H_INCLUDED
#define WEAPON_H_INCLUDED

#include "Game.h"

class RobotSlot;
class Weapon
{
public:
	Weapon(WeaponType type, RobotSlot* owner);
	~Weapon();

	WeaponType m_type;

	vector<WeaponAttack*> m_attacks;

	int m_weight;
	bool m_energetic;
	bool m_ranged;
	RobotSlot* m_owner;
	bool m_used;
	bool m_requires_close_distance;
};


#endif //WEAPON_H_INCLUDED