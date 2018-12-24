#include "Equipment.h"

extern Game* CurrentGame;

Equipment::Equipment(EquipmentType type, RobotSlot* owner)
{
	m_type = type;
	m_owner = owner;

	m_weight = 0;
	m_energy_cells = 0;
	m_energy_cells_max = 0;
	m_unique = false;
	m_size = 1;
	m_crew_max = 0;
	m_module_equipable = NB_MODULE_TYPES;
	
	switch (type)
	{
		case Equipment_EnergeticWeapon:
		{
			m_energy_cells_max = 1;
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
			m_energy_cells_max = 1;
			m_energy_cells = 0;
			m_module_equipable = Module_Weapon;
			break;
		}
	}

	m_health = m_health_max;
	m_cooldown_timer = m_cooldown;
}
