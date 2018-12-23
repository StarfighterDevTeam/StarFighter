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
	RobotSlot(SlotType type, int size, SlotIndex index){ m_size = size; m_type = type; m_index = index; m_module = NULL; m_weapon = NULL; };

	int m_size;
	SlotType m_type;
	SlotIndex m_index;

	vector<CrewMember> m_crew;
	Module* m_module;
	vector<Equipment*> m_equipments;
	Weapon* m_weapon;
};


class Robot
{
public:
	Robot();
	~Robot();

	vector<CrewMember> m_crew;
	vector<RobotSlot> m_slots;

	bool SetModule(ModuleType type, SlotIndex index);
	bool SetEquipment(EquipmentType type, SlotIndex index);
	bool SetCrewMember(CrewType type, SlotIndex index);
	bool SetWeapon(WeaponType type, SlotIndex index);

	int m_health;
	int m_health_max;
	int GetHealthMax();
	int m_weight;
	int m_crew_max;
	int m_crew_nb;
	int GetCrewMax();
	int m_balance;
	int m_balance_bonus;
	int m_attack_speed_bonus;
	void GetWeightModifiers(int &balance_bonus, int &attack_speed_bonus);

	void Initialize();
	void Update();
};



#endif //ROBOT_H_INCLUDED