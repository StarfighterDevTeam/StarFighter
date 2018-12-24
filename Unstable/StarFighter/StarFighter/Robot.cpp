#include "Robot.h"

extern Game* CurrentGame;


Robot::Robot()
{
	m_weight = 0;
	m_crew_max = 0;
	m_energy_cells = 0;
	m_energy_cells_available = 0;
	m_unbalanced = false;
	m_shutdown_global = false;

	m_ready_to_change_phase = false;

	//Robot slots
	for (int i = 0; i < NB_SLOT_INDEX; i++)
	{
		m_slots.push_back(RobotSlot((SlotIndex)i, this));
	}
	/*
	m_slots.push_back(RobotSlot(Index_Head, this));//0
	m_slots.push_back(RobotSlot(Index_LegL, this));//1
	m_slots.push_back(RobotSlot(Index_LegR, this));//2
	m_slots.push_back(RobotSlot(Index_FootL, this));//3
	m_slots.push_back(RobotSlot(Index_FootR, this));//4
	m_slots.push_back(RobotSlot(Index_ShoulderL, this));//5
	m_slots.push_back(RobotSlot(Index_ShoulderR, this));//6
	m_slots.push_back(RobotSlot(Index_ForearmL, this));//7
	m_slots.push_back(RobotSlot(Index_ForearmR, this));//8
	m_slots.push_back(RobotSlot(Index_BodyU, this));//9
	m_slots.push_back(RobotSlot(Index_BodyM, this));//10
	m_slots.push_back(RobotSlot(Index_BodyD, this));//11
	*/

	//Mandatory slots
	SetModule(Module_Head, Index_Head);
	SetModule(Module_Stabilizers, Index_FootL);
	SetModule(Module_Stabilizers, Index_FootR);
}

Robot::~Robot()
{
	for (int k = 0; k < m_slots.size(); k++)
	{
		delete m_slots[k].m_module;
	}

	for (vector<CrewMember*>::iterator it = m_crew.begin(); it != m_crew.end(); it++)
	{
		delete *it;
		*it = NULL;
	}
}

bool Robot::SetCrewMember(CrewType type, SlotIndex index)
{
	CrewMember* crew_member = new CrewMember(type, &m_slots[index]);

	for (vector<CrewMember*>::iterator it = m_crew.begin(); it != m_crew.end(); it++)
	{
		if ((*it)->m_type == Crew_Captain && type == Crew_Captain)
		{
			printf("Cannot have 2 Captains on a robot.\n");
			delete crew_member;
			crew_member = NULL;
			return false;
		}
	}

	if (m_crew.size() >= m_crew_max)
	{
		printf("Cannnot add crew member. Max crew size reached (%d)\n", m_crew_max);
		delete crew_member;
		crew_member = NULL;
		return false;
	}
	else
	{
		crew_member->m_index = index;

		m_crew.push_back(crew_member);
		m_slots[index].m_crew.push_back(crew_member);

		return true;
	}
}

