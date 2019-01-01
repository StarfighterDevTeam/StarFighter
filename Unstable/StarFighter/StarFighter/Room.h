#ifndef ROOM_H_INCLUDED
#define ROOM_H_INCLUDED

#include "Game.h"

#include "RoomTile.h"

enum RoomType
{
	Room_Empty,
	Room_Crewquarter,
	Room_Navigation,
	Room_Weapon,
	Room_Gold,
	Room_Fishing,
	Room_Kitchen,
	Room_Relic,
	Room_Sword,
	Room_Lifeboat,
	Room_Ammo,
	Room_Engine,
	Room_Mecha,
	NB_ROOM_TYPES,
};

class Room : public GameEntity
{
public:
	Room(int upcorner_x, int upcorner_y, int width, int height, RoomType type);
	~Room();

	RoomType m_type;

	vector<RoomTile*> m_tiles;
	vector<CrewMember*> m_crew;
	RoomTile* GetFreeRoomTile();

	int m_upcorner_x;
	int m_upcorner_y;
	int m_width;
	int m_height;

	vector<RoomConnexion*> m_connexions;
};

#endif //ROOM_H_INCLUDED