#ifndef MODULE_H_INCLUDED
#define MODULE_H_INCLUDED

#include "Game.h"

class RobotSlot;
class Module
{
public:
	Module(ModuleType type, RobotSlot* owner);

	ModuleType m_type;

	int m_health;
	int m_health_max;
	int m_weight;
	int m_energy_cells;
	int m_energy_cells_max;
	int m_size;
	bool m_unique;
	int m_cooldown;
	int m_cooldown_timer;//ready when cooldown_timer = cooldown. Increasing every turn.
	int m_crew_max;
	int m_fire_counter;//-1: not burning; 0= burning and ready to propagate; 1= burning but not propagating
	bool m_is_shutdown;
	RobotSlot* m_owner;
};

#endif //MODULE_H_INCLUDED