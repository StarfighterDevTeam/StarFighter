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
		case Weapon_BlasterRifle:
		{
			m_energetic = true;
			m_ranged = true;
			m_weight = 3;
			m_UI_display_name = "BLASTER RIFLE";
			
			WeaponAttack* attack_primary = new WeaponAttack(WeaponAttack_BlasterRifle_1, this);
			attack_primary->m_damage = 4;
			attack_primary->m_chance_of_hit = 4;
			attack_primary->m_chance_of_fire = 5;
			attack_primary->m_speed = 7;
			attack_primary->m_energy_cost = 3;
			attack_primary->m_nb_hits = 3;
			attack_primary->m_UI_display_name = "Three-shots";
			attack_primary->m_UI_description = "Hit 1 target, 3 times";
			m_attacks.push_back(attack_primary);

			WeaponAttack* attack_secondary = new WeaponAttack(WeaponAttack_BlasterRifle_2, this);
			attack_secondary->m_damage = 4;
			attack_secondary->m_chance_of_hit = 4;
			attack_secondary->m_chance_of_fire = 5;
			attack_secondary->m_speed = 7;
			attack_secondary->m_energy_cost = 3;
			attack_secondary->m_crew_required = Crew_Gunner;
			attack_secondary->m_UI_display_name = "Barrage";
			attack_secondary->m_UI_description = "Hit all targets on a line";
			m_attacks.push_back(attack_secondary);

			break;
		}
		case Weapon_FireSword:
		{
			m_weight = 2;
			m_energetic = true;
			m_UI_display_name = "FIRE SWORD";

			WeaponAttack* attack_primary = new WeaponAttack(WeaponAttack_FireSword_1, this);
			attack_primary->m_damage = 3;
			attack_primary->m_chance_of_fire = 5;
			attack_primary->m_speed = 8;
			attack_primary->m_energy_cost = 3;
			attack_primary->m_UI_display_name = "Lateral strike";
			attack_primary->m_UI_description = "Hit all targets on a line";
			m_attacks.push_back(attack_primary);

			WeaponAttack* attack_secondary = new WeaponAttack(WeaponAttack_FireSword_2, this);
			attack_secondary->m_damage = 5;
			attack_secondary->m_chance_of_fire = 4;
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

			WeaponAttack* attack_primary = new WeaponAttack(WeaponAttack_Hammer_1, this);
			attack_primary->m_damage = 3;
			attack_primary->m_chance_of_stun = 5;
			attack_primary->m_chance_of_unbalance = 3;
			attack_primary->m_speed = 5;
			attack_primary->m_energy_cost = 1;
			attack_primary->m_UI_display_name = "Hammer smash";
			attack_primary->m_UI_description = "Hit 1 target";
			m_attacks.push_back(attack_primary);

			WeaponAttack* attack_secondary = new WeaponAttack(WeaponAttack_Hammer_2, this);
			attack_secondary->m_damage = 1;
			attack_secondary->m_chance_of_stun = 5;
			attack_secondary->m_chance_of_unbalance = 1;
			attack_secondary->m_speed = 5;
			attack_secondary->m_energy_cost = 3;
			attack_secondary->m_crew_required = Crew_Warrior;
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

			WeaponAttack* attack_primary = new WeaponAttack(WeaponAttack_Gun_1, this);
			attack_primary->m_damage = 3;
			attack_primary->m_chance_of_hit = 1;
			attack_primary->m_speed = 8;
			attack_primary->m_energy_cost = 1;
			attack_primary->m_nb_targets = 2;
			attack_primary->m_nb_targets_remaining = attack_primary->m_nb_targets;
			attack_primary->m_UI_display_name = "Two-shots";
			attack_primary->m_UI_description = "Hit 2 targets";
			m_attacks.push_back(attack_primary);
		
			WeaponAttack* attack_secondary = new WeaponAttack(WeaponAttack_Gun_2, this);
			attack_secondary->m_damage = 2;
			attack_secondary->m_chance_of_hit = 4;
			attack_secondary->m_speed = 8;
			attack_secondary->m_energy_cost = 1;
			attack_secondary->m_nb_targets = 6;
			attack_secondary->m_nb_targets_remaining = attack_secondary->m_nb_targets;
			attack_secondary->m_UI_display_name = "One-mag";
			attack_secondary->m_UI_description = "Hit 6 targets";
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

	m_selected_attack = m_attacks.front();
}

Weapon::~Weapon()
{
	for (vector<WeaponAttack*>::iterator it = m_attacks.begin(); it != m_attacks.end(); it++)
	{
		delete *it;
		*it = NULL;
	}
}