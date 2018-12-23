#include "Robot.h"

extern Game* CurrentGame;


Robot::Robot()
{
	m_weight = 0;
	m_crew_nb = 0;
	m_crew_max = 0;
	m_energy_cells = 0;
	m_unbalanced = false;
	m_shutdown_global = false;

	m_ready_to_change_phase = false;

	//Robot slots
	m_slots.push_back(RobotSlot(Index_Head));//0
	m_slots.push_back(RobotSlot(Index_LegL));//1
	m_slots.push_back(RobotSlot(Index_LegR));//2
	m_slots.push_back(RobotSlot(Index_FootL));//3
	m_slots.push_back(RobotSlot(Index_FootR));//4
	m_slots.push_back(RobotSlot(Index_ShoulderL));//5
	m_slots.push_back(RobotSlot(Index_ShoulderR));//6
	m_slots.push_back(RobotSlot(Index_ForearmL));//7
	m_slots.push_back(RobotSlot(Index_ForearmR));//8
	m_slots.push_back(RobotSlot(Index_BodyU));//9
	m_slots.push_back(RobotSlot(Index_BodyM));//10
	m_slots.push_back(RobotSlot(Index_BodyD));//11

	//Mandatory slots
	SetModule(Module_Head, Index_Head);
	SetModule(Module_Stabilizers, Index_FootL);
	SetModule(Module_Stabilizers, Index_FootR);

	//Crew members
	m_crew.push_back(CrewMember(Crew_Captain));//0
	m_crew.push_back(CrewMember(Crew_Scientist));//1
	m_crew.push_back(CrewMember(Crew_Mechanic));//2
	m_crew.push_back(CrewMember(Crew_Pilot));//3
	m_crew.push_back(CrewMember(Crew_Engineer));//4
	m_crew.push_back(CrewMember(Crew_Warrior));//5
	m_crew.push_back(CrewMember(Crew_Medic));//6
	m_crew.push_back(CrewMember(Crew_Gunner));//7
}

Robot::~Robot()
{
	for (int k = 0; k < m_slots.size(); k++)
	{
		delete m_slots[k].m_module;
	}
}

bool Robot::SetCrewMember(CrewType type, SlotIndex index)
{
	if (m_crew_nb >= m_crew_max)
	{
		printf("Cannnot add crew member. Max crew size reached (%d)\n", m_crew_max);
		return false;
	}
	else
	{
		m_slots[index].m_crew.push_back(m_crew[(int)type]);
		m_crew[(int)type].m_slot_index = index;

		m_crew_nb++;
		return true;
	}
}

bool Robot::SetModule(ModuleType type, SlotIndex index)
{
	Module* module = new Module(type);

	if (m_weight + module->m_weight > MAX_ROBOT_WEIGHT)
	{
		printf("Cannot equip module on slot %d. Weight max (%d) exceeded.\n", (int)index, MAX_ROBOT_WEIGHT);
		delete module;
		module = NULL;
		return false;
	}
	else if (m_slots[index].m_module)
	{
		printf("Cannot equip module. Slot %d already occupied.\n", (int)index);
		delete module;
		module = NULL;
		return false;
	}
	else if (module->m_size > m_slots[index].m_size)
	{
		printf("Cannot equip module. Module size (%d) exceeds the slot's max size (%d).\n", module->m_size, m_slots[index].m_size);
		delete module;
		module = NULL;
		return false;
	}
	else if (module->m_type == Module_Weapon && m_slots[index].m_type != Slot_Forearm && m_slots[index].m_type != Slot_Shoulder)
	{
		printf("Cannot equip weapon on this slot. Weapons go on arms and shoulders only.\n");
		delete module;
		module = NULL;
		return false;
	}
	else
	{
		m_slots[index].m_module = module;

		m_weight += module->m_weight;
		m_crew_max += module->m_crew_max;

		return true;
	}
}

