#ifndef CREWMEMBER_H_INCLUDED
#define CREWMEMBER_H_INCLUDED

#include "Game.h"

class RobotSlot;
class CrewMember
{
public:
	CrewMember(CrewType type, RobotSlot* owner);

	CrewType m_type;
	int m_health_max;
	int m_health;
	int m_steps;
	SlotIndex m_index;
	int m_stun_counter;//0= not stunned; 1=stunned until next turn; 2=gets stunned
	bool m_is_healed_by_medic;
	int m_overcharge;
	RobotSlot* m_owner;

	void Update();
};


#endif //CREWMEMBER_H_INCLUDED