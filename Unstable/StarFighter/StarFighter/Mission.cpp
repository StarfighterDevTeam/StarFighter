#include "Mission.h"

extern Game* CurrentGame;

using namespace sf;

Mission::Mission(MissionType mission_type, SpatialObject* target)
{
	m_mission_type = mission_type;
	m_status = MissionStatus_Accepted;

	if (target != NULL)
		m_marked_objectives.push_back(target);
}