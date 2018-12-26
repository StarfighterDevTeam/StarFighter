#ifndef EQUIPMENT_H_INCLUDED
#define EQUIPMENT_H_INCLUDED

#include "Game.h"

class RobotSlot;
class Equipment : public GameEntity
{
public:
	Equipment(EquipmentType type, RobotSlot* owner);
	~Equipment();

	EquipmentType m_type;
	ModuleType m_module_equipable;

	int m_health;
	int m_health_max;
	int m_weight;
	int m_energy_cells;
	int m_energy_cells_max;
	int m_size;
	bool m_unique;
	int m_cooldown;
	int m_cooldown_timer;
	int m_crew_max;
	RobotSlot* m_owner;
	bool m_used;

	EquipmentEffect* m_effect;
};

#endif //EQUIPMENT_H_INCLUDED