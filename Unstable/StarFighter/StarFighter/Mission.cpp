#include "Mission.h"

extern Game* CurrentGame;

using namespace sf;

Mission::Mission(MissionType mission_type, SpatialObject* target, Planet* owner)
{
	m_mission_type = mission_type;
	m_status = MissionStatus_Accepted;
	m_owner = owner;
	m_money_prize = 100;

	if (target != NULL)
		m_marked_objectives.push_back(target);

	switch (mission_type)
	{
		case Mission_GoTo:
		{
			Planet* planet = (Planet*)target;
			m_title_text = "GoTo Mission";
			break;
		}
		case Mission_Convoy:
		{
			m_title_text = "Convoy Mission";
			break;
		}
		case Mission_Bounty:
		{
			m_title_text = "Bounty Mission";
			break;
		}
		case Mission_EliminateBoss:
		{
			m_title_text = "Eliminate Mission";
			break;
		}
	}
}