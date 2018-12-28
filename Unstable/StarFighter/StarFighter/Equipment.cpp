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

			m_UI_display_name = "Energetic weapon";
			m_UI_display_name_short = "Energetic\nweapon";
			m_UI_description = "Allows an Energetic weapon to be equipped in the Weapon module";
			break;
		}
		case Equipment_CQExtension:
		{
			m_weight = 1;
			m_crew_max = 2;
			m_module_equipable = Module_CrewQuarter;

			m_UI_display_name = "CQ Extension";
			m_UI_display_name_short = "CQ\nExtension";
			m_UI_description = "Allows 2 additional crew members to live in the robot";
			break;
		}
		case Equipment_LightPlate:
		{
			m_health_max = 6;
			m_weight = 1;
			
			m_UI_display_name = "Light plate";
			m_UI_description = "Physical attacks hit the armor plate instead of the module";
			break;
		}
		case Equipment_HeavyPlate:
		{
			m_health_max = 10;
			m_weight = 2;
			
			m_UI_display_name = "Heavy plate";
			m_UI_description = "Physical attacks hit the armor plate instead of the module";
			break;
		}
		case Equipment_GeneratorBooster:
		{
			m_weight = 1;
			m_cooldown = 3;
			m_module_equipable = Module_Generator;

			m_effect = new EquipmentEffect(Effect_GeneratorBooster, NULL, this);
			
			m_UI_display_name = "Generator booster";
			m_UI_display_name_short = "Generator\nbooster";
			m_UI_description = "Generates automatically 3 EC when every 4 turns";

			break;
		}
		case Equipment_GadgetJammer:
		{
			m_weight = 1;
			m_cooldown = 2;
			m_module_equipable = Module_Gadget;

			m_effect = new EquipmentEffect(Effect_Jammer, NULL, this);

			m_UI_display_name = "Gadget Jammer";
			m_UI_display_name_short = "Gadget\nJammer";
			m_UI_description = "Unknown";
			break;
		}
		case Equipment_GadgetEMP:
		{
			m_weight = 1;
			m_cooldown = 2;
			m_module_equipable = Module_Gadget;

			m_effect = new EquipmentEffect(Effect_EMP, NULL, this);
			m_effect->m_energy_cost = 1;

			m_UI_display_name = "Gadget E.M.P.";
			m_UI_display_name_short = "Gadget\nE.M.P.";
			m_UI_description = "Removes all EC from an opponent module";
			break;
		}
		case Equipment_WeaponsScope:
		{
			m_weight = 1;
			m_energy_cells_max = 1;
			m_energy_cells = 0;
			m_module_equipable = Module_Weapon;
			m_effect = new EquipmentEffect(Effect_WeaponScopes, NULL, this);

			m_UI_display_name = "Weapon Scope";
			m_UI_display_name_short = "Weapon\nScope";
			m_UI_description = "+1 chance to hit on this ranged weapon";
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

Equipment::~Equipment()
{
	if (m_effect != NULL)
	{
		delete m_effect;
		m_effect = NULL;
	}
}