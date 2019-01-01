#ifndef CREWMEMBER_H_INCLUDED
#define CREWMEMBER_H_INCLUDED

#include "Game.h"

#include "Room.h"

#define CREWMEMBER_SIZE		16.f

enum CrewMemberType
{
	Crew_Unknown,
	Crew_Pirate,
	Crew_Civilian,
	Crew_Slave,
	Crew_Mecha,
	NB_CREW_TYPES,
};

class CrewMember : public GameEntity
{
public:
	CrewMember(CrewMemberType type);
	~CrewMember();

	CrewMemberType m_type;
};

#endif //CREWMEMBER_H_INCLUDED