bool Robot::SetModule(ModuleType type, SlotIndex index)
{
	Module* module = new Module(type, &m_slots[index]);

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
	Equipment* equipment = new Equipment(type, &m_slots[index]);

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
	Weapon* weapon = new Weapon(type, &m_slots[index]);

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
	else if (weapon->m_ranged == true && m_slots[index].m_type != Slot_Shoulder && m_slots[index].m_type != Slot_Forearm)
	{
		printf("Cannot equip a ranged weapon on slot %d because it must be equipped on Shoulders or Forearms.\n", (int)index);
		delete weapon;
		weapon = NULL;
		return false;
	}
	else if (weapon->m_ranged == false && m_slots[index].m_type != Slot_Forearm)
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

int Robot::GetBalanceScore()
{
	int balance = 0;

	for (vector<RobotSlot>::iterator it = m_slots.begin(); it != m_slots.end(); it++)
	{
		Module* module = it->m_module;
		if (module != NULL && module->m_type == Module_Stabilizers && module->IsOperationnal())
		{
			balance += module->m_health;
		}
	}

	balance += m_balance_bonus;//bonus from weight modifier

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
	//Health
	m_health_max = GetHealthMax();
	m_health = m_health_max;

	//Balance and attack speed bonus/malus
	GetWeightModifiers(m_balance_bonus, m_attack_speed_bonus);

	//Energy cells
	for (vector<RobotSlot>::iterator it = m_slots.begin(); it != m_slots.end(); it++)
	{
		if (it->m_module != NULL && it->m_module->m_energy_cells_max > 0)
		{
			if (m_energy_cells < MAX_ROBOT_ENERGY_CELLS)
			{
				it->m_module->m_energy_cells = 1;
				m_energy_cells++;
			}
		}
	}
}

void Robot::Update()
{
	switch ((*CurrentGame).m_phase)
	{
		case Phase_GenerateEC:
		{
			GenerateEnergyCells();

			m_ready_to_change_phase = true;
			break;
		}
		case Phase_CrewMovement:
		{
			//todo
			//pas de délplacement si stunned
			//déplacements gratuits sur slots vides + cheminement

			//TEST
			//if (m_index == 1)
			//{
			//	MoveCrewMemberToSlot(m_slots[Index_Head].m_crew.back(), Index_ShoulderR);
			//}

			m_ready_to_change_phase = true;
			break;
		}
		case Phase_AttackPlanning:
		{
			if (m_shutdown_global == false)
			{
				//TEST
				if (m_index == 0)
				{
					SetEnergyCell(m_slots[Index_ForearmL].m_weapon->m_attacks.front());
					SetEnergyCell(m_slots[Index_ForearmL].m_weapon->m_attacks.front());
					SetEnergyCell(m_slots[Index_ForearmL].m_weapon->m_attacks.front());
				
					SetWeaponAttackOnSlot(m_slots[Index_ForearmL].m_weapon->m_attacks.front(), Index_Head);
				}

				//verifier que le module / equipement utilisé n'est pas détruit
				//verifier que le crew member requis n'est pas stunned - notamment les gadgets requirent un Ingenieur

				//repartition armes que l'on veut utiliser parmi les emplacements weapon mod
				//+ répartition actions des modules
				//les armes de CaC ne peuvent viser que les parties basses
				//appliquer +2 de speed aux armes ranged si distance de combat ranged (max 10)

				//depense des EC  ( + choisir attaque 1 ou 2 de l'arme)

				//choix des cibles (module)

				//choix de cible du medic (heald by medic au sein du même module)

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

		// ****************************************************
		//					ATTACK RESOLUTION
		// ****************************************************

		case Phase_CooldownResolution:
		{
			UpdateCooldowns();

			m_ready_to_change_phase = true;
			break;
		}
		case Phase_FireResolution:
		{
			UpdateFirePropagation();

			UpdateShudownGlobal();//in case fire destroys the generator or kills the captain or the pilot

			m_ready_to_change_phase = true;
			break;
		}
	}
}

int Robot::GenerateEnergyCells()
{
	int energy_cells_generated = 0;

	for (vector<RobotSlot>::iterator it = m_slots.begin(); it != m_slots.end(); it++)
	{
		if ((*it).m_module && (*it).m_module->m_type == Module_Generator)
		{
			int cells = 3;

			for (vector<CrewMember*>::iterator it2 = (*it).m_crew.begin(); it2 != (*it).m_crew.end(); it2++)
			{
				if ((*it2)->m_type == Crew_Scientist && (*it2)->m_stun_counter == 0)//checking if not stunned
				{
					cells++;
				}
			}

			for (int i = 0; i < cells; i++)
			{
				if (m_energy_cells < MAX_ROBOT_ENERGY_CELLS)
				{
					m_energy_cells++;
					m_energy_cells_available++;
					energy_cells_generated++;
				}
			}
		}
	}

	return energy_cells_generated;
}

int Robot::HealCrewMembers()
{
	int health_healed = 0;

	for (vector<RobotSlot>::iterator it = m_slots.begin(); it != m_slots.end(); it++)
	{
		int health = 0;

		Module* module = it->m_module;
		//Infirmary heals 1pv/energy cell
		if (module != NULL && module->m_type == Module_Infirmary && module->IsOperationnal())
		{
			health += module->m_energy_cells;
		}

		for (vector<CrewMember*>::iterator it2 = (*it).m_crew.begin(); it2 != (*it).m_crew.end(); it2++)
		{
			//Target of a Medic heals 2pv
			int health_medic = 0;
			if ((*it2)->m_is_healed_by_medic)
			{
				health_medic = 2;
				(*it2)->m_is_healed_by_medic = false;
			}

			for (int i = 0; i < health + health_medic; i++)
			{
				if (m_health < m_health_max)
				{
					m_health++;
					health_healed++;
				}
			}
		}
	}
			
	return health_healed;
}


int Robot::RepairModules()
{
	int health_repaired = 0;

	for (vector<RobotSlot>::iterator it = m_slots.begin(); it != m_slots.end(); it++)
	{
		Module* module = (*it).m_module;
		if (module != NULL)
		{ 
			//Shutdown update
			if (module->m_shutdown_counter > 0)
			{
				module->m_shutdown_counter--;
			}

			//Repair modules
			int health = 0;
			for (vector<CrewMember*>::iterator it2 = (*it).m_crew.begin(); it2 != (*it).m_crew.end(); it2++)
			{
				//Crew member must not be stunned in order to repair
				if ((*it2)->m_stun_counter == 0)
				{
					health++;

					//Mechanic passive effect heals +1pv
					if ((*it2)->m_type == Crew_Mechanic)
					{
						health++;
					}

					for (int i = 0; i < health; i++)
					{
						if (module->m_health < module->m_health_max)
						{
							module->m_health++;
							health_repaired++;
						}
					}

					//Ends fire status
					if (module->m_fire_counter > 0)
					{
						module->m_fire_counter = 0;
						printf("Fire extinguished.\n");
					}

					//Ends shutdown status
					if (module->m_shutdown_counter > 0)
					{
						module->m_shutdown_counter = 0;
						printf("Shutdown repaired.\n");
					}
				}
			}
		}
	}

	return health_repaired;
}


void Robot::UpdateFirePropagation()
{
	for (vector<RobotSlot>::iterator it = m_slots.begin(); it != m_slots.end(); it++)
	{
		Module* module = (*it).m_module;
		if (module != NULL)
		{		
			if (module->m_fire_counter > 0)
			{
				//Damage to module
				if (module->m_health > 0)
				{
					module->m_health--;
					if (module->m_health == 0)
					{
						DestroySlot(it->m_index);
					}

					//Damage to robot
					m_health--;
					if (m_health == 0)
					{
						printf("Robot %d's health reached 0 and is destroyed. GAME OVER.\n", m_index);
					}
				}

				//Damage to crew
				for (vector<CrewMember*>::iterator it2 = (*it).m_crew.begin(); it2 != (*it).m_crew.end(); it2++)
				{
					(*it2)->m_health--;
				}
				UpdateCrew(it->m_index);
				UpdateShudownGlobal();
			}

			//Fire propagation
			if (module->m_fire_counter == 1)
			{
				for (vector<RobotSlot>::iterator it2 = m_slots.begin(); it2 != m_slots.end(); it2++)
				{
					if (it2->m_module != NULL && it2->m_module->m_fire_counter != 1)//propagation to a module that is not burning yet
					{
						//adjacent slots
						if ((abs(it2->m_coord_x - it->m_coord_x) == 1 && abs(it2->m_coord_y - it->m_coord_y) == 0)
							|| (abs(it2->m_coord_x - it->m_coord_x) == 0 && abs(it2->m_coord_y - it->m_coord_y) == 1))
						{
							bool propagation_sucess = RandomizeIntBetweenValues(1, 6) >= 4;

							if (propagation_sucess)
							{
								it2->m_module->m_fire_counter = 1;
								printf("Fire propagated from slot %d to slot %d.\n", (int)it->m_index, (int)it2->m_index);
							}
						}
					}
				}
			}
		}
	}
}

void Robot::UpdateCooldowns()
{
	for (vector<RobotSlot>::iterator it = m_slots.begin(); it != m_slots.end(); it++)
	{
		//Module cooldown
		if (it->m_module != NULL && it->m_module->m_cooldown > 0 && it->m_module->m_cooldown_timer < it->m_module->m_cooldown)
		{
			it->m_module->m_cooldown_timer++;
		}

		//Equipments cooldown
		for (vector<Equipment*>::iterator it2 = it->m_equipments.begin(); it2 != it->m_equipments.end(); it2++)
		{
			if ((*it2)->m_cooldown > 0 && (*it2)->m_cooldown_timer < (*it2)->m_cooldown)
			{
				(*it2)->m_cooldown_timer++;
			}
		}

		//Crew members stun status and captain overcharge
		for (vector<CrewMember*>::iterator it2 = it->m_crew.begin(); it2 != it->m_crew.end(); it2++)
		{
			if ((*it2)->m_stun_counter > 0)
			{
				(*it2)->m_stun_counter--;
			}

			if ((*it2)->m_type == Crew_Captain)
			{
				(*it2)->m_overcharge++;
			}
		}

		//Fire update
		if (it->m_module != NULL && it->m_module->m_fire_counter > 1)
		{
			it->m_module->m_fire_counter--;
		}

		//Resets weapon attack choice
		if (it->m_weapon)
		{
			it->m_weapon->m_attack_selected = NULL;
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
			for (vector<CrewMember*>::iterator it2 = it->m_crew.begin(); it2 != it->m_crew.end(); it2++)
			{
				if ((*it2)->m_type == Crew_Captain && (*it2)->m_health > 0 && (*it2)->m_stun_counter == 0)
				{
					return false;
				}

				if ((*it2)->m_type == Crew_Pilot && (*it2)->m_health > 0 && (*it2)->m_stun_counter == 0)
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

void Robot::DestroySlot(SlotIndex index)
{
	Module* module = m_slots[index].m_module;
	if (module != NULL)
	{
		//Loss of energy cells
		m_energy_cells -= module->m_energy_cells;
		module->m_energy_cells = 0;

		for (vector<Equipment*>::iterator it = m_slots[index].m_equipments.begin(); it != m_slots[index].m_equipments.end(); it++)
		{
			m_energy_cells -= (*it)->m_energy_cells;
			(*it)->m_energy_cells = 0;
		}
	}
}

void Robot::ShutdownSlot(SlotIndex index)
{
	Module* module = m_slots[index].m_module;
	if (module != NULL)
	{
		module->m_shutdown_counter = 2;

		//Loss of energy cells
		m_energy_cells -= module->m_energy_cells;
		module->m_energy_cells = 0;

		for (vector<Equipment*>::iterator it = m_slots[index].m_equipments.begin(); it != m_slots[index].m_equipments.end(); it++)
		{
			m_energy_cells -= (*it)->m_energy_cells;
			(*it)->m_energy_cells = 0;
		}
	}
}

bool Robot::MoveCrewMemberToSlot(CrewMember* crew, SlotIndex target_index)
{
	RobotSlot& current_slot = m_slots[crew->m_index];
	RobotSlot& target_slot = m_slots[target_index];

	int distance = abs(current_slot.m_coord_x - target_slot.m_coord_x) + abs(current_slot.m_coord_y - target_slot.m_coord_y);

	if (target_index == crew->m_index)
	{
		printf("Move cancelled: crew member is already on this position.\n");
		return false;
	}
	else if (crew->m_stun_counter > 0)
	{
		printf("Cannot move crew member because he's stunned.\n");
		return false;
	}
	else if (distance > crew->m_steps)
	{
		printf("Cannot move crew member to slot %d because the distance to cross (%d) exceeds his movement ability (%d)\n", (int)target_index, distance, crew->m_steps);
		return false;
	}

	//Movement
	vector<CrewMember*> old_crew;
	for (vector<CrewMember*>::iterator it = current_slot.m_crew.begin(); it != current_slot.m_crew.end(); it++)
	{
		old_crew.push_back(*it);
	}

	current_slot.m_crew.clear();

	for (vector<CrewMember*>::iterator it = old_crew.begin(); it != old_crew.end(); it++)
	{
		if (*it != crew)
		{
			current_slot.m_crew.push_back(*it);
		}
	}

	target_slot.m_crew.push_back(crew);
	crew->m_index = current_slot.m_index;

	printf("Crew moved from slot %d to slot %d.\n", (int)current_slot.m_index, (int)target_index);

	return true;
}


void Robot::UpdateCrew(SlotIndex index)
{
	//Remove dead crew members
	vector<CrewMember*> old_crew;
	for (vector<CrewMember*>::iterator it = m_crew.begin(); it != m_crew.end(); it++)
	{
		old_crew.push_back(*it);
	}

	m_crew.clear();
	m_slots[index].m_crew.clear();

	for (vector<CrewMember*>::iterator it = old_crew.begin(); it != old_crew.end(); it++)
	{
		if ((*it)->m_health > 0)
		{
			m_crew.push_back(*it);

			if ((*it)->m_index == index)
			{
				m_slots[index].m_crew.push_back(*it);
			}
		}
		else
		{
			printf("Crew member dead.\n");
			if ((*it)->m_type == Crew_Captain)
			{
				printf("Captain was killed. GAME OVER.\n");
			}
		}
	}
}

bool Robot::SetEnergyCell(Module* module)
{
	if (m_energy_cells_available > 0)
	{
		if (module->m_energy_cells < module->m_energy_cells_max)
		{
			module->m_energy_cells++;
			m_energy_cells_available--;
			return true;
		}
		else
		{
			printf("Cannot add Energy Cell: no slot available.\n");
		}
	}
	else
	{
		printf("No Energy Cell available.\n");
		return false;
	}
}

bool Robot::SetEnergyCell(Equipment* equipment)
{
	if (m_energy_cells_available > 0)
	{
		if (equipment->m_energy_cells < equipment->m_energy_cells_max)
		{
			equipment->m_energy_cells++;
			m_energy_cells_available--;
			return true;
		}
		else
		{
			printf("Cannot add Energy Cell: no slot available.\n");
		}
	}
	else
	{
		printf("No Energy Cell available.\n");
		return false;
	}
}

bool Robot::SetEnergyCell(WeaponAttack* attack)
{
	if (m_energy_cells_available > 0)
	{
		if (attack->m_energy_cells < attack->m_energy_cost)
		{
			attack->m_energy_cells++;
			m_energy_cells_available--;
			return true;
		}
		else
		{
			printf("Cannot add Energy Cell: no slot available.\n");
		}
	}
	else
	{
		printf("No Energy Cell available.\n");
		return false;
	}
}


bool Robot::SetWeaponAttackOnSlot(WeaponAttack* attack, SlotIndex target_index)
{
	Module* module = attack->m_owner->m_owner->m_module;

	if (attack->m_energy_cells < attack->m_energy_cost)
	{
		printf("Cannot attack: not enough Enery Cells to use this attack: (current cells: %d ; cost: %d)\n", attack->m_energy_cells, attack->m_energy_cost);
		return false;
	}
	else if (module->m_shutdown_counter > 0)
	{
		printf("Cannot attack: weapon module is shut down.\n");
		return false;
	}
	else if (module->m_health == 0)
	{
		printf("Cannot attack: weapon module is destroyed and must be repaired first in order to be used.\n");
		return false;
	}
	else if (m_unbalanced == true)
	{
		printf("Cannot attack: robot is unbalanced.\n");
		return false;
	}
	else if (attack->m_owner->m_attack_selected != NULL)
	{
		printf("Cannot attack: weapon already is attacking.\n");
		return false;
	}
	else if (attack->m_owner->m_ranged == false && (target_index == Index_FootL || target_index == Index_FootR || target_index == Index_LegL || target_index == Index_LegR))
	{
		printf("Cannot attack: lower parts (legs and feet) can't be targeted by close-combat attacks.\n");
		return false;
	}
	else if (attack->m_crew_required != NB_CREW_TYPES && attack->m_owner->m_owner->HasCrewRequired(attack->m_crew_required) == false)
	{
		printf("Cannot attack: the attack requires a specific type of crew member in the module (currently not present or stunned).\n");
		return false;
	}
	else
	{
		attack->m_owner->m_attack_selected = attack;

		ActionAttack action;
		action.m_attack = attack;
		action.m_target_index = target_index;

		(*CurrentGame).m_actions_list.push_back(action);

		return true;
	}
}