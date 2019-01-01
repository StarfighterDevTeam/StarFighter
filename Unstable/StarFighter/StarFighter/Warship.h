#ifndef WARSHIP_H_INCLUDED
#define WARSHIP_H_INCLUDED

#include "Game.h"

#include "CrewMember.h"

class Warship : public GameEntity
{
public:
	Warship();
	~Warship();

	vector<Room*> m_rooms;
	vector<CrewMember*> m_crew;

	void InitWarship();
	Room* AddRoom(int upcorner_x, int upcorner_y, int width, int height, RoomType type);
	CrewMember* AddCrewMember(CrewMember* crew, Room* room);
};

#endif //WARSHIP_H_INCLUDED