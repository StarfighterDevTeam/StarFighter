#include "Weapon.h"

extern Game* CurrentGame;


Weapon::Weapon(WeaponType type)
{
	m_type = type;
	m_attack_selected = NULL;

	m_weight = 0;
	m_energetic = false;
	m_ranged = false;

	WeaponAttack attack_primary;
	WeaponAttack attack_secondary;

	switch (type)
	{
		case Weapon_BlasterRifle:
		{
			m_energetic = true;
			m_ranged = true;
			m_weight = 3;

			m_attacks.push_back(attack_primary);
			WeaponAttack& attack1 = m_attacks.back();
			attack1.m_damage = 4;
			attack1.m_chance_of_hit = 4;
			attack1.m_chance_of_fire = 5;
			attack1.m_speed = 7;
			attack1.m_energy_cost = 3;

			m_attacks.push_back(attack_secondary);
			WeaponAttack& attack2 = m_attacks.back();
			attack2.m_damage = 4;
			attack2.m_chance_of_hit = 4;
			attack2.m_chance_of_fire = 5;
			attack2.m_speed = 7;
			attack2.m_energy_cost = 3;
			attack2.m_crew_required = Crew_Gunner;
			break;
		}
		case Weapon_FireSword:
		{
			m_weight = 2;
			m_energetic = true;

			m_attacks.push_back(attack_primary);
			WeaponAttack& attack1 = m_attacks.back();
			attack1.m_damage = 3;
			attack1.m_chance_of_fire = 5;
			attack1.m_speed = 8;
			attack1.m_energy_cost = 3;

			m_attacks.push_back(attack_secondary);
			WeaponAttack& attack2 = m_attacks.back();
			attack2.m_damage = 5;
			attack2.m_chance_of_fire = 4;
			attack2.m_speed = 8;
			attack2.m_energy_cost = 3;
			attack2.m_crew_required = Crew_Warrior;
			break;
		}
		case Weapon_Hammer:
		{
			m_weight = 4;

			m_attacks.push_back(attack_primary);
			WeaponAttack& attack1 = m_attacks.back();
			attack1.m_damage = 3;
			attack1.m_chance_of_stun = 5;
			attack1.m_chance_of_unbalance = 3;
			attack1.m_speed = 5;
			attack1.m_energy_cost = 1;

			m_attacks.push_back(attack_secondary);
			WeaponAttack& attack2 = m_attacks.back();
			attack2.m_damage = 1;
			attack2.m_chance_of_stun = 5;
			attack2.m_chance_of_unbalance = 1;
			attack2.m_speed = 5;
			attack2.m_energy_cost = 3;
			attack2.m_crew_required = Crew_Warrior;
			break;
		}
		case Weapon_Gun:
		{
			m_weight = 1;
			m_ranged = true;

			m_attacks.push_back(attack_primary);
			WeaponAttack& attack1 = m_attacks.back();
			attack1.m_damage = 3;
			attack1.m_chance_of_hit = 1;
			attack1.m_speed = 8;
			attack1.m_energy_cost = 1;

			m_attacks.push_back(attack_secondary);
			WeaponAttack& attack2 = m_attacks.back();
			attack2.m_damage = 2;
			attack2.m_chance_of_hit = 4;
			attack2.m_speed = 8;
			attack2.m_energy_cost = 1;
			break;
		}
	}
}