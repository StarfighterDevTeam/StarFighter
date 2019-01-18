#include "Robot.h"

extern Game* CurrentGame;

Robot::Robot()
{
	m_weight = 0;
	m_crew_max = 0;
	m_energy_cells = 0;
	m_energy_cells_available = 0;
	m_unbalanced_counter = 0;
	m_unbalanced_value = 0;
	m_shutdown_global = false;
	m_grounded = false;
	m_grabbed = NULL;
	m_guard_speed = 0;
	m_jammer_bonus = 0;

	m_ready_to_change_phase = false;

	//Robot slots
	for (int i = 0; i < NB_SLOT_INDEX; i++)
	{
		m_slots.push_back(RobotSlot((SlotIndex)i, this));
	}

	//Mandatory slots
	SetModule(Module_Head, Index_Head);
	SetModule(Module_Stabilizers, Index_FootL);
	SetModule(Module_Stabilizers, Index_FootR);

	//Map the distance between slots
	int robot_map[NB_SLOT_INDEX][NB_SLOT_INDEX] = {
		{ 0, 4, 4, 5, 5, 2, 2, 3, 3, 1, 2, 3 },
		{ 4, 0, 2, 1, 3, 4, 4, 5, 5, 3, 2, 1 },
		{ 4, 2, 0, 3, 1, 4, 4, 5, 5, 3, 2, 1 },
		{ 5, 1, 3, 0, 4, 5, 5, 6, 6, 4, 3, 2 },
		{ 5, 3, 1, 4, 0, 5, 5, 6, 6, 4, 3, 2 },
		{ 2, 4, 4, 5, 5, 0, 2, 1, 3, 1, 2, 3 },
		{ 2, 4, 4, 5, 5, 2, 0, 3, 1, 1, 2, 3 },
		{ 3, 5, 5, 6, 6, 1, 3, 0, 4, 2, 3, 4 },
		{ 3, 5, 5, 6, 6, 3, 1, 4, 0, 2, 3, 4 },
		{ 1, 3, 3, 4, 4, 1, 1, 2, 2, 0, 1, 2 },
		{ 2, 2, 2, 3, 3, 2, 2, 3, 3, 1, 0, 1 },
		{ 3, 1, 1, 2, 2, 3, 3, 4, 4, 2, 1, 0 } };
	for (int i = 0; i < NB_SLOT_INDEX; i++)
	{
		for (int j = 0; j < NB_SLOT_INDEX; j++)
		{
			m_robot_map[i][j] = robot_map[i][j];
		}
	}
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

	delete m_grab_slot->m_weapon;
	delete m_guard_slot->m_weapon;
	delete m_grab_slot;
	delete m_guard_slot;
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
	else if (module->m_type == Module_Weapon && m_slots[index].m_type != Slot_Hand && m_slots[index].m_type != Slot_Shoulder)
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
	else if (weapon->m_ranged == true && m_slots[index].m_type != Slot_Shoulder && m_slots[index].m_type != Slot_Hand)
	{
		printf("Cannot equip a ranged weapon on slot %d because it must be equipped on Shoulders or Hands.\n", (int)index);
		delete weapon;
		weapon = NULL;
		return false;
	}
	else if (weapon->m_ranged == false && m_slots[index].m_type != Slot_Hand)
	{
		printf("Cannot equip a close-combat weapon on slot %d because it must be equipped on Hands.\n", (int)index);
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
		}
	}

	health_max /= 2;

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

	//Copy the original list of crew members
	for (vector<CrewMember*>::iterator it = m_crew.begin(); it != m_crew.end(); it++)
	{
		m_crew_start.push_back(*it);
	}

	//Grab and guard
	m_grab_slot = new RobotSlot(Index_HandL, this);
	m_guard_slot = new RobotSlot(Index_HandL, this);
	Weapon* grab_weapon = new Weapon(Weapon_Grab, m_grab_slot);
	Weapon* guard_weapon = new Weapon(Weapon_Guard, m_guard_slot);
	m_grab_slot->m_weapon = grab_weapon;
	m_guard_slot->m_weapon = guard_weapon;

	m_has_attacked = false;
	m_has_attacked_RightHand = false;
	m_has_attacked_LeftHand = false;
	m_has_guarded = false;
	m_has_grabbed_LeftHand = false;
	m_has_grabbed_RightHand = false;

	//UI
	InitializeUI();
}


//Slots - distance from the faremost left part (left hand) and the faremost upper part (head) of the robot in pixels
float slot_coord[NB_SLOT_INDEX][2] = {
	{ 225.f, 0.f },		//Index_Head
	{ 100.f, 450.f },	//Index_LegL
	{ 350.f, 450.f },	//Index_LegR
	{ 70.f, 600.f },		//Index_FootL
	{ 380.f, 600.f },	//Index_FootR
	{ 40.f, 75.f },		//Index_ShoulderL
	{ 410.f, 75.f },		//Index_ShoulderR
	{ 0.f, 280.f },		//Index_HandL
	{ 450.f, 280.f },	//Index_HandR
	{ 225.f, 100.f },	//Index_BodyU
	{ 225.f, 200.f },	//Index_BodyM
	{ 225.f, 300.f } };	//Index_BodyD

int slot_size[NB_SLOT_INDEX][2] = {
	{ 1, 1 },		//Index_Head
	{ 1, 2 },		//Index_LegL
	{ 1, 2 },		//Index_LegR
	{ 2, 1 },		//Index_FootL
	{ 2, 1 },		//Index_FootR
	{ 1, 2 },		//Index_ShoulderL
	{ 1, 2 },		//Index_ShoulderR
	{ 1, 2 },		//Index_HandL
	{ 1, 2 },		//Index_HandR
	{ 3, 1 },		//Index_BodyU
	{ 3, 1 },		//Index_BodyM
	{ 2, 1 } };		//Index_BodyD

#define CREW_SIZE_X		100.f
#define CREW_SIZE_Y		80.f
#define CREWSML_SIZE_X	25.f
#define CREWSML_SIZE_Y	20.f
#define MODULE_SIZE_X	80.f
#define MODULE_SIZE_Y	80.f
#define EC_SIZE_X		16.f
#define EC_SIZE_Y		16.f

