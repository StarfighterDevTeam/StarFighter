#ifndef EQUIPMENT_H_INCLUDED
#define EQUIPMENT_H_INCLUDED

#include "Game.h"


class Equipment
{
public:
	Equipment(EquipmentType type);

	EquipmentType m_type;
	ModuleType m_module_equipable;

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

#endif //EQUIPMENT_H_INCLUDED