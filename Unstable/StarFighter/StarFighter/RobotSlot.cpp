#include "RobotSlot.h"

extern Game* CurrentGame;

RobotSlot::RobotSlot(SlotIndex index)
{
	m_index = index;
	m_module = NULL;
	m_weapon = NULL;

	switch (index)
	{
		case Index_Head:
		{
			m_size = 1;
			m_type = Slot_Head;
			m_coord_x = 0;
			m_coord_y = 3;
			break;
		}
		case Index_BodyU:
		{
			m_size = 3;
			m_type = Slot_Body;
			m_coord_x = 0;
			m_coord_y = 2;
			break;
		}
		case Index_BodyM:
		{
			m_size = 3;
			m_type = Slot_Body;
			m_coord_x = 0;
			m_coord_y = 1;
			break;
		}
		case Index_BodyD:
		{
			m_size = 2;
			m_type = Slot_Body;
			m_coord_x = 0;
			m_coord_y = 0;
			break;
		}
		case Index_LegL:
		{
			m_size = 2;
			m_type = Slot_Leg;
			m_coord_x = -1;
			m_coord_y = 0;
			break;
		}
		case Index_LegR:
		{
			m_size = 2;
			m_type = Slot_Leg;
			m_coord_x = 1;
			m_coord_y = 0;
			break;
		}
		case Index_FootL:
		{
			m_size = 2;
			m_type = Slot_Foot;
			m_coord_x = -2;
			m_coord_y = 0;
			break;
		}
		case Index_FootR:
		{
			m_size = 2;
			m_type = Slot_Foot;
			m_coord_x = 2;
			m_coord_y = 0;
			break;
		}
		case Index_ShoulderL:
		{
			m_size = 2;
			m_type = Slot_Shoulder;
			m_coord_x = -1;
			m_coord_y = 2;
			break;
		}
		case Index_ShoulderR:
		{
			m_size = 2;
			m_type = Slot_Shoulder;
			m_coord_x = 1;
			m_coord_y = 2;
			break;
		}
		case Index_ForearmL:
		{
			m_size = 2;
			m_type = Slot_Forearm;
			m_coord_x = -2;
			m_coord_y = 2;
			break;
		}
		case Index_ForearmR:
		{
			m_size = 2;
			m_type = Slot_Forearm;
			m_coord_x = 2;
			m_coord_y = 2;
			break;
		}
	}
}

void RobotSlot::UpdateCrew()
{
	vector<CrewMember*> old_crew;
	for (vector<CrewMember*>::iterator it = m_crew.begin(); it != m_crew.end(); it++)
	{
		old_crew.push_back(*it);
	}

	m_crew.clear();

	for (vector<CrewMember*>::iterator it = old_crew.begin(); it != old_crew.end(); it++)
	{
		if ((*it)->m_health > 0)
		{
			m_crew.push_back(*it);
		}
		else
		{
			printf("Crew member dead.\n");
			if ((*it)->m_type == Crew_Captain)
			{
				printf("Captain was killed. GAME OVER.\n");
			}
		}
	}
}