#ifndef MISSION_H_INCLUDED
#define MISSION_H_INCLUDED

#include "Beacon.h"

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
	Mission_Bounty,
	NB_MISSION_TYPES,
};

class Mission
{
public :
	Mission(MissionType mission_type, SpatialObject* target, Planet* owner);
	~Mission(){};

	MissionStatus m_status;
	MissionType m_mission_type;
	vector<SpatialObject*> m_marked_objectives;
	Planet* m_owner;
	int m_money_prize;
	string m_title_text;
	string m_body_text;
};

#endif // MISSION_H_INCLUDED
