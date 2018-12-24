#ifndef ROBOT_H_INCLUDED
#define ROBOT_H_INCLUDED

#include "Game.h"

#include "RobotSlot.h"

class Robot
{
public:
	Robot();
	~Robot();

	int m_index;

	vector<CrewMember*> m_crew;
	vector<RobotSlot> m_slots;

	int m_robot_map[NB_SLOT_INDEX][NB_SLOT_INDEX];

	bool SetModule(ModuleType type, SlotIndex index);
	bool SetEquipment(EquipmentType type, SlotIndex index);
	bool SetCrewMember(CrewType type, SlotIndex index);
	bool SetWeapon(WeaponType type, SlotIndex index);

	int m_health;
	int m_health_max;
	int GetHealthMax();
	int m_weight;
	int m_energy_cells;
	int m_energy_cells_available;
	int m_crew_max;
	int m_balance;
	int GetBalanceScore();
	int m_balance_bonus;
	int m_attack_speed_bonus;
	void GetWeightModifiers(int &balance_bonus, int &attack_speed_bonus);
	bool m_unbalanced;
	bool m_shutdown_global;

	bool m_ready_to_change_phase;

	void Initialize();
	void Update();
	bool UpdateShudownGlobal();
	bool CheckShudownGlobalConditions();
	void DestroySlot(SlotIndex index);
	void ShutdownSlot(SlotIndex index);
	void UpdateCrew(SlotIndex index);
	int GetDistanceFromSlotToSlot(SlotIndex index, SlotIndex target_index);

	int GenerateEnergyCells();
	int HealCrewMembers();
	int RepairModules();
	void UpdateFirePropagation();
	void UpdateCooldowns();
	bool MoveCrewMemberToSlot(CrewMember* crew, SlotIndex target_index);
	bool SetEnergyCell(Module* module);
	bool SetEnergyCell(Equipment* equipment);
	bool SetEnergyCell(WeaponAttack* attack);
	bool SetWeaponAttackOnSlot(WeaponAttack* attack, SlotIndex target_index);
};

#endif //ROBOT_H_INCLUDED