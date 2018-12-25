#include "Equipment.h"

extern Game* CurrentGame;

Equipment::Equipment(EquipmentType type, RobotSlot* owner)
{
	m_type = type;
	m_owner = owner;
	m_used = false;

	m_health_max = 0;
	m_weight = 0;
	m_energy_cells = 0;
	m_energy_cells_max = 0;
	m_unique = false;
	m_size = 1;
	m_crew_max = 0;
	m_module_equipable = NB_MODULE_TYPES;
	m_effect = NULL;
	
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

			m_effect = new EquipmentEffect(Effect_GeneratorBooster, NULL, this);
			m_effect->m_energy_cost = 2;
			break;
		}
		case Equipment_GadgetJammer:
		{
			m_weight = 1;
			m_cooldown = 2;
			m_module_equipable = Module_Gadget;

			m_effect = new EquipmentEffect(Effect_Jammer, NULL, this);
			break;
		}
		case Equipment_GadgetEMP:
		{
			m_weight = 1;
			m_cooldown = 2;
			m_module_equipable = Module_Gadget;

			m_effect = new EquipmentEffect(Effect_EMP, NULL, this);
			m_effect->m_energy_cost = 1;
			break;
		}
		case Equipment_WeaponsScope:
		{
			m_weight = 1;
			m_energy_cells_max = 1;
			m_energy_cells = 0;
			m_module_equipable = Module_Weapon;
			m_effect = new EquipmentEffect(Effect_WeaponScopes, NULL, this);
			break;
		}
	}

	m_health = m_health_max;
	m_cooldown_timer = m_cooldown;
}

Equipment::~Equipment()
{
	if (m_effect != NULL)
	{
		delete m_effect;
		m_effect = NULL;
	}
}