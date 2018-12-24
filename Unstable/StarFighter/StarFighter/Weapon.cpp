#include "Weapon.h"

extern Game* CurrentGame;


Weapon::Weapon(WeaponType type, RobotSlot* owner)
{
	m_type = type;
	m_owner = owner;
	m_attack_selected = NULL;

	m_weight = 0;
	m_energetic = false;
	m_ranged = false;

	switch (type)
	{
		case Weapon_BlasterRifle:
		{
			m_energetic = true;
			m_ranged = true;
			m_weight = 3;
			
			WeaponAttack* attack_primary = new WeaponAttack(this);
			attack_primary->m_type = WeaponAttack_BlasterRifle_1;
			attack_primary->m_damage = 4;
			attack_primary->m_chance_of_hit = 4;
			attack_primary->m_chance_of_fire = 5;
			attack_primary->m_speed = 7;
			attack_primary->m_energy_cost = 3;
			m_attacks.push_back(attack_primary);

			WeaponAttack* attack_secondary = new WeaponAttack(this);
			attack_secondary->m_type = WeaponAttack_BlasterRifle_2;
			attack_secondary->m_damage = 4;
			attack_secondary->m_chance_of_hit = 4;
			attack_secondary->m_chance_of_fire = 5;
			attack_secondary->m_speed = 7;
			attack_secondary->m_energy_cost = 3;
			attack_secondary->m_crew_required = Crew_Gunner;
			m_attacks.push_back(attack_secondary);

			break;
		}
		case Weapon_FireSword:
		{
			m_weight = 2;
			m_energetic = true;

			WeaponAttack* attack_primary = new WeaponAttack(this);
			attack_primary->m_type = WeaponAttack_FireSword_1;
			attack_primary->m_damage = 3;
			attack_primary->m_chance_of_fire = 5;
			attack_primary->m_speed = 8;
			attack_primary->m_energy_cost = 3;
			m_attacks.push_back(attack_primary);

			WeaponAttack* attack_secondary = new WeaponAttack(this);
			attack_secondary->m_type = WeaponAttack_FireSword_2;
			attack_secondary->m_damage = 5;
			attack_secondary->m_chance_of_fire = 4;
			attack_secondary->m_speed = 8;
			attack_secondary->m_energy_cost = 3;
			attack_secondary->m_crew_required = Crew_Warrior;
			m_attacks.push_back(attack_secondary);

			break;
		}
		case Weapon_Hammer:
		{
			m_weight = 4;

			WeaponAttack* attack_primary = new WeaponAttack(this);
			attack_primary->m_type = WeaponAttack_Hammer_1;
			attack_primary->m_damage = 3;
			attack_primary->m_chance_of_stun = 5;
			attack_primary->m_chance_of_unbalance = 3;
			attack_primary->m_speed = 5;
			attack_primary->m_energy_cost = 1;
			m_attacks.push_back(attack_primary);

			WeaponAttack* attack_secondary = new WeaponAttack(this);
			attack_secondary->m_type = WeaponAttack_Hammer_2;
			attack_secondary->m_damage = 1;
			attack_secondary->m_chance_of_stun = 5;
			attack_secondary->m_chance_of_unbalance = 1;
			attack_secondary->m_speed = 5;
			attack_secondary->m_energy_cost = 3;
			attack_secondary->m_crew_required = Crew_Warrior;
			m_attacks.push_back(attack_secondary);
			
			break;
		}
		case Weapon_Gun:
		{
			m_weight = 1;
			m_ranged = true;

			WeaponAttack* attack_primary = new WeaponAttack(this);
			attack_primary->m_type = WeaponAttack_Gun_1;
			attack_primary->m_damage = 3;
			attack_primary->m_chance_of_hit = 1;
			attack_primary->m_speed = 8;
			attack_primary->m_energy_cost = 1;
			m_attacks.push_back(attack_primary);
		
			WeaponAttack* attack_secondary = new WeaponAttack(this);
			attack_secondary->m_type = WeaponAttack_Gun_2;
			attack_secondary->m_damage = 2;
			attack_secondary->m_chance_of_hit = 4;
			attack_secondary->m_speed = 8;
			attack_secondary->m_energy_cost = 1;
			m_attacks.push_back(attack_secondary);

			break;
		}
	}
}

Weapon::~Weapon()
{
	for (vector<WeaponAttack*>::iterator it = m_attacks.begin(); it != m_attacks.end(); it++)
	{
		delete *it;
		*it = NULL;
	}
}