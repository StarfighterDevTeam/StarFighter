#ifndef WARSHIP_H_INCLUDED
#define WARSHIP_H_INCLUDED

#include "Game.h"

#include "Room.h"

class Warship : public GameEntity
{
public:
	Warship();
	~Warship();

	vector<Room*> m_rooms;

	void InitWarship();
	void AddRoom(int upcorner_x, int upcorner_y, int width, int height, RoomType type);
};

#endif //WARSHIP_H_INCLUDED