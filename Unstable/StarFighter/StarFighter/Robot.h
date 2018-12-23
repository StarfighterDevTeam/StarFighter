#ifndef ROBOT_H_INCLUDED
#define ROBOT_H_INCLUDED

#include "Game.h"
#include "Weapon.h"
#include "Module.h"
#include "Equipment.h"
#include "CrewMember.h"

class RobotSlot
{
public:
	RobotSlot(SlotIndex index);

	int m_size;
	SlotType m_type;
	SlotIndex m_index;
	int m_coord_x;
	int m_coord_y;

	vector<CrewMember*> m_crew;//destroyed in class Robot
	Module* m_module;
	vector<Equipment*> m_equipments;
	Weapon* m_weapon;

	void UpdateCrew();
};

class Robot
{
public:
	Robot();
	~Robot();

	int m_index;

	vector<CrewMember*> m_crew;
	vector<RobotSlot> m_slots;

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
	int m_crew_nb;
	int m_balance;
	int GetBalance();
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

	int GenerateEnergyCells();
	int HealCrewMembers();
	int RepairModules();
	void UpdateFirePropagation();
	void UpdateCooldowns();
	int GetGunnerRangeBonus();
	int GetWarriorBalanceBonus();
	bool MoveCrewMemberToSlot(CrewMember* crew, SlotIndex target_index);
};

#endif //ROBOT_H_INCLUDED