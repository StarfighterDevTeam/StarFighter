#include "Mission.h"

extern Game* CurrentGame;

using namespace sf;

Mission::Mission(MissionType mission_type)
{
	m_mission_type = mission_type;
	m_status = MissionStatus_Accepted;
}