bool Robot::SetEquipment(EquipmentType type, SlotIndex index)
{
	Equipment* equipment = new Equipment(type);

	int current_equipment_size = 0;
	for (vector<Equipment*>::iterator it = m_slots[index].m_equipments.begin(); it != m_slots[index].m_equipments.end(); it++)
	{
		current_equipment_size += (*it)->m_size;
	}

	if (m_weight + equipment->m_weight > MAX_ROBOT_WEIGHT)
	{
		printf("Cannot equip equipment on slot %d. Weight max (%d) exceeded.\n", (int)index, MAX_ROBOT_WEIGHT);
		delete equipment;
		equipment = NULL;
		return false;
	}
	else if (m_slots[index].m_module == NULL)
	{
		printf("Cannot equip equipment on slot %d because it has no Module. Equipments must be set on slots with modules.\n", (int)index);
		delete equipment;
		equipment = NULL;
		return false;
	}
	else if (equipment->m_size + m_slots[index].m_module->m_size + current_equipment_size > m_slots[index].m_size)
	{
		printf("Cannot equip equipment on slot %d because it exceeds the remaining size (%d) on the slot it.\n", (int)index, m_slots[index].m_size - m_slots[index].m_module->m_size - current_equipment_size);
		delete equipment;
		equipment = NULL;
		return false;
	}
	else if (equipment->m_module_equipable != m_slots[index].m_module->m_type && equipment->m_module_equipable != NB_MODULE_TYPES)
	{
		printf("Equipment not compatible with this type of Module.\n");
		delete equipment;
		equipment = NULL;
		return false;
	}
	else
	{
		m_slots[index].m_equipments.push_back(equipment);

		m_weight += equipment->m_weight;
		m_crew_max += equipment->m_crew_max;
		return true;
	}
}


bool Robot::SetWeapon(WeaponType type, SlotIndex index)
{
	Weapon* weapon = new Weapon(type);

	if (m_weight + weapon->m_weight > MAX_ROBOT_WEIGHT)
	{
		printf("Cannot equip weapon on slot %d. Weight max (%d) exceeded.\n", (int)index, MAX_ROBOT_WEIGHT);
		delete weapon;
		weapon = NULL;
		return false;
	}
	else if (m_slots[index].m_module == NULL)
	{
		printf("Cannot equip weapon on slot %d because it has no Module. Weapon must be set on Weapon modules.\n", (int)index);
		delete weapon;
		weapon = NULL;
		return false;
	}
	else if (m_slots[index].m_module->m_type != Module_Weapon)
	{
		printf("Cannot equip weapon on slot %d because it has no Weapon module. Weapon must be set on Weapon modules.\n", (int)index);
		delete weapon;
		weapon = NULL;
		return false;
	}
	else if (weapon->m_ranged && m_slots[index].m_type != Slot_Shoulder)
	{
		printf("Cannot equip a ranged weapon on slot %d because it must be equipped on Shoulders.\n", (int)index);
		delete weapon;
		weapon = NULL;
		return false;
	}
	else if (!weapon->m_ranged && m_slots[index].m_type != Slot_Forearm)
	{
		printf("Cannot equip a close-combat weapon on slot %d because it must be equipped on Forearms.\n", (int)index);
		delete weapon;
		weapon = NULL;
		return false;
	}
	else if (m_slots[index].m_weapon)
	{
		printf("Cannot equip weapon on slot %d because it is already equipped with a weapon.\n", (int)index);
		delete weapon;
		weapon = NULL;
		return false;
	}
	else
	{
		m_slots[index].m_weapon = weapon;

		m_weight += weapon->m_weight;
		return true;
	}
}

int Robot::GetHealthMax()
{
	int health_max = 0;

	for (int k = 0; k < m_slots.size(); k++)
	{
		if (m_slots[k].m_module)
		{
			health_max += m_slots[k].m_module->m_health_max;

			for (vector<Equipment*>::iterator it = m_slots[k].m_equipments.begin(); it < m_slots[k].m_equipments.end(); it++)
			{
				health_max += (*it)->m_health_max;
			}
		}
	}

	health_max = health_max / 4;

	return health_max;
}

