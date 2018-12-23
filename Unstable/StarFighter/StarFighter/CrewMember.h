#ifndef CREWMEMBER_H_INCLUDED
#define CREWMEMBER_H_INCLUDED

#include "Game.h"

class CrewMember
{
public:
	CrewMember(CrewType type);

	CrewType m_type;
	int m_health_max;
	int m_health;
	int m_steps;
	SlotIndex m_slot_index;

	void Update();
};


#endif //CREWMEMBER_H_INCLUDED