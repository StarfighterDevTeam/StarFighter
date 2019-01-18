#ifndef ROBOTSLOT_H_INCLUDED
#define ROBOTSLOT_H_INCLUDED

#include "Game.h"

#include "Weapon.h"
#include "Module.h"
#include "Equipment.h"
#include "CrewMember.h"

class Robot;
class RobotSlot : public GameEntity
{
public:
	RobotSlot(SlotIndex index, Robot* owner);

	int m_size;
	SlotType m_type;
	SlotIndex m_index;

	vector<CrewMember*> m_crew;//destroyed in class Robot
	Module* m_module;
	vector<Equipment*> m_equipments;
	Weapon* m_weapon;
	Robot* m_owner;

	bool m_is_revealed;

	int GetGunnerRangeBonus();
	int GetPilotSpeedBonus();
	int GetEquipmentRangeBonus();
	int GetEquipmentSpeedBonus();
	int GetWarriorBalanceBonus();
	CrewMember* HasCrewRequired(CrewType type);
	bool CanEquipEnergeticWeapon();
};

#endif //ROBOTSLOT_H_INCLUDED