int Robot::GetBalance()
{
	int balance = 0;

	if (m_slots[Index_FootL].m_module && m_slots[Index_FootL].m_module->m_type == Module_Stabilizers)
	{
		balance += m_slots[Index_FootL].m_module->m_health;
	}
	
	if (m_slots[Index_FootR].m_module && m_slots[Index_FootR].m_module->m_type == Module_Stabilizers)
	{
		balance += m_slots[Index_FootR].m_module->m_health;
	}

	balance += m_balance_bonus;

	return balance;
}

void Robot::GetWeightModifiers(int &balance_bonus, int &attack_speed_bonus)
{
	balance_bonus = 0;
	attack_speed_bonus = 0;

	if (m_weight >= 16 && m_weight <= 17)
	{
		balance_bonus = 1;
		attack_speed_bonus = -1;
	}
	else if (m_weight >= 18 && m_weight <= 19)
	{
		balance_bonus = 2;
		attack_speed_bonus = -2;
	}
	else if (m_weight >= 20 && m_weight <= 21)
	{
		balance_bonus = 3;
		attack_speed_bonus = -3;
	}
	else if (m_weight >= 22 && m_weight <= 23)
	{
		balance_bonus = 4;
		attack_speed_bonus = -4;
	}
	else if (m_weight >= 24)
	{
		balance_bonus = 5;
		attack_speed_bonus = -5;
	}
}

void Robot::Initialize()
{
	m_health_max = GetHealthMax();
	m_health = m_health_max;

	GetWeightModifiers(m_balance_bonus, m_attack_speed_bonus);
}

void Robot::Update()
{
	switch ((*CurrentGame).m_phase)
	{
		case Phase_GenerateEC:
		{
			m_energy_cells += Generateenergy_cells();

			m_ready_to_change_phase = true;
			break;
		}
		case Phase_CrewMovement:
		{
			//todo

			m_ready_to_change_phase = true;
			break;
		}
		case Phase_AttackPlanning:
		{
			if (m_shutdown_global == false)
			{
				//repartition armes que l'on veut utiliser parmi les emplacements weapon mod
				//+ r�partition actions des modules
				//les armes de CaC ne peuvent viser que les parties basses
				//appliquer +2 de speed aux armes ranged si distance de combat ranged (max 10)

				//depense des EC pour attaque ( choisir attaque 1 ou 2 de l'arme)

				//choix des cibles (module)

				//choix de cible du medic (heald by medic au sein du m�me module)

			}
			
			m_ready_to_change_phase = true;
			break;
		}
		case Phase_HealCrew:
		{
			HealCrewMembers();

			m_ready_to_change_phase = true;
			break;
		}
		case Phase_RepairModules:
		{
			RepairModules();

			m_ready_to_change_phase = true;
			break;
		}
		case Phase_EquipmentResolution:
		{
			//todo , jetpacks

			m_ready_to_change_phase = true;
			break;
		}
		case Phase_GrabResolution:
		{
			//todo

			m_ready_to_change_phase = true;
			break;
		}
		case Phase_GuardResolution:
		{
			//todo

			m_ready_to_change_phase = true;
			break;
		}
		case Phase_FireResolution://after the attack resolutions
		{
			UpdateFirePropagation();

			UpdateShudownGlobal();//in case fire destroys the generator or kills the captain or the pilot

			m_ready_to_change_phase = true;
			break;
		}
	}
}

int Robot::Generateenergy_cells()
{
	int energy_cells = 0;

	for (vector<RobotSlot>::iterator it = m_slots.begin(); it != m_slots.end(); it++)
	{
		if ((*it).m_module && (*it).m_module->m_type == Module_Generator)
		{
			energy_cells += 3;

			for (vector<CrewMember>::iterator it2 = (*it).m_crew.begin(); it2 != (*it).m_crew.end(); it2++)
			{
				if ((*it2).m_type == Crew_Scientist)
				{
					energy_cells++;
				}
			}
		}
	}

	return energy_cells;
}

