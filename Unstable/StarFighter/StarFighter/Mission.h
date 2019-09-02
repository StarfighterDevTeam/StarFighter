#ifndef MISSION_H_INCLUDED
#define MISSION_H_INCLUDED

#include "AIShip.h"

enum MissionStatus
{
	MissionStatus_Accepted,
	MissionStatus_Current,
	MissionStatus_Complete,
	MissionStatus_Failed,
};

enum MissionType
{
	Mission_GoTo,
	NB_MISSION_TYPES,
};

class Mission
{
public :
	Mission(MissionType mission_type, SpatialObject* target, SpatialObject* owner);
	~Mission(){};

	MissionStatus m_status;
	MissionType m_mission_type;
	vector<SpatialObject*> m_marked_objectives;
	SpatialObject* m_owner;
};

#endif // MISSION_H_INCLUDED
