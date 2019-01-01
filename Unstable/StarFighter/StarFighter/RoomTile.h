#ifndef ROOMTILE_H_INCLUDED
#define ROOMTILE_H_INCLUDED

#include "Game.h"

#include "GameEntity.h"

class Room;//forward declaration

#define ROOMTILE_SIZE		32.f
#define ROOMTILE_OFFSET_X	200.f
#define ROOMTILE_OFFSET_Y	100.f

class RoomTile : public GameEntity
{
public:
	RoomTile(int coord_x, int coord_y, Room* room, float size = ROOMTILE_SIZE);
	~RoomTile();

	Room* m_room;
	int m_coord_x;
	int m_coord_y;
};

#endif //ROOMTILE_H_INCLUDED