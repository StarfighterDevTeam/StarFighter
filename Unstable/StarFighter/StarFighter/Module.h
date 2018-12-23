#ifndef MODULE_H_INCLUDED
#define MODULE_H_INCLUDED

#include "Game.h"


class Module
{
public:
	Module(ModuleType type);

	ModuleType m_type;

	int m_health;
	int m_health_max;
	int m_weight;
	int m_cells;
	int m_cells_max;
	int m_size;
	bool m_unique;
	int m_cooldown;
	int m_cooldown_timer;
	int m_crew_max;
};

#endif //MODULE_H_INCLUDED