bool Robot::HealCrewMembers()
{
	bool healed = false;

	for (vector<RobotSlot>::iterator it = m_slots.begin(); it != m_slots.end(); it++)
	{
		for (vector<CrewMember>::iterator it2 = (*it).m_crew.begin(); it2 != (*it).m_crew.end(); it2++)		
		{
			int health_missing = (*it2).m_health_max - (*it2).m_health;

			//Infirmary
			if ((*it).m_module && (*it).m_module->m_type == Module_Infirmary)
			{
				if (health_missing > 0)
				{
					(*it2).m_health++;
					healed = true;
				}
			}

			//Medic
			if ((*it2).m_is_healed_by_medic)
			{
				if (health_missing > 0)
				{
					if (health_missing == 1)
					{
						(*it2).m_health++;
					}
					else
					{
						(*it2).m_health += 2;
					}
					healed = true;
				}

				(*it2).m_is_healed_by_medic = false;
			}
		}
	}

	return healed;
}


bool Robot::RepairModules()
{
	bool repaired = false;

	for (vector<RobotSlot>::iterator it = m_slots.begin(); it != m_slots.end(); it++)
	{
		Module* module = (*it).m_module;
		if (module != NULL)
		{
			for (vector<CrewMember>::iterator it2 = (*it).m_crew.begin(); it2 != (*it).m_crew.end(); it2++)
			{
				int health_missing = module->m_health_max - module->m_health;

				if (health_missing > 0)
				{
					if ((*it2).m_type != Crew_Mechanic)
					{
						module->m_health++;
					}
					else
					{
						if (health_missing == 1)
						{
							module->m_health++;
						}
						else
						{
							module->m_health += 2;
						}
					}
					repaired = true;
				}
			}

			if ((*it).m_crew.empty() == false)
			{
				if (module->m_is_burning)
				{
					module->m_is_burning = false;
					printf("Fire extinguished.\n");
				}
				
				if (module->m_is_shutdown)
				{
					module->m_is_shutdown = false;
					printf("Shutdown repaired.\n");
				}
			}
		}
	}

	return repaired;
}


void Robot::UpdateFirePropagation()
{
	for (vector<RobotSlot>::iterator it = m_slots.begin(); it != m_slots.end(); it++)
	{
		Module* module = (*it).m_module;
		if (module != NULL && module->m_is_burning)
		{
			//Damage to module
			if (module->m_health > 0)
			{
				module->m_health--;
			}

			//Damage to crew
			for (vector<CrewMember>::iterator it2 = (*it).m_crew.begin(); it2 != (*it).m_crew.end(); it2++)
			{
				(*it2).m_health--;
			}
			(*it).UpdateCrew();
			UpdateShudownGlobal();

			//Fire propagation
			for (vector<RobotSlot>::iterator it2 = m_slots.begin(); it2 != m_slots.end(); it2++)
			{
				if (it2->m_module != NULL && it2->m_module->m_is_burning == false)
				{
					//adjacent slots
					if ((abs(it2->m_coord_x - it->m_coord_x) == 1 && abs(it2->m_coord_y - it->m_coord_y) == 0) 
						|| (abs(it2->m_coord_x - it->m_coord_x) == 0 && abs(it2->m_coord_y - it->m_coord_y) == 1))
					{
						bool propagation_sucess = RandomizeIntBetweenValues(1, 6) >= 4;

						if (propagation_sucess)
						{
							it2->m_module->m_is_burning = true;
							printf("Fire propagated from slot %d to slot %d.\n", (int)it->m_index, (int)it2->m_index);
						}
					}
				}
			}
		}
	}
}

