#include "InGameState.h"

extern Game* CurrentGame;

void InGameState::Initialize(Player player)
{
	this->mainWindow = player.m_playerWindow;
	(*CurrentGame).init(this->mainWindow);
	
	Ship* playerShip = new Ship(sf::Vector2f(SHIP_START_X, SHIP_START_Y), sf::Vector2f(0, 0), "2D/natalia.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32), 3);
	(*CurrentGame).m_playerShip = playerShip;
	//(*CurrentGame).addToScene((*CurrentGame).m_playerShip, PlayerShipLayer, PlayerShip);

	//Load saved file
	//if (!Ship::LoadShip(playerShip))
	//{
	//	//or create a new save file
	//	Ship::SaveShip(playerShip);
	//}

	//Loading scripts
	LoadCSVFile(SHIP_CSV_FILE);

	GameObject* background = new GameObject(sf::Vector2f(990, 540), sf::Vector2f(0, 0), "2D/background.png", sf::Vector2f(1980, 1080), sf::Vector2f(990, 540));
	(*CurrentGame).addToScene(background, BackgroundLayer, BackgroundObject);

	(*CurrentGame).m_map_size = background->m_size;
	//(*CurrentGame).m_view.setCenter((*CurrentGame).m_playerShip->getPosition());
	//(*CurrentGame).m_playerShip->SetControllerType(AllControlDevices);

	//ROBOT
	InitRobots();
}

void InGameState::InitRobots()
{
	Robot& robot = m_robots[0];

	//Robot 1
	robot.SetModule(Module_Infirmary, Index_BodyU);

	robot.SetModule(Module_Deflectors, Index_BodyM);
	robot.SetEquipment(Equipment_LightPlate, Index_BodyM);

	robot.SetModule(Module_CrewQuarter, Index_BodyD);
	robot.SetEquipment(Equipment_CQExtension, Index_BodyD);

	robot.SetModule(Module_Generator, Index_LegR);
	robot.SetEquipment(Equipment_GeneratorBooster, Index_LegR);

	robot.SetModule(Module_Gadget, Index_LegL);
	robot.SetEquipment(Equipment_GadgetEMP, Index_LegL);

	robot.SetEquipment(Equipment_LightPlate, Index_FootL);

	robot.SetEquipment(Equipment_LightPlate, Index_FootR);

	robot.SetModule(Module_Weapon, Index_ForearmL);
	robot.SetEquipment(Equipment_EnergeticWeapon, Index_ForearmL);

	robot.SetModule(Module_Weapon, Index_ForearmR);
	robot.SetEquipment(Equipment_LightPlate, Index_ForearmR);

	robot.SetModule(Module_Radar, Index_ShoulderL);
	
	robot.SetModule(Module_Weapon, Index_ShoulderR);
	robot.SetEquipment(Equipment_WeaponsScope, Index_ShoulderR);

	robot.SetCrewMember(Crew_Pilot, Index_Head);
	robot.SetCrewMember(Crew_Mechanic, Index_BodyM);
	robot.SetCrewMember(Crew_Scientist, Index_LegR);
	robot.SetCrewMember(Crew_Engineer, Index_LegL);
	robot.SetCrewMember(Crew_Warrior, Index_ForearmL);

	robot.SetWeapon(Weapon_FireSword, Index_ForearmL);

	robot.Initialize();

	robot = m_robots[1];

	//Robot 2
	robot.SetModule(Module_Generator, Index_BodyU);
	robot.SetEquipment(Equipment_GeneratorBooster, Index_BodyU);
	robot.SetEquipment(Equipment_LightPlate, Index_BodyU);

	robot.SetModule(Module_CrewQuarter, Index_BodyM);
	robot.SetEquipment(Equipment_CQExtension, Index_BodyM);
	robot.SetEquipment(Equipment_CQExtension, Index_BodyM);

	robot.SetModule(Module_Sensors, Index_BodyD);

	robot.SetModule(Module_Deflectors, Index_LegR);

	robot.SetModule(Module_Gadget, Index_LegL);
	robot.SetEquipment(Equipment_GadgetJammer, Index_LegL);

	robot.SetEquipment(Equipment_LightPlate, Index_FootL);

	robot.SetEquipment(Equipment_LightPlate, Index_FootR);

	robot.SetModule(Module_Weapon, Index_ForearmL);
	robot.SetEquipment(Equipment_HeavyPlate, Index_ForearmL);

	robot.SetModule(Module_Weapon, Index_ShoulderL);
	robot.SetEquipment(Equipment_EnergeticWeapon, Index_ShoulderL);

	robot.SetModule(Module_Weapon, Index_ShoulderR);
	robot.SetEquipment(Equipment_HeavyPlate, Index_ShoulderR);

	robot.SetModule(Module_Weapon, Index_ForearmR);
	robot.SetEquipment(Equipment_EnergeticWeapon, Index_ForearmR);

	robot.SetCrewMember(Crew_Pilot, Index_Head);
	robot.SetCrewMember(Crew_Medic, Index_BodyU);
	robot.SetCrewMember(Crew_Scientist, Index_BodyU);
	robot.SetCrewMember(Crew_Gunner, Index_ShoulderR);
	robot.SetCrewMember(Crew_Warrior, Index_ForearmR);
	robot.SetCrewMember(Crew_Mechanic, Index_BodyM);
	robot.SetCrewMember(Crew_Engineer, Index_LegL);

	robot.SetWeapon(Weapon_Gun, Index_ShoulderR);
	robot.SetWeapon(Weapon_Hammer, Index_ForearmR);

	robot.Initialize();
}

void Robot::Initialize()
{
	m_health_max = GetHealthMax();
	m_health = m_health_max;

	GetWeightModifiers(m_balance_bonus, m_attack_speed_bonus);
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


CrewMember::CrewMember(CrewType type)
{
	m_type = type;
	m_slot_index = NB_SLOT_INDEX;

	switch (type)
	{
		case Crew_Captain:
		{
			m_health_max = 8;
			m_steps = 5;
			break;
		}
		case Crew_Pilot:
		{
			m_health_max = 3;
			m_steps = 3;
			break;
		}
		case Crew_Mechanic:
		{
			m_health_max = 4;
			m_steps = 4;
			break;
		}
		case Crew_Engineer:
		{
			m_health_max = 4;
			m_steps = 3;
			break;
		}
		case Crew_Scientist:
		{
			m_health_max = 3;
			m_steps = 3;
			break;
		}
		case Crew_Warrior:
		{
			m_health_max = 4;
			m_steps = 3;
			break;
		}
		case Crew_Medic:
		{
			m_health_max = 4;
			m_steps = 4;
			break;
		}
		case Crew_Gunner:
		{
			m_health_max = 4;
			m_steps = 3;
			break;
		}
	}

	m_health = m_health_max;
}

Module::Module(ModuleType type)
{
	m_type = type;

	m_weight = 0;
	m_cells = 0;
	m_cells_max = 0;
	m_unique = false;
	m_size = 1;
	m_health_max = 0;
	m_cooldown = -1;
	m_crew_max = 0;

	switch (type)
	{
		case Module_Head:
		{
			m_health_max = 20;
			m_crew_max = 1;
			break;
		}
		case Module_Stabilizers:
		{
			m_health_max = 10;
			m_cells_max = 1;
			m_cells = 1;
			break;
		}
		case Module_Generator:
		{
			m_health_max = 10;
			m_weight = 1;
			m_unique = true;
			break;
		}
		case Module_Weapon:
		{
			m_health_max = 10;
			m_cells_max = 1;
			m_cells = 1;
			break;
		}
		case Module_Gadget:
		{
			m_health_max = 6;
			m_cells_max = 2;
			m_cells = 1;
			break;
		}
		case Module_CrewQuarter:
		{
			m_health_max = 6;
			m_weight = 1;
			m_unique = true;
			m_crew_max = 4;
			break;
		}
		case Module_Deflectors:
		{
			m_health_max = 10;
			m_weight = 1;
			m_unique = true;
			m_size = 2;
			m_cells_max = 2;
			m_cells = 1;
			break;
		}
		case Module_Infirmary:
		{
			m_health_max = 6;
			m_weight = 1;
			m_cells_max = 2;
			m_cells = 1;
			break;
		}
		case Module_Radar:
		{
			m_health_max = 6;
			m_weight = 1;
			m_unique = true;
			m_cells_max = 1;
			m_cells = 1;
			m_cooldown = 2;
			break;
		}
		case Module_Sensors:
		{
			m_health_max = 6;
			m_weight = 1;
			m_unique = true;
			m_cells_max = 1;
			m_cells = 1;
			m_cooldown = 2;
			break;
		}
	}

	m_health = m_health_max;
	m_cooldown_timer = m_cooldown;
}

Equipment::Equipment(EquipmentType type)
{
	m_type = type;

	m_weight = 0;
	m_cells = 0;
	m_cells_max = 0;
	m_unique = false;
	m_size = 1;
	m_crew_max = 0;
	m_module_equipable = NB_MODULE_TYPES;
	
	switch (type)
	{
		case Equipment_EnergeticWeapon:
		{
			m_cells_max = 1;
			m_module_equipable = Module_Weapon;
			break;
		}
		case Equipment_CQExtension:
		{
			m_weight = 1;
			m_crew_max = 2;
			m_module_equipable = Module_CrewQuarter;
			break;
		}
		case Equipment_LightPlate:
		{
			m_health_max = 6;
			m_weight = 1;
			break;
		}
		case Equipment_HeavyPlate:
		{
			m_health_max = 10;
			m_weight = 2;
			break;
		}
		case Equipment_GeneratorBooster:
		{
			m_weight = 1;
			m_cooldown = 3;
			m_module_equipable = Module_Generator;
			break;
		}
		case Equipment_GadgetJammer:
		{
			m_weight = 1;
			m_cooldown = 2;
			m_module_equipable = Module_Gadget;
			break;
		}
		case Equipment_GadgetEMP:
		{
			m_weight = 1;
			m_cooldown = 2;
			m_module_equipable = Module_Gadget;
			break;
		}
		case Equipment_WeaponsScope:
		{
			m_weight = 1;
			m_cells_max = 1;
			m_cells = 0;
			m_module_equipable = Module_Weapon;
			break;
		}
	}

	m_health = m_health_max;
	m_cooldown_timer = m_cooldown;
}

WeaponAttack::WeaponAttack()
{
	m_speed = 0;
	m_damage = 0;
	m_energy_cost = 0;
	m_chance_of_hit = 0;
	m_chance_of_fire = 0;
	m_chance_of_electricity = 0;
	m_chance_of_stun = 0;
	m_balance = 0;
	m_crew_required = NB_CREW_TYPES;
}

Weapon::Weapon(WeaponType type)
{
	m_type = type;

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
			attack1.m_balance = 3;
			attack1.m_speed = 5;
			attack1.m_energy_cost = 1;

			m_attacks.push_back(attack_secondary);
			WeaponAttack& attack2 = m_attacks.back();
			attack2.m_damage = 1;
			attack2.m_chance_of_stun = 5;
			attack2.m_balance = 1;
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

	printf("");
}

Robot::Robot()
{
	m_weight = 0;
	m_crew_nb = 0;
	m_crew_max = 0;

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

void Robot::Update()
{
	printf("");
	printf("");
}

/*
void InGameState::InitTable()
{
	(*CurrentGame).m_hovered_slot = NULL;
	(*CurrentGame).m_selected_slot = NULL;
	(*CurrentGame).m_play_card_slot = NULL;
	(*CurrentGame).m_target_slot = NULL;
	m_current_player = 0;

	//Altar
	m_altar_slot.m_stack = Stack_Altar;

	m_altar_slot.m_shape_container.setPosition(sf::Vector2f(800 + ((CARD_WIDTH + 20) * NB_CARDS_ALTAR) * 0.5, 500));
	m_altar_slot.m_shape_container.setOrigin(sf::Vector2f(((CARD_WIDTH + 20) * NB_CARDS_ALTAR + 10) * 0.5, CARD_HEIGHT / 2 + 5));
	m_altar_slot.m_shape_container.setSize(sf::Vector2f((CARD_WIDTH + 20) * NB_CARDS_ALTAR + 10, CARD_HEIGHT + 10));
	m_altar_slot.m_shape_container.setOutlineColor(sf::Color(255, 255, 255, 255));
	m_altar_slot.m_shape_container.setOutlineThickness(2);
	m_altar_slot.m_shape_container.setFillColor(sf::Color(0, 0, 0, 0));

	m_altar_slot.m_shape.setPosition(sf::Vector2f(800 + ((CARD_WIDTH + 20) * NB_CARDS_ALTAR) * 0.5, 500));
	m_altar_slot.m_shape.setOrigin(sf::Vector2f(((CARD_WIDTH + 20) * NB_CARDS_ALTAR + 10) * 0.5, CARD_HEIGHT / 2 + 5));
	m_altar_slot.m_shape.setSize(sf::Vector2f((CARD_WIDTH + 20) * NB_CARDS_ALTAR + 10, CARD_HEIGHT + 10));
	m_altar_slot.m_shape.setOutlineColor(sf::Color(0, 0, 0, 0));
	m_altar_slot.m_shape.setOutlineThickness(0);
	m_altar_slot.m_shape.setFillColor(sf::Color(255, 255, 255, 0));

	for (int i = 0; i < NB_CARDS_ALTAR; i++)
	{
		m_altar_slots[i].m_status = CardSlot_Free;
		m_altar_slots[i].m_stack = Stack_AltarSlot;
		m_altar_slots[i].m_index = i;

		m_altar_slots[i].m_shape_container.setPosition(sf::Vector2f(800 + CARD_WIDTH / 2 + (CARD_WIDTH + 20) * i, 500));
		m_altar_slots[i].m_shape_container.setOrigin(sf::Vector2f(CARD_WIDTH / 2, CARD_HEIGHT / 2));
		m_altar_slots[i].m_shape_container.setSize(sf::Vector2f(CARD_WIDTH, CARD_HEIGHT));
		m_altar_slots[i].m_shape_container.setOutlineColor(sf::Color(255, 255, 255, 255));
		m_altar_slots[i].m_shape_container.setOutlineThickness(2);
		m_altar_slots[i].m_shape_container.setFillColor(sf::Color(0, 0, 0, 0));

		m_altar_slots[i].m_shape.setPosition(sf::Vector2f(800 + CARD_WIDTH / 2 + (CARD_WIDTH + 20) * i, 500));
		m_altar_slots[i].m_shape.setOrigin(sf::Vector2f(CARD_WIDTH / 2, CARD_HEIGHT / 2));
		m_altar_slots[i].m_shape.setSize(sf::Vector2f(CARD_WIDTH, CARD_HEIGHT));
		m_altar_slots[i].m_shape.setOutlineColor(sf::Color(0, 0, 0, 0));
		m_altar_slots[i].m_shape.setOutlineThickness(0);
		m_altar_slots[i].m_shape.setFillColor(sf::Color(255, 255, 255, 0));

		m_altar_slots[i].m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_altar_slots[i].m_text.setCharacterSize(18);
		m_altar_slots[i].m_text.setColor(sf::Color(0, 0, 0, 255));
		m_altar_slots[i].m_text.setPosition(sf::Vector2f(800 + CARD_WIDTH / 2 + (CARD_WIDTH + 20) * i, 500));
		m_altar_slots[i].m_text.setString("");
	}
}
*/
void InGameState::Update(sf::Time deltaTime)
{
	//Get mouse inputs
	sf::Vector2i mousepos2i = sf::Mouse::getPosition(*(*CurrentGame).getMainWindow());
	(*CurrentGame).m_mouse_pos = (*CurrentGame).getMainWindow()->mapPixelToCoords(mousepos2i, (*CurrentGame).m_view);

	if ((*CurrentGame).m_mouse_click_timer > 0)
	{
		(*CurrentGame).m_mouse_click_timer -= deltaTime.asSeconds();
	}

	(*CurrentGame).m_mouse_click = Mouse_None;
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && (*CurrentGame).m_window_has_focus && (*CurrentGame).m_mouse_click_timer <= 0)
	{
		(*CurrentGame).m_mouse_click = Mouse_LeftClick;
		(*CurrentGame).m_mouse_click_timer = 0.2f;
	}
	else if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && (*CurrentGame).m_window_has_focus && (*CurrentGame).m_mouse_click_timer <= 0)
	{
		(*CurrentGame).m_mouse_click = Mouse_RightClick;
		(*CurrentGame).m_mouse_click_timer = 0.2f;
	}

	//Reset flags
	(*CurrentGame).m_hovered_slot = NULL;
	(*CurrentGame).m_play_card_slot = NULL;
	(*CurrentGame).m_target_slot = NULL;

	if ((*CurrentGame).m_mouse_click == Mouse_LeftClick)
	{
		(*CurrentGame).m_selected_slot = NULL;
	}

	//ROBOT
	//m_robots[0].Update();
}

void InGameState::Draw()
{
	(*CurrentGame).m_mainScreen.clear();

	//Draw
	(*CurrentGame).drawScene();//background

	//ROBOT

	//Display
	(*CurrentGame).m_mainScreen.display();
	sf::Sprite temp((*CurrentGame).m_mainScreen.getTexture());
	temp.scale((*CurrentGame).m_scale_factor.x, (*CurrentGame).m_scale_factor.y);
	temp.setPosition(sf::Vector2f(0, 0));
	(*CurrentGame).getMainWindow()->draw(temp);

	
}

void InGameState::Release()
{
	//TODO
}

void InGameState::UpdateCamera(sf::Time deltaTime)
{
	(*CurrentGame).m_view.move(sf::Vector2f((*CurrentGame).m_playerShip->m_speed.x * deltaTime.asSeconds(), (*CurrentGame).m_playerShip->m_speed.y * deltaTime.asSeconds()));

	//Map border constraints
	const float x = (*CurrentGame).m_view.getSize().x / 2;
	const float y = (*CurrentGame).m_view.getSize().y / 2;
	const float a = (*CurrentGame).m_playerShip->getPosition().x;
	const float b = (*CurrentGame).m_playerShip->getPosition().y;
	if (a < x)
		(*CurrentGame).m_view.setCenter(x, (*CurrentGame).m_view.getCenter().y);
	if (a >(*CurrentGame).m_map_size.x - x)
		(*CurrentGame).m_view.setCenter((*CurrentGame).m_map_size.x - x, (*CurrentGame).m_view.getCenter().y);
	if (b < y)
		(*CurrentGame).m_view.setCenter((*CurrentGame).m_view.getCenter().x, y);
	if (b >(*CurrentGame).m_map_size.y - y)
		(*CurrentGame).m_view.setCenter((*CurrentGame).m_view.getCenter().x, (*CurrentGame).m_map_size.y - y);
}

void InGameState::DestroySFPanel(Ship* playerShip)
{
	if (playerShip->m_SFTargetPanel)
	{
		(*CurrentGame).removeFromFeedbacks(playerShip->m_SFTargetPanel);
		delete playerShip->m_SFTargetPanel;
		playerShip->m_SFTargetPanel = NULL;
	}
}

void InGameState::CreateSFPanel(SFPanelTypes panel_type, Ship* playerShip)
{
	switch (panel_type)
	{
		case SFPanel_Specific:
		{
			playerShip->m_SFTargetPanel = new SFPanelSpecific(sf::Vector2f(SFPANEL_SPECIFIC_WIDTH, SFPANEL_SPECIFIC_HEIGHT), SFPanel_Specific, playerShip);
			break;
		}
	}
	(*CurrentGame).addToFeedbacks((*CurrentGame).m_playerShip->m_SFTargetPanel);
}

void InGameState::LoadCSVFile(string scenes_file)
{
	LOGGER_WRITE(Logger::DEBUG, "Loading scripts.");

	vector<vector<string> > allConfigs = *(FileLoaderUtils::FileLoader(scenes_file));
	size_t allConfigVectorSize = allConfigs.size();
	for (size_t i = 0; i < allConfigVectorSize; i++)
	{
		(*CurrentGame).m_gameObjectsConfig.insert(std::map<string, vector<string> >::value_type(allConfigs[i][0], allConfigs[i]));
	}

	allConfigs.clear();
}

//ROBOT
/*
void Mage::InitSlots(int player_index)
{
	//Hand
	for (int i = 0; i < NB_CARDS_HAND_MAX; i++)
	{
		m_hand_slots[i].m_status = CardSlot_Free;
		m_hand_slots[i].m_stack = Stack_Hand;
		m_hand_slots[i].m_index = i;

		m_hand_slots[i].m_shape_container.setPosition(sf::Vector2f(200 + CARD_WIDTH / 2 + (CARD_WIDTH + 20) * i + player_index * 700, 900));
		m_hand_slots[i].m_shape_container.setOrigin(sf::Vector2f(CARD_WIDTH / 2, CARD_HEIGHT / 2));
		m_hand_slots[i].m_shape_container.setSize(sf::Vector2f(CARD_WIDTH, CARD_HEIGHT));
		m_hand_slots[i].m_shape_container.setOutlineColor(sf::Color(255, 255, 255, 255));
		m_hand_slots[i].m_shape_container.setOutlineThickness(2);
		m_hand_slots[i].m_shape_container.setFillColor(sf::Color(0, 0, 0, 0));

		m_hand_slots[i].m_shape.setPosition(sf::Vector2f(200 + CARD_WIDTH / 2 + (CARD_WIDTH + 20) * i + player_index * 700, 900));
		m_hand_slots[i].m_shape.setOrigin(sf::Vector2f(CARD_WIDTH / 2, CARD_HEIGHT / 2));
		m_hand_slots[i].m_shape.setSize(sf::Vector2f(CARD_WIDTH, CARD_HEIGHT));
		m_hand_slots[i].m_shape.setOutlineColor(sf::Color(0, 0, 0, 0));
		m_hand_slots[i].m_shape.setOutlineThickness(0);
		m_hand_slots[i].m_shape.setFillColor(sf::Color(255, 255, 255, 0));

		m_hand_slots[i].m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_hand_slots[i].m_text.setCharacterSize(18);
		m_hand_slots[i].m_text.setColor(sf::Color(0, 0, 0, 255));
		m_hand_slots[i].m_text.setPosition(sf::Vector2f(200 + CARD_WIDTH / 2 + (CARD_WIDTH + 20) * i + player_index * 700, 900));
		m_hand_slots[i].m_text.setString("");
	}

void CardSlot::Update(MouseAction mouse_click)
{
	if ((*CurrentGame).m_hovered_slot == NULL &&
		(*CurrentGame).m_mouse_pos.x > m_shape_container.getPosition().x - m_shape_container.getSize().x / 2 && (*CurrentGame).m_mouse_pos.x < m_shape_container.getPosition().x + m_shape_container.getSize().x / 2
		&& (*CurrentGame).m_mouse_pos.y > m_shape_container.getPosition().y - m_shape_container.getSize().y / 2 && (*CurrentGame).m_mouse_pos.y < m_shape_container.getPosition().y + m_shape_container.getSize().y / 2)
	{
		m_hovered = true;
		(*CurrentGame).m_hovered_slot = this;
	}
	else
	{
		m_hovered = false;
	}

	if (mouse_click == Mouse_LeftClick)
	{
		if (m_hovered)
		{
			m_selected = true;
			(*CurrentGame).m_selected_slot = this;
		}
		else
		{
			m_selected = false;
		}
	}

	m_shape_container.setOutlineColor(sf::Color(255, 255, 255, 255));
	if (m_hovered)
	{
		m_shape_container.setOutlineColor(sf::Color(255, 0, 0, 255));
	}
	if (m_selected)
	{
		m_shape_container.setOutlineColor(sf::Color(0, 255, 0, 255));
	}

	//Actions
	//PLAY CARD
	if (m_hovered && mouse_click == Mouse_RightClick && (*CurrentGame).m_selected_slot && (*CurrentGame).m_selected_slot->m_stack == Stack_Hand && (*CurrentGame).m_selected_slot->m_status == CardSlot_Occupied && m_stack == Stack_AltarSlot)
	{
		(*CurrentGame).m_play_card_slot = (*CurrentGame).m_selected_slot;
		(*CurrentGame).m_target_slot = this;
	}
	else if (m_hovered && mouse_click == Mouse_RightClick && (*CurrentGame).m_selected_slot && (*CurrentGame).m_selected_slot->m_stack == Stack_AltarSlot && m_stack == Stack_MonsterCurses)
	{
		(*CurrentGame).m_target_slot = this;
	}
	else if (m_hovered && mouse_click == Mouse_RightClick && (*CurrentGame).m_selected_slot && (*CurrentGame).m_selected_slot->m_stack == Stack_Altar && m_stack == Stack_MonsterCurses)
	{
		(*CurrentGame).m_target_slot = this;
	}
	else if (m_hovered && mouse_click == Mouse_RightClick && m_stack == Stack_MonsterCosts)
	{
		(*CurrentGame).m_target_slot = this;
	}
	else if (m_hovered && mouse_click == Mouse_RightClick && (*CurrentGame).m_selected_slot && (*CurrentGame).m_selected_slot->m_stack == Stack_AltarSlot && m_stack == Stack_Blessings)
	{
		(*CurrentGame).m_target_slot = this;
	}
	else if (m_hovered && mouse_click == Mouse_RightClick && (*CurrentGame).m_selected_slot && (*CurrentGame).m_selected_slot->m_stack == Stack_Altar && m_stack == Stack_Blessings)
	{
		(*CurrentGame).m_target_slot = this;
	}
	else if (m_hovered && mouse_click == Mouse_RightClick && m_stack == Stack_Library)
	{
		(*CurrentGame).m_target_slot = this;
		m_shape_container.setOutlineColor(sf::Color(255, 255, 255, 255));
	}
	else if (m_hovered && mouse_click == Mouse_RightClick && m_stack == Stack_EndOfTurn)
	{
		(*CurrentGame).m_target_slot = this;
	}
}

*/