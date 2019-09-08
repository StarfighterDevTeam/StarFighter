#include "Mission.h"

extern Game* CurrentGame;

using namespace sf;

Mission::Mission(MissionType mission_type, SpatialObject* target, Planet* owner)
{
	m_mission_type = mission_type;
	m_status = MissionStatus_Accepted;
	m_owner = owner;

	if (target != NULL)
		m_marked_objectives.push_back(target);

	switch (mission_type)
	{
		case Mission_GoTo:
		{
			Planet* planet = (Planet*)target;
			m_title_text = "GoTo Mission";
			m_body_text = "Go to planet " + to_string(planet->m_planet_id);
			break;
		}
		case Mission_Bounty:
		{
			AIShip* ship = (AIShip*)target;
			m_title_text = "Bounty Mission";
			m_body_text = "Go bounty hunting";
			break;
		}
	}
}