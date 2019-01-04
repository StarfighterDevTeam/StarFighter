#ifndef MODULE_H_INCLUDED
#define MODULE_H_INCLUDED

#include "Game.h"

class RobotSlot;
class Module : public GameEntity
{
public:
	Module(ModuleType type, RobotSlot* owner);

	ModuleType m_type;

	int m_health;
	int m_health_max;
	int m_shield;
	int m_shield_max;
	int m_weight;
	int m_energy_cells;
	int m_energy_cells_max;
	int m_size;
	bool m_unique;
	int m_cooldown;
	int m_cooldown_timer;//ready when cooldown_timer = cooldown. Increasing every turn.
	int m_crew_max;
	int m_fire_counter;//0: not burning; 1= burning and ready to propagate; 2= burning but not propagating
	int m_shutdown_counter;//0: no shutdown; 1=shutdown will end next turn; 2= shutdown just started
	RobotSlot* m_owner;
	bool m_used;
	CrewType m_crew_required;
	EquipmentEffect* m_effect;

	string m_UI_display_name;
	string m_UI_display_name_short;
	string m_UI_description;

	bool IsOperationnal();
};

#endif //MODULE_H_INCLUDED