void Robot::InitializeUI()
{
	TextureLoader *loader;
	loader = TextureLoader::getInstance();

	//Slots
	float robot_offset = m_index == 0 ? 0.f : 970;//1920 - 250*2 - 450
	float offset_x = 250.f + robot_offset;
	float offset_y = 215.f;
	float size_x = MODULE_SIZE_X;
	float size_y = MODULE_SIZE_Y;

	for (int i = 0; i < NB_SLOT_INDEX; i++)
	{
		UI_Element ui(&m_slots[i]);

		ui.m_type = UI_Slot;
		ui.m_team = (TeamAlliances)m_index;//AllianceNeutral;

		ui.m_shape_container.setPosition(sf::Vector2f(offset_x + slot_coord[i][0], offset_y + slot_coord[i][1]));
		ui.m_shape_container.setSize(sf::Vector2f(size_x * slot_size[i][0], size_y * slot_size[i][1]));
		ui.m_shape_container.setOrigin(sf::Vector2f(size_x * 0.5f * (float)slot_size[i][0], size_y * 0.5f * (float)slot_size[i][1]));
		ui.m_shape_container.setOutlineColor(sf::Color(255, 255, 255, 255));
		ui.m_shape_container.setOutlineThickness(2);
		if (m_slots[i].m_module != NULL)
		{
			ui.m_shape_container.setFillColor(sf::Color(0, 0, 0, 255));
		}
		else
		{
			ui.m_shape_container.setFillColor(sf::Color(128, 128, 128, 255));
		}
		
		ui.m_shape.setPosition(sf::Vector2f(offset_x + slot_coord[i][0], offset_y + slot_coord[i][1]));
		ui.m_shape.setSize(sf::Vector2f(size_x * slot_size[i][0], size_y * slot_size[i][1]));
		ui.m_shape.setOrigin(sf::Vector2f(size_x * 0.5f * (float)slot_size[i][0], size_y * 0.5f * (float)slot_size[i][1]));
		ui.m_shape.setOutlineColor(sf::Color(0, 0, 0, 0));
		ui.m_shape.setOutlineThickness(0);
		ui.m_shape.setFillColor(sf::Color(0, 0, 0, 0));

		sf::Texture* texture_target = loader->loadTexture("2D/ui_target.png", 80, 80);
		ui.m_shape.setTexture(texture_target);

		m_UI_slots.push_back(ui);

		//Modules
		for (int j = 0; j < m_slots[i].m_size; j++)
		{
			bool is_equipment = false;
			GameEntity* entity = NULL;
			if (j == 0)
			{
				entity = (GameEntity*)m_slots[i].m_module;
			}
			else if (j == 1)
			{
				if (m_slots[i].m_module != NULL && m_slots[i].m_module->m_size == 2)
				{
					entity = (GameEntity*)m_slots[i].m_module;
				}
				else if (m_slots[i].m_equipments.size() > 0)
				{
					entity = (GameEntity*)m_slots[i].m_equipments[0];
					is_equipment = true;
				}
			}
			else if (j == 2)
			{
				if (m_slots[i].m_module != NULL && m_slots[i].m_module->m_size == 2 && m_slots[i].m_equipments.size() > 0)
				{
					entity = (GameEntity*)m_slots[i].m_equipments[0];
					is_equipment = true;
				}
				else if (m_slots[i].m_equipments.size() > 1)
				{
					entity = (GameEntity*)m_slots[i].m_equipments[1];
					is_equipment = true;
				}
			}
			
			vector<UI_Element> ui_module;
			UI_Element ui2(entity);

			ui2.m_type = is_equipment == true ? UI_Equipment : UI_Module;
			ui2.m_team = (TeamAlliances)m_index;

			//quick hardcoding, known that vertical slots have max 2 modules and horizontal slots have max 3 modules
			bool vertical = slot_size[i][1] > slot_size[i][0];
			bool odd_size = m_slots[i].m_size % 2 == 1;
			float offset_module_x = 0;
			float offset_module_y = 0;
			if (vertical == true)
			{
				offset_module_y = j == 0 ? -size_y * 0.5 : size_y * 0.5f;
			}
			else
			{
				if (odd_size == false)
				{
					offset_module_x = j == 0 ? -size_x * 0.5 : size_x * 0.5f;
				}
				else if (m_slots[i].m_size > 1)
				{
					if (j == 0)
					{
						offset_module_x = -size_x;
					}
					else if (j == 2)
					{
						offset_module_x = size_x;
					}
				}
			}

			ui2.m_shape_container.setPosition(sf::Vector2f(offset_x + slot_coord[i][0] + offset_module_x, offset_y + slot_coord[i][1] + offset_module_y));
			ui2.m_shape_container.setSize(sf::Vector2f(size_x, size_y));
			ui2.m_shape_container.setOrigin(sf::Vector2f(size_x * 0.5f, size_y * 0.5f));
			ui2.m_shape_container.setOutlineColor(sf::Color(255, 255, 255, 255));
			ui2.m_shape_container.setOutlineThickness(2);
			ui2.m_shape_container.setFillColor(sf::Color(0, 0, 0, 255));
			if (entity != NULL)
			{
				if (is_equipment == false)
				{
					ui2.m_shape_container.setFillColor(sf::Color(0, 132, 232, 255));//blue module
				}
				else
				{
					ui2.m_shape_container.setFillColor(sf::Color(153, 217, 234, 255));//lighter blue
				}
			}
			else
			{
				//empty slots' module & equipement greyed out
				if (j == 0 || m_UI_modules.back().front().m_shape_container.getFillColor() == sf::Color(128, 128, 128, 255))
				{
					ui2.m_shape_container.setFillColor(sf::Color(128, 128, 128, 255));
				}
				else
				{
					ui2.m_shape_container.setFillColor(sf::Color(0, 0, 0, 255));
				}
			}

			ui2.m_shape.setPosition(sf::Vector2f(offset_x + slot_coord[i][0] + offset_module_x, offset_y + slot_coord[i][1] + offset_module_y));
			ui2.m_shape.setSize(sf::Vector2f(size_x, size_y));
			ui2.m_shape.setOrigin(sf::Vector2f(size_x * 0.5f, size_y * 0.5f));
			ui2.m_shape.setOutlineColor(sf::Color(0, 0, 0, 0));
			ui2.m_shape.setOutlineThickness(0);
			ui2.m_shape.setFillColor(sf::Color(0, 0, 0, 0));

			int energy_cells_max = 0;
			UI_Type ec_slot_type;
			if (entity != NULL)
			{
				if (is_equipment == false)
				{
					Module* module = (Module*)entity;
					Weapon* weapon = module->m_owner->m_weapon;
					if (weapon == NULL)
					{
						//Non-weapon module
						ui2.m_text.setString(module->m_UI_display_name_short);

						if (j == 0)
						{
							energy_cells_max = module->m_energy_cells_max;
							ec_slot_type = UI_EC_Slot_Module;
						}
					}
					else
					{
						//Weapon
						ui2.m_text.setString(weapon->m_UI_display_name_short);

						if (j == 0)
						{
							energy_cells_max = module->m_energy_cells_max;
							ec_slot_type = UI_EC_Slot_Module;
						}
					}
				}
				else
				{
					//Equipment
					Equipment* equipment = (Equipment*)entity;
					ui2.m_text.setString(equipment->m_UI_display_name_short);
					energy_cells_max = equipment->m_energy_cells_max;
					ec_slot_type = UI_EC_Slot_Equipment;
				}

				ui2.m_text.setPosition(sf::Vector2f(offset_x + slot_coord[i][0] + offset_module_x - MODULE_SIZE_X * 0.5f + 2.f, offset_y + slot_coord[i][1] + offset_module_y - MODULE_SIZE_Y * 0.5f + 2.f));
				ui2.m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
				ui2.m_text.setCharacterSize(14);
				ui2.m_text.setColor(sf::Color(0, 0, 0, 255));
			}
			
			ui_module.push_back(ui2);

			//EC slots
			for (int e = 0; e < energy_cells_max; e++)
			{
				UI_Element ui_ec(entity);

				ui_ec.m_type = ec_slot_type;
				ui_ec.m_team = (TeamAlliances)m_index;

				float offset_ec_x = offset_x + slot_coord[i][0] + offset_module_x - 30.f + (e * (EC_SIZE_X + 8.f));
				float offset_ec_y = offset_y + slot_coord[i][1] + offset_module_y + 30.f;
				ui_ec.m_shape_container.setPosition(sf::Vector2f(offset_ec_x, offset_ec_y));
				ui_ec.m_shape_container.setSize(sf::Vector2f(EC_SIZE_X, EC_SIZE_Y));
				ui_ec.m_shape_container.setOrigin(sf::Vector2f(EC_SIZE_X * 0.5f, EC_SIZE_Y * 0.5f));
				ui_ec.m_shape_container.setOutlineColor(sf::Color(255, 255, 255, 255));
				ui_ec.m_shape_container.setOutlineThickness(2);
				ui_ec.m_shape_container.setFillColor(sf::Color(0, 0, 0, 255));

				ui_module.push_back(ui_ec);
			}

			m_UI_modules.push_back(ui_module);
		}
	}

	//Crew members
	int c = 0;
	for (vector<CrewMember*>::iterator it = m_crew_start.begin(); it != m_crew_start.end(); it++)
	{
		//selection tab
		UI_Element ui(*it);

		ui.m_type = UI_Crew;
		ui.m_team = (TeamAlliances)m_index;

		float size_x = CREW_SIZE_X;
		float size_y = CREW_SIZE_Y;
		float offset_x = m_index == 0 ? 135.f : 1920.f - 135.f;
		float offset_y = 180.f;

		ui.m_shape_container.setPosition(sf::Vector2f(offset_x, offset_y + size_y * (float)c));
		ui.m_shape_container.setSize(sf::Vector2f(size_x, size_y));
		ui.m_shape_container.setOrigin(sf::Vector2f(size_x * 0.5f, size_y * 0.5));
		ui.m_shape_container.setOutlineColor(sf::Color(0, 0, 0, 255));
		ui.m_shape_container.setOutlineThickness(2);
		ui.m_shape_container.setFillColor(sf::Color(0, 0, 0, 0));

		ui.m_shape.setPosition(sf::Vector2f(offset_x, offset_y + size_y * (float)c));
		ui.m_shape.setSize(sf::Vector2f(size_x, size_y));
		ui.m_shape.setOrigin(sf::Vector2f(size_x * 0.5f, size_y * 0.5));
		ui.m_shape.setOutlineColor(sf::Color(0, 0, 0, 0));
		ui.m_shape.setOutlineThickness(0);
		ui.m_shape.setFillColor(sf::Color(255, 255, 255, 255));

		string textureName;
		switch ((*it)->m_type)
		{
			case Crew_Captain:
			{
				textureName = "2D/crew_captain.png";
				break;
			}
			case Crew_Scientist:
			{
				textureName = "2D/crew_scientist.png";
				break;
			}
			case Crew_Mechanic:
			{
				textureName = "2D/crew_mechanic.png";
				break;
			}
			case Crew_Pilot:
			{
				textureName = "2D/crew_pilot.png";
				break;
			}
			case Crew_Engineer:
			{
				textureName = "2D/crew_engineer.png";
				break;
			}
			case Crew_Warrior:
			{
				textureName = "2D/crew_warrior.png";
				break;
			}
			case Crew_Medic:
			{
				textureName = "2D/crew_medic.png";
				break;
			}
			case Crew_Gunner:
			{
				textureName = "2D/crew_gunner.png";
				break;
			}
		}

		sf::Texture* texture = loader->loadTexture(textureName, size_x, size_y);
		ui.m_shape.setTexture(texture);

		//text
		offset_x -= m_index == 0 ? CREW_SIZE_X + 28.f : - 28.f * 2.f ;
		offset_y += 12.f;

		ui.m_text.setPosition(sf::Vector2f(offset_x, offset_y + size_y * (float)c));
		ui.m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
		ui.m_text.setCharacterSize(16);
		ui.m_text.setColor(sf::Color(255, 255, 255, 255));
		ui.m_text.setString("\n\n\n");

		ui.m_text.setPosition(sf::Vector2f(ui.m_text.getPosition().x, ui.m_text.getPosition().y - ui.m_text.getGlobalBounds().height));

		m_UI_crew.push_back(ui);
		c++;

		//crew on board
		UI_Element ui2(*it);

		ui2.m_type = UI_Crew;
		ui2.m_team = (TeamAlliances)m_index;

		float sizesml_x = CREWSML_SIZE_X;
		float sizesml_y = CREWSML_SIZE_Y;

		float pos_x = m_UI_slots[(*it)->m_index].m_shape.getPosition().x;
		float pos_y = m_UI_slots[(*it)->m_index].m_shape.getPosition().y + 8.f;

		ui2.m_shape_container.setPosition(sf::Vector2f(pos_x, pos_y));
		ui2.m_shape_container.setSize(sf::Vector2f(sizesml_x, sizesml_y));
		ui2.m_shape_container.setOrigin(sf::Vector2f(sizesml_x * 0.5f, sizesml_y * 0.5));
		ui2.m_shape_container.setOutlineColor(sf::Color(0, 0, 0, 255));
		ui2.m_shape_container.setOutlineThickness(2);
		ui2.m_shape_container.setFillColor(sf::Color(0, 0, 0, 0));

		ui2.m_shape.setPosition(sf::Vector2f(pos_x, pos_y));
		ui2.m_shape.setSize(sf::Vector2f(sizesml_x, sizesml_y));
		ui2.m_shape.setOrigin(sf::Vector2f(sizesml_x * 0.5f, sizesml_y * 0.5));
		ui2.m_shape.setOutlineColor(sf::Color(0, 0, 0, 0));
		ui2.m_shape.setOutlineThickness(0);
		ui2.m_shape.setFillColor(sf::Color(255, 255, 255, 255));

		string textureName_sml;
		textureName_sml = ReplaceAll(textureName, ".png", "_sml.png");

		sf::Texture* texture_sml = loader->loadTexture(textureName_sml, size_x, size_y);
		ui2.m_shape.setTexture(texture_sml);

		m_UI_crew_sml.push_back(ui2);
	}

	//"End turn" button
	UI_Element ui(this);

	ui.m_type = UI_EndTurn;
	ui.m_team = (TeamAlliances)m_index;

	float end_of_turn_offset = m_index == 0 ? 0.f : 200.f;
	float offset_eot_x = 150.f + robot_offset + end_of_turn_offset;
	float offset_eot_y = 80.f;
	float sizeeot_x = 250;
	float sizeeot_y = 50;

	ui.m_shape_container.setPosition(sf::Vector2f(slot_coord[0][0] + offset_eot_x, slot_coord[0][1] + offset_eot_y));
	ui.m_shape_container.setSize(sf::Vector2f(sizeeot_x, sizeeot_y));
	ui.m_shape_container.setOrigin(sf::Vector2f(sizeeot_x * 0.5f, sizeeot_y * 0.5));
	ui.m_shape_container.setOutlineColor(sf::Color(0, 0, 0, 255));
	ui.m_shape_container.setOutlineThickness(2);
	ui.m_shape_container.setFillColor(sf::Color(0, 0, 0, 0));

	ui.m_shape.setPosition(sf::Vector2f(slot_coord[0][0] + offset_eot_x, slot_coord[0][1] + offset_eot_y));
	ui.m_shape.setSize(sf::Vector2f(sizeeot_x, sizeeot_y));
	ui.m_shape.setOrigin(sf::Vector2f(sizeeot_x * 0.5f, sizeeot_y * 0.5));
	ui.m_shape.setOutlineColor(sf::Color(0, 0, 0, 0));
	ui.m_shape.setOutlineThickness(0);
	ui.m_shape.setFillColor(sf::Color(255, 255, 255, 255));

	ui.m_text.setPosition(sf::Vector2f(slot_coord[0][0] + offset_eot_x, slot_coord[0][1] + offset_eot_y));
	ui.m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	ui.m_text.setCharacterSize(26);
	ui.m_text.setColor(sf::Color(0, 0, 0, 255));
	ui.m_text.setString("END OF TURN");

	float global_bounds_x = ui.m_text.getGlobalBounds().width;
	float global_bounds_y = ui.m_text.getCharacterSize();//ui.m_text.getGlobalBounds().height;
	ui.m_text.setPosition(sf::Vector2f(ui.m_text.getPosition().x - global_bounds_x * 0.5f, ui.m_text.getPosition().y - global_bounds_y * 0.6f));

	m_UI_buttons.push_back(ui);

	//"Grab" button
	for (int g = 0; g < 2; g++)
	{
		UI_Element ui_grab(this);
		ui_grab.m_team = (TeamAlliances)m_index;

		ui_grab.m_type = g == 0 ? UI_GrabL : UI_GrabR;

		float offset_grab_x = g == 0 ? 180.f + robot_offset + end_of_turn_offset : 570.f + robot_offset + end_of_turn_offset;
		float size_grab_x = 100.f;

		ui_grab.m_shape_container.setPosition(sf::Vector2f(offset_grab_x, slot_coord[0][1] + offset_eot_y));
		ui_grab.m_shape_container.setSize(sf::Vector2f(size_grab_x, sizeeot_y));
		ui_grab.m_shape_container.setOrigin(sf::Vector2f(size_grab_x * 0.5f, sizeeot_y * 0.5));
		ui_grab.m_shape_container.setOutlineColor(sf::Color(0, 0, 0, 255));
		ui_grab.m_shape_container.setOutlineThickness(2);
		ui_grab.m_shape_container.setFillColor(sf::Color(0, 0, 0, 0));

		ui_grab.m_shape.setPosition(sf::Vector2f(offset_grab_x, slot_coord[0][1] + offset_eot_y));
		ui_grab.m_shape.setSize(sf::Vector2f(size_grab_x, sizeeot_y));
		ui_grab.m_shape.setOrigin(sf::Vector2f(size_grab_x * 0.5f, sizeeot_y * 0.5));
		ui_grab.m_shape.setOutlineColor(sf::Color(0, 0, 0, 0));
		ui_grab.m_shape.setOutlineThickness(0);
		ui_grab.m_shape.setFillColor(sf::Color(255, 255, 255, 255));

		if (g == 0)
		{
			ui_grab.m_text.setString("GRAB Left");
		}
		else
		{
			ui_grab.m_text.setString("GRAB Right");
		}
		ui_grab.m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
		ui_grab.m_text.setCharacterSize(18);
		ui_grab.m_text.setColor(sf::Color(0, 0, 0, 255));
		ui_grab.m_text.setPosition(sf::Vector2f(offset_grab_x - ui_grab.m_text.getGlobalBounds().width * 0.5f, slot_coord[0][1] + offset_eot_y - ui_grab.m_text.getGlobalBounds().height * 0.5f));

		float global_bounds_x = ui_grab.m_text.getGlobalBounds().width;
		float global_bounds_y = ui_grab.m_text.getCharacterSize();//ui.m_text.getGlobalBounds().height;
		ui.m_text.setPosition(sf::Vector2f(ui_grab.m_text.getPosition().x - global_bounds_x * 0.5f, ui_grab.m_text.getPosition().y - global_bounds_y * 0.6f));

		m_UI_buttons.push_back(ui_grab);

	}

	//"Guard" button
	for (int g = 0; g < 2; g++)
	{
		UI_Element ui_guard(this);

		ui_guard.m_team = (TeamAlliances)m_index;

		ui_guard.m_type = g == 0 ? UI_GuardL : UI_GuardR;

		float offset_guard_x = g == 0 ? 60.f + robot_offset + end_of_turn_offset : 690.f + robot_offset + end_of_turn_offset;
		float size_guard_x = 100.f;

		ui_guard.m_shape_container.setPosition(sf::Vector2f(offset_guard_x, slot_coord[0][1] + offset_eot_y));
		ui_guard.m_shape_container.setSize(sf::Vector2f(size_guard_x, sizeeot_y));
		ui_guard.m_shape_container.setOrigin(sf::Vector2f(size_guard_x * 0.5f, sizeeot_y * 0.5));
		ui_guard.m_shape_container.setOutlineColor(sf::Color(0, 0, 0, 255));
		ui_guard.m_shape_container.setOutlineThickness(2);
		ui_guard.m_shape_container.setFillColor(sf::Color(0, 0, 0, 0));

		ui_guard.m_shape.setPosition(sf::Vector2f(offset_guard_x, slot_coord[0][1] + offset_eot_y));
		ui_guard.m_shape.setSize(sf::Vector2f(size_guard_x, sizeeot_y));
		ui_guard.m_shape.setOrigin(sf::Vector2f(size_guard_x * 0.5f, sizeeot_y * 0.5));
		ui_guard.m_shape.setOutlineColor(sf::Color(0, 0, 0, 0));
		ui_guard.m_shape.setOutlineThickness(0);
		ui_guard.m_shape.setFillColor(sf::Color(255, 255, 255, 255));

		if (g == 0)
		{
			ui_guard.m_text.setString("Guard Left\n(14)");
		}
		else
		{
			ui_guard.m_text.setString("Guard Right\n(14)");
		}
		ui_guard.m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
		ui_guard.m_text.setCharacterSize(18);
		ui_guard.m_text.setColor(sf::Color(0, 0, 0, 255));
		ui_guard.m_text.setPosition(sf::Vector2f(offset_guard_x - ui_guard.m_text.getGlobalBounds().width * 0.5f, slot_coord[0][1] + offset_eot_y - ui_guard.m_text.getGlobalBounds().height * 0.5f));

		float global_bounds_x = ui_guard.m_text.getGlobalBounds().width;
		float global_bounds_y = ui_guard.m_text.getCharacterSize();//ui.m_text.getGlobalBounds().height;
		ui.m_text.setPosition(sf::Vector2f(ui_guard.m_text.getPosition().x - global_bounds_x * 0.5f, ui_guard.m_text.getPosition().y - global_bounds_y * 0.6f));

		m_UI_buttons.push_back(ui_guard);
	}
	
	//Robot stats
	float offsettext_x = 20.f - robot_offset;
	float offsettext_y = 900.f;
	m_UI_stats.setPosition(sf::Vector2f(slot_coord[0][0] - offsettext_x, offsettext_y));
	m_UI_stats.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_UI_stats.setCharacterSize(24);
	m_UI_stats.setColor(sf::Color(255, 255, 255, 255));
	m_UI_stats.setString("HEALTH POINTS\nENERGY CELLS\nBALANCE\nWEIGHT\nSTATUS");

	//"Balance" button
	UI_Element ui_b(this);

	ui_b.m_type = UI_Balance;
	ui_b.m_team = (TeamAlliances)m_index;

	float offset_bal_x = 660.f + robot_offset;
	float offset_bal_y = 935.f;
	float sizebal_x = 180;
	float sizebal_y = 30.f;

	ui_b.m_shape_container.setPosition(sf::Vector2f(offset_bal_x, offset_bal_y));
	ui_b.m_shape_container.setSize(sf::Vector2f(sizebal_x, sizebal_y));
	ui_b.m_shape_container.setOrigin(sf::Vector2f(sizebal_x * 0.5f, sizebal_y * 0.5));
	ui_b.m_shape_container.setOutlineColor(sf::Color(0, 0, 0, 255));
	ui_b.m_shape_container.setOutlineThickness(2);
	ui_b.m_shape_container.setFillColor(sf::Color(0, 0, 0, 0));

	ui_b.m_shape.setPosition(sf::Vector2f(offset_bal_x, offset_bal_y));
	ui_b.m_shape.setSize(sf::Vector2f(sizebal_x, sizebal_y));
	ui_b.m_shape.setOrigin(sf::Vector2f(sizebal_x * 0.5f, sizebal_y * 0.5));
	ui_b.m_shape.setOutlineColor(sf::Color(0, 0, 0, 0));
	ui_b.m_shape.setOutlineThickness(0);
	ui_b.m_shape.setFillColor(sf::Color(255, 255, 255, 255));

	ui_b.m_text.setPosition(sf::Vector2f(offset_bal_x, offset_bal_y));
	ui_b.m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	ui_b.m_text.setCharacterSize(16);
	ui_b.m_text.setColor(sf::Color(0, 0, 0, 255));
	ui_b.m_text.setString("GET BALANCED: -10 EC");
	ui_b.m_text.setPosition(sf::Vector2f(offset_bal_x - ui_b.m_text.getGlobalBounds().width * 0.5f + 8.f, offset_bal_y - ui_b.m_text.getGlobalBounds().height * 0.5f - 4.f));

	m_UI_buttons.push_back(ui_b);
}

