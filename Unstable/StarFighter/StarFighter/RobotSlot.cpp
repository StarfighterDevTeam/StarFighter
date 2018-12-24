#include "RobotSlot.h"

extern Game* CurrentGame;

RobotSlot::RobotSlot(SlotIndex index, Robot* owner)
{
	m_index = index;
	m_owner = owner;

	m_module = NULL;
	m_weapon = NULL;

	switch (index)
	{
		case Index_Head:
		{
			m_size = 1;
			m_type = Slot_Head;
			break;
		}
		case Index_BodyU:
		{
			m_size = 3;
			m_type = Slot_Body;
			break;
		}
		case Index_BodyM:
		{
			m_size = 3;
			m_type = Slot_Body;
			break;
		}
		case Index_BodyD:
		{
			m_size = 2;
			m_type = Slot_Body;
			break;
		}
		case Index_LegL:
		{
			m_size = 2;
			m_type = Slot_Leg;
			break;
		}
		case Index_LegR:
		{
			m_size = 2;
			m_type = Slot_Leg;
			break;
		}
		case Index_FootL:
		{
			m_size = 2;
			m_type = Slot_Foot;
			break;
		}
		case Index_FootR:
		{
			m_size = 2;
			m_type = Slot_Foot;
			break;
		}
		case Index_ShoulderL:
		{
			m_size = 2;
			m_type = Slot_Shoulder;
			break;
		}
		case Index_ShoulderR:
		{
			m_size = 2;
			m_type = Slot_Shoulder;
			break;
		}
		case Index_ForearmL:
		{
			m_size = 2;
			m_type = Slot_Forearm;
			break;
		}
		case Index_ForearmR:
		{
			m_size = 2;
			m_type = Slot_Forearm;
			break;
		}
	}
}

int RobotSlot::GetGunnerRangeBonus()
{
	int gunner_bonus = 0;

	if (m_module != NULL && m_module->m_type == Module_Weapon && m_weapon != NULL && m_weapon->m_ranged == true) 
	{
		for (vector<CrewMember*>::iterator it2 = m_crew.begin(); it2 != m_crew.end(); it2++)
		{
			if ((*it2)->m_type == Crew_Gunner && (*it2)->m_stun_counter == 0)
			{
				gunner_bonus++;
			}
		}
	}

	return gunner_bonus;
}

int RobotSlot::GetWarriorBalanceBonus()
{
	int warrior_bonus = 0;
	
	if (m_module != NULL && m_module->m_type == Module_Weapon && m_weapon != NULL && m_weapon->m_ranged == false)
	{
		for (vector<CrewMember*>::iterator it2 = m_crew.begin(); it2 != m_crew.end(); it2++)
		{
			if ((*it2)->m_type == Crew_Warrior && (*it2)->m_stun_counter == 0)
			{
				warrior_bonus += 2;
			}
		}
	}

	return warrior_bonus;
}

bool RobotSlot::HasCrewRequired(CrewType type)
{
	for (vector<CrewMember*>::iterator it = m_crew.begin(); it != m_crew.end(); it++)
	{
		if ((*it)->m_type == type && (*it)->m_stun_counter == 0)
		{
			return true;
		}
	}

	return false;
}