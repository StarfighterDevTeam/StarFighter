#include "CrewMember.h"

extern Game* CurrentGame;

CrewMember::CrewMember(CrewType type, RobotSlot* owner)
{
	m_type = type;
	m_owner = owner;

	m_stun_counter = 0;
	m_medic_target = NULL;
	m_overcharge = 0;
	m_used = false;

	switch (type)
	{
		case Crew_Captain:
		{
			m_health_max = 8;
			m_steps = 4;
			break;
		}
		case Crew_Pilot:
		{
			m_health_max = 5;
			m_steps = 3;
			break;
		}
		case Crew_Mechanic:
		{
			m_health_max = 5;
			m_steps = 4;
			break;
		}
		case Crew_Engineer:
		{
			m_health_max = 5;
			m_steps = 3;
			break;
		}
		case Crew_Scientist:
		{
			m_health_max = 5;
			m_steps = 3;
			break;
		}
		case Crew_Warrior:
		{
			m_health_max = 5;
			m_steps = 3;
			break;
		}
		case Crew_Medic:
		{
			m_health_max = 5;
			m_steps = 4;
			break;
		}
		case Crew_Gunner:
		{
			m_health_max = 5;
			m_steps = 3;
			break;
		}
	}

	m_health = m_health_max;
	m_steps_remaining = m_steps;
}