RobotSlot* Robot::GetEntityParentSlot(GameEntity* entity, UI_Type type)
{
	if (type == UI_Module)
	{
		Module* module = (Module*)entity;
		if (module == NULL)
		{
			return NULL;
		}
		else
		{
			return module->m_owner;
		}
	}
	else if (type == UI_Equipment)
	{
		Equipment* equipment = (Equipment*)entity;
		if (equipment == NULL)
		{
			return NULL;
		}
		else
		{
			return equipment->m_owner;
		}
	}
	else
	{
		return NULL;
	}
}

string Robot::GetCrewMemberName(CrewType type)
{
	switch (type)
	{
		case Crew_Captain:
		{
			return "Captain\n";
		}
		case Crew_Warrior:
		{
			return "Warrior\n";
		}
		case Crew_Gunner:
		{
			return "Gunner\n";
		}
		case Crew_Pilot:
		{
			return "Pilot\n";
		}
		case Crew_Mechanic:
		{
			return "Mechanic\n";
		}
		case Crew_Medic:
		{
			return "Medic\n";
		}
		case Crew_Scientist:
		{
			return "Scientist\n";
		}
		case Crew_Engineer:
		{
			return "Engineer\n";
		}
		case Crew_Any:
		{
			return "crew member";
		}
	}
}

