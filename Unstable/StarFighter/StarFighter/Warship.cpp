#include "Warship.h"

extern Game* CurrentGame;

Warship::Warship()
{
	
}

Warship::~Warship()
{
	
}

void Warship::InitWarship()
{
	//left
	AddRoom(1, 3, 3, 4, Room_Weapon);
	AddRoom(0, 7, 4, 6, Room_Gold);
	AddRoom(0, 13, 4, 6, Room_Relic);
	AddRoom(0, 19, 4, 3, Room_Sword);

	//mid
	AddRoom(6, 0, 4, 3, Room_Weapon);
	AddRoom(4, 3, 8, 6, Room_Navigation);
	AddRoom(4, 9, 8, 10, Room_Crewquarter);
	AddRoom(4, 19, 8, 3, Room_Ammo);
	AddRoom(5, 22, 6, 3, Room_Engine);

	//right
	AddRoom(12, 3, 3, 4, Room_Weapon);
	AddRoom(12, 7, 4, 6, Room_Fishing);
	AddRoom(12, 13, 4, 6, Room_Kitchen);
	AddRoom(12, 19, 4, 3, Room_Lifeboat);
}

void Warship::AddRoom(int upcorner_x, int upcorner_y, int width, int height, RoomType type)
{
	Room* room = new Room(upcorner_x, upcorner_y, width, height, type);
	m_rooms.push_back(room);
}