bool Robot::CheckShudownGlobalConditions()
{
	for (vector<RobotSlot>::iterator it = m_slots.begin(); it != m_slots.end(); it++)
	{
		//Generator destroyed?
		if (it->m_module && it->m_module->m_type == Module_Generator && it->m_module->m_health == 0)
		{
			return true;
		}

		//Captain or pilot are alive and not stunned in the head?
		if (it->m_type == Module_Head)
		{
			for (vector<CrewMember>::iterator it2 = it->m_crew.begin(); it2 != it->m_crew.end(); it2++)
			{
				if (it2->m_type == Crew_Captain && it2->m_health > 0 && it2->m_is_stunned == false)
				{
					return false;
				}

				if (it2->m_type == Crew_Pilot && it2->m_health > 0 && it2->m_is_stunned == false)
				{
					return false;
				}
			}
		}
	}	

	return true;
}

bool Robot::UpdateShudownGlobal()
{
	//Start of a shutdown?
	if (m_shutdown_global == false)
	{
		if (CheckShudownGlobalConditions() == true)
		{
			m_shutdown_global = true;
			printf("Global shutdown triggered on robot %d.\n", m_index);
			return true;
		}
		else
		{
			return false;
		}
	}
	//End of a shutdown?
	else
	{
		if (CheckShudownGlobalConditions() == false)
		{
			m_shutdown_global = false;
			printf("Global shutdown ended on robot %d.\n", m_index);
			return true;
		}
		else
		{
			return false;
		}
	}
}

//ROBOT SLOT
RobotSlot::RobotSlot(SlotIndex index)
{
	m_index = index;
	m_module = NULL;
	m_weapon = NULL;

	switch (index)
	{
		case Index_Head:
		{
			m_size = 1;
			m_type = Slot_Head;
			m_coord_x = 0;
			m_coord_y = 3;
			break;
		}
		case Index_BodyU:
		{
			m_size = 3;
			m_type = Slot_Body;
			m_coord_x = 0;
			m_coord_y = 2;
			break;
		}
		case Index_BodyM:
		{
			m_size = 3;
			m_type = Slot_Body;
			m_coord_x = 0;
			m_coord_y = 1;
			break;
		}
		case Index_BodyD:
		{
			m_size = 2;
			m_type = Slot_Body;
			m_coord_x = 0;
			m_coord_y = 0;
			break;
		}
		case Index_LegL:
		{
			m_size = 2;
			m_type = Slot_Leg;
			m_coord_x = -1;
			m_coord_y = 0;
			break;
		}
		case Index_LegR:
		{
			m_size = 2;
			m_type = Slot_Leg;
			m_coord_x = 1;
			m_coord_y = 0;
			break;
		}
		case Index_FootL:
		{
			m_size = 2;
			m_type = Slot_Foot;
			m_coord_x = -2;
			m_coord_y = 0;
			break;
		}
		case Index_FootR:
		{
			m_size = 2;
			m_type = Slot_Foot;
			m_coord_x = 2;
			m_coord_y = 0;
			break;
		}
		case Index_ShoulderL:
		{
			m_size = 2;
			m_type = Slot_Shoulder;
			m_coord_x = -1;
			m_coord_y = 2;
			break;
		}
		case Index_ShoulderR:
		{
			m_size = 2;
			m_type = Slot_Shoulder;
			m_coord_x = 1;
			m_coord_y = 2;
			break;
		}
		case Index_ForearmL:
		{
			m_size = 2;
			m_type = Slot_Forearm;
			m_coord_x = -2;
			m_coord_y = 2;
			break;
		}
		case Index_ForearmR:
		{
			m_size = 2;
			m_type = Slot_Forearm;
			m_coord_x = 2;
			m_coord_y = 2;
			break;
		}
	}
}

void RobotSlot::UpdateCrew()
{
	vector<CrewMember> old_crew;
	for (vector<CrewMember>::iterator it = m_crew.begin(); it != m_crew.end(); it++)
	{
		old_crew.push_back(*it);
	}

	m_crew.clear();

	for (vector<CrewMember>::iterator it = old_crew.begin(); it != old_crew.end(); it++)
	{
		if ((*it).m_health > 0)
		{
			m_crew.push_back(*it);
		}
		else
		{
			printf("Crew member dead.\n");
			if ((*it).m_type == Crew_Captain)
			{
				printf("Captain was killed. GAME OVER.\n");
			}
		}
	}
}