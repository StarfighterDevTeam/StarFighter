#ifndef ROOM_H_INCLUDED
#define ROOM_H_INCLUDED

#include "Game.h"
#include "RoomConnexion.h"
#include "RoomTile.h"

class Weapon;
class Engine;

class Room : public GameEntity
{
public:
	Room(int upcorner_x, int upcorner_y, int width, int height, RoomType type);
	~Room();

	RoomType m_type;

	int m_upcorner_x;
	int m_upcorner_y;
	int m_width;
	int m_height;

	vector<RoomConnexion*> m_connexions;
	vector<RoomTile*> m_tiles;

	bool m_is_flooded;//0-100

	void Update(Time deltaTime);
	static bool IsConnectedToRoomTile(RoomTile* tileA, RoomTile* tileB);
	bool IsConnectedToRoom(Room* room);
};

#endif //ROOM_H_INCLUDED