void Robot::UpdateUI()
{
	//Crew members selection tab
	int c = 0;
	for (vector<CrewMember*>::iterator it = m_crew_start.begin(); it != m_crew_start.end(); it++)
	{
		//status
		ostringstream ss;
		ss << GetCrewMemberName((*it)->m_type);
		ss << "HP: " << (*it)->m_health << "/" << (*it)->m_health_max << "\n";
		ss << "Move: " << (*it)->m_steps_remaining << "/" << (*it)->m_steps << "\n";
		
		if ((*it)->m_health == 0)
		{
			m_UI_crew[c].m_shape.setFillColor(sf::Color::Red);
			ss << "Dead";
		}
		else if ((*it)->m_stun_counter > 0)
		{
			m_UI_crew[c].m_shape.setFillColor(sf::Color::Cyan);
			ss << "K.O.";
		}
		else if (m_slots[(*it)->m_index].m_module->m_fire_counter > 0)
		{
			m_UI_crew[c].m_shape.setFillColor(sf::Color(255, 201, 14, 255));//orange
			ss << "On fire";
		}
		else
		{
			m_UI_crew[c].m_shape.setFillColor(sf::Color::White);
			ss << "OK";
		}

		//text update
		m_UI_crew[c].m_text.setString(ss.str());

		//position
		float pos_x = m_UI_slots[(*it)->m_index].m_shape.getPosition().x;
		float pos_y = m_UI_slots[(*it)->m_index].m_shape.getPosition().y + 8.f;

		float offset_x = 0;
		float offset_y = 0;

		int nb_crew_in_module = (*it)->m_owner->m_crew.size();
		if (nb_crew_in_module > 1)
		{
			int c2 = 0;
			for (vector<CrewMember*>::iterator it2 = (*it)->m_owner->m_crew.begin(); it2 != (*it)->m_owner->m_crew.end(); it2++)
			{
				if ((*it2) == (*it))
				{
					offset_x = -(CREWSML_SIZE_X * 0.5) + (CREWSML_SIZE_X * c2);
					break;//crew member found
				}
				c2++;
			}
		}

		//quick hardcoding, known that vertical slots have max 2 modules and horizontal slots have max 3 modules
		bool vertical = slot_size[(*it)->m_index][1] > slot_size[(*it)->m_index][0];
		bool odd_size = m_slots[(*it)->m_index].m_size % 2 == 1;

		if (vertical == true)
		{
			offset_y -= MODULE_SIZE_Y * 0.5;
		}
		else
		{
			if (odd_size == false)
			{
				offset_x -= MODULE_SIZE_X * 0.5;
			}
			else if (m_slots[(*it)->m_index].m_size > 1)
			{
				offset_x -= MODULE_SIZE_X;
			}
		}

		m_UI_crew_sml[c].m_shape_container.setPosition(sf::Vector2f(pos_x + offset_x, pos_y + offset_y));
		m_UI_crew_sml[c].m_shape.setPosition(sf::Vector2f(pos_x + offset_x, pos_y + offset_y));

		c++;
	}

	//Modules on the robot
	for (vector<vector<UI_Element> >::iterator it = m_UI_modules.begin(); it != m_UI_modules.end(); it++)
	{	
		Module* module = NULL;

		//equipment
		if (it->front().m_type == UI_Equipment && it->front().m_parent != NULL)
		{
			Equipment* equipment = (Equipment*)it->front().m_parent;
			module = equipment->m_owner->m_module;

			//EC slots
			int ecs = 0;
			for (vector<UI_Element>::iterator it2 = it->begin(); it2 != it->end(); it2++)
			{
				if (ecs > 0)
				{
					if (ecs <= equipment->m_energy_cells)
					{
						it2->m_shape_container.setFillColor(sf::Color::Green);
					}
					else
					{
						it2->m_shape_container.setFillColor(sf::Color::Black);
					}
				}
				ecs++;
			}

			//hp
			ostringstream ss_mod;
			ss_mod << equipment->m_UI_display_name_short;
			if (equipment->m_health_max > 0)
			{
				ss_mod << "\nHP: " << equipment->m_health << "/" << equipment->m_health_max;
			}
			it->front().m_text.setString(ss_mod.str());
		}

		//module
		if (it->front().m_type == UI_Module && it->front().m_parent != NULL)
		{
			module = (Module*)it->front().m_parent;

			//EC slots
			int ecs = 0;
			for (vector<UI_Element>::iterator it2 = it->begin(); it2 != it->end(); it2++)
			{
				if (ecs > 0)
				{
					if (ecs <= module->m_energy_cells)
					{
						it2->m_shape_container.setFillColor(sf::Color::Green);
					}
					else
					{
						it2->m_shape_container.setFillColor(sf::Color::Black);
					}
				}
				ecs++;
			}

			//non-weapon module
			if (module->m_owner->m_weapon == NULL)
			{
				//hp and shield
				ostringstream ss_mod;
				ss_mod << module->m_UI_display_name_short;
				if (module->m_health_max > 0)
				{
					ss_mod << "\nHP: " << module->m_health << "/" << module->m_health_max;
					if (module->m_shield_max > 0)
					{
						ss_mod << "\nShield : " << module->m_shield << "/" << module->m_shield_max;
					}
				}
				it->front().m_text.setString(ss_mod.str());
			}
			//weapon module
			else
			{
				//hp and shield
				ostringstream ss_mod;
				Weapon* weapon = module->m_owner->m_weapon;
				ss_mod << weapon->m_UI_display_name_short;
				if (module->m_health_max > 0)
				{
					ss_mod << "\nHP: " << module->m_health << "/" << module->m_health_max;
				}
				it->front().m_text.setString(ss_mod.str());
			}
		}

		if (module != NULL)
		{
			if (module->m_health == 0)
			{
				it->front().m_shape_container.setFillColor(sf::Color::Red);
			}
			else if (module->m_shutdown_counter > 0)
			{
				it->front().m_shape_container.setFillColor(sf::Color::Yellow);
			}
			else if (m_shutdown_global == true)
			{
				it->front().m_shape_container.setFillColor(sf::Color::Yellow);
			}
			else if (module->m_fire_counter > 0)
			{
				it->front().m_shape_container.setFillColor(sf::Color(255, 201, 14, 255));//orange
			}
			else
			{
				if (it->front().m_type == UI_Module)
				{
					it->front().m_shape_container.setFillColor(sf::Color(0, 132, 232, 255));//default blue
				}
				else if (it->front().m_type == UI_Equipment)
				{
					it->front().m_shape_container.setFillColor(sf::Color(153, 217, 234, 255));//default lighter blue
				}
			}
		}
	}

	//Robot stats
	ostringstream sr;
	sr << "HEALTH: " << m_health << "/" << m_health_max << "\n";
	sr << "BALANCE: " << GetBalanceScore();
	if (m_unbalanced_counter > 0)
	{
		sr << ", UNBALANCE: -" << m_unbalanced_value;
	}
	sr << "\n";
	sr << "ENERGY CELLS: " << m_energy_cells << "/" << MAX_ROBOT_ENERGY_CELLS << "\n";
	sr << "WEIGHT: " << m_weight;
	if (m_balance_bonus > 0 && m_attack_speed_bonus < 0)
	{
		sr << " (BALANCE +" << m_balance_bonus << ", SPEED " << m_attack_speed_bonus << ")";
	}
	sr << "\n";
	sr << "STATUS: ";
	bool ok = true;
	if (m_shutdown_global == true)
	{
		sr << "GLOBAL SHUTDOWN! ";
		ok = false;
	}
	if (m_unbalanced_counter > 0)
	{
		if (m_grounded == true)
		{
			sr << "ON GROUND (" << m_unbalanced_value << ")!";
		}
		else
		{
			sr << "UNBALANCED (" << m_unbalanced_value << ")!";
		}
		ok = false;
	}
	if (m_grabbed != NULL)
	{
		if (m_grabbed->m_owner->m_index == Index_HandL)
		{
			sr << "GRABBED by Left Hand (x2 damage)! ";
		}
		else if (m_grabbed->m_owner->m_index == Index_HandR)
		{
			sr << "GRABBED by Right Hand (x2 damage)! ";
		}
		ok = false;
	}
	if (ok == true)
	{
		sr << "OK";
	}

	m_UI_stats.setString(sr.str());

	//Focused element (including weapons)
	for (vector<vector<UI_Element> >::iterator it = m_UI_focus.begin(); it != m_UI_focus.end(); it++)
	{
		it->clear();
	}
	m_UI_focus.clear();

	UI_Element* focused_ui = NULL;
	if ((*CurrentGame).m_hovered_ui != NULL && ((*CurrentGame).m_hovered_ui->m_type == UI_Module || (*CurrentGame).m_hovered_ui->m_type == UI_Equipment))// || (*CurrentGame).m_hovered_ui->m_type == UI_Crew))
	{
		focused_ui = (*CurrentGame).m_hovered_ui;
	}
	else if ((*CurrentGame).m_selected_ui != NULL && ((*CurrentGame).m_selected_ui->m_type == UI_Module || (*CurrentGame).m_selected_ui->m_type == UI_Equipment))// || (*CurrentGame).m_hovered_ui->m_type == UI_Crew))
	{
		focused_ui = (*CurrentGame).m_selected_ui;
	}

	if (focused_ui != NULL)
	{
		EquipmentEffect* effect = NULL;

		float offset_focus_x = 960.f;
		float offset_focus_y = 550.f;
		float sizefocus_x = 370.f;
		float sizefocus_y = 154.f;

		//Module focused
		if (focused_ui->m_type == UI_Module && focused_ui->m_parent != NULL)
		{
			Module* module = (Module*)focused_ui->m_parent;
			Weapon* weapon = module->m_owner->m_weapon;

			//Weapon focused
			if (weapon != NULL)
			{
				vector<UI_Element> ui;
				UI_Element ui_weapon(weapon);

				ui_weapon.m_type = UI_Focus;
				ui_weapon.m_team = (TeamAlliances)m_index;//AllianceNeutral;

				ui_weapon.m_shape_container.setPosition(sf::Vector2f(offset_focus_x, offset_focus_y));
				ui_weapon.m_shape_container.setSize(sf::Vector2f(sizefocus_x, sizefocus_y));
				ui_weapon.m_shape_container.setOrigin(sf::Vector2f(sizefocus_x * 0.5f, sizefocus_y * 0.5f));
				ui_weapon.m_shape_container.setOutlineColor(sf::Color(255, 255, 255, 255));
				ui_weapon.m_shape_container.setOutlineThickness(2);
				ui_weapon.m_shape_container.setFillColor(sf::Color(0, 0, 0, 0));

				ui_weapon.m_shape.setPosition(sf::Vector2f(offset_focus_x, offset_focus_y));
				ui_weapon.m_shape.setSize(sf::Vector2f(sizefocus_x, sizefocus_y));
				ui_weapon.m_shape.setOrigin(sf::Vector2f(sizefocus_x * 0.5f, sizefocus_y * 0.5f));
				ui_weapon.m_shape.setOutlineColor(sf::Color(0, 0, 0, 0));
				ui_weapon.m_shape.setOutlineThickness(0);
				ui_weapon.m_shape.setFillColor(sf::Color(0, 0, 0, 255));

				ui_weapon.m_text.setPosition(sf::Vector2f(offset_focus_x - sizefocus_x * 0.5f + 8.f, offset_focus_y - sizefocus_y * 0.5f + 8.f));
				ui_weapon.m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
				ui_weapon.m_text.setCharacterSize(20);
				ui_weapon.m_text.setColor(sf::Color(255, 255, 255, 255));

				ostringstream sw;
				sw << weapon->m_UI_display_name;

				if (weapon->m_energetic == true)
				{
					sw << "\nEnergetic weapon";
				}
				else
				{
					sw << "\nPhysical weapon";
				}

				if (weapon->m_ranged == true)
				{
					sw << "\nRanged";
				}
				else
				{
					sw << "\nClose-Combat";
				}

				sw << "\nWeight: " << weapon->m_weight;

				ui_weapon.m_text.setString(sw.str());

				ui.push_back(ui_weapon);
				m_UI_focus.push_back(ui);

				//weapon attacks
				int w = 1;
				for (vector<WeaponAttack*>::iterator it = weapon->m_attacks.begin(); it != weapon->m_attacks.end(); it++)
				{
					vector<UI_Element> ui2;
					UI_Element ui_attack(*it);

					ui_attack.m_type = UI_WeaponAttack;
					ui_attack.m_team = (TeamAlliances)m_index;//AllianceNeutral;

					float sizeattack_x = sizefocus_x;
					float sizeattack_y = 160.f;
					float offset_attack_x = offset_focus_x;
					float offset_attack_y = ui_weapon.m_text.getPosition().y + 68.f + sizeattack_y * w; //750.f;

					ui_attack.m_shape_container.setPosition(sf::Vector2f(offset_attack_x, offset_attack_y));
					ui_attack.m_shape_container.setSize(sf::Vector2f(sizeattack_x, sizeattack_y));
					ui_attack.m_shape_container.setOrigin(sf::Vector2f(sizeattack_x * 0.5f, sizeattack_y * 0.5f));
					ui_attack.m_shape_container.setOutlineColor(sf::Color(255, 255, 255, 255));
					ui_attack.m_shape_container.setOutlineThickness(2);
					ui_attack.m_shape_container.setFillColor(sf::Color(0, 0, 0, 0));

					ui_attack.m_shape.setPosition(sf::Vector2f(offset_attack_x, offset_attack_y));
					ui_attack.m_shape.setSize(sf::Vector2f(sizeattack_x, sizeattack_y));
					ui_attack.m_shape.setOrigin(sf::Vector2f(sizeattack_x * 0.5f, sizeattack_y * 0.5f));
					ui_attack.m_shape.setOutlineColor(sf::Color(0, 0, 0, 0));
					ui_attack.m_shape.setOutlineThickness(0);
					if (*it == weapon->m_selected_attack)
					{
						if (weapon->m_selected_attack->m_nb_targets_remaining < weapon->m_selected_attack->m_nb_targets)
						{
							ui_attack.m_shape.setFillColor(sf::Color(255, 201, 14, 255));//orange = selected and locked
						}
						else
						{
							ui_attack.m_shape.setFillColor(sf::Color(0, 132, 232, 255));//blue (module) = selection
						}
					}
					else
					{
						ui_attack.m_shape.setFillColor(sf::Color(0, 0, 0, 255));//black = default
					}
					
					ui_attack.m_text.setPosition(sf::Vector2f(offset_attack_x - sizeattack_x * 0.5f + 8.f, offset_attack_y - sizeattack_y * 0.5f + 8.f));
					ui_attack.m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
					ui_attack.m_text.setCharacterSize(20);
					ui_attack.m_text.setColor(sf::Color(255, 255, 255, 255));

					ostringstream sa;
					sa << (*it)->m_UI_display_name;
					sa << "\n" << (*it)->m_UI_description;
					sa << "\n" << "Speed: " << (*it)->m_speed << ", EC: " << (*it)->m_energy_cost;
					if ((*it)->m_crew_required != NB_CREW_TYPES)
					{
						sa << "\n\n\n" << "Requires a " << GetCrewMemberName((*it)->m_crew_required);
					}

					ui_attack.m_text.setString(sa.str());
					ui2.push_back(ui_attack);
					
					//attack stats
					vector<pair<int, int> > stats;
					if ((*it)->m_damage > 0)
					{
						stats.push_back(pair<int, int>((*it)->m_damage, UI_Weapon_Damage));
					}
					if ((*it)->m_chance_of_hit > 0)
					{
						stats.push_back(pair<int, int>((*it)->m_chance_of_hit, UI_Weapon_HitChance));
					}
					if ((*it)->m_chance_of_unbalance > 0)
					{
						stats.push_back(pair<int, int>((*it)->m_chance_of_unbalance, UI_Weapon_Unbalance));
					}
					if ((*it)->m_chance_of_fire > 0)
					{
						stats.push_back(pair<int, int>((*it)->m_chance_of_fire, UI_Weapon_Fire));
					}
					if ((*it)->m_chance_of_electricity > 0)
					{
						stats.push_back(pair<int, int>((*it)->m_chance_of_electricity, UI_Weapon_Electricity));
					}
					if ((*it)->m_chance_of_stun > 0)
					{
						stats.push_back(pair<int, int>((*it)->m_chance_of_stun, UI_Weapon_Stun));
					}

					int s = 0;
					for (vector<pair<int, int> >::iterator it2 = stats.begin(); it2 != stats.end(); it2++)
					{
						UI_Element ui_stat(*it);

						ui_stat.m_type = UI_WeaponAttackStat;
						ui_stat.m_team = (TeamAlliances)m_index;//AllianceNeutral;

						float offset_stat_x = offset_focus_x - sizefocus_x * 0.5f + 8.f + EC_SIZE_X + s * 55.f;
						float offset_stat_y = offset_attack_y + 20.f;

						ui_stat.m_shape_container.setPosition(sf::Vector2f(offset_stat_x, offset_stat_y));
						ui_stat.m_shape_container.setSize(sf::Vector2f(EC_SIZE_X, EC_SIZE_Y));
						ui_stat.m_shape_container.setOrigin(sf::Vector2f(EC_SIZE_X * 0.5f, EC_SIZE_Y * 0.5f));
						ui_stat.m_shape_container.setOutlineColor(sf::Color(255, 255, 255, 255));
						ui_stat.m_shape_container.setOutlineThickness(2);
						ui_stat.m_shape_container.setFillColor(sf::Color(0, 0, 0, 0));

						ui_stat.m_shape.setPosition(sf::Vector2f(offset_stat_x, offset_stat_y));
						ui_stat.m_shape.setSize(sf::Vector2f(EC_SIZE_X, EC_SIZE_Y));
						ui_stat.m_shape.setOrigin(sf::Vector2f(EC_SIZE_X * 0.5f, EC_SIZE_Y * 0.5f));
						ui_stat.m_shape.setOutlineColor(sf::Color(0, 0, 0, 0));
						ui_stat.m_shape.setOutlineThickness(0);
						
						switch (it2->second)
						{
							case UI_Weapon_Damage:
							{
								ui_stat.m_shape.setFillColor(sf::Color::Red);
								break;
							}
							case UI_Weapon_HitChance:
							{
								ui_stat.m_shape.setFillColor(sf::Color::White);
								break;
							}
							case UI_Weapon_Unbalance:
							{
								ui_stat.m_shape.setFillColor(sf::Color::Magenta);
								break;
							}
							case UI_Weapon_Fire:
							{
								ui_stat.m_shape.setFillColor(sf::Color(255, 201, 14, 255));//orange
								break;
							}
							case UI_Weapon_Electricity:
							{
								ui_stat.m_shape.setFillColor(sf::Color::Yellow);
								break;
							}
							case UI_Weapon_Stun:
							{
								ui_stat.m_shape.setFillColor(sf::Color::Cyan);
								break;
							}
						}

						ui_stat.m_text.setPosition(sf::Vector2f(offset_stat_x + EC_SIZE_X * 0.5f + 4.f, offset_stat_y - EC_SIZE_Y * 0.5f - 4.f));
						ui_stat.m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
						ui_stat.m_text.setCharacterSize(20);
						ui_stat.m_text.setColor(sf::Color(255, 255, 255, 255));

						ostringstream ss;
						ss << it2->first;
						if (it2->second != UI_Weapon_Damage && it2->second != UI_Weapon_Unbalance)
						{
							ss << "+";
						}

						ui_stat.m_text.setString(ss.str());
						ui2.push_back(ui_stat);

						s++;
					}

					m_UI_focus.push_back(ui2);

					w++;
				}
			}
			//Non-weapon focused
			else
			{
				vector<UI_Element> ui;
				UI_Element ui_module(module);

				effect = module->m_effect;

				ui_module.m_type = UI_Focus;
				ui_module.m_team = (TeamAlliances)m_index;//AllianceNeutral;

				ui_module.m_shape_container.setPosition(sf::Vector2f(offset_focus_x, offset_focus_y));
				ui_module.m_shape_container.setSize(sf::Vector2f(sizefocus_x, sizefocus_y));
				ui_module.m_shape_container.setOrigin(sf::Vector2f(sizefocus_x * 0.5f, sizefocus_y * 0.5f));
				ui_module.m_shape_container.setOutlineColor(sf::Color(255, 255, 255, 255));
				ui_module.m_shape_container.setOutlineThickness(2);
				ui_module.m_shape_container.setFillColor(sf::Color(0, 0, 0, 0));

				ui_module.m_shape.setPosition(sf::Vector2f(offset_focus_x, offset_focus_y));
				ui_module.m_shape.setSize(sf::Vector2f(sizefocus_x, sizefocus_y));
				ui_module.m_shape.setOrigin(sf::Vector2f(sizefocus_x * 0.5f, sizefocus_y * 0.5f));
				ui_module.m_shape.setOutlineColor(sf::Color(0, 0, 0, 0));
				ui_module.m_shape.setOutlineThickness(0);
				ui_module.m_shape.setFillColor(sf::Color(0, 0, 0, 255));

				ui_module.m_text.setPosition(sf::Vector2f(offset_focus_x - sizefocus_x * 0.5f + 8.f, offset_focus_y - sizefocus_y * 0.5f + 8.f));
				ui_module.m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
				ui_module.m_text.setCharacterSize(20);
				ui_module.m_text.setColor(sf::Color(255, 255, 255, 255));

				ostringstream sm;
				sm << module->m_UI_display_name;
				
				sm << "\nHP: " << module->m_health << "/" << module->m_health_max;
				if (module->m_shield_max > 0)
				{
					sm << ", Shield: " << module->m_shield << "/" << module->m_shield_max;
				}

				sm << "\nWeight: " << module->m_weight;
				if (module->m_cooldown > 0)
				{
					sm << "\nCooldown: " << module->m_cooldown_timer << "/" << module->m_cooldown;
				}
				sm << "\n" << module->m_UI_description;

				ui_module.m_text.setString(sm.str());

				ui.push_back(ui_module);
				m_UI_focus.push_back(ui);
			}
		}
		//Equipment focused
		else if (focused_ui->m_type == UI_Equipment && focused_ui->m_parent != NULL)
		{
			Equipment* equipment = (Equipment*)focused_ui->m_parent;

			effect = equipment->m_effect;

			vector<UI_Element> ui;
			UI_Element ui_equipment(equipment);

			effect = equipment->m_effect;

			ui_equipment.m_type = UI_Focus;
			ui_equipment.m_team = (TeamAlliances)m_index;//AllianceNeutral;

			float offset_equipment_x = 960.f;
			float offset_equipment_y = 550.f;
			float sizeequipment_x = sizefocus_x;
			float sizeequipment_y = sizefocus_y;

			ui_equipment.m_shape_container.setPosition(sf::Vector2f(offset_equipment_x, offset_equipment_y));
			ui_equipment.m_shape_container.setSize(sf::Vector2f(sizeequipment_x, sizeequipment_y));
			ui_equipment.m_shape_container.setOrigin(sf::Vector2f(sizeequipment_x * 0.5f, sizeequipment_y * 0.5f));
			ui_equipment.m_shape_container.setOutlineColor(sf::Color(255, 255, 255, 255));
			ui_equipment.m_shape_container.setOutlineThickness(2);
			ui_equipment.m_shape_container.setFillColor(sf::Color(0, 0, 0, 0));

			ui_equipment.m_shape.setPosition(sf::Vector2f(offset_equipment_x, offset_equipment_y));
			ui_equipment.m_shape.setSize(sf::Vector2f(sizeequipment_x, sizeequipment_y));
			ui_equipment.m_shape.setOrigin(sf::Vector2f(sizeequipment_x * 0.5f, sizeequipment_y * 0.5f));
			ui_equipment.m_shape.setOutlineColor(sf::Color(0, 0, 0, 0));
			ui_equipment.m_shape.setOutlineThickness(0);
			ui_equipment.m_shape.setFillColor(sf::Color(0, 0, 0, 255));

			ui_equipment.m_text.setPosition(sf::Vector2f(offset_equipment_x - sizeequipment_x * 0.5f + 8.f, offset_equipment_y - sizeequipment_y * 0.5f + 8.f));
			ui_equipment.m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
			ui_equipment.m_text.setCharacterSize(20);
			ui_equipment.m_text.setColor(sf::Color(255, 255, 255, 255));

			ostringstream sm;
			sm << equipment->m_UI_display_name;

			if (equipment->m_health_max > 0)
			{
				sm << "\nHP: " << equipment->m_health << "/" << equipment->m_health_max;
			}
			sm << "\nWeight: " << equipment->m_weight;
			if (equipment->m_cooldown > 0)
			{
				sm << "\nCooldown: " << equipment->m_cooldown_timer << "/" << equipment->m_cooldown;
			}
			sm << "\n" << equipment->m_UI_description;

			ui_equipment.m_text.setString(sm.str());

			ui.push_back(ui_equipment);
			m_UI_focus.push_back(ui);
		}
		//crew member
		else if (focused_ui->m_type == UI_Crew && focused_ui->m_parent != NULL)
		{
			CrewMember* crew = (CrewMember*)focused_ui->m_parent;

			vector<UI_Element> ui;
			UI_Element ui_crew(crew);

			ui_crew.m_type = UI_Focus;
			ui_crew.m_team = (TeamAlliances)m_index;//AllianceNeutral;

			float offset_crew_x = 960.f;
			float offset_crew_y = 550.f;
			float sizecrew_x = sizefocus_x;
			float sizecrew_y = sizefocus_y;

			ui_crew.m_shape_container.setPosition(sf::Vector2f(offset_crew_x, offset_crew_y));
			ui_crew.m_shape_container.setSize(sf::Vector2f(sizecrew_x, sizecrew_y));
			ui_crew.m_shape_container.setOrigin(sf::Vector2f(sizecrew_x * 0.5f, sizecrew_y * 0.5f));
			ui_crew.m_shape_container.setOutlineColor(sf::Color(255, 255, 255, 255));
			ui_crew.m_shape_container.setOutlineThickness(2);
			ui_crew.m_shape_container.setFillColor(sf::Color(0, 0, 0, 0));

			ui_crew.m_shape.setPosition(sf::Vector2f(offset_crew_x, offset_crew_y));
			ui_crew.m_shape.setSize(sf::Vector2f(sizecrew_x, sizecrew_y));
			ui_crew.m_shape.setOrigin(sf::Vector2f(sizecrew_x * 0.5f, sizecrew_y * 0.5f));
			ui_crew.m_shape.setOutlineColor(sf::Color(0, 0, 0, 0));
			ui_crew.m_shape.setOutlineThickness(0);
			ui_crew.m_shape.setFillColor(sf::Color(0, 0, 0, 255));

			ui_crew.m_text.setPosition(sf::Vector2f(offset_crew_x - sizecrew_x * 0.5f + 8.f, offset_crew_y - sizecrew_y * 0.5f + 8.f));
			ui_crew.m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
			ui_crew.m_text.setCharacterSize(20);
			ui_crew.m_text.setColor(sf::Color(255, 255, 255, 255));
			
			ostringstream sm;
			string name;
			int c = 0;
			for (vector<CrewMember*>::iterator it = m_crew_start.begin(); it != m_crew_start.end(); it++)
			{
				if (*it == crew)
				{
					name = m_UI_crew[c].m_text.getString();
					break;
				}
				c++;
			}

			sm << "\n";
			switch (crew->m_type)
			{
				case Crew_Medic:
				{
					sm << "+2 hp to a target crew member \npresent in the module.";
					break;
				}
				case Crew_Gunner:
				{
					sm << "+1 precision to Range Weapons. \n(must be in the WEAPON'S MODULE)";
					break;
				}
				case Crew_Scientist:
				{
					sm << "+1 Energy cell per turn. (Must be \n in the GENERATOR)";
					break;
				}
				case Crew_Pilot:
				{
					sm << "Replace the Captain in the HEAD.\n+1 Speed attack. (must be in the HEAD)";
					break;
				}
				case Crew_Mechanic:
				{
					sm << "+1 module life point repaired per turn.";
					break;
				}
				case Crew_Warrior:
				{
					sm << "+2 Unbalance to Close Combat Weapons. \n(Must be in the WEAPON'S MODULE)";
					break;
				}
				case Crew_Engineer:
				{
					sm << "Required to use the equipements of \nthe GADGET MODULE.";
					break;
				}
				case Crew_Captain:
				{
					sm << "Required in the head to pilot the Robot \n3 turns in a module => OVERCHARGE.";
					break;
				}

			}
			

			ui_crew.m_text.setString(name + sm.str());

			ui.push_back(ui_crew);
			m_UI_focus.push_back(ui);
		}

		//Active ability of module or equipment
		if (effect != NULL)
		{
			vector<UI_Element> ui2;
			UI_Element ui_effect(effect);

			ui_effect.m_type = UI_Effect;
			ui_effect.m_team = (TeamAlliances)m_index;//AllianceNeutral;


			float sizeeffect_x = sizefocus_x;
			float sizeeffect_y = 160.f;
			float offset_effect_x = offset_focus_x;
			float offset_effect_y = offset_focus_y + 5.f + sizefocus_y;

			ui_effect.m_shape_container.setPosition(sf::Vector2f(offset_effect_x, offset_effect_y));
			ui_effect.m_shape_container.setSize(sf::Vector2f(sizeeffect_x, sizeeffect_y));
			ui_effect.m_shape_container.setOrigin(sf::Vector2f(sizeeffect_x * 0.5f, sizeeffect_y * 0.5f));
			ui_effect.m_shape_container.setOutlineColor(sf::Color(255, 255, 255, 255));
			ui_effect.m_shape_container.setOutlineThickness(2);
			ui_effect.m_shape_container.setFillColor(sf::Color(0, 0, 0, 0));

			ui_effect.m_shape.setPosition(sf::Vector2f(offset_effect_x, offset_effect_y));
			ui_effect.m_shape.setSize(sf::Vector2f(sizeeffect_x, sizeeffect_y));
			ui_effect.m_shape.setOrigin(sf::Vector2f(sizeeffect_x * 0.5f, sizeeffect_y * 0.5f));
			ui_effect.m_shape.setOutlineColor(sf::Color(0, 0, 0, 0));
			ui_effect.m_shape.setOutlineThickness(0);

			if (effect->m_owner_equipment != NULL && (effect->m_owner_equipment->m_used == true || effect->m_owner_equipment->m_cooldown_timer < effect->m_owner_equipment->m_cooldown))
			{
				ui_effect.m_shape.setFillColor(sf::Color(255, 201, 14, 255));//orange = selected and locked
			}
			else if (effect->m_owner_module != NULL && (effect->m_owner_module->m_used == true || effect->m_owner_module->m_cooldown_timer < effect->m_owner_module->m_cooldown))
			{
				ui_effect.m_shape.setFillColor(sf::Color(255, 201, 14, 255));//orange = selected and locked
			}
			else
			{
				ui_effect.m_shape.setFillColor(sf::Color(0, 132, 232, 255));//blue (module) = selection
			}
			
			ui_effect.m_text.setPosition(sf::Vector2f(offset_effect_x - sizefocus_x * 0.5f + 4.f, offset_effect_y - sizeeffect_y * 0.5f + 8.f));
			ui_effect.m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
			ui_effect.m_text.setCharacterSize(20);
			ui_effect.m_text.setColor(sf::Color(255, 255, 255, 255));

			ostringstream ss_effect;

			ss_effect << "Use ";
			if (effect->m_owner_module != NULL)
			{
				ss_effect << effect->m_owner_module->m_UI_display_name << "\n";
			}
			if (effect->m_owner_equipment != NULL)
			{
				ss_effect << effect->m_owner_equipment->m_UI_display_name << "\n";
			}
			ss_effect << "Cost: " << effect->m_energy_cost << " EC";

			ui_effect.m_text.setString(ss_effect.str());

			ui2.push_back(ui_effect);
			m_UI_focus.push_back(ui2);
		}
	}

	//Targets on slots
	for (vector<UI_Element>::iterator it = m_UI_slots.begin(); it != m_UI_slots.end(); it++)
	{
		it->m_shape.setFillColor(sf::Color(0, 0, 0, 0));
	}

	//are we the target of a planned attack?
	for (vector<ActionAttack>::iterator it = (*CurrentGame).m_attacks_list.begin(); it != (*CurrentGame).m_attacks_list.end(); it++)
	{
		WeaponAttack* attack = it->m_attack;
		int robot_index = attack->m_owner->m_owner->m_owner->m_index;
		robot_index = (robot_index + 1) % 2;

		if (robot_index == m_index)
		{
			//Display target on the targeted slot
			m_UI_slots[it->m_target_index].m_shape.setFillColor(sf::Color::White);
		}
	}

	for (vector<ActionEffect>::iterator it = (*CurrentGame).m_effects_list.begin(); it != (*CurrentGame).m_effects_list.end(); it++)
	{
		EquipmentEffect* effect = it->m_effect;
		int robot_index = effect->m_owner_module != NULL ? effect->m_owner_module->m_owner->m_owner->m_index : effect->m_owner_equipment->m_owner->m_owner->m_index;
		robot_index = (robot_index + 1) % 2;

		if (robot_index == m_index)
		{
			//Display target on the targeted slot
			m_UI_slots[it->m_target_index].m_shape.setFillColor(sf::Color::White);
		}
	}

	//EC available
	m_UI_ec_available.clear();
	for (int e = 0; e < m_energy_cells_available; e++)
	{
		UI_Element ui_ec(this);

		ui_ec.m_type = UI_EC_Available;
		ui_ec.m_team = (TeamAlliances)m_index;

		float offset_ec_x = 490.f + (m_index == 0 ? 0.f : 970.f) + e * (EC_SIZE_X + 8.f);
		float offset_ec_y = 970.f;

		ui_ec.m_shape_container.setPosition(sf::Vector2f(offset_ec_x, offset_ec_y));
		ui_ec.m_shape_container.setSize(sf::Vector2f(EC_SIZE_X, EC_SIZE_Y));
		ui_ec.m_shape_container.setOrigin(sf::Vector2f(EC_SIZE_X * 0.5f, EC_SIZE_Y * 0.5f));
		ui_ec.m_shape_container.setOutlineColor(sf::Color(255, 255, 255, 255));
		ui_ec.m_shape_container.setOutlineThickness(2);
		ui_ec.m_shape_container.setFillColor(sf::Color::Green);

		m_UI_ec_available.push_back(ui_ec);
	}


	//"Balance" button
	for (vector<UI_Element>::iterator it = m_UI_buttons.begin(); it != m_UI_buttons.end(); it++)
	{
		if (it->m_type == UI_Balance)
		{
			ostringstream ss_bal;
			ss_bal << "GET BALANCED: " << m_unbalanced_value << " EC";
			it->m_text.setString(ss_bal.str());
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
		// ***** PHASE CREW MOVEMENT *****
		// **** PHASE ATTACK PLANNING ****
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

		// ****************************************************
		//					ATTACK RESOLUTION
		// ****************************************************

		case Phase_FireResolution:
		{
			UpdateFirePropagation();

			UpdateShudownGlobal();//in case fire destroys the generator or kills the captain or the pilot

			m_ready_to_change_phase = true;
			break;
		}
		case Phase_CooldownResolution:
		{
			UpdateCooldowns();

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
		if ((*it).m_module && (*it).m_module->m_type == Module_Generator && (*it).m_module->IsOperationnal())
		{
			int cells = 3;

			for (vector<CrewMember*>::iterator it2 = (*it).m_crew.begin(); it2 != (*it).m_crew.end(); it2++)
			{
				if ((*it2)->m_type == Crew_Scientist && (*it2)->m_stun_counter == 0)//checking if not stunned
				{
					cells++;
				}
			}

			//for (vector<Equipment*>::iterator it2 = (*it).m_equipments.begin(); it2 != (*it).m_equipments.end(); it2++)
			//{
			//	if ((*it2)->m_type == Equipment_GeneratorBooster && (*it2)->m_cooldown == (*it2)->m_cooldown_timer)
			//	{
			//		cells += 3;
			//		(*it2)->m_cooldown_timer = 0;
			//	}
			//}

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

	ostringstream s_ec;
	s_ec << energy_cells_generated << " EC generated.";
	(*CurrentGame).UI_AddEventLog(s_ec.str(), Event_EC, m_index);

	return energy_cells_generated;
}

int Robot::HealCrewMembers()
{
	int health_healed = 0;

	for (vector<RobotSlot>::iterator it = m_slots.begin(); it != m_slots.end(); it++)
	{
		//Infirmary heals 1pv/energy cell
		int health = 0;
		Module* module = it->m_module;
		if (module != NULL && module->m_type == Module_Infirmary && module->IsOperationnal())
		{
			health += module->m_energy_cells;

			for (vector<CrewMember*>::iterator it2 = (*it).m_crew.begin(); it2 != (*it).m_crew.end(); it2++)
			{
				//Healing every crew member in the Infirmary
				for (int i = 0; i < health; i++)
				{
					if ((*it2)->m_health < (*it2)->m_health_max)
					{
						(*it2)->m_health++;
						health_healed++;
					}
				}
			}
		}

		//Medic can heal up to 2pv to a targeted crew member
		for (vector<CrewMember*>::iterator it2 = (*it).m_crew.begin(); it2 != (*it).m_crew.end(); it2++)
		{	
			if ((*it2)->m_type == Crew_Medic)
			{
				if ((*it2)->m_stun_counter == 0 && (*it2)->m_medic_target != NULL)
				{
					int health_medic = 2;
					for (int i = 0; i < health_medic; i++)
					{
						if ((*it2)->m_medic_target->m_health < (*it2)->m_medic_target->m_health_max)
						{
							(*it2)->m_medic_target->m_health++;
							health_healed++;
						}
					}
				}

				(*it2)->m_medic_target = NULL;
			}
		}
	}
			
	return health_healed;
}

int Robot::RepairModules()
{
	int health_repaired = 0;
	int shield_repaired = 0;

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

			//Crew reparairing
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

					//Ends fire status
					if (module->m_fire_counter > 0)
					{
						module->m_fire_counter = 0;

						(*CurrentGame).UI_AddEventLog("Fire extinguished.", Event_Fire, m_index);
					}

					//Ends shutdown status
					if (module->m_shutdown_counter > 0)
					{
						module->m_shutdown_counter = 0;
						
						(*CurrentGame).UI_AddEventLog("Shutdown repaired.", Event_Shutdown, m_index);
					}
				}
			}

			//Deflector auto-regen
			int shield = 0;
			if (module->m_type == Module_Deflectors && module->IsOperationnal() && module->m_energy_cells == 2)
			{
				shield += 2;
				shield_repaired += 2;
			}

			//Apply reparations
			for (int i = 0; i < health; i++)
			{
				if (module->m_health < module->m_health_max)
				{
					module->m_health++;
					health_repaired++;
				}
			}

			for (int i = 0; i < shield; i++)
			{
				if (module->m_shield < module->m_shield_max)
				{
					module->m_shield++;
					shield_repaired++;
				}
			}
		}
	}

	if (shield_repaired > 0)
	{
		(*CurrentGame).UI_AddEventLog("Deflectors regain 2 shield points.", Event_Shield, m_index);
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
						(*CurrentGame).UI_AddEventLog("Fire deals 1 damage to the module and destroys it.", Event_Fire, m_index);
						DestroySlot(it->m_index);
					}
					else
					{
						(*CurrentGame).UI_AddEventLog("Fire deals 1 damage to the module.", Event_Fire, m_index);
					}

					//Damage to robot
					m_health--;
					if (m_health == 0)
					{
						(*CurrentGame).UI_AddEventLog("Robot is destroyed.\nGAME OVER", Event_Damage, m_index);
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
					//Propagation to a module that is not burning yet
					if (it2->m_module != NULL && it2->m_module->m_fire_counter != 1)
					{
						//Get adjacent slots
						if (GetDistanceFromSlotToSlot(it->m_index, it2->m_index) == 1)
						{
							bool propagation_sucess = RandomizeIntBetweenValues(1, 6) >= 4;

							if (propagation_sucess)
							{
								it2->m_module->m_fire_counter = 1;
								printf("Fire propagated from slot %d to slot %d.\n", (int)it->m_index, (int)it2->m_index);
								(*CurrentGame).UI_AddEventLog("Fire propagates to an gadjacent module.", Event_Fire, m_index);
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
	//buff & debuff reset
	m_jammer_bonus = 0;

	for (vector<RobotSlot>::iterator it = m_slots.begin(); it != m_slots.end(); it++)
	{
		//Module cooldown
		if (it->m_module != NULL && it->m_module->m_cooldown > 0 && it->m_module->m_cooldown_timer < it->m_module->m_cooldown)
		{
			it->m_module->m_cooldown_timer++;
		}

		//Reset module
		if (it->m_module != NULL)
		{
			it->m_module->m_used = false;
		}

		//Equipments cooldown
		for (vector<Equipment*>::iterator it2 = it->m_equipments.begin(); it2 != it->m_equipments.end(); it2++)
		{
			if ((*it2)->m_cooldown > 0 && (*it2)->m_cooldown_timer < (*it2)->m_cooldown)
			{
				(*it2)->m_cooldown_timer++;
			}
		}

		//Reset equipments
		for (vector<Equipment*>::iterator it2 = it->m_equipments.begin(); it2 != it->m_equipments.end(); it2++)
		{
			(*it2)->m_used = false;
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

			(*it2)->m_steps_remaining = (*it2)->m_steps;
			(*it2)->m_used = false;
		}

		//Fire update
		if (it->m_module != NULL && it->m_module->m_fire_counter > 1)
		{
			it->m_module->m_fire_counter--;
		}

		//Reset weapon attacks
		if (it->m_weapon != NULL)
		{
			for (vector<WeaponAttack*>::iterator it2 = it->m_weapon->m_attacks.begin(); it2 != it->m_weapon->m_attacks.end(); it2++)
			{
				(*it2)->m_nb_targets_remaining = (*it2)->m_nb_targets;
			}
		}
	}

	//Unbalanced status update
	if (m_unbalanced_counter > 0)
	{
		m_unbalanced_counter--;

		//Gets balanced again
		if (m_unbalanced_counter == 0)
		{
			m_unbalanced_value = 0;
			m_grounded = false;
			(*CurrentGame).UI_AddEventLog("gets balanced again.", Event_Balance, m_index);
		}
	}

	//Reset Guard and Grab cooldowns and speed
	m_guard_speed = 0;
	m_has_attacked = false;
	m_has_attacked_RightHand = false;
	m_has_attacked_LeftHand = false;
	m_has_guarded = false;
	m_has_grabbed_LeftHand = false;
	m_has_grabbed_RightHand = false;
}

void Robot::ReloadWeapons()
{
	for (vector<RobotSlot>::iterator it = m_slots.begin(); it != m_slots.end(); it++)
	{
		if (it->m_weapon != NULL)
		{
			for (vector<WeaponAttack*>::iterator it2 = it->m_weapon->m_attacks.begin(); it2 != it->m_weapon->m_attacks.end(); it2++)
			{
				(*it2)->m_nb_targets_remaining = (*it2)->m_nb_targets;
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
	}

	for (vector<RobotSlot>::iterator it = m_slots.begin(); it != m_slots.end(); it++)
	{
		//Captain or pilot are alive and not stunned in the head?
		if (it->m_type == Index_Head)
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
			(*CurrentGame).UI_AddEventLog("is now facing a global shutdown of his systems.\n", Event_Shutdown, m_index);
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
			(*CurrentGame).UI_AddEventLog("has now ended the global shutdown of his systems.\n", Event_Shutdown, m_index);
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

		//Release the grab?
		if (m_opponent->m_grabbed == module)
		{
			m_opponent->m_grabbed = NULL;

			(*CurrentGame).UI_AddEventLog("Hand destroyed. Grab has been released", Event_Shutdown, m_index);
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

bool Robot::MoveCrewMemberToSlot(CrewMember* crew, RobotSlot* target_slot)
{
	if (target_slot == NULL)
	{
		(*CurrentGame).UI_AddEventLog("Cannot move crew to empty modules.", Event_Error, m_index);
		return false;
	}

	RobotSlot& current_slot = m_slots[crew->m_index];
	SlotIndex target_index = target_slot->m_index;
	int distance_to_cross = GetDistanceFromSlotToSlot(crew->m_index, target_index);

	if (target_index == crew->m_index)
	{
		(*CurrentGame).UI_AddEventLog("Crew member is already on this position.", Event_Error, m_index);
		return false;
	}
	else if (crew->m_stun_counter > 0)
	{
		(*CurrentGame).UI_AddEventLog("Crew member is stunned and cannot move.", Event_Error, m_index);
		return false;
	}
	else if (distance_to_cross > crew->m_steps_remaining)
	{
		(*CurrentGame).UI_AddEventLog("Not enough mouvement points left for this move.", Event_Error, m_index);
		printf("Cannot move crew member to slot %d because the distance to cross (%d) exceeds his remaining movement points (%d)\n", (int)target_index, distance_to_cross, crew->m_steps_remaining);
		return false;
	}
	else if (m_slots[target_index].m_module == NULL)
	{
		(*CurrentGame).UI_AddEventLog("Cannot move to empty modules", Event_Error, m_index);
		return false;
	}
	else if (crew->m_used == true)
	{
		(*CurrentGame).UI_AddEventLog("Cannot move: already used to attack or activate an ability.", Event_Error, m_index);
		return false;
	}
	else
	{
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

		target_slot->m_crew.push_back(crew);
		crew->m_index = target_index;
		crew->m_owner = target_slot;

		crew->m_steps_remaining -= distance_to_cross;

		UpdateShudownGlobal();

		printf("Crew moved from slot %d to slot %d.\n", (int)current_slot.m_index, (int)target_index);

		return true;
	}
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
			if ((*it)->m_type == Crew_Captain)
			{
				(*CurrentGame).UI_AddEventLog("Captain killed.\nGAME OVER", Event_Damage, m_index);
			}
			else
			{
				(*CurrentGame).UI_AddEventLog("Crew member killed.", Event_Damage, m_index);
			}
		}
	}
}

Module* Robot::GetShield()
{
	for (vector<RobotSlot>::iterator it = m_slots.begin(); it != m_slots.end(); it++)
	{
		if (it->m_module != NULL && it->m_weapon != NULL && it->m_weapon->m_type == Weapon_Shield)
		{
			return it->m_module;
		}
	}

	return NULL;
}

int Robot::GetDistanceFromSlotToSlot(SlotIndex index, SlotIndex target_index)
{
	int distance_to_cross = m_robot_map[index][target_index];
	int distance = 0;
	
	SlotIndex temp_index = index;

	while (distance_to_cross > 0)
	{
		for (int i = 0; i < NB_SLOT_INDEX; i++)
		{
			//Looking for the slot that gets us 1 step closer to the destination, while being at 1 step away from our current 
			if (m_robot_map[i][target_index] == distance_to_cross - 1 && m_robot_map[i][temp_index] == 1)
			{
				distance_to_cross--;
				temp_index = (SlotIndex)i;

				//Module empty? (in this case, movement is free and doesn't increase the distance)
				if (m_slots[temp_index].m_module != NULL)
				{
					distance++;
				}

				break;
			}
		}
	}

	return distance;
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
			printf("All Energy Cell slots are already full on this module.\n");
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
			printf("All Energy Cell slots are already full on this equipment.\n");
		}
	}
	else
	{
		printf("No Energy Cell available.\n");
		return false;
	}
}

bool Robot::RemoveEnergyCell(Module* module)
{
	if (module->m_energy_cells > 0)
	{
		module->m_energy_cells--;
		m_energy_cells_available++;
		return true;
	}
	else
	{
		return false;
	}
}


bool Robot::RemoveEnergyCell(Equipment* equipment)
{
	if (equipment->m_energy_cells > 0)
	{
		equipment->m_energy_cells--;
		m_energy_cells_available++;
		return true;
	}
	else
	{
		return false;
	}
}

bool Robot::SetWeaponAttackOnSlot(WeaponAttack* attack, SlotIndex target_index)
{
	Module* module = attack->m_owner->m_owner->m_module;

	if (module == NULL)
	{
		(*CurrentGame).UI_AddEventLog("Cannot attack empty modules.", Event_Error, m_index);
		return false;
	}
	else if (attack->m_nb_targets_remaining == 0)
	{
		(*CurrentGame).UI_AddEventLog("All targets already assigned for this weapon during this turn.", Event_Error, m_index);
		return false;
	}
	else if (m_energy_cells_available < attack->m_energy_cost && attack->m_nb_targets_remaining == attack->m_nb_targets)
	{
		(*CurrentGame).UI_AddEventLog("Insufficient Energy Cells to pay the cost of this attack.", Event_Error, m_index);
		return false;
	}
	else if (module->m_shutdown_counter > 0)
	{
		(*CurrentGame).UI_AddEventLog("Weapon module is shut down.", Event_Error, m_index);
		return false;
	}
	else if (module->m_health == 0)
	{
		(*CurrentGame).UI_AddEventLog("Weapon module is destroyed.", Event_Error, m_index);
		return false;
	}
	else if (m_unbalanced_counter > 0)
	{
		(*CurrentGame).UI_AddEventLog("Robot cannot attack because he's unbalanced.", Event_Error, m_index);
		return false;
	}
	else if (m_shutdown_global == true)
	{
		(*CurrentGame).UI_AddEventLog("Robot cannot attack because he's undergoing a global shutdown.", Event_Error, m_index);
		return false;
	}
	else if (m_grounded == true)
	{
		(*CurrentGame).UI_AddEventLog("Robot cannot attack because he's on the ground.", Event_Error, m_index);
		return false;
	}
	else if (attack->m_owner->m_ranged == false && (target_index == Index_FootL || target_index == Index_FootR || target_index == Index_LegL || target_index == Index_LegR))
	{
		(*CurrentGame).UI_AddEventLog("Cannot attack lower parts (lets and feet) with close-combat attacks.", Event_Error, m_index);
		return false;
	}
	else if (attack->m_crew_required != NB_CREW_TYPES && attack->m_crew_required != Crew_Any && attack->m_owner->m_owner->HasCrewRequired(attack->m_crew_required) == NULL)
	{
		(*CurrentGame).UI_AddEventLog("Specific crew member required missing or stunned.", Event_Error, m_index);
		return false;
	}
	else if (attack->m_crew_required == Crew_Any && attack->m_owner->m_owner->HasCrewRequired(attack->m_crew_required) == NULL)
	{
		(*CurrentGame).UI_AddEventLog("Crew member missing or stunned, can't execute this attack.", Event_Error, m_index);
		return false;
	}
	else if (attack->m_owner->m_energetic == true && module->m_owner->CanEquipEnergeticWeapon() == false)
	{
		(*CurrentGame).UI_AddEventLog("Energetic weapons require an operational Energy weapon equipment\n(powered with 1 Energy Cell).", Event_Error, m_index);
		return false;
	}
	else if (attack->m_owner->m_requires_close_distance == true && (*CurrentGame).m_distance == Distance_Ranged)
	{
		(*CurrentGame).UI_AddEventLog("This attack requires to be in close-combat.", Event_Error, m_index);
		return false;
	}
	else if (attack->m_owner->m_ranged == true && (*CurrentGame).m_phase == Phase_CounterAttack)
	{
		(*CurrentGame).UI_AddEventLog("Counter-attack must use a close-combat weapon.", Event_Error, m_index);
		return false;
	}
	else if (m_has_guarded == true)
	{
		(*CurrentGame).UI_AddEventLog("Cannot attack when using Guard.", Event_Error, m_index);
		return false;
	}
	else if (attack->m_owner->m_owner->m_index == Index_HandL && m_has_grabbed_LeftHand == true)
	{
		(*CurrentGame).UI_AddEventLog("Cannot attack with Left Hand when using Grab with this hand.", Event_Error, m_index);
		return false;
	}
	else if (attack->m_owner->m_owner->m_index == Index_HandR && m_has_grabbed_RightHand == true)
	{
		(*CurrentGame).UI_AddEventLog("Cannot attack with Right Hand when using Grab with this hand.", Event_Error, m_index);
		return false;
	}
	else
	{
		//Consumption of Energy Cells
		if (attack->m_nb_targets_remaining == attack->m_nb_targets)
		{
			m_energy_cells_available -= attack->m_energy_cost;
			m_energy_cells -= attack->m_energy_cost;
		}
		
		attack->m_nb_targets_remaining--;

		//Stick crew member used (if any)
		if (attack->m_crew_required != NB_CREW_TYPES)
		{
			CrewMember* crew = attack->m_owner->m_owner->HasCrewRequired(attack->m_crew_required);
			crew->m_used = true;
		}

		ActionAttack action;
		action.m_attack = attack;
		action.m_target_index = target_index;
		action.m_resolved = false;

		for (int i = 0; i < attack->m_nb_hits; i++)
		{
			(*CurrentGame).m_attacks_list.push_back(action);
		}

		m_has_attacked = true;
		if (module->m_owner->m_index == Index_HandL)
		{
			m_has_attacked_LeftHand = true;
		}
		if (module->m_owner->m_index == Index_HandR)
		{
			m_has_attacked_RightHand = true;
		}

		return true;
	}
}

bool Robot::SetEquipmentEffectOnSlot(EquipmentEffect* effect, SlotIndex target_index)
{
	Module* module = effect->m_owner_module;
	if (module != NULL)
	{
		if (module->m_used == true)
		{
			(*CurrentGame).UI_AddEventLog("Module already used during this turn.", Event_Error, m_index);
			return false;
		}
		else if (module->IsOperationnal() == false)
		{
			(*CurrentGame).UI_AddEventLog("Module is not operational (destroyed, shutdown, or not powered by an Energy Cell.", Event_Error, m_index);
			return false;
		}
		else if (module->m_cooldown > 0 && module->m_cooldown_timer < module->m_cooldown)
		{

			(*CurrentGame).UI_AddEventLog("Cooldown is not ready.", Event_Error, m_index);
			printf("Cannot use module ability: cooldown is not over (%d turns remaining for cooldown).\n", module->m_cooldown - module->m_cooldown_timer);
			return false;
		}
		else if (module->m_crew_required != NB_CREW_TYPES && module->m_crew_required != Crew_Any && module->m_owner->HasCrewRequired(module->m_crew_required) == NULL)
		{
			(*CurrentGame).UI_AddEventLog("Specific crew member required missing or stunned or already used for this ability.", Event_Error, m_index);
			return false;
		}
		else if (module->m_crew_required == Crew_Any && module->m_owner->HasCrewRequired(module->m_crew_required) == NULL)
		{
			(*CurrentGame).UI_AddEventLog("Crew member missing or stunned or already used for this ability.", Event_Error, m_index);
			return false;
		}
	}

	Equipment* equipment = effect->m_owner_equipment;
	if (equipment != NULL)
	{
		if (equipment->m_used == true)
		{
			(*CurrentGame).UI_AddEventLog("Ability already used during this turn.", Event_Error, m_index);
			return false;
		}
		else if (equipment->m_owner->m_module->IsOperationnal() == false)
		{
			(*CurrentGame).UI_AddEventLog("Required module is not operational (destroyed, shutdown, or not powered by an Energy Cell.", Event_Error, m_index);
			return false;
		}
		else if (equipment->m_cooldown > 0 && equipment->m_cooldown_timer < equipment->m_cooldown)
		{

			(*CurrentGame).UI_AddEventLog("Cooldown is not ready.", Event_Error, m_index);
			printf("Cannot use equipment ability: cooldown is not over (%d turns remaining for cooldown).\n", equipment->m_cooldown - equipment->m_cooldown_timer);
			return false;
		}
		else if (equipment->m_owner->m_module->m_type == Module_Gadget && equipment->m_owner->HasCrewRequired(Crew_Engineer) == NULL)
		{
			(*CurrentGame).UI_AddEventLog("Engineer missing or stunned in the Gadget module.", Event_Error, m_index);
			return false;
		}
		else if (equipment->m_cooldown_timer < equipment->m_cooldown)
		{
			(*CurrentGame).UI_AddEventLog("Cooldown is not ready.", Event_Error, m_index);
			return false;
		}
		else if (m_energy_cells_available < effect->m_energy_cost)
		{
			(*CurrentGame).UI_AddEventLog("Not enough Energy Cells available to use this ability.", Event_Error, m_index);
			return false;
		}
	}
	
	if (module != NULL)
	{
		module->m_used = true;
		if (module->m_cooldown > 0)
		{
			module->m_cooldown_timer = 0;
		}

		//Stick crew member used (if any)
		if (effect->m_crew_required != NB_CREW_TYPES)
		{
			CrewMember* crew = module->m_owner->HasCrewRequired(effect->m_crew_required);
			crew->m_used = true;
		}
	}
	if (equipment != NULL)
	{
		equipment->m_used = true;
		if (equipment->m_cooldown > 0)
		{
			equipment->m_cooldown_timer = 0;
		}

		//Stick crew member used (if any)
		if (effect->m_crew_required != NB_CREW_TYPES)
		{
			CrewMember* crew = equipment->m_owner->HasCrewRequired(effect->m_crew_required);
			crew->m_used = true;
			crew->m_steps_remaining = 0;
		}
	}

	//EC consumption
	m_energy_cells_available -= effect->m_energy_cost;
	m_energy_cells -= effect->m_energy_cost;

	if (effect->m_type == Effect_GeneratorBooster)
	{
		int EC_generated = 0;
		for (int i = 0; i < 3; i++)
		{
			if (m_energy_cells < MAX_ROBOT_ENERGY_CELLS)
			{
				m_energy_cells_available++;
				m_energy_cells++;
				EC_generated++;
			}
		}

		ostringstream s_ec;
		s_ec << EC_generated << " EC generated by the Generator Booster.";
		(*CurrentGame).UI_AddEventLog(s_ec.str(), Event_EC, m_index);
		return true;
	}

	ActionEffect action;
	action.m_effect = effect;
	action.m_target_index = target_index;
	(*CurrentGame).m_effects_list.push_back(action);

	return true;
}

bool Robot::SetMedicTarget(CrewMember* medic, CrewMember* target)
{
	if (medic->m_type != Crew_Medic)
	{
		(*CurrentGame).UI_AddEventLog("Crew member selected to heal is not a Medic.", Event_Error, m_index);
		return false;
	}
	else if (medic->m_stun_counter > 0)
	{
		(*CurrentGame).UI_AddEventLog("Medic is stunned and cannot use his\nactive ability to heal other crew members.", Event_Error, m_index);
		return false;
	}
	else if (target->m_health == 0)
	{
		(*CurrentGame).UI_AddEventLog("Targeted crew member is already dead and cannot be healed.", Event_Error, m_index);
		return false;
	}
	else if (target->m_health == target->m_health_max)
	{
		(*CurrentGame).UI_AddEventLog("Targeted crew member already has full health.", Event_Error, m_index);
		return false;
	}
	else if (medic->m_owner->m_index != target->m_owner->m_index)
	{
		(*CurrentGame).UI_AddEventLog("Medic must be in the same module as the target to heal.", Event_Error, m_index);
		return false;
	}
	else
	{
		medic->m_medic_target = target;
		(*CurrentGame).UI_AddEventLog("Medic's heal target assigned.", Event_Neutral, m_index);
		return true;
	}
}

bool Robot::SetEnergyCellsOnBalance()
{
	if (m_unbalanced_counter == 0)
	{
		(*CurrentGame).UI_AddEventLog("is already balanced", Event_Error, m_index);
		return false;
	}
	else if (m_energy_cells_available < m_unbalanced_value)
	{
		printf("Insufficient Energy Cells available (%d) to cancel unbalance (%d).\n", m_energy_cells_available, m_unbalanced_value);
		(*CurrentGame).UI_AddEventLog("Insufficient EC available to balance the robot.", Event_Error, m_index);
		return false;
	}
	else
	{
		//Spend Energy Cells to cancel the unbalance of the robot
		m_energy_cells_available -= m_unbalanced_value;
		m_energy_cells -= m_unbalanced_value;
		m_unbalanced_counter = 0;
		m_unbalanced_value = 0;

		(*CurrentGame).UI_AddEventLog("'s balance restored by using ECs.", Event_Balance, m_index);
		return true;
	}
}

bool Robot::SetGuard(int speed, bool left)
{
	if (m_has_attacked == true)
	{
		(*CurrentGame).UI_AddEventLog("Cannot Guard if robot already attacked during this turn.", Event_Error, m_index);
		return false;
	}

	m_guard_speed = speed;
	m_guard_index = left == true ? Index_HandL : Index_HandR;
	m_has_guarded = true;
}

bool Robot::SetGrab(bool left, SlotIndex target_index)
{
	if (left == true && m_has_attacked_LeftHand == true)
	{
		(*CurrentGame).UI_AddEventLog("Cannot Grab wih Left Hand if robot already attacked with this hand.", Event_Error, m_index);
		return false;
	}
	else if (left == false && m_has_attacked_RightHand == true)
	{
		(*CurrentGame).UI_AddEventLog("Cannot Grab wih Right Hand if robot already attacked with this hand.", Event_Error, m_index);
		return false;
	}
	else if (m_has_guarded == true)
	{
		(*CurrentGame).UI_AddEventLog("Cannot Grab after a Guard during this same turn.", Event_Error, m_index);
		return false;
	}

	//setting hand
	m_grab_slot->m_index = left == true ? Index_HandL : Index_HandR;

	ActionAttack action;
	action.m_attack = m_grab_slot->m_weapon->m_attacks.front();
	action.m_target_index = target_index;
	action.m_resolved = false;
	
	(*CurrentGame).m_attacks_list.push_back(action);

	if (left == true)
	{
		m_has_grabbed_LeftHand = true;
	}
	else
	{
		m_has_grabbed_RightHand = true;
	}
}