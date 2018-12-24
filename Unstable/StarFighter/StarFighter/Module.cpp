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

	switch (type)
	{
		case Module_Head:
		{
			m_health_max = 20;
			break;
		}
		case Module_Stabilizers:
		{
			m_health_max = 10;
			m_energy_cells_max = 1;
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
			m_energy_cells_max = 1;
			break;
		}
		case Module_Gadget:
		{
			m_health_max = 6;
			m_energy_cells_max = 2;
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
			m_energy_cells_max = 2;
			break;
		}
		case Module_Infirmary:
		{
			m_health_max = 6;
			m_weight = 1;
			m_energy_cells_max = 2;
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
			break;
		}
		case Module_Sensors:
		{
			m_health_max = 6;
			m_weight = 1;
			m_unique = true;
			m_energy_cells_max = 1;
			m_cooldown = 2;
			break;
		}
	}

	m_health = m_health_max;
	m_cooldown_timer = m_cooldown;
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