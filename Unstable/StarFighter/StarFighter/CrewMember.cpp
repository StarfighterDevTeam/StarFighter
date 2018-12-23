#include "CrewMember.h"

extern Game* CurrentGame;
CrewMember::CrewMember(CrewType type)
{
	m_type = type;
	m_slot_index = NB_SLOT_INDEX;

	switch (type)
	{
		case Crew_Captain:
		{
			m_health_max = 8;
			m_steps = 5;
			break;
		}
		case Crew_Pilot:
		{
			m_health_max = 3;
			m_steps = 3;
			break;
		}
		case Crew_Mechanic:
		{
			m_health_max = 4;
			m_steps = 4;
			break;
		}
		case Crew_Engineer:
		{
			m_health_max = 4;
			m_steps = 3;
			break;
		}
		case Crew_Scientist:
		{
			m_health_max = 3;
			m_steps = 3;
			break;
		}
		case Crew_Warrior:
		{
			m_health_max = 4;
			m_steps = 3;
			break;
		}
		case Crew_Medic:
		{
			m_health_max = 4;
			m_steps = 4;
			break;
		}
		case Crew_Gunner:
		{
			m_health_max = 4;
			m_steps = 3;
			break;
		}
	}

	m_health = m_health_max;
}