#include "Module.h"

extern Game* CurrentGame;

Module::Module(ModuleType type, RobotSlot* owner)
{
	m_type = type;
	m_owner = owner;
	m_used = false;

	m_weight = 0;
	m_energy_cells = 0;
	m_energy_cells_max = 0;
	m_unique = false;
	m_size = 1;
	m_health_max = 0;
	m_cooldown = -1;
	m_crew_max = 0;
	m_fire_counter = 0;
	m_shutdown_counter = 0;
	m_crew_required = NB_CREW_TYPES;
	m_effect = NULL;

	switch (type)
	{
		case Module_Head:
		{
			m_health_max = 20;

			m_UI_display_name = "Head";
			m_UI_description = "Requires a Captain or a Pilot to active the robot.\nYou LOSE the game if the Head is destroyed.";
			break;
		}
		case Module_Stabilizers:
		{
			m_health_max = 10;
			m_energy_cells_max = 1;
			
			m_UI_display_name = "Stabilizer";
			m_UI_description = "1EC = +1 Balance";
			break;
		}
		case Module_Generator:
		{
			m_health_max = 10;
			m_weight = 1;
			m_unique = true;
			
			m_UI_display_name = "Generator";
			m_UI_description = "Generates 3EC per turn";
			break;
		}
		case Module_Weapon:
		{
			m_health_max = 10;
			m_energy_cells_max = 1;

			m_UI_display_name = "Weapon mod";
			m_UI_display_name_short = "Weapon\nmod";
			m_UI_description = "Weapon slot";
			break;
		}
		case Module_Gadget:
		{
			m_health_max = 6;
			m_energy_cells_max = 2;

			m_UI_display_name = "Gadget";
			m_UI_description = "Requires an Egineer to use active equipment";
			break;
		}
		case Module_CrewQuarter:
		{
			m_health_max = 6;
			m_weight = 1;
			m_unique = true;
			m_crew_max = 4;

			m_UI_display_name = "Crew Quarter";
			m_UI_display_name_short = "Crew\nQuarter";
			m_UI_description = "Allows 4 crew members to live in the robot";
			break;
		}
		case Module_Deflectors:
		{
			m_health_max = 10;
			m_weight = 1;
			m_unique = true;
			m_size = 2;
			m_energy_cells_max = 2;

			m_UI_display_name = "Deflectors";
			m_UI_description = "Shield Generator.\n1EC = 10hp shield.\n1EC = 2hp regen per turn";
			break;
		}
		case Module_Infirmary:
		{
			m_health_max = 6;
			m_weight = 1;
			m_energy_cells_max = 2;

			m_UI_display_name = "Infirmary";
			m_UI_description = "Heals crew members present in the module each turn.\n1 EC = 3hp regent per turn";
			break;
		}
		case Module_Radar:
		{
			m_health_max = 6;
			m_weight = 1;
			m_unique = true;
			m_energy_cells_max = 1;
			m_cooldown = 2;
			m_crew_required = Crew_Any;
			m_effect = new EquipmentEffect(Effect_Radar, this, NULL);
			m_effect->m_energy_cost = 1;

			m_UI_display_name = "Radar";
			m_UI_description = "Reveals a target enemy module and its equipments for 1 EC";
			break;
		}
		case Module_Sensors:
		{
			m_health_max = 6;
			m_weight = 1;
			m_unique = true;
			m_energy_cells_max = 1;
			m_cooldown = 2;
			m_crew_required = Crew_Any;

			m_UI_display_name = "Sensors";
			m_UI_description = "Reveals a target enemy module and its equipments";

			break;
		}
	}

	m_health = m_health_max;
	m_cooldown_timer = m_cooldown;
	if (m_UI_display_name_short.empty() == true)
	{
		m_UI_display_name_short = m_UI_display_name;
	}
}

bool Module::IsOperationnal()
{
	bool operational = false;

	bool not_shutdown = m_shutdown_counter == 0;
	bool not_destroyed = m_health > 0;
	bool powered_with_energy = m_energy_cells_max == 0 || m_energy_cells >= 1;

	operational = not_shutdown && not_destroyed && powered_with_energy;

	return operational;
}