#include "Weapon.h"

extern Game* CurrentGame;


Weapon::Weapon(WeaponType type, RobotSlot* owner)
{
	m_type = type;
	m_owner = owner;

	m_weight = 0;
	m_energetic = false;
	m_ranged = false;
	m_requires_close_distance = false;

	switch (type)
	{
		case Weapon_Fist:
		{
			m_weight = 1;
			m_ranged = false;
			m_UI_display_name = "FIST";
			m_UI_display_name_short = "Fist";

			WeaponAttack* attack_primary = new WeaponAttack(WeaponAttack_Fist_1, this);
			attack_primary->m_damage = 5;
			attack_primary->m_chance_of_unbalance = 2;
			attack_primary->m_speed = 9;
			attack_primary->m_energy_cost = 1;
			attack_primary->m_UI_display_name = "Punch";
			attack_primary->m_UI_description = "Hit 1 target";
			m_attacks.push_back(attack_primary);

			WeaponAttack* attack_secondary = new WeaponAttack(WeaponAttack_Fist_2, this);
			attack_secondary->m_damage = 2;
			attack_secondary->m_nb_targets = 3;
			attack_secondary->m_speed = 9;
			attack_secondary->m_energy_cost = 1;
			attack_secondary->m_UI_display_name = "Chain kicks";
			attack_secondary->m_UI_description = "Hit 3 targets";
			m_attacks.push_back(attack_secondary);

			break;
		}
		case Weapon_StunFist:
		{
			m_weight = 1;
			m_ranged = false;
			m_energetic = true;
			m_UI_display_name = "STUN FIST";
			m_UI_display_name_short = "Stun Fist";

			WeaponAttack* attack_primary = new WeaponAttack(WeaponAttack_StunFist_1, this);
			attack_primary->m_damage = 5;
			attack_primary->m_chance_of_unbalance = 2;
			attack_primary->m_chance_of_electricity = 4;
			attack_primary->m_speed = 9;
			attack_primary->m_energy_cost = 3;
			attack_primary->m_UI_display_name = "Punch";
			attack_primary->m_UI_description = "Hit 1 target";
			m_attacks.push_back(attack_primary);

			WeaponAttack* attack_secondary = new WeaponAttack(WeaponAttack_StunFist_2, this);
			attack_secondary->m_damage = 2;
			attack_secondary->m_chance_of_electricity = 5;
			attack_secondary->m_nb_targets = 3;
			attack_secondary->m_speed = 9;
			attack_secondary->m_energy_cost = 3;
			attack_secondary->m_UI_display_name = "Chain kicks";
			attack_secondary->m_UI_description = "Hit 3 targets";
			m_attacks.push_back(attack_secondary);

			break;
		}
		case Weapon_BlasterRifle:
		{
			m_energetic = true;
			m_ranged = true;
			m_weight = 3;
			m_UI_display_name = "BLASTER RIFLE";
			m_UI_display_name_short = "Blaster\nRifle";
			
			WeaponAttack* attack_primary = new WeaponAttack(WeaponAttack_BlasterRifle_1, this);
			attack_primary->m_damage = 3;
			attack_primary->m_chance_of_hit = 4;
			attack_primary->m_chance_of_fire = 5;
			attack_primary->m_speed = 7;
			attack_primary->m_energy_cost = 3;
			attack_primary->m_nb_hits = 3;
			attack_primary->m_UI_display_name = "Three-shots";
			attack_primary->m_UI_description = "Hit 1 target, 3 times";
			m_attacks.push_back(attack_primary);

			WeaponAttack* attack_secondary = new WeaponAttack(WeaponAttack_BlasterRifle_2, this);
			attack_secondary->m_damage = 3;
			attack_secondary->m_chance_of_hit = 4;
			attack_secondary->m_chance_of_fire = 5;
			attack_secondary->m_speed = 7;
			attack_secondary->m_energy_cost = 3;
			attack_secondary->m_nb_targets = 3;
			attack_secondary->m_crew_required = Crew_Gunner;
			attack_secondary->m_UI_display_name = "Barrage";
			attack_secondary->m_UI_description = "Hit 3 targets";
			m_attacks.push_back(attack_secondary);

			break;
		}
		case Weapon_LaserCannon:
		{
			m_energetic = true;
			m_ranged = true;
			m_weight = 4;
			m_UI_display_name = "LASER CANNON";
			m_UI_display_name_short = "Laser\nCannon";

			WeaponAttack* attack_primary = new WeaponAttack(WeaponAttack_LaserCannon_1, this);
			attack_primary->m_damage = 6;
			attack_primary->m_chance_of_hit = 2;
			attack_primary->m_chance_of_fire = 5;
			attack_primary->m_speed = 5;
			attack_primary->m_energy_cost = 3;
			attack_primary->m_nb_hits = 1;
			attack_primary->m_UI_display_name = "Charge Shot";
			attack_primary->m_UI_description = "Hit 1 target";
			m_attacks.push_back(attack_primary);

			WeaponAttack* attack_secondary = new WeaponAttack(WeaponAttack_LaserCannon_2, this);
			attack_secondary->m_damage = 6;
			attack_secondary->m_chance_of_hit = 3;
			attack_secondary->m_chance_of_fire = 5;
			attack_secondary->m_speed = 5;
			attack_secondary->m_energy_cost = 5;
			attack_secondary->m_nb_targets = 2;
			attack_secondary->m_crew_required = Crew_Gunner;
			attack_secondary->m_UI_display_name = "Double Shots";
			attack_secondary->m_UI_description = "Hit 2 targets";
			m_attacks.push_back(attack_secondary);

			break;
		}
		case Weapon_FireSword:
		{
			m_weight = 2;
			m_energetic = true;
			m_UI_display_name = "FIRE SWORD";
			m_UI_display_name_short = "Fire Sword";

			WeaponAttack* attack_primary = new WeaponAttack(WeaponAttack_FireSword_1, this);
			attack_primary->m_damage = 2;
			attack_primary->m_chance_of_fire = 5;
			attack_primary->m_speed = 8;
			attack_primary->m_energy_cost = 3;
			attack_primary->m_nb_targets = 3;
			attack_primary->m_UI_display_name = "Lateral strike";
			attack_primary->m_UI_description = "Hit 3 targets";
			m_attacks.push_back(attack_primary);

			WeaponAttack* attack_secondary = new WeaponAttack(WeaponAttack_FireSword_2, this);
			attack_secondary->m_damage = 5;
			attack_secondary->m_chance_of_fire = 3;
			attack_secondary->m_speed = 8;
			attack_secondary->m_energy_cost = 3;
			attack_secondary->m_crew_required = Crew_Warrior;
			attack_secondary->m_UI_display_name = "Estoc";
			attack_secondary->m_UI_description = "Hit 1 target";
			m_attacks.push_back(attack_secondary);

			break;
		}
		case Weapon_Hammer:
		{
			m_weight = 4;
			m_UI_display_name = "HAMMER";
			m_UI_display_name_short = "Hammer";

			WeaponAttack* attack_primary = new WeaponAttack(WeaponAttack_Hammer_1, this);
			attack_primary->m_damage = 3;
			attack_primary->m_chance_of_stun = 4;
			attack_primary->m_chance_of_unbalance = 4;
			attack_primary->m_speed = 5;
			attack_primary->m_energy_cost = 1;
			attack_primary->m_UI_display_name = "Hammer smash";
			attack_primary->m_UI_description = "Hit 1 target";
			m_attacks.push_back(attack_primary);

			WeaponAttack* attack_secondary = new WeaponAttack(WeaponAttack_Hammer_2, this);
			attack_secondary->m_damage = 1;
			attack_secondary->m_chance_of_stun = 5;
			attack_secondary->m_chance_of_unbalance = 2;
			attack_secondary->m_speed = 5;
			attack_secondary->m_energy_cost = 2;
			attack_secondary->m_crew_required = Crew_Warrior;
			attack_secondary->m_hitmode = Hit_AdjacentSlots;
			attack_secondary->m_UI_display_name = "Explosive smash";
			attack_secondary->m_UI_description = "Hit 1 target and all adjacents";
			m_attacks.push_back(attack_secondary);
			
			break;
		}
		case Weapon_Gun:
		{
			m_weight = 1;
			m_ranged = true;
			m_UI_display_name = "GUN";
			m_UI_display_name_short = "Gun";

			WeaponAttack* attack_primary = new WeaponAttack(WeaponAttack_Gun_1, this);
			attack_primary->m_damage = 3;
			attack_primary->m_chance_of_hit = 1;
			attack_primary->m_speed = 8;
			attack_primary->m_energy_cost = 1;
			attack_primary->m_nb_targets = 2;
			attack_primary->m_UI_display_name = "Two-shots";
			attack_primary->m_UI_description = "Hit 2 targets";
			m_attacks.push_back(attack_primary);
		
			WeaponAttack* attack_secondary = new WeaponAttack(WeaponAttack_Gun_2, this);
			attack_secondary->m_damage = 2;
			attack_secondary->m_chance_of_hit = 4;
			attack_secondary->m_speed = 8;
			attack_secondary->m_energy_cost = 1;
			attack_secondary->m_nb_targets = 5;
			attack_secondary->m_UI_display_name = "One-mag";
			attack_secondary->m_UI_description = "Hit 5 targets";
			m_attacks.push_back(attack_secondary);

			break;
		}
		case Weapon_MachineGun:
		{
			m_weight = 3;
			m_ranged = true;
			m_UI_display_name = "MACHINE GUN";
			m_UI_display_name_short = "Machine Gun";

			WeaponAttack* attack_primary = new WeaponAttack(WeaponAttack_MachineGun_1, this);
			attack_primary->m_damage = 3;
			attack_primary->m_chance_of_hit = 4;
			attack_primary->m_speed = 7;
			attack_primary->m_energy_cost = 1;
			attack_primary->m_nb_hits = 3;
			attack_primary->m_UI_display_name = "Three-shots";
			attack_primary->m_UI_description = "Hit 1 target, 3 times";
			m_attacks.push_back(attack_primary);

			WeaponAttack* attack_secondary = new WeaponAttack(WeaponAttack_MachineGun_2, this);
			attack_secondary->m_damage = 3;
			attack_secondary->m_chance_of_hit = 4;
			attack_secondary->m_speed = 7;
			attack_secondary->m_energy_cost = 1;
			attack_secondary->m_nb_targets = 3;
			attack_secondary->m_crew_required = Crew_Gunner;
			attack_secondary->m_UI_display_name = "Barrage";
			attack_secondary->m_UI_description = "Hit 3 targets";
			m_attacks.push_back(attack_secondary);

			break;
		}
		case Weapon_Grab:
		{
			m_requires_close_distance = true;
			m_UI_display_name = "GRAB";

			WeaponAttack* attack_primary = new WeaponAttack(WeaponAttack_Grab_1, this);
			attack_primary->m_UI_display_name = "Grab";
			attack_primary->m_UI_description = "Requires Close-combat & hitting a destroyed/shutdown module\nStays in close combat & deals x2 damage";
			m_attacks.push_back(attack_primary);
			break;
		}
		case Weapon_Guard:
		{
			m_UI_display_name = "GUARD";

			WeaponAttack* attack_primary = new WeaponAttack(WeaponAttack_Guard_1, this);
			attack_primary->m_UI_display_name = "Guard";
			attack_primary->m_UI_description = "Choose guard speed`between 1 and 10.\nRedirects slower attack speeds to hand.";
			m_attacks.push_back(attack_primary);
			break;
		}
	}

	//reset targets available
	for (vector<WeaponAttack*>::iterator it = m_attacks.begin(); it != m_attacks.end(); it++)
	{
		if ((*it)->m_nb_targets > 1)
		{
			(*it)->m_nb_targets_remaining = (*it)->m_nb_targets;
		}
	}

	m_selected_attack = m_attacks.front();
	if (m_UI_display_name_short.empty() == true)
	{
		m_UI_display_name_short = m_UI_display_name;
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