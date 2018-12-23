#include "Robot.h"

extern Game* CurrentGame;


Robot::Robot()
{
	m_weight = 0;
	m_crew_nb = 0;
	m_crew_max = 0;
	m_cells = 0;
	m_unbalanced = false;

	m_ready_to_change_phase = false;

	//Robot slots
	m_slots.push_back(RobotSlot(Slot_Head, 1, Index_Head));//0

	m_slots.push_back(RobotSlot(Slot_Leg, 2, Index_LegL));//1
	m_slots.push_back(RobotSlot(Slot_Leg, 2, Index_LegR));//2

	m_slots.push_back(RobotSlot(Slot_Foot, 2, Index_FootL));//3
	m_slots.push_back(RobotSlot(Slot_Foot, 2, Index_FootR));//4

	m_slots.push_back(RobotSlot(Slot_Shoulder, 2, Index_ShoulderL));//5
	m_slots.push_back(RobotSlot(Slot_Shoulder, 2, Index_ShoulderR));//6

	m_slots.push_back(RobotSlot(Slot_Forearm, 2, Index_ForearmL));//7
	m_slots.push_back(RobotSlot(Slot_Forearm, 2, Index_ForearmR));//8

	m_slots.push_back(RobotSlot(Slot_Body, 3, Index_BodyU));//9
	m_slots.push_back(RobotSlot(Slot_Body, 3, Index_BodyM));//10
	m_slots.push_back(RobotSlot(Slot_Body, 2, Index_BodyD));//11

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

	//Mandatory crew member
	SetCrewMember(Crew_Captain, Index_Head);
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
			m_cells += GenerateCells();

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
			//repartition armes que l'on veut utiliser parmi les emplacements weapon mod
			//+ répartition actions des modules
			//les armes de CaC ne peuvent viser que les parties basses
			//appliquer +2 de speed aux armes ranged si distance de combat ranged (max 10)

			//depense des EC pour attaque ( choisir attaque 1 ou 2 de l'arme)

			//choix des cibles (module)

			m_ready_to_change_phase = true;
			break;
		}
	}
}

int Robot::GenerateCells()
{
	int cells = 0;

	for (vector<RobotSlot>::iterator it = m_slots.begin(); it != m_slots.end(); it++)
	{
		if ((*it).m_module && (*it).m_module->m_type == Module_Generator)
		{
			cells += 3;

			for (vector<CrewMember>::iterator it2 = (*it).m_crew.begin(); it2 != (*it).m_crew.end(); it2++)
			{
				if ((*it2).m_type == Crew_Scientist)
				{
					cells++;
				}
			}
		}
	}

